#pragma once

#include <JuceHeader.h>
#include "OrchGateProcessor.h"

class OrchGateAudioProcessorEditor : public juce::AudioProcessorEditor,
                                     private juce::Timer
{
public:
    explicit OrchGateAudioProcessorEditor (OrchGateAudioProcessor&);
    ~OrchGateAudioProcessorEditor() override;

    void paint (juce::Graphics&) override;
    void resized() override;

private:
    void timerCallback() override;
    void updateLabels();

    OrchGateAudioProcessor& audioProcessor;

    juce::Label titleLabel;
    juce::Label subtitleLabel;
    juce::Label buildLabel;

    juce::ToggleButton manualGateButton;
    juce::Label participationLabel;
    juce::Slider participationSlider;

        juce::Label muteModeLabel;
    juce::ComboBox muteModeBox;

juce::Label ccSummaryLabel;
    juce::ToggleButton ccGateEnableButton;
    juce::ToggleButton ccInvertButton;

    juce::Label ccNumberLabel;
    juce::Slider ccNumberSlider;

    juce::Label ccThresholdLabel;
    juce::Slider ccThresholdSlider;

    juce::ToggleButton ccToParticipationButton;
    juce::Label ccPartRangeLabel;
    juce::Slider ccPartMinSlider;
    juce::Slider ccPartMaxSlider;

        juce::ToggleButton passKeyswitchesButton;
    juce::Label keyswitchMinLabel;
    juce::Slider keyswitchMinSlider;
    juce::Label keyswitchMaxLabel;
    juce::Slider keyswitchMaxSlider;

juce::Label gateStatusLabel;

    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    std::unique_ptr<ButtonAttachment> manualGateAttachment;
    std::unique_ptr<SliderAttachment> participationAttachment;
    std::unique_ptr<ComboBoxAttachment> muteModeAttachment;
    std::unique_ptr<ButtonAttachment> ccGateEnableAttachment;
    std::unique_ptr<SliderAttachment> ccNumberAttachment;
    std::unique_ptr<SliderAttachment> ccThresholdAttachment;
    std::unique_ptr<ButtonAttachment> ccInvertAttachment;
    std::unique_ptr<ButtonAttachment> ccToParticipationAttachment;
    std::unique_ptr<SliderAttachment> ccPartMinAttachment;
    std::unique_ptr<SliderAttachment> ccPartMaxAttachment;
    std::unique_ptr<ButtonAttachment> passKeyswitchesAttachment;
    std::unique_ptr<SliderAttachment> keyswitchMinAttachment;
    std::unique_ptr<SliderAttachment> keyswitchMaxAttachment;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (OrchGateAudioProcessorEditor)
};


