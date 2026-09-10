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
    float getEffectiveParticipationForUi() const;

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

    std::array<std::array<bool, 128>, 16> activeNotes {};

    bool getEffectiveGateOpen() const;
    void closeGateSafely (juce::MidiBuffer& outputBuffer, int samplePosition);
    void sendAllNotesOff (juce::MidiBuffer& outputBuffer, int samplePosition);

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OrchGateAudioProcessor)
};











