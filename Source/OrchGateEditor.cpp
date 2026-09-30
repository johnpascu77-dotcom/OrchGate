#include "OrchGateEditor.h"
#include "OrchGateBuildInfo.h"

OrchGateAudioProcessorEditor::OrchGateAudioProcessorEditor (OrchGateAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    // Two-column layout (see resized()) instead of the old single tall
    // column - the whole point being that every control fits in the window
    // at once, no scrolling/clipping. Resizable too, since a fixed size
    // was part of the original complaint.
    setResizable (true, true);
    setResizeLimits (620, 460, 1000, 760);
    setSize (700, 560);

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

    // orchGateBuildTimestamp is regenerated on every single build (see
    // cmake/GenerateOrchGateBuildInfo.cmake) - a hand-maintained phase tag
    // here can't answer "is this actually the build I just installed", a
    // fresh timestamp always can.
    buildLabel.setText (juce::String ("Build: ") + orchGateBuildTimestamp, juce::dontSendNotification);
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

    ccToParticipationButton.setButtonText ("CC -> Participation");
    ccToParticipationButton.setColour (juce::ToggleButton::textColourId, juce::Colours::white);
    addAndMakeVisible (ccToParticipationButton);

    ccPartRangeLabel.setText ("Participation range at CC 0 .. 127  (Floor / Ceiling %)", juce::dontSendNotification);
    ccPartRangeLabel.setJustificationType (juce::Justification::centred);
    ccPartRangeLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    ccPartRangeLabel.setFont (juce::FontOptions (12.0f));
    addAndMakeVisible (ccPartRangeLabel);

    for (auto* s : { &ccPartMinSlider, &ccPartMaxSlider })
    {
        s->setSliderStyle (juce::Slider::LinearHorizontal);
        s->setTextBoxStyle (juce::Slider::TextBoxRight, false, 48, 22);
        s->setRange (0.0, 100.0, 1.0);
        s->setColour (juce::Slider::thumbColourId, juce::Colour::fromRGB (95, 220, 140));
        s->setColour (juce::Slider::trackColourId, juce::Colour::fromRGB (95, 200, 245));
        s->setColour (juce::Slider::textBoxTextColourId, juce::Colours::white);
        s->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour::fromRGB (28, 36, 46));
        addAndMakeVisible (*s);
    }

    followConductorResponseButton.setButtonText ("Follow Conductor Response");
    followConductorResponseButton.setColour (juce::ToggleButton::textColourId, juce::Colours::white);
    addAndMakeVisible (followConductorResponseButton);

    for (auto* b : { &responseAffectsInvertButton, &responseAffectsThresholdButton, &responseAffectsParticipationButton })
        b->setColour (juce::ToggleButton::textColourId, juce::Colours::white);

    responseAffectsInvertButton.setButtonText ("Invert");
    responseAffectsThresholdButton.setButtonText ("Threshold");
    responseAffectsParticipationButton.setButtonText ("Participation");
    addAndMakeVisible (responseAffectsInvertButton);
    addAndMakeVisible (responseAffectsThresholdButton);
    addAndMakeVisible (responseAffectsParticipationButton);

    responseCcLabel.setText ("Response CCs from OrchConductor  (Mode / Amount)", juce::dontSendNotification);
    responseCcLabel.setJustificationType (juce::Justification::centred);
    responseCcLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    responseCcLabel.setFont (juce::FontOptions (12.0f));
    addAndMakeVisible (responseCcLabel);

    for (auto* s : { &responseModeCcSlider, &responseAmountCcSlider })
    {
        s->setSliderStyle (juce::Slider::LinearHorizontal);
        s->setTextBoxStyle (juce::Slider::TextBoxRight, false, 48, 22);
        s->setRange (0.0, 127.0, 1.0);
        s->setColour (juce::Slider::thumbColourId, juce::Colour::fromRGB (95, 220, 140));
        s->setColour (juce::Slider::trackColourId, juce::Colour::fromRGB (95, 200, 245));
        s->setColour (juce::Slider::textBoxTextColourId, juce::Colours::white);
        s->setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour::fromRGB (28, 36, 46));
        addAndMakeVisible (*s);
    }

    responseSummaryLabel.setText ("Bridge off", juce::dontSendNotification);
    responseSummaryLabel.setJustificationType (juce::Justification::centred);
    responseSummaryLabel.setColour (juce::Label::textColourId, juce::Colour::fromRGB (140, 200, 245));
    responseSummaryLabel.setFont (juce::FontOptions (12.0f));
    addAndMakeVisible (responseSummaryLabel);

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

    stuckNoteLabel.setText ("Stuck Note Timeout", juce::dontSendNotification);
    stuckNoteLabel.setJustificationType (juce::Justification::centred);
    stuckNoteLabel.setColour (juce::Label::textColourId, juce::Colours::white);
    stuckNoteLabel.setFont (juce::FontOptions (13.0f));
    addAndMakeVisible (stuckNoteLabel);

    stuckNoteTimeoutSlider.setSliderStyle (juce::Slider::LinearHorizontal);
    stuckNoteTimeoutSlider.setTextBoxStyle (juce::Slider::TextBoxRight, false, 56, 22);
    stuckNoteTimeoutSlider.setRange (0.0, 60.0, 0.1);
    stuckNoteTimeoutSlider.setColour (juce::Slider::thumbColourId, juce::Colour::fromRGB (245, 170, 95));
    stuckNoteTimeoutSlider.setColour (juce::Slider::trackColourId, juce::Colour::fromRGB (95, 200, 245));
    stuckNoteTimeoutSlider.setColour (juce::Slider::textBoxTextColourId, juce::Colours::white);
    stuckNoteTimeoutSlider.setColour (juce::Slider::textBoxBackgroundColourId, juce::Colour::fromRGB (28, 36, 46));
    addAndMakeVisible (stuckNoteTimeoutSlider);

    stuckNoteStatusLabel.setJustificationType (juce::Justification::centred);
    stuckNoteStatusLabel.setColour (juce::Label::textColourId, juce::Colour::fromRGB (140, 160, 180));
    stuckNoteStatusLabel.setFont (juce::FontOptions (12.0f));
    addAndMakeVisible (stuckNoteStatusLabel);

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

    ccToParticipationAttachment = std::make_unique<ButtonAttachment> (
        audioProcessor.getParameters(), "ccToParticipation", ccToParticipationButton);
    ccPartMinAttachment = std::make_unique<SliderAttachment> (
        audioProcessor.getParameters(), "ccPartMin", ccPartMinSlider);
    ccPartMaxAttachment = std::make_unique<SliderAttachment> (
        audioProcessor.getParameters(), "ccPartMax", ccPartMaxSlider);

    followConductorResponseAttachment = std::make_unique<ButtonAttachment> (
        audioProcessor.getParameters(), "followConductorResponse", followConductorResponseButton);
    responseAffectsInvertAttachment = std::make_unique<ButtonAttachment> (
        audioProcessor.getParameters(), "responseAffectsInvert", responseAffectsInvertButton);
    responseAffectsThresholdAttachment = std::make_unique<ButtonAttachment> (
        audioProcessor.getParameters(), "responseAffectsThreshold", responseAffectsThresholdButton);
    responseAffectsParticipationAttachment = std::make_unique<ButtonAttachment> (
        audioProcessor.getParameters(), "responseAffectsParticipation", responseAffectsParticipationButton);
    responseModeCcAttachment = std::make_unique<SliderAttachment> (
        audioProcessor.getParameters(), "responseModeCc", responseModeCcSlider);
    responseAmountCcAttachment = std::make_unique<SliderAttachment> (
        audioProcessor.getParameters(), "responseAmountCc", responseAmountCcSlider);

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

    stuckNoteTimeoutAttachment = std::make_unique<SliderAttachment> (
        audioProcessor.getParameters(),
        "stuckNoteTimeoutSeconds",
        stuckNoteTimeoutSlider);

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
    auto area = getLocalBounds().reduced (18);

    titleLabel.setBounds (area.removeFromTop (36));
    subtitleLabel.setBounds (area.removeFromTop (20));
    buildLabel.setBounds (area.removeFromTop (16));

    area.removeFromTop (10);

    // Gate status is the single most important live readout - full width,
    // pinned at the bottom, always visible regardless of window size.
    auto statusArea = area.removeFromBottom (40);
    gateStatusLabel.setBounds (statusArea);

    area.removeFromBottom (8);

    const int gap = 16;
    auto leftColumn = area.removeFromLeft ((area.getWidth() - gap) / 2);
    area.removeFromLeft (gap);
    auto& rightColumn = area;

    // --- Left column: gate + participation + CC gate -------------------
    manualGateButton.setBounds (leftColumn.removeFromTop (28).withSizeKeepingCentre (180, 28));
    leftColumn.removeFromTop (10);

    participationLabel.setBounds (leftColumn.removeFromTop (20));
    participationSlider.setBounds (leftColumn.removeFromTop (30));
    leftColumn.removeFromTop (8);

    muteModeLabel.setBounds (leftColumn.removeFromTop (16));
    muteModeBox.setBounds (leftColumn.removeFromTop (26).withSizeKeepingCentre (200, 26));
    leftColumn.removeFromTop (10);

    ccSummaryLabel.setBounds (leftColumn.removeFromTop (18));
    {
        auto row = leftColumn.removeFromTop (26);
        ccGateEnableButton.setBounds (row.removeFromLeft (row.getWidth() / 2));
        ccInvertButton.setBounds (row);
    }

    ccNumberLabel.setBounds (leftColumn.removeFromTop (16));
    ccNumberSlider.setBounds (leftColumn.removeFromTop (26));

    ccThresholdLabel.setBounds (leftColumn.removeFromTop (16));
    ccThresholdSlider.setBounds (leftColumn.removeFromTop (26));
    leftColumn.removeFromTop (8);

    ccToParticipationButton.setBounds (leftColumn.removeFromTop (24).withSizeKeepingCentre (200, 24));
    ccPartRangeLabel.setBounds (leftColumn.removeFromTop (16));
    {
        auto row = leftColumn.removeFromTop (26);
        ccPartMinSlider.setBounds (row.removeFromLeft ((row.getWidth() - 8) / 2));
        row.removeFromLeft (8);
        ccPartMaxSlider.setBounds (row);
    }

    // --- Right column: conductor response bridge + keyswitches + watchdog
    followConductorResponseButton.setBounds (rightColumn.removeFromTop (24).withSizeKeepingCentre (240, 24));
    {
        auto row = rightColumn.removeFromTop (22);
        const int third = row.getWidth() / 3;
        responseAffectsInvertButton.setBounds (row.removeFromLeft (third));
        responseAffectsThresholdButton.setBounds (row.removeFromLeft (third));
        responseAffectsParticipationButton.setBounds (row);
    }
    responseCcLabel.setBounds (rightColumn.removeFromTop (14));
    {
        auto row = rightColumn.removeFromTop (24);
        responseModeCcSlider.setBounds (row.removeFromLeft ((row.getWidth() - 8) / 2));
        row.removeFromLeft (8);
        responseAmountCcSlider.setBounds (row);
    }
    responseSummaryLabel.setBounds (rightColumn.removeFromTop (16));
    rightColumn.removeFromTop (10);

    passKeyswitchesButton.setBounds (rightColumn.removeFromTop (24).withSizeKeepingCentre (200, 24));
    {
        auto row = rightColumn.removeFromTop (44);
        auto left = row.removeFromLeft ((row.getWidth() - 8) / 2);
        row.removeFromLeft (8);
        keyswitchMinLabel.setBounds (left.removeFromTop (16));
        keyswitchMinSlider.setBounds (left);
        keyswitchMaxLabel.setBounds (row.removeFromTop (16));
        keyswitchMaxSlider.setBounds (row);
    }
    rightColumn.removeFromTop (10);

    stuckNoteLabel.setBounds (rightColumn.removeFromTop (16));
    stuckNoteTimeoutSlider.setBounds (rightColumn.removeFromTop (26));
    stuckNoteStatusLabel.setBounds (rightColumn.removeFromTop (18));
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

    auto* ccToPartParam = audioProcessor.getParameters().getRawParameterValue ("ccToParticipation");
    const bool ccToParticipation = ccToPartParam != nullptr && ccToPartParam->load() >= 0.5f;
    const int effectiveParticipation = juce::roundToInt (audioProcessor.getEffectiveParticipationForUi());

    participationLabel.setText (
        ccToParticipation
            ? "Participation: " + juce::String (effectiveParticipation) + "%  (from CC" + juce::String (ccNumber) + ")"
            : "Participation: " + juce::String (participationPercent) + "%",
        juce::dontSendNotification);

    ccSummaryLabel.setText (
        muteModeText
        + " | CC " + juce::String (ccGateEnabled ? "On" : "Off")
        + " | CC" + juce::String (ccNumber)
        + " | Th" + juce::String (ccThreshold)
        + " | " + juce::String (ccInvert ? "Inv" : "Norm")
        + juce::String (ccToParticipation ? " | Part<-CC" : "")
        + " | KS " + juce::String (passKeyswitchesUi ? "" : "Off ")
        + juce::String (juce::jmin (keyswitchMinUi, keyswitchMaxUi))
        + "-" + juce::String (juce::jmax (keyswitchMinUi, keyswitchMaxUi)),
        juce::dontSendNotification);

    responseSummaryLabel.setText (audioProcessor.getResponseOverlaySummaryForUi(), juce::dontSendNotification);

    const float stuckTimeoutSeconds = audioProcessor.getStuckNoteTimeoutSecondsForUi();
    const int stuckRecoveredCount = audioProcessor.getStuckNotesRecoveredCountForUi();

    stuckNoteStatusLabel.setText (
        stuckTimeoutSeconds <= 0.0f
            ? "Watchdog off"
            : "Recovered: " + juce::String (stuckRecoveredCount),
        juce::dontSendNotification);
    stuckNoteStatusLabel.setColour (
        juce::Label::textColourId,
        stuckRecoveredCount > 0 ? juce::Colour::fromRGB (245, 170, 95)
                                 : juce::Colour::fromRGB (140, 160, 180));

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































