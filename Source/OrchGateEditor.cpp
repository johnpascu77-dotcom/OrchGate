#include "OrchGateEditor.h"

OrchGateAudioProcessorEditor::OrchGateAudioProcessorEditor (OrchGateAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setSize (640, 720);

    titleLabel.setText ("OrchGate", juce::dontSendNotification);
    titleLabel.setJustificationType (juce::Justification::centred);
    titleLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    titleLabel.setFont (juce::FontOptions (30.0f, juce::Font::bold));
    addAndMakeVisible (titleLabel);

    subtitleLabel.setText ("Orchestral Participation Gate", juce::dontSendNotification);
    subtitleLabel.setJustificationType (juce::Justification::centred);
    subtitleLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    subtitleLabel.setFont (juce::FontOptions (15.0f));
    addAndMakeVisible (subtitleLabel);

    buildLabel.setText ("Build: Phase 9D", juce::dontSendNotification);
    buildLabel.setJustificationType (juce::Justification::centred);
    buildLabel.setColour (juce::Label::textColourId, juce::Colour::fromRGB (140, 160, 180));
    buildLabel.setFont (juce::FontOptions (12.0f));
    addAndMakeVisible (buildLabel);

    manualGateButton.setButtonText ("Manual Gate");
    manualGateButton.setColour (juce::ToggleButton::textColourId, juce::Colours::white);
    addAndMakeVisible (manualGateButton);

    participationLabel.setText ("Participation: 100%", juce::dontSendNotification);
    participationLabel.setJustificationType (juce::Justification::centred);
    participationLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    participationLabel.setFont (juce::FontOptions (14.0f));
    addAndMakeVisible (participationLabel);

    participationSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    participationSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 72, 24);
    participationSlider.setRange (0.0, 100.0, 1.0);
    participationSlider.setColour (juce::Slider::thumbColourId, juce::Colour::fromRGB (95, 220, 140));
    participationSlider.setColour (juce::Slider::trackColourId, juce::Colour::fromRGB (95, 200, 245));
    participationSlider.setColour (juce::Slider::textBoxTextColourId, juce::Colours::white);
    participationSlider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour::fromRGB (28, 36, 46));
    addAndMakeVisible (participationSlider);

    ccSummaryLabel.setText ("CC Gate: Off | CC 20 | Threshold 64 | Normal", juce::dontSendNotification);

    muteModeLabel.setText ("Mute Mode", juce::dontSendNotification);
    muteModeLabel.setJustificationType (juce::Justification::centred);
    muteModeLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    muteModeLabel.setFont (juce::FontOptions (13.0f));
    addAndMakeVisible (muteModeLabel);

    muteModeBox.addItem ("Hard Gate", 1);
    muteModeBox.addItem ("No New Notes", 2);
    muteModeBox.setColour (juce::ComboBox::backgroundColourId, juce::Colour::fromRGB (28, 36, 46));
    muteModeBox.setColour (juce::ComboBox::textColourId, juce::Colours::white);
    muteModeBox.setColour (juce::ComboBox::outlineColourId, juce::Colour::fromRGB (120, 135, 150));
    addAndMakeVisible (muteModeBox);

    ccSummaryLabel.setJustificationType (juce::Justification::centred);
    ccSummaryLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    ccSummaryLabel.setFont (juce::FontOptions (13.0f));
    addAndMakeVisible (ccSummaryLabel);

    ccGateEnableButton.setButtonText ("CC Gate");
    ccGateEnableButton.setColour (juce::ToggleButton::textColourId, juce::Colours::white);
    addAndMakeVisible (ccGateEnableButton);

    ccInvertButton.setButtonText ("Invert CC");
    ccInvertButton.setColour (juce::ToggleButton::textColourId, juce::Colours::white);
    addAndMakeVisible (ccInvertButton);

    ccNumberLabel.setText ("CC Number", juce::dontSendNotification);
    ccNumberLabel.setJustificationType (juce::Justification::centred);
    ccNumberLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    ccNumberLabel.setFont (juce::FontOptions (13.0f));
    addAndMakeVisible (ccNumberLabel);

    ccNumberSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    ccNumberSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 56, 22);
    ccNumberSlider.setRange (0.0, 127.0, 1.0);
    ccNumberSlider.setColour (juce::Slider::thumbColourId, juce::Colour::fromRGB (95, 220, 140));
    ccNumberSlider.setColour (juce::Slider::trackColourId, juce::Colour::fromRGB (95, 200, 245));
    ccNumberSlider.setColour (juce::Slider::textBoxTextColourId, juce::Colours::white);
    ccNumberSlider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour::fromRGB (28, 36, 46));
    addAndMakeVisible (ccNumberSlider);

    ccThresholdLabel.setText ("CC Threshold", juce::dontSendNotification);
    ccThresholdLabel.setJustificationType (juce::Justification::centred);
    ccThresholdLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    ccThresholdLabel.setFont (juce::FontOptions (13.0f));
    addAndMakeVisible (ccThresholdLabel);

    ccThresholdSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    ccThresholdSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 56, 22);
    ccThresholdSlider.setRange (0.0, 127.0, 1.0);
    ccThresholdSlider.setColour (juce::Slider::thumbColourId, juce::Colour::fromRGB (95, 220, 140));
    ccThresholdSlider.setColour (juce::Slider::trackColourId, juce::Colour::fromRGB (95, 200, 245));
    ccThresholdSlider.setColour (juce::Slider::textBoxTextColourId, juce::Colours::white);
    ccThresholdSlider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour::fromRGB (28, 36, 46));
    addAndMakeVisible (ccThresholdSlider);


    passKeyswitchesButton.setButtonText ("Pass Keyswitches");
    passKeyswitchesButton.setColour (juce::ToggleButton::textColourId, juce::Colours::white);
    addAndMakeVisible (passKeyswitchesButton);

    keyswitchMinLabel.setText ("KS Min", juce::dontSendNotification);
    keyswitchMinLabel.setJustificationType (juce::Justification::centred);
    keyswitchMinLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    keyswitchMinLabel.setFont (juce::FontOptions (13.0f));
    addAndMakeVisible (keyswitchMinLabel);

    keyswitchMinSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    keyswitchMinSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 56, 22);
    keyswitchMinSlider.setRange (0.0, 127.0, 1.0);
    keyswitchMinSlider.setColour (juce::Slider::thumbColourId, juce::Colour::fromRGB (95, 220, 140));
    keyswitchMinSlider.setColour (juce::Slider::trackColourId, juce::Colour::fromRGB (95, 200, 245));
    keyswitchMinSlider.setColour (juce::Slider::textBoxTextColourId, juce::Colours::white);
    keyswitchMinSlider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour::fromRGB (28, 36, 46));
    addAndMakeVisible (keyswitchMinSlider);

    keyswitchMaxLabel.setText ("KS Max", juce::dontSendNotification);
    keyswitchMaxLabel.setJustificationType (juce::Justification::centred);
    keyswitchMaxLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    keyswitchMaxLabel.setFont (juce::FontOptions (13.0f));
    addAndMakeVisible (keyswitchMaxLabel);

    keyswitchMaxSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    keyswitchMaxSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 56, 22);
    keyswitchMaxSlider.setRange (0.0, 127.0, 1.0);
    keyswitchMaxSlider.setColour (juce::Slider::thumbColourId, juce::Colour::fromRGB (95, 220, 140));
    keyswitchMaxSlider.setColour (juce::Slider::trackColourId, juce::Colour::fromRGB (95, 200, 245));
    keyswitchMaxSlider.setColour (juce::Slider::textBoxTextColourId, juce::Colours::white);
    keyswitchMaxSlider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour::fromRGB (28, 36, 46));
    addAndMakeVisible (keyswitchMaxSlider);

    gateStatusLabel.setJustificationType (juce::Justification::centred);
    gateStatusLabel.setFont (juce::FontOptions (20.0f, juce::Font::bold));
    addAndMakeVisible (gateStatusLabel);

    manualGateAttachment = std::make_unique<ButtonAttachment> (
        audioProcessor.getParameters(),
        "manualGate",
        manualGateButton);

    participationAttachment = std::make_unique<SliderAttachment> (
        audioProcessor.getParameters(),
        "participation",
        participationSlider);
    muteModeAttachment = std::make_unique<ComboBoxAttachment> (
        audioProcessor.getParameters(),
        "muteMode",
        muteModeBox);

    ccGateEnableAttachment = std::make_unique<ButtonAttachment> (
        audioProcessor.getParameters(),
        "ccGateEnable",
        ccGateEnableButton);

    ccInvertAttachment = std::make_unique<ButtonAttachment> (
        audioProcessor.getParameters(),
        "ccInvert",
        ccInvertButton);

    ccNumberAttachment = std::make_unique<SliderAttachment> (
        audioProcessor.getParameters(),
        "ccNumber",
        ccNumberSlider);

    ccThresholdAttachment = std::make_unique<SliderAttachment> (
        audioProcessor.getParameters(),
        "ccThreshold",
        ccThresholdSlider);
    passKeyswitchesAttachment = std::make_unique<ButtonAttachment> (
        audioProcessor.getParameters(),
        "passKeyswitches",
        passKeyswitchesButton);

    keyswitchMinAttachment = std::make_unique<SliderAttachment> (
        audioProcessor.getParameters(),
        "keyswitchMin",
        keyswitchMinSlider);

    keyswitchMaxAttachment = std::make_unique<SliderAttachment> (
        audioProcessor.getParameters(),
        "keyswitchMax",
        keyswitchMaxSlider);

    startTimerHz (15);
    timerCallback();
}


OrchGateAudioProcessorEditor::~OrchGateAudioProcessorEditor()
{
    stopTimer();
}
void OrchGateAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll (juce::Colour::fromRGB (20, 26, 34));

    auto bounds = getLocalBounds().toFloat().reduced (3.0f);

    g.setColour (juce::Colour::fromRGB (95, 200, 245));
    g.drawRoundedRectangle (bounds, 10.0f, 2.0f);
}

void OrchGateAudioProcessorEditor::resized()
{
    auto area = getLocalBounds().reduced (28);

    titleLabel.setBounds (area.removeFromTop (42));
    subtitleLabel.setBounds (area.removeFromTop (24));
    buildLabel.setBounds (area.removeFromTop (18));

    area.removeFromTop (30);

    manualGateButton.setBounds (area.removeFromTop (36).withSizeKeepingCentre (180, 36));

    area.removeFromTop (18);

    participationLabel.setBounds (area.removeFromTop (24));
    participationSlider.setBounds (area.removeFromTop (38).withSizeKeepingCentre (360, 38));

    area.removeFromTop (14);

        muteModeLabel.setBounds (area.removeFromTop (20));
    muteModeBox.setBounds (area.removeFromTop (28).withSizeKeepingCentre (220, 28));

    area.removeFromTop (12);
    ccSummaryLabel.setBounds (area.removeFromTop (22));

    auto ccButtonRow = area.removeFromTop (30).withSizeKeepingCentre (360, 30);
    ccGateEnableButton.setBounds (ccButtonRow.removeFromLeft (180));
    ccInvertButton.setBounds (ccButtonRow.removeFromLeft (180));

    ccNumberLabel.setBounds (area.removeFromTop (20));
    ccNumberSlider.setBounds (area.removeFromTop (30).withSizeKeepingCentre (440, 30));

    ccThresholdLabel.setBounds (area.removeFromTop (20));
    ccThresholdSlider.setBounds (area.removeFromTop (30).withSizeKeepingCentre (440, 30));

    area.removeFromTop (12);

    passKeyswitchesButton.setBounds (area.removeFromTop (30).withSizeKeepingCentre (220, 30));

    keyswitchMinLabel.setBounds (area.removeFromTop (20));
    keyswitchMinSlider.setBounds (area.removeFromTop (30).withSizeKeepingCentre (440, 30));

    keyswitchMaxLabel.setBounds (area.removeFromTop (20));
    keyswitchMaxSlider.setBounds (area.removeFromTop (30).withSizeKeepingCentre (440, 30));

    area.removeFromTop (10);

    gateStatusLabel.setBounds (area.removeFromTop (46));
}

void OrchGateAudioProcessorEditor::timerCallback()
{
    auto* participationParam = audioProcessor.getParameters().getRawParameterValue ("participation");
    auto* muteModeParam = audioProcessor.getParameters().getRawParameterValue ("muteMode");
    auto* ccGateEnableParam = audioProcessor.getParameters().getRawParameterValue ("ccGateEnable");
    auto* ccNumberParam = audioProcessor.getParameters().getRawParameterValue ("ccNumber");
    auto* ccThresholdParam = audioProcessor.getParameters().getRawParameterValue ("ccThreshold");
    auto* ccInvertParam = audioProcessor.getParameters().getRawParameterValue ("ccInvert");
    auto* passKeyswitchesParam = audioProcessor.getParameters().getRawParameterValue ("passKeyswitches");
    auto* keyswitchMinParam = audioProcessor.getParameters().getRawParameterValue ("keyswitchMin");
    auto* keyswitchMaxParam = audioProcessor.getParameters().getRawParameterValue ("keyswitchMax");

    const int participationPercent = participationParam != nullptr
        ? juce::roundToInt (participationParam->load())
        : 100;

    const bool ccGateEnabled = ccGateEnableParam != nullptr && ccGateEnableParam->load() >= 0.5f;
    const int ccNumber = ccNumberParam != nullptr ? juce::roundToInt (ccNumberParam->load()) : 20;
    const int ccThreshold = ccThresholdParam != nullptr ? juce::roundToInt (ccThresholdParam->load()) : 64;
    const bool ccInvert = ccInvertParam != nullptr && ccInvertParam->load() >= 0.5f;

    const bool passKeyswitchesUi = passKeyswitchesParam != nullptr && passKeyswitchesParam->load() >= 0.5f;
    const int keyswitchMinUi = keyswitchMinParam != nullptr ? juce::roundToInt (keyswitchMinParam->load()) : 0;
    const int keyswitchMaxUi = keyswitchMaxParam != nullptr ? juce::roundToInt (keyswitchMaxParam->load()) : 35;
    const int muteModeUi = muteModeParam != nullptr ? juce::roundToInt (muteModeParam->load()) : 0;
    const juce::String muteModeText = muteModeUi == 0 ? "Hard" : "No New";

    participationLabel.setText ("Participation: " + juce::String (participationPercent) + "%", juce::dontSendNotification);

    ccSummaryLabel.setText (
        muteModeText
        + " | CC " + juce::String (ccGateEnabled ? "On" : "Off")
        + " | CC" + juce::String (ccNumber)
        + " | Th" + juce::String (ccThreshold)
        + " | " + juce::String (ccInvert ? "Inv" : "Norm")
        + " | KS " + juce::String (passKeyswitchesUi ? "" : "Off ")
        + juce::String (juce::jmin (keyswitchMinUi, keyswitchMaxUi))
        + "-" + juce::String (juce::jmax (keyswitchMinUi, keyswitchMaxUi)),
        juce::dontSendNotification);

    const bool effectiveOpen = audioProcessor.isEffectiveGateOpenForUi();
    const bool manualOpen = audioProcessor.isManualGateOpenForUi();
    const bool ccOpen = audioProcessor.isCcGateOpenForUi();
    const int lastCcValue = audioProcessor.getLastCcValueForUi();

    juce::String statusText;

    if (! manualOpen)
    {
        statusText = "CLOSED BY MANUAL GATE";
    }
    else if (ccGateEnabled && lastCcValue < 0)
    {
        statusText = "WAITING FOR CC" + juce::String (ccNumber);
    }
    else if (ccGateEnabled && ! ccOpen)
    {
        const juce::String comparison = ccInvert ? " >= " : " < ";

        statusText = juce::String (muteModeUi == 1 ? "CLOSED TO NEW NOTES: CC" : "CLOSED BY CC")
                   + juce::String (ccNumber)
                   + " = " + juce::String (lastCcValue)
                   + comparison + juce::String (ccThreshold);
    }
    else if (ccGateEnabled && ccOpen)
    {
        const juce::String comparison = ccInvert ? " < " : " >= ";

        statusText = "EFFECTIVE GATE OPEN: CC" + juce::String (ccNumber)
                   + " = " + juce::String (lastCcValue)
                   + comparison + juce::String (ccThreshold);
    }
    else
    {
        statusText = "EFFECTIVE GATE OPEN";
    }

    gateStatusLabel.setText (statusText, juce::dontSendNotification);
    gateStatusLabel.setColour (
        juce::Label::textColourId,
        effectiveOpen ? juce::Colour::fromRGB (95, 220, 140)
                      : juce::Colour::fromRGB (245, 120, 120));
}































