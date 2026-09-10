#pragma once


#include <atomic>
#include <JuceHeader.h>

class OrchGateAudioProcessor final : public juce::AudioProcessor
{
public:
    OrchGateAudioProcessor();
    ~OrchGateAudioProcessor() override = default;

    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;

    void processBlock (juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const juce::String getProgramName (int index) override;
    void changeProgramName (int index, const juce::String& newName) override;

    void getStateInformation (juce::MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

        bool isEffectiveGateOpenForUi() const;
    bool isManualGateOpenForUi() const;
    bool isCcGateEnabledForUi() const;
    bool isCcGateOpenForUi() const;
    int getLastCcValueForUi() const;
    // The participation % actually in effect: the CC-derived value when
    // "CC -> Participation" is on and a CC has been seen, else the slider.
    // Includes the Conductor-response overlay when "Follow Conductor Response"
    // is on.
    float getEffectiveParticipationForUi() const;

    // --- Conductor response bridge (see resolveResponseOverlay) -------------
    bool isFollowingConductorResponseForUi() const;
    int getLastResponseModeValueForUi() const;
    int getLastResponseAmountValueForUi() const;
    // A compact human-readable description of what the overlay is doing right
    // now (e.g. "Inv flip | Th 64>58 | Part 0-100>5-91"), or an idle note.
    juce::String getResponseOverlaySummaryForUi() const;

    // The per-instance result of the OrchConductor response bridge. Every
    // field starts equal to the plugin's own literal setting; when the bridge
    // is active a deterministic per-instance offset (seeded by the response
    // "mode" CC value and this instance's own gate CC number) is folded in,
    // scaled by the response "amount" CC. Same mode value + same instance =
    // same overlay, always - no drift across reloads.
    struct ResponseOverlay
    {
        bool  invert    = false;
        int   threshold = 64;
        float partFloor = 0.0f;
        float partCeil  = 100.0f;
        bool  active    = false;   // true only when the bridge actually moved something
    };

juce::AudioProcessorValueTreeState& getParameters();

    static juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();

private:
    juce::AudioProcessorValueTreeState parameters;

    std::atomic<float>* manualGateParameter = nullptr;
    std::atomic<float>* participationParameter = nullptr;
    std::atomic<float>* ccGateEnableParameter = nullptr;
    std::atomic<float>* ccNumberParameter = nullptr;
    std::atomic<float>* ccThresholdParameter = nullptr;
    std::atomic<float>* ccInvertParameter = nullptr;
    std::atomic<float>* ccToParticipationParameter = nullptr;
    std::atomic<float>* ccPartMinParameter = nullptr;
    std::atomic<float>* ccPartMaxParameter = nullptr;

    std::atomic<float>* followConductorResponseParameter = nullptr;
    std::atomic<float>* responseAffectsInvertParameter = nullptr;
    std::atomic<float>* responseAffectsThresholdParameter = nullptr;
    std::atomic<float>* responseAffectsParticipationParameter = nullptr;
    std::atomic<float>* responseModeCcParameter = nullptr;
    std::atomic<float>* responseAmountCcParameter = nullptr;

        std::atomic<float>* muteModeParameter = nullptr;

std::atomic<float>* passKeyswitchesParameter = nullptr;
    std::atomic<float>* keyswitchMinParameter = nullptr;
    std::atomic<float>* keyswitchMaxParameter = nullptr;

    juce::Random random;

    // Fail-safe default: closed, not open. Until this instance's selected CC
    // has actually arrived at least once (from OrchConductor, directly or via
    // OrchPercMapper's Arbiter), there is no basis for assuming participation
    // was ever granted - "silence until told otherwise" is the correct
    // default for a plugin whose entire purpose is conductor-driven gating.
    // Was `true` (fail-open): live-rig bug 2026-09-10, an extra MIDI-FX hop
    // (the Arbiter) made the corrective CC late or dropped from Bitwig's
    // point of view often enough that percussion tracks played through
    // unconditionally until manually "woken up" by chance.
    bool ccGateOpen = false;
    std::atomic<int> lastCcValue { -1 };
    bool previousEffectiveGateOpen = false;

    // Last seen values of the two OrchConductor response-bridge CCs. -1 = the
    // corresponding CC has not arrived since this instance last (re)started,
    // in which case the bridge stays neutral and the plugin obeys its own
    // literal settings.
    std::atomic<int> lastResponseModeValue { -1 };
    std::atomic<int> lastResponseAmountValue { -1 };

    std::array<std::array<bool, 128>, 16> activeNotes {};

    bool getEffectiveGateOpen() const;
    ResponseOverlay resolveResponseOverlay (bool baseInvert, int baseThreshold,
                                            float basePartFloor, float basePartCeil) const;
    void closeGateSafely (juce::MidiBuffer& outputBuffer, int samplePosition);
    void sendAllNotesOff (juce::MidiBuffer& outputBuffer, int samplePosition);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OrchGateAudioProcessor)
};











