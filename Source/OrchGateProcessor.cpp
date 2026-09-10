#include "OrchGateProcessor.h"
#include "OrchGateEditor.h"

OrchGateAudioProcessor::OrchGateAudioProcessor()
    : AudioProcessor (BusesProperties()),
      parameters (*this, nullptr, "OrchGateParameters", createParameterLayout())
{
    manualGateParameter = parameters.getRawParameterValue ("manualGate");
    participationParameter = parameters.getRawParameterValue ("participation");
    ccGateEnableParameter = parameters.getRawParameterValue ("ccGateEnable");
    ccNumberParameter = parameters.getRawParameterValue ("ccNumber");
    ccThresholdParameter = parameters.getRawParameterValue ("ccThreshold");
    ccInvertParameter = parameters.getRawParameterValue ("ccInvert");
    ccToParticipationParameter = parameters.getRawParameterValue ("ccToParticipation");
    ccPartMinParameter = parameters.getRawParameterValue ("ccPartMin");
    ccPartMaxParameter = parameters.getRawParameterValue ("ccPartMax");
    followConductorResponseParameter = parameters.getRawParameterValue ("followConductorResponse");
    responseAffectsInvertParameter = parameters.getRawParameterValue ("responseAffectsInvert");
    responseAffectsThresholdParameter = parameters.getRawParameterValue ("responseAffectsThreshold");
    responseAffectsParticipationParameter = parameters.getRawParameterValue ("responseAffectsParticipation");
    responseModeCcParameter = parameters.getRawParameterValue ("responseModeCc");
    responseAmountCcParameter = parameters.getRawParameterValue ("responseAmountCc");
        muteModeParameter = parameters.getRawParameterValue ("muteMode");
passKeyswitchesParameter = parameters.getRawParameterValue ("passKeyswitches");
    keyswitchMinParameter = parameters.getRawParameterValue ("keyswitchMin");
    keyswitchMaxParameter = parameters.getRawParameterValue ("keyswitchMax");
}

void OrchGateAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    juce::ignoreUnused (sampleRate, samplesPerBlock);

    for (auto& channelNotes : activeNotes)
        channelNotes.fill (false);

    // Closed until proven otherwise - see ccGateOpen's declaration. Every
    // transport start (prepareToPlay) re-arms this, so a stale "open" from a
    // previous session/run can never survive into a new one.
    ccGateOpen = false;
    lastCcValue.store (-1, std::memory_order_relaxed);
    lastResponseModeValue.store (-1, std::memory_order_relaxed);
    lastResponseAmountValue.store (-1, std::memory_order_relaxed);

    previousEffectiveGateOpen = this->getEffectiveGateOpen();
}

void OrchGateAudioProcessor::releaseResources()
{
}

bool OrchGateAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
    juce::ignoreUnused (layouts);
    return true;
}

void OrchGateAudioProcessor::processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    buffer.clear();

    const float manualParticipation = participationParameter != nullptr
        ? juce::jlimit (0.0f, 100.0f, participationParameter->load())
        : 100.0f;

    const bool ccToParticipation = ccToParticipationParameter != nullptr
        && ccToParticipationParameter->load() >= 0.5f;

    // The plugin's own literal settings, before the OrchConductor response
    // bridge (if any) is folded in.
    const float basePartFloor = ccPartMinParameter != nullptr
        ? juce::jlimit (0.0f, 100.0f, ccPartMinParameter->load()) : 0.0f;
    const float basePartCeil = ccPartMaxParameter != nullptr
        ? juce::jlimit (0.0f, 100.0f, ccPartMaxParameter->load()) : 100.0f;
    const bool baseInvert = ccInvertParameter != nullptr && ccInvertParameter->load() >= 0.5f;
    const int baseThreshold = ccThresholdParameter != nullptr
        ? juce::jlimit (0, 127, juce::roundToInt (ccThresholdParameter->load())) : 64;

    const int responseModeCc = responseModeCcParameter != nullptr
        ? juce::jlimit (0, 127, juce::roundToInt (responseModeCcParameter->load())) : 106;
    const int responseAmountCc = responseAmountCcParameter != nullptr
        ? juce::jlimit (0, 127, juce::roundToInt (responseAmountCcParameter->load())) : 107;

    // Non-const: a response-bridge CC (mode / amount) arriving mid-block
    // re-resolves this for the notes that follow it.
    ResponseOverlay overlay = resolveResponseOverlay (baseInvert, baseThreshold, basePartFloor, basePartCeil);

    auto participationForCc = [&] (int ccValue) -> float
    {
        if (ccValue < 0)
            return manualParticipation;   // no CC seen yet - fall back to the slider

        const float lo = juce::jmin (overlay.partFloor, overlay.partCeil);
        const float hi = juce::jmax (overlay.partFloor, overlay.partCeil);
        return juce::jlimit (0.0f, 100.0f, lo + (hi - lo) * static_cast<float> (ccValue) / 127.0f);
    };

    // Non-const: a matching CC arriving mid-block updates this for the notes
    // that follow it.
    float participation = ccToParticipation
        ? participationForCc (lastCcValue.load (std::memory_order_relaxed))
        : manualParticipation;

    juce::MidiBuffer output;

    for (const auto metadata : midiMessages)
    {
        const auto message = metadata.getMessage();
        const auto samplePosition = metadata.samplePosition;

        if (message.isController())
        {
            const int selectedCc = ccNumberParameter != nullptr
                ? juce::jlimit (0, 127, juce::roundToInt (ccNumberParameter->load()))
                : 20;

            if (message.getControllerNumber() == selectedCc)
            {
                const int value = juce::jlimit (0, 127, message.getControllerValue());

                // Effective threshold / invert = the plugin's own settings with
                // the OrchConductor response overlay folded in (identity when
                // the bridge is off or neutral).
                const int threshold = overlay.threshold;
                const bool inverted = overlay.invert;

                lastCcValue.store (value, std::memory_order_relaxed);

                if (ccToParticipation)
                    participation = participationForCc (value);

                const bool normalOpen = value >= threshold;
                ccGateOpen = inverted ? ! normalOpen : normalOpen;

                const bool nowOpen = this->getEffectiveGateOpen();

                // Deliberately NOT gated on "previousEffectiveGateOpen && !
                // nowOpen" (an edge trigger) here - live-rig bug 2026-09-10:
                // a note that reached the synth by some path this gate never
                // tracked (the Pass Keyswitches bypass below, a mid-stream
                // plugin reset, anything) leaves activeNotes/
                // previousEffectiveGateOpen believing the gate was already
                // closed, so an edge-triggered cleanup never fires for it -
                // "Send All Off" (which resends CC=0 unconditionally) landed
                // here and still did nothing. An explicit "closed" CC should
                // always mean closed, full stop, regardless of what this
                // gate's own bookkeeping previously believed.
                if (! nowOpen)
                {
                    const int muteMode = muteModeParameter != nullptr
                        ? juce::roundToInt (muteModeParameter->load())
                        : 0;

                    const bool hardGateMode = muteMode == 0;

                    if (hardGateMode)
                        this->closeGateSafely (output, samplePosition);
                }

                previousEffectiveGateOpen = nowOpen;
            }
            else if (message.getControllerNumber() == responseModeCc
                     || message.getControllerNumber() == responseAmountCc)
            {
                // An OrchConductor response-bridge CC. Store it, re-resolve the
                // overlay, and re-apply it to anything already in flight: the
                // participation curve, and the binary gate (a flipped invert or
                // a shifted threshold can change open/closed with no new gate
                // CC of its own).
                if (message.getControllerNumber() == responseModeCc)
                    lastResponseModeValue.store (juce::jlimit (0, 127, message.getControllerValue()),
                                                 std::memory_order_relaxed);
                else
                    lastResponseAmountValue.store (juce::jlimit (0, 127, message.getControllerValue()),
                                                   std::memory_order_relaxed);

                overlay = resolveResponseOverlay (baseInvert, baseThreshold, basePartFloor, basePartCeil);

                const int lastCc = lastCcValue.load (std::memory_order_relaxed);

                if (ccToParticipation)
                    participation = participationForCc (lastCc);

                if (lastCc >= 0)
                {
                    const bool normalOpen = lastCc >= overlay.threshold;
                    ccGateOpen = overlay.invert ? ! normalOpen : normalOpen;

                    const bool nowOpen = this->getEffectiveGateOpen();

                    if (! nowOpen && previousEffectiveGateOpen)
                    {
                        const int muteMode = muteModeParameter != nullptr
                            ? juce::roundToInt (muteModeParameter->load()) : 0;

                        if (muteMode == 0)
                            this->closeGateSafely (output, samplePosition);
                    }

                    previousEffectiveGateOpen = nowOpen;
                }
            }

            // Always pass CC through to the next instrument.
            output.addEvent (message, samplePosition);
            continue;
        }

        const bool passKeyswitches = passKeyswitchesParameter != nullptr
            && passKeyswitchesParameter->load() >= 0.5f;

        const int ksMinRaw = keyswitchMinParameter != nullptr
            ? juce::roundToInt (keyswitchMinParameter->load())
            : 0;

        const int ksMaxRaw = keyswitchMaxParameter != nullptr
            ? juce::roundToInt (keyswitchMaxParameter->load())
            : 35;

        const int ksMin = juce::jlimit (0, 127, juce::jmin (ksMinRaw, ksMaxRaw));
        const int ksMax = juce::jlimit (0, 127, juce::jmax (ksMinRaw, ksMaxRaw));

        if (passKeyswitches && (message.isNoteOn() || message.isNoteOff()))
        {
            const int noteNumber = juce::jlimit (0, 127, message.getNoteNumber());

            if (noteNumber >= ksMin && noteNumber <= ksMax)
            {
                output.addEvent (message, samplePosition);
                continue;
            }
        }

        const bool effectiveGateOpen = this->getEffectiveGateOpen();

        if (previousEffectiveGateOpen && ! effectiveGateOpen)
        {
            const int muteMode = muteModeParameter != nullptr
                ? juce::roundToInt (muteModeParameter->load())
                : 0;

            const bool hardGateMode = muteMode == 0;

            if (hardGateMode)
                this->closeGateSafely (output, samplePosition);
        }

        previousEffectiveGateOpen = effectiveGateOpen;

        if (message.isNoteOn())
        {
            const auto channel = juce::jlimit (1, 16, message.getChannel()) - 1;
            const auto note = juce::jlimit (0, 127, message.getNoteNumber());

            const bool passesParticipation =
                participation >= 100.0f
                || (participation > 0.0f && random.nextFloat() * 100.0f < participation);

            if (effectiveGateOpen && passesParticipation)
            {
                activeNotes[static_cast<size_t> (channel)][static_cast<size_t> (note)] = true;
                output.addEvent (message, samplePosition);
            }

            continue;
        }

        if (message.isNoteOff())
        {
            const auto channel = juce::jlimit (1, 16, message.getChannel()) - 1;
            const auto note = juce::jlimit (0, 127, message.getNoteNumber());

            if (activeNotes[static_cast<size_t> (channel)][static_cast<size_t> (note)])
            {
                activeNotes[static_cast<size_t> (channel)][static_cast<size_t> (note)] = false;
                output.addEvent (message, samplePosition);
            }

            continue;
        }

        // Always pass pitch bend, aftertouch, program change, clock, etc.
        output.addEvent (message, samplePosition);
    }

    midiMessages.swapWith (output);
}

juce::AudioProcessorEditor* OrchGateAudioProcessor::createEditor()
{
    return new OrchGateAudioProcessorEditor (*this);
}

bool OrchGateAudioProcessor::hasEditor() const
{
    return true;
}

const juce::String OrchGateAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool OrchGateAudioProcessor::acceptsMidi() const
{
    return true;
}

bool OrchGateAudioProcessor::producesMidi() const
{
    return true;
}

bool OrchGateAudioProcessor::isMidiEffect() const
{
    return true;
}

double OrchGateAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int OrchGateAudioProcessor::getNumPrograms()
{
    return 1;
}

int OrchGateAudioProcessor::getCurrentProgram()
{
    return 0;
}

void OrchGateAudioProcessor::setCurrentProgram (int index)
{
    juce::ignoreUnused (index);
}

const juce::String OrchGateAudioProcessor::getProgramName (int index)
{
    juce::ignoreUnused (index);
    return {};
}

void OrchGateAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
    juce::ignoreUnused (index, newName);
}

void OrchGateAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
    if (auto state = parameters.copyState(); state.isValid())
    {
        std::unique_ptr<juce::XmlElement> xml (state.createXml());

        if (xml != nullptr)
            copyXmlToBinary (*xml, destData);
    }
}

void OrchGateAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xml (getXmlFromBinary (data, sizeInBytes));

    if (xml != nullptr && xml->hasTagName (parameters.state.getType()))
        parameters.replaceState (juce::ValueTree::fromXml (*xml));

    for (auto& channelNotes : activeNotes)
        channelNotes.fill (false);

    // Same fail-safe-closed reasoning as prepareToPlay() - a restored/reloaded
    // project has no more basis for assuming the gate was left open than a
    // fresh instance does.
    ccGateOpen = false;
    lastCcValue.store (-1, std::memory_order_relaxed);
    lastResponseModeValue.store (-1, std::memory_order_relaxed);
    lastResponseAmountValue.store (-1, std::memory_order_relaxed);

    previousEffectiveGateOpen = this->getEffectiveGateOpen();
}

juce::AudioProcessorValueTreeState& OrchGateAudioProcessor::getParameters()
{
    return parameters;
}

juce::AudioProcessorValueTreeState::ParameterLayout OrchGateAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    params.push_back (std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "manualGate", 1 },
        "Manual Gate",
        true));

    params.push_back (std::make_unique<juce::AudioParameterFloat>(
        juce::ParameterID { "participation", 1 },
        "Participation",
        juce::NormalisableRange<float> (0.0f, 100.0f, 1.0f),
        100.0f,
        juce::AudioParameterFloatAttributes()
            .withLabel ("%")
            .withStringFromValueFunction ([] (float value, int)
            {
                return juce::String (juce::roundToInt (value)) + "%";
            })
            .withValueFromStringFunction ([] (const juce::String& text)
            {
                return text.retainCharacters ("0123456789.").getFloatValue();
            })));

    params.push_back (std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "ccGateEnable", 1 },
        "CC Gate Enable",
        false));

    params.push_back (std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID { "ccNumber", 1 },
        "CC Number",
        0,
        127,
        20));

    params.push_back (std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID { "ccThreshold", 1 },
        "CC Threshold",
        0,
        127,
        64));

    params.push_back (std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "ccInvert", 1 },
        "CC Invert",
        false));

    // CC -> Participation: independent of the binary CC gate. When on, the
    // incoming CC value is mapped across [floor, ceiling] % and used as the
    // per-note participation probability instead of the manual slider. Turn
    // the binary CC Gate off for a pure density curve, or leave both on for
    // "silent below threshold, then fade in from sparse to dense".
    params.push_back (std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "ccToParticipation", 1 },
        "CC -> Participation",
        false));

    params.push_back (std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID { "ccPartMin", 1 },
        "CC Participation Floor %",
        0, 100, 0));

    params.push_back (std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID { "ccPartMax", 1 },
        "CC Participation Ceiling %",
        0, 100, 100));

    // --- OrchConductor response bridge ------------------------------------
    // When "Follow Conductor Response" is on, two broadcast CCs from
    // OrchConductor (mode = 106, amount = 107 by default) drive a
    // deterministic per-instance overlay on top of the plugin's own CC
    // Invert / CC Threshold / CC Participation range. The seed mixes the
    // "mode" CC value with this instance's own gate CC number, so every
    // OrchGate in the rig diverges but they all shift together when the
    // conductor steps the mode, and identically after a project reload.
    // Default OFF - a standalone OrchGate (LFOs from the host, no
    // OrchConductor) is completely unaffected.
    params.push_back (std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "followConductorResponse", 1 },
        "Follow Conductor Response",
        false));

    params.push_back (std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "responseAffectsInvert", 1 },
        "Response Affects Invert",
        true));

    params.push_back (std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "responseAffectsThreshold", 1 },
        "Response Affects Threshold",
        true));

    params.push_back (std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "responseAffectsParticipation", 1 },
        "Response Affects Participation",
        true));

    params.push_back (std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID { "responseModeCc", 1 },
        "Response Mode CC",
        0, 127, 106));

    params.push_back (std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID { "responseAmountCc", 1 },
        "Response Amount CC",
        0, 127, 107));

    params.push_back (std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID { "muteMode", 1 },
        "Mute Mode",
        juce::StringArray { "Hard Gate", "No New Notes" },
        0));
    params.push_back (std::make_unique<juce::AudioParameterBool>(
        juce::ParameterID { "passKeyswitches", 1 },
        "Pass Keyswitches",
        true));

    params.push_back (std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID { "keyswitchMin", 1 },
        "Keyswitch Min",
        0,
        127,
        0));

    params.push_back (std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID { "keyswitchMax", 1 },
        "Keyswitch Max",
        0,
        127,
        35));


    return { params.begin(), params.end() };
}

void OrchGateAudioProcessor::closeGateSafely (juce::MidiBuffer& outputBuffer, int samplePosition)
{
    for (int channel = 0; channel < 16; ++channel)
    {
        for (int note = 0; note < 128; ++note)
        {
            if (activeNotes[static_cast<size_t> (channel)][static_cast<size_t> (note)])
            {
                outputBuffer.addEvent (juce::MidiMessage::noteOff (channel + 1, note), samplePosition);
                activeNotes[static_cast<size_t> (channel)][static_cast<size_t> (note)] = false;
            }
        }
    }

    sendAllNotesOff (outputBuffer, samplePosition);
}

bool OrchGateAudioProcessor::getEffectiveGateOpen() const
{
    const bool manualGateOpen = manualGateParameter != nullptr && manualGateParameter->load() >= 0.5f;
    const bool ccGateEnabled = ccGateEnableParameter != nullptr && ccGateEnableParameter->load() >= 0.5f;

    return manualGateOpen && (! ccGateEnabled || ccGateOpen);
}

OrchGateAudioProcessor::ResponseOverlay
OrchGateAudioProcessor::resolveResponseOverlay (bool baseInvert, int baseThreshold,
                                                float basePartFloor, float basePartCeil) const
{
    ResponseOverlay o;
    o.invert     = baseInvert;
    o.threshold  = baseThreshold;
    o.partFloor  = basePartFloor;
    o.partCeil   = basePartCeil;

    const bool follow = followConductorResponseParameter != nullptr
        && followConductorResponseParameter->load() >= 0.5f;

    if (! follow)
        return o;

    const int mode = lastResponseModeValue.load (std::memory_order_relaxed);

    if (mode < 0)
        return o;   // no response "chapter" has arrived - obey the literal settings

    const int amountRaw = lastResponseAmountValue.load (std::memory_order_relaxed);
    const float amount = juce::jlimit (0.0f, 1.0f,
        static_cast<float> (amountRaw >= 0 ? amountRaw : 0) / 127.0f);

    if (amount <= 0.0f)
        return o;   // amount 0 = neutral overlay (also the "Send All Off" state)

    const int instanceKey = ccNumberParameter != nullptr
        ? juce::jlimit (0, 127, juce::roundToInt (ccNumberParameter->load()))
        : 20;

    auto hash = [] (int a, int b, int salt) -> juce::uint32
    {
        juce::uint32 h = 2166136261u;

        for (int v : { a, b, salt, 0x27d4eb2f })
        {
            h ^= static_cast<juce::uint32> (v & 0xff);          h *= 16777619u;
            h ^= static_cast<juce::uint32> ((v >> 8) & 0xff);   h *= 16777619u;
            h ^= static_cast<juce::uint32> ((v >> 16) & 0xff);  h *= 16777619u;
        }

        h ^= h >> 15; h *= 2246822519u;
        h ^= h >> 13; h *= 3266489917u;
        h ^= h >> 16;
        return h;
    };

    auto unitFor = [&] (int salt)
    {
        return static_cast<float> (hash (mode, instanceKey, salt) & 0xffffffu)
             / static_cast<float> (0xffffff);
    };

    auto signedDelta = [&] (int salt, int span)
    {
        if (span <= 0)
            return 0;

        return static_cast<int> (hash (mode, instanceKey, salt)
                                 % static_cast<juce::uint32> (2 * span + 1)) - span;
    };

    // --- Invert: at most half the rig flips at full amount ----------------
    if (responseAffectsInvertParameter != nullptr && responseAffectsInvertParameter->load() >= 0.5f)
    {
        const bool bridgeFlip = unitFor (1) < amount * 0.5f;

        if (bridgeFlip)
        {
            o.invert = ! baseInvert;
            o.active = true;
        }
    }

    // --- Threshold: +/- up to 24 around the user's value -----------------
    if (responseAffectsThresholdParameter != nullptr && responseAffectsThresholdParameter->load() >= 0.5f)
    {
        const int delta = signedDelta (2, juce::roundToInt (amount * 24.0f));

        if (delta != 0)
        {
            o.threshold = juce::jlimit (0, 127, baseThreshold + delta);
            o.active = true;
        }
    }

    // --- Participation floor / ceiling: +/- up to 20% each --------------
    if (responseAffectsParticipationParameter != nullptr && responseAffectsParticipationParameter->load() >= 0.5f)
    {
        const int span = juce::roundToInt (amount * 20.0f);
        const int fDelta = signedDelta (3, span);
        const int cDelta = signedDelta (4, span);

        if (fDelta != 0 || cDelta != 0)
        {
            o.partFloor = juce::jlimit (0.0f, 100.0f, basePartFloor + static_cast<float> (fDelta));
            o.partCeil  = juce::jlimit (0.0f, 100.0f, basePartCeil  + static_cast<float> (cDelta));
            o.active = true;
        }
    }

    return o;
}

bool OrchGateAudioProcessor::isEffectiveGateOpenForUi() const
{
    return getEffectiveGateOpen();
}

bool OrchGateAudioProcessor::isManualGateOpenForUi() const
{
    return manualGateParameter != nullptr && manualGateParameter->load() >= 0.5f;
}

bool OrchGateAudioProcessor::isCcGateEnabledForUi() const
{
    return ccGateEnableParameter != nullptr && ccGateEnableParameter->load() >= 0.5f;
}

bool OrchGateAudioProcessor::isCcGateOpenForUi() const
{
    return ccGateOpen;
}

int OrchGateAudioProcessor::getLastCcValueForUi() const
{
    return lastCcValue.load (std::memory_order_relaxed);
}

float OrchGateAudioProcessor::getEffectiveParticipationForUi() const
{
    const float manual = participationParameter != nullptr
        ? juce::jlimit (0.0f, 100.0f, participationParameter->load()) : 100.0f;

    const bool ccToPart = ccToParticipationParameter != nullptr
        && ccToParticipationParameter->load() >= 0.5f;

    const int v = lastCcValue.load (std::memory_order_relaxed);

    if (! ccToPart || v < 0)
        return manual;

    const float baseFloor = ccPartMinParameter != nullptr ? ccPartMinParameter->load() : 0.0f;
    const float baseCeil  = ccPartMaxParameter != nullptr ? ccPartMaxParameter->load() : 100.0f;
    const bool  baseInvert = ccInvertParameter != nullptr && ccInvertParameter->load() >= 0.5f;
    const int   baseThreshold = ccThresholdParameter != nullptr
        ? juce::jlimit (0, 127, juce::roundToInt (ccThresholdParameter->load())) : 64;

    const auto overlay = resolveResponseOverlay (baseInvert, baseThreshold, baseFloor, baseCeil);

    const float lo = juce::jmin (overlay.partFloor, overlay.partCeil);
    const float hi = juce::jmax (overlay.partFloor, overlay.partCeil);

    return juce::jlimit (0.0f, 100.0f, lo + (hi - lo) * static_cast<float> (v) / 127.0f);
}

bool OrchGateAudioProcessor::isFollowingConductorResponseForUi() const
{
    return followConductorResponseParameter != nullptr
        && followConductorResponseParameter->load() >= 0.5f;
}

int OrchGateAudioProcessor::getLastResponseModeValueForUi() const
{
    return lastResponseModeValue.load (std::memory_order_relaxed);
}

int OrchGateAudioProcessor::getLastResponseAmountValueForUi() const
{
    return lastResponseAmountValue.load (std::memory_order_relaxed);
}

juce::String OrchGateAudioProcessor::getResponseOverlaySummaryForUi() const
{
    if (! isFollowingConductorResponseForUi())
        return "Bridge off";

    const int mode = lastResponseModeValue.load (std::memory_order_relaxed);

    if (mode < 0)
    {
        const int cc = responseModeCcParameter != nullptr
            ? juce::roundToInt (responseModeCcParameter->load()) : 106;
        return "Bridge armed - waiting for CC" + juce::String (cc);
    }

    const float baseFloor = ccPartMinParameter != nullptr ? ccPartMinParameter->load() : 0.0f;
    const float baseCeil  = ccPartMaxParameter != nullptr ? ccPartMaxParameter->load() : 100.0f;
    const bool  baseInvert = ccInvertParameter != nullptr && ccInvertParameter->load() >= 0.5f;
    const int   baseThreshold = ccThresholdParameter != nullptr
        ? juce::jlimit (0, 127, juce::roundToInt (ccThresholdParameter->load())) : 64;

    const auto o = resolveResponseOverlay (baseInvert, baseThreshold, baseFloor, baseCeil);

    if (! o.active)
        return "Bridge M" + juce::String (mode) + " - neutral here";

    juce::StringArray parts;

    if (o.invert != baseInvert)
        parts.add (juce::String ("Inv ") + (o.invert ? "on" : "off"));

    if (o.threshold != baseThreshold)
        parts.add ("Th " + juce::String (baseThreshold) + ">" + juce::String (o.threshold));

    if (o.partFloor != baseFloor || o.partCeil != baseCeil)
        parts.add ("Part " + juce::String (juce::roundToInt (baseFloor)) + "-" + juce::String (juce::roundToInt (baseCeil))
                   + ">" + juce::String (juce::roundToInt (o.partFloor)) + "-" + juce::String (juce::roundToInt (o.partCeil)));

    return "Bridge M" + juce::String (mode) + ": " + parts.joinIntoString ("  ");
}
void OrchGateAudioProcessor::sendAllNotesOff (juce::MidiBuffer& outputBuffer, int samplePosition)
{
    for (int channel = 0; channel < 16; ++channel)
    {
        for (int note = 0; note < 128; ++note)
        {
            if (activeNotes[static_cast<size_t> (channel)][static_cast<size_t> (note)])
            {
                outputBuffer.addEvent (juce::MidiMessage::noteOff (channel + 1, note), samplePosition);
                activeNotes[static_cast<size_t> (channel)][static_cast<size_t> (note)] = false;
            }
        }

        outputBuffer.addEvent (juce::MidiMessage::allNotesOff (channel + 1), samplePosition);
    }
}
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new OrchGateAudioProcessor();
}






















