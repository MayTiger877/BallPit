/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BallPitAudioProcessorEditor::BallPitAudioProcessorEditor (BallPitAudioProcessor& p)
	: AudioProcessorEditor (&p), presetPanel(p.getPresetManager()), audioProcessor (p), ballBeingDragged((int)MOUSE_NOT_IN_BALL, MOUSE_NOT_IN_BALL)
{
	auto backgroundSVGFile = juce::MemoryInputStream(BinaryData::ByKnobs_svg, BinaryData::ByKnobs_svgSize, false);
	backgroundDrawable = juce::Drawable::createFromImageDataStream(backgroundSVGFile);

	auto tabsSVGFile = juce::MemoryInputStream(BinaryData::Ball_tab_0_svg, BinaryData::Ball_tab_0_svgSize, false);
	tabsDrawable = juce::Drawable::createFromImageDataStream(tabsSVGFile);

	setSize(APP_WINDOW_WIDTH, APP_WINDOW_HIGHT);
	startTimerHz(VISUAL_FRAMES_PER_SECOND);

	GUIState = juce::ValueTree("GUIState");

	initiateComponents();
	configureAttachments();
	
	audioProcessor.addChangeListener(this); // Register as listener

	content.addChildComponent(&presetPanel);

	if (audioProcessor.getWasGUIUploaded() == false) // save default state only at first time gui is built
	{
		saveGUIState();
		audioProcessor.setWasGUIUploaded(true);
	}
	else
	{
		loadGUIState();
	}

	content.setInterceptsMouseClicks(false, true);
	setWantsKeyboardFocus(true);
	addKeyListener(this);
	addAndMakeVisible(content);

	BallGUIEssentials GUIBallToEnter = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, false, 0, 0, 0.0, 0.0, 0.0 };
	for (int i = 0; i < 3; i++)
	{
		GUIBallToEnter.ballIndex = i;
		GUIBallToEnter.x = 0.0;
		GUIBallToEnter.y = 0.0;
		GUIBallToEnter.radius = 0.0;
		GUIBallToEnter.angle = 0.0;
		GUIBallToEnter.velocity = 0.0;
		GUIBallToEnter.isMoving = 0.0;
		GUIBallToEnter.ballSpeedType = 0;
		GUIBallToEnter.delayAmount = 0;
		GUIBallToEnter.delayFeedback = 0.0;
		GUIBallToEnter.delayRate = 0.0;
		GUIBallToEnter.delayNoteMovement = 0;
		GUIBallToEnter.delayPoints[0] = { 0.0, 0.0 };
		GUIBallToEnter.delayPoints[1] = { 0.0, 0.0 };
		GUIBallToEnter.delayPoints[2] = { 0.0, 0.0 };
		GUIBalls.push_back(std::make_unique<BallGUIEssentials>(GUIBallToEnter));
	}
}

BallPitAudioProcessorEditor::~BallPitAudioProcessorEditor()
{
	if (audioProcessor.getWasGUIUploaded() == true) { saveGUIState(); }
	for (int i = 0; i < 3; i++)
	{
		ballsSlidersAndAttachments[i].xSlider.setLookAndFeel(nullptr);
		ballsSlidersAndAttachments[i].ySlider.setLookAndFeel(nullptr);
		ballsSlidersAndAttachments[i].velocitySlider.setLookAndFeel(nullptr);
		ballsSlidersAndAttachments[i].angleSlider.setLookAndFeel(nullptr);
		ballsSlidersAndAttachments[i].radiusSlider.setLookAndFeel(nullptr);
		ballsSlidersAndAttachments[i].xVelocitySlider.setLookAndFeel(nullptr);
		ballsSlidersAndAttachments[i].yVelocitySlider.setLookAndFeel(nullptr);
		ballsSlidersAndAttachments[i].xVelocityInverter.setLookAndFeel(nullptr);
		ballsSlidersAndAttachments[i].yVelocityInverter.setLookAndFeel(nullptr);
		ballEffectsSlidersAndAttachments[i].delayFeedbackSlider.setLookAndFeel(nullptr);
		ballEffectsSlidersAndAttachments[i].delayAmountSlider.setLookAndFeel(nullptr);
		ballEffectsSlidersAndAttachments[i].delayRateComboBox.setLookAndFeel(nullptr);
		ballEffectsSlidersAndAttachments[i].delayNoteMovementComboBox.setLookAndFeel(nullptr);
	}
	scaleChoiceComboBox.setLookAndFeel(nullptr);
	edgeTypeComboBox.setLookAndFeel(nullptr);
	edgePhaseSlider.setLookAndFeel(nullptr);
	rootNoteComboBox.setLookAndFeel(nullptr);
	ballsPositioningTypeComboBox.setLookAndFeel(nullptr);
	quantizationDivisionComboBox.setLookAndFeel(nullptr);
	quantizationSlider.setLookAndFeel(nullptr);
	volumeVariationSlider.setLookAndFeel(nullptr);
	sizePercentageComboBox.setLookAndFeel(nullptr);
	probabilitySlider.setLookAndFeel(nullptr);

	edgeDenomenatorSlider.setLookAndFeel(nullptr);
	edgeRangeSlider.setLookAndFeel(nullptr);

	snapToGridButton.setLookAndFeel(nullptr);
	collisionButton.setLookAndFeel(nullptr);
}

void BallPitAudioProcessorEditor::saveGUIState()
{
	for (int i = 0; i < 3; ++i)
	{
		std::string ballXId = "ballX" + std::to_string(i);
		std::string ballYId = "ballY" + std::to_string(i);
		std::string ballRadiusId = "ballRadius" + std::to_string(i);
		std::string ballVelocityId = "ballVelocity" + std::to_string(i);
		std::string ballAngleId = "ballAngle" + std::to_string(i);
		std::string ballXVelocityId = "ballXVelocity" + std::to_string(i);
		std::string ballYVelocityId = "ballYVelocity" + std::to_string(i);
		std::string xVelocityInverterId = "xVelocityInverter" + std::to_string(i);
		std::string yVelocityInverterId = "yVelocityInverter" + std::to_string(i);
		std::string delayAmountId = "delayAmount" + std::to_string(i);
		std::string delayFeedbackId = "delayFeedback" + std::to_string(i);
		std::string delayRateId = "delayRate" + std::to_string(i);
		std::string delayNoteMovementId = "delayNoteMovement" + std::to_string(i);

		GUIState.setProperty(juce::Identifier(ballXId), ballsSlidersAndAttachments[i].xSlider.getValue(), nullptr);
		GUIState.setProperty(juce::Identifier(ballYId), ballsSlidersAndAttachments[i].ySlider.getValue(), nullptr);
		GUIState.setProperty(juce::Identifier(ballRadiusId), ballsSlidersAndAttachments[i].radiusSlider.getValue(), nullptr);
		GUIState.setProperty(juce::Identifier(ballVelocityId), ballsSlidersAndAttachments[i].velocitySlider.getValue(), nullptr);
		GUIState.setProperty(juce::Identifier(ballAngleId), ballsSlidersAndAttachments[i].angleSlider.getValue(), nullptr);
		GUIState.setProperty(juce::Identifier(ballXVelocityId), ballsSlidersAndAttachments[i].xVelocitySlider.getValue(), nullptr);
		GUIState.setProperty(juce::Identifier(ballYVelocityId), ballsSlidersAndAttachments[i].yVelocitySlider.getValue(), nullptr);
		GUIState.setProperty(juce::Identifier(xVelocityInverterId), ballsSlidersAndAttachments[i].xVelocityInverter.getToggleState(), nullptr);
		GUIState.setProperty(juce::Identifier(yVelocityInverterId), ballsSlidersAndAttachments[i].yVelocityInverter.getToggleState(), nullptr);
		GUIState.setProperty(juce::Identifier(delayAmountId), ballEffectsSlidersAndAttachments[i].delayAmountSlider.getValue(), nullptr);
		GUIState.setProperty(juce::Identifier(delayFeedbackId), ballEffectsSlidersAndAttachments[i].delayFeedbackSlider.getValue(), nullptr);
		GUIState.setProperty(juce::Identifier(delayRateId), ballEffectsSlidersAndAttachments[i].delayRateComboBox.getSelectedId(), nullptr);
		GUIState.setProperty(juce::Identifier(delayNoteMovementId), ballEffectsSlidersAndAttachments[i].delayNoteMovementComboBox.getSelectedId(), nullptr);
	}

	GUIState.setProperty("edgePhase", edgePhaseSlider.getValue(), nullptr);
	GUIState.setProperty("edgeDenomenator", edgeDenomenatorSlider.getValue(), nullptr);
	GUIState.setProperty("edgeRange", edgeRangeSlider.getValue(), nullptr);
	GUIState.setProperty("scaleChoice", scaleChoiceComboBox.getSelectedItemIndex(), nullptr);
	GUIState.setProperty("rootNote", rootNoteComboBox.getSelectedItemIndex(), nullptr);
	GUIState.setProperty("edgeType", edgeTypeComboBox.getSelectedItemIndex(), nullptr);
	GUIState.setProperty("ballsPositioningType", ballsPositioningTypeComboBox.getSelectedItemIndex(), nullptr);
	GUIState.setProperty("snapToGrid", snapToGridButton.getToggleState(), nullptr);
	GUIState.setProperty("quantization", quantizationSlider.getValue(), nullptr);
	GUIState.setProperty("quantization", quantizationDivisionComboBox.getSelectedItemIndex(), nullptr);
	GUIState.setProperty("volumeVariation", volumeVariationSlider.getValue(), nullptr);
	GUIState.setProperty("sizePercentage", sizePercentageComboBox.getSelectedItemIndex(), nullptr);
	GUIState.setProperty("probability", probabilitySlider.getValue(), nullptr);

	audioProcessor.saveGUIState(GUIState);
	audioProcessor.removeChangeListener(this);
}

void BallPitAudioProcessorEditor::loadGUIState()
{
	auto& GUIState = audioProcessor.getGUIState();

	for (int i = 0; i < 3; i++)
	{
		std::string ballXId = "ballX" + std::to_string(i);
		std::string ballYId = "ballY" + std::to_string(i);
		std::string ballRadiusId = "ballRadius" + std::to_string(i);
		std::string ballVelocityId = "ballVelocity" + std::to_string(i);
		std::string ballAngleId = "ballAngle" + std::to_string(i);
		std::string ballXVelocityId = "ballXVelocity" + std::to_string(i);
		std::string ballYVelocityId = "ballYVelocity" + std::to_string(i);
		std::string xVelocityInverterId = "xVelocityInverter" + std::to_string(i);
		std::string yVelocityInverterId = "yVelocityInverter" + std::to_string(i);
		std::string delayAmountId = "delayAmount" + std::to_string(i);
		std::string delayFeedbackId = "delayFeedback" + std::to_string(i);
		std::string delayRateId = "delayRate" + std::to_string(i);
		std::string delayNoteMovementId = "delayNoteMovement" + std::to_string(i);

		ballsSlidersAndAttachments[i].xSlider.setValue(GUIState.getProperty(juce::Identifier(ballXId)), juce::dontSendNotification);
		ballsSlidersAndAttachments[i].ySlider.setValue(GUIState.getProperty(juce::Identifier(ballYId)), juce::dontSendNotification);
		ballsSlidersAndAttachments[i].radiusSlider.setValue(GUIState.getProperty(juce::Identifier(ballRadiusId)), juce::dontSendNotification);
		ballsSlidersAndAttachments[i].velocitySlider.setValue(GUIState.getProperty(juce::Identifier(ballVelocityId)), juce::dontSendNotification);
		ballsSlidersAndAttachments[i].angleSlider.setValue(GUIState.getProperty(juce::Identifier(ballAngleId)), juce::dontSendNotification);
		ballsSlidersAndAttachments[i].xVelocitySlider.setValue(GUIState.getProperty(juce::Identifier(ballXVelocityId)), juce::dontSendNotification);
		ballsSlidersAndAttachments[i].yVelocitySlider.setValue(GUIState.getProperty(juce::Identifier(ballYVelocityId)), juce::dontSendNotification);
		ballsSlidersAndAttachments[i].xVelocityInverter.setToggleState(GUIState.getProperty(juce::Identifier(xVelocityInverterId)), juce::dontSendNotification);
		ballsSlidersAndAttachments[i].yVelocityInverter.setToggleState(GUIState.getProperty(juce::Identifier(yVelocityInverterId)), juce::dontSendNotification);
		ballEffectsSlidersAndAttachments[i].delayAmountSlider.setValue(GUIState.getProperty(juce::Identifier(delayAmountId)), juce::dontSendNotification);
		ballEffectsSlidersAndAttachments[i].delayFeedbackSlider.setValue(GUIState.getProperty(juce::Identifier(delayFeedbackId)), juce::dontSendNotification);
		ballEffectsSlidersAndAttachments[i].delayRateComboBox.setSelectedId(GUIState.getProperty(juce::Identifier(delayRateId)), juce::dontSendNotification);
		ballEffectsSlidersAndAttachments[i].delayNoteMovementComboBox.setSelectedId(GUIState.getProperty(juce::Identifier(delayNoteMovementId)), juce::dontSendNotification);
	}

	edgePhaseSlider.setValue(GUIState.getProperty("edgePhase"), juce::dontSendNotification);
	edgeDenomenatorSlider.setValue(GUIState.getProperty("edgeDenomenator"), juce::dontSendNotification);
	edgeRangeSlider.setValue(GUIState.getProperty("edgeRange"), juce::dontSendNotification);
	scaleChoiceComboBox.setSelectedItemIndex(GUIState.getProperty("scaleChoice"), juce::dontSendNotification);
	rootNoteComboBox.setSelectedItemIndex(GUIState.getProperty("rootNote"), juce::dontSendNotification);
	edgeTypeComboBox.setSelectedItemIndex(GUIState.getProperty("edgeType"), juce::dontSendNotification);
	ballsPositioningTypeComboBox.setSelectedItemIndex(GUIState.getProperty("ballsPositioningType"), juce::dontSendNotification);
	snapToGridButton.setToggleState(GUIState.getProperty("snapToGrid"), juce::dontSendNotification);
	collisionButton.setToggleState(GUIState.getProperty("collision"), juce::dontSendNotification);
	quantizationSlider.setValue(GUIState.getProperty("quantization"), juce::dontSendNotification);
	quantizationDivisionComboBox.setSelectedItemIndex(GUIState.getProperty("quantizationDivision"), juce::dontSendNotification);
	volumeVariationSlider.setValue(GUIState.getProperty("volumeVariation"), juce::dontSendNotification);
	sizePercentageComboBox.setSelectedItemIndex(GUIState.getProperty("sizePercentage"), juce::dontSendNotification);
	probabilitySlider.setValue(GUIState.getProperty("probability"), juce::dontSendNotification);
}

void BallPitAudioProcessorEditor::configureAttachments()
{
	// Configure and attach parameters and sliders and fx
	for (int i = 0; i < 3; i++)
	{
		std::string ballXId = "ballX" + std::to_string(i);
		ballsSlidersAndAttachments[i].xAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTreeState, ballXId, ballsSlidersAndAttachments[i].xSlider);

		std::string ballYID = "ballY" + std::to_string(i);
		ballsSlidersAndAttachments[i].yAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTreeState, ballYID, ballsSlidersAndAttachments[i].ySlider);

		std::string ballRadiusID = "ballRadius" + std::to_string(i);
		ballsSlidersAndAttachments[i].radiusAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTreeState, ballRadiusID, ballsSlidersAndAttachments[i].radiusSlider);

		std::string ballVelocityID = "ballVelocity" + std::to_string(i);
		ballsSlidersAndAttachments[i].velocityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTreeState, ballVelocityID, ballsSlidersAndAttachments[i].velocitySlider);

		std::string ballAngleID = "ballAngle" + std::to_string(i);
		ballsSlidersAndAttachments[i].angleAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTreeState, ballAngleID, ballsSlidersAndAttachments[i].angleSlider);

		std::string ballXVelocityID = "ballXVelocity" + std::to_string(i);
		ballsSlidersAndAttachments[i].xVelocityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTreeState, ballXVelocityID, ballsSlidersAndAttachments[i].xVelocitySlider);

		std::string ballYVelocityID = "ballYVelocity" + std::to_string(i);
		ballsSlidersAndAttachments[i].yVelocityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTreeState, ballYVelocityID, ballsSlidersAndAttachments[i].yVelocitySlider);

		std::string xVelocityInverterID = "xVelocityInverter" + std::to_string(i);
		ballsSlidersAndAttachments[i].xVelocityInverterAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.valueTreeState, xVelocityInverterID, ballsSlidersAndAttachments[i].xVelocityInverter);

		std::string yVelocityInverterID = "yVelocityInverter" + std::to_string(i);
		ballsSlidersAndAttachments[i].yVelocityInverterAttachment = std::make_unique < juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.valueTreeState, yVelocityInverterID, ballsSlidersAndAttachments[i].yVelocityInverter);

		// delay
		std::string dealyAmountID = "delayAmount" + std::to_string(i);
		ballEffectsSlidersAndAttachments[i].delayAmountAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTreeState, dealyAmountID, ballEffectsSlidersAndAttachments[i].delayAmountSlider);

		std::string delayFeedbackID = "delayFeedback" + std::to_string(i);
		ballEffectsSlidersAndAttachments[i].delayFeedbackAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTreeState, delayFeedbackID, ballEffectsSlidersAndAttachments[i].delayFeedbackSlider);

		std::string delayRateID = "delayRate" + std::to_string(i);
		ballEffectsSlidersAndAttachments[i].delayRateAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.valueTreeState, delayRateID, ballEffectsSlidersAndAttachments[i].delayRateComboBox);

		std::string delayNoteMovementID = "delayNoteMovement" + std::to_string(i);
		ballEffectsSlidersAndAttachments[i].delayNoteMovementAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.valueTreeState, delayNoteMovementID, ballEffectsSlidersAndAttachments[i].delayNoteMovementComboBox);
	}

	std::string edgePhaseID = "edgePhase";
	edgePhaseAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTreeState, edgePhaseID, edgePhaseSlider);

	std::string edgeDenomenatorID = "edgeDenomenator";
	edgeDenomenatorAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTreeState, edgeDenomenatorID, edgeDenomenatorSlider);

	std::string edgeRangeID = "edgeRange";
	edgeRangeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTreeState, edgeRangeID, edgeRangeSlider);

	std::string scaleChoiceID = "scaleChoice";
	scaleChoiceAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.valueTreeState, scaleChoiceID, scaleChoiceComboBox);

	std::string rootNoteID = "rootNote";
	rootNoteAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.valueTreeState, rootNoteID, rootNoteComboBox);

	std::string edgeTypeID = "edgeType";
	edgeTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.valueTreeState, edgeTypeID, edgeTypeComboBox);

	std::string ballsPositioningTypeID = "ballsPositioningType";
	ballsPositioningTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.valueTreeState, ballsPositioningTypeID, ballsPositioningTypeComboBox);

	std::string snapToGridID = "snapToGrid";
	snapToGridAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.valueTreeState, snapToGridID, snapToGridButton);

	std::string collisionID = "collision";
	collisionAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.valueTreeState, collisionID, collisionButton);

	std::string quantizationID = "quantization";
	quantizationAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTreeState, quantizationID, quantizationSlider);

	std::string quantizationDivisionID = "quantizationDivision";
	quantizationDivisionAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.valueTreeState, quantizationDivisionID, quantizationDivisionComboBox);

	std::string volumeVariationID = "volumeVariation";
	volumeVariationAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTreeState, volumeVariationID, volumeVariationSlider);

	std::string sizePercentageID = "sizePercentage";
	sizePercentageAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.valueTreeState, sizePercentageID, sizePercentageComboBox);

	std::string probabilityID = "probability";
	probabilityAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTreeState, probabilityID, probabilitySlider);
}

void BallPitAudioProcessorEditor::displayKnobsByTab()
{
	int ballsPosType = 1 + audioProcessor.valueTreeState.getRawParameterValue("ballsPositioningType")->load(); // 1 is offset
	int currentTab = this->currentBallFocused;
	int otherTab1 = (currentTab + 1) % 3;
	int otherTab2 = (currentTab + 2) % 3;
	
	ballsSlidersAndAttachments[currentTab].xSlider.setVisible(true);
	ballsSlidersAndAttachments[currentTab].ySlider.setVisible(true);
	ballsSlidersAndAttachments[currentTab].radiusSlider.setVisible(true);
	ballEffectsSlidersAndAttachments[currentTab].delayAmountSlider.setVisible(true);
	ballEffectsSlidersAndAttachments[currentTab].delayFeedbackSlider.setVisible(true);
	ballEffectsSlidersAndAttachments[currentTab].delayRateComboBox.setVisible(true);
	ballEffectsSlidersAndAttachments[currentTab].delayNoteMovementComboBox.setVisible(true);
	switch (ballsPosType)
	{
		case 1: // chaos
		{
			ballsSlidersAndAttachments[currentTab].angleSlider.setVisible(true);
			ballsSlidersAndAttachments[currentTab].velocitySlider.setVisible(true);
			ballsSlidersAndAttachments[currentTab].xVelocitySlider.setVisible(false);
			ballsSlidersAndAttachments[currentTab].yVelocitySlider.setVisible(false);
			ballsSlidersAndAttachments[currentTab].xVelocityInverter.setVisible(false);
			ballsSlidersAndAttachments[currentTab].yVelocityInverter.setVisible(false);
			break;
		}
		case 2: // by tempo
		{
			ballsSlidersAndAttachments[currentTab].angleSlider.setVisible(false);
			ballsSlidersAndAttachments[currentTab].velocitySlider.setVisible(false);
			ballsSlidersAndAttachments[currentTab].xVelocitySlider.setVisible(true);
			ballsSlidersAndAttachments[currentTab].yVelocitySlider.setVisible(true);
			ballsSlidersAndAttachments[currentTab].xVelocityInverter.setVisible(true);
			ballsSlidersAndAttachments[currentTab].yVelocityInverter.setVisible(true);
			break;
		}
		default:
			break;
	}

	ballsSlidersAndAttachments[otherTab1].xSlider.setVisible(false);
	ballsSlidersAndAttachments[otherTab1].ySlider.setVisible(false);
	ballsSlidersAndAttachments[otherTab1].angleSlider.setVisible(false);
	ballsSlidersAndAttachments[otherTab1].radiusSlider.setVisible(false);
	ballsSlidersAndAttachments[otherTab1].velocitySlider.setVisible(false);
	ballsSlidersAndAttachments[otherTab1].xVelocitySlider.setVisible(false);
	ballsSlidersAndAttachments[otherTab1].yVelocitySlider.setVisible(false);
	ballsSlidersAndAttachments[otherTab1].xVelocityInverter.setVisible(false);
	ballsSlidersAndAttachments[otherTab1].yVelocityInverter.setVisible(false);
	ballEffectsSlidersAndAttachments[otherTab1].delayAmountSlider.setVisible(false);
	ballEffectsSlidersAndAttachments[otherTab1].delayFeedbackSlider.setVisible(false);
	ballEffectsSlidersAndAttachments[otherTab1].delayRateComboBox.setVisible(false);
	ballEffectsSlidersAndAttachments[otherTab1].delayNoteMovementComboBox.setVisible(false);

	ballsSlidersAndAttachments[otherTab2].xSlider.setVisible(false);
	ballsSlidersAndAttachments[otherTab2].ySlider.setVisible(false);
	ballsSlidersAndAttachments[otherTab2].angleSlider.setVisible(false);
	ballsSlidersAndAttachments[otherTab2].radiusSlider.setVisible(false);
	ballsSlidersAndAttachments[otherTab2].velocitySlider.setVisible(false);
	ballsSlidersAndAttachments[otherTab2].xVelocitySlider.setVisible(false);
	ballsSlidersAndAttachments[otherTab2].yVelocitySlider.setVisible(false);
	ballsSlidersAndAttachments[otherTab2].xVelocityInverter.setVisible(false);
	ballsSlidersAndAttachments[otherTab2].yVelocityInverter.setVisible(false);
	ballEffectsSlidersAndAttachments[otherTab2].delayAmountSlider.setVisible(false);
	ballEffectsSlidersAndAttachments[otherTab2].delayFeedbackSlider.setVisible(false);
	ballEffectsSlidersAndAttachments[otherTab2].delayRateComboBox.setVisible(false);
	ballEffectsSlidersAndAttachments[otherTab2].delayNoteMovementComboBox.setVisible(false);
	
	if (GUIBalls[this->currentBallFocused]->active == true)
	{
		addRemoveBallButton.setButtonText("Remove");
	}
	else
	{
		addRemoveBallButton.setButtonText("Add");
	}
}


void BallPitAudioProcessorEditor::initiateComponents()
{
	for (int i = 0; i < 3; i++)
	{
		// X Slider
		ballsSlidersAndAttachments[i].xSlider.setBounds(BALL_X_KNOB_BOUNDS);
		ballsSlidersAndAttachments[i].xSlider.setValue(BALL_X_DEFAULT_1 + (i * 130.0f));
		ballsSlidersAndAttachments[i].xSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		ballsSlidersAndAttachments[i].xSlider.setDoubleClickReturnValue(true, BALL_X_DEFAULT_1 + (i * 130.0f));
		ballsSlidersAndAttachments[i].xSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballsSlidersAndAttachments[i].xSlider.setRange(BALL_X_SLIDER_MIN, BALL_X_SLIDER_MAX, BALL_X_SLIDER_STEP);
		ballsSlidersAndAttachments[i].xSlider.toFront(false);
		ballsSlidersAndAttachments[i].xSlider.setLookAndFeel(&this->m_costumeDialLAF);
		content.addChildComponent(ballsSlidersAndAttachments[i].xSlider);

		// Y Slider
		ballsSlidersAndAttachments[i].ySlider.setBounds(BALL_Y_KNOB_BOUNDS);
		ballsSlidersAndAttachments[i].ySlider.setValue(BALL_Y_DEFAULT);
		ballsSlidersAndAttachments[i].ySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		ballsSlidersAndAttachments[i].ySlider.setDoubleClickReturnValue(true, BALL_Y_DEFAULT);
		ballsSlidersAndAttachments[i].ySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballsSlidersAndAttachments[i].ySlider.setRange(BALL_Y_SLIDER_MIN, BALL_Y_SLIDER_MAX, BALL_Y_SLIDER_STEP);
		ballsSlidersAndAttachments[i].ySlider.toFront(false);
		ballsSlidersAndAttachments[i].ySlider.setLookAndFeel(&this->m_costumeDialLAF);
		content.addChildComponent(ballsSlidersAndAttachments[i].ySlider);

		// Angle Slider
		ballsSlidersAndAttachments[i].angleSlider.setBounds(BALL_DIRECTION_KNOB_BOUNDS);
		ballsSlidersAndAttachments[i].angleSlider.setValue(BALL_ANGLE_DEFAULT_1 + (i * 60.0f));
		ballsSlidersAndAttachments[i].angleSlider.setSliderStyle(juce::Slider::MayT_DirectionKnob);
		ballsSlidersAndAttachments[i].angleSlider.setDoubleClickReturnValue(true, BALL_ANGLE_DOUBLE_CLICK_VALUE);
		ballsSlidersAndAttachments[i].angleSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballsSlidersAndAttachments[i].angleSlider.setRotaryParameters(0, 2 * juce::MathConstants<double>::pi, false);
		ballsSlidersAndAttachments[i].angleSlider.setRange(BALL_ANGLE_MIN, BALL_ANGLE_MAX, BALL_ANGLE_STEP);
		ballsSlidersAndAttachments[i].angleSlider.toFront(false);
		ballsSlidersAndAttachments[i].angleSlider.setLookAndFeel(&this->m_costumeDialLAF);
		content.addChildComponent(ballsSlidersAndAttachments[i].angleSlider);

		// Velocity Slider
		ballsSlidersAndAttachments[i].velocitySlider.setBounds(BALL_SPEED_KNOB_BOUNDS);
		ballsSlidersAndAttachments[i].velocitySlider.setValue(BALL_VELOCITY_DEFAULT);
		ballsSlidersAndAttachments[i].velocitySlider.setSliderStyle(juce::Slider::MayT_VelocityKnob);
		ballsSlidersAndAttachments[i].velocitySlider.setDoubleClickReturnValue(true, BALL_VELOCITY_DOUBLE_CLICK_VALUE);
		ballsSlidersAndAttachments[i].velocitySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballsSlidersAndAttachments[i].velocitySlider.setRange(BALL_VELOCITY_MIN, BALL_VELOCITY_MAX, BALL_VELOCITY_STEP);
		ballsSlidersAndAttachments[i].velocitySlider.toFront(false);
		ballsSlidersAndAttachments[i].velocitySlider.setLookAndFeel(&this->m_costumeDialLAF);
		content.addChildComponent(ballsSlidersAndAttachments[i].velocitySlider);

		// Radius Slider
		ballsSlidersAndAttachments[i].radiusSlider.setBounds(BALL_SIZE_KNOB_BOUNDS);
		ballsSlidersAndAttachments[i].radiusSlider.setValue(BALL_RADIUS_DEFAULT);
		ballsSlidersAndAttachments[i].radiusSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		ballsSlidersAndAttachments[i].radiusSlider.setDoubleClickReturnValue(true, BALL_RADIUS_DOUBLE_CLICK_VALUE);
		ballsSlidersAndAttachments[i].radiusSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballsSlidersAndAttachments[i].radiusSlider.setRange(BALL_RADIUS_MIN, BALL_RADIUS_MAX, BALL_RADIUS_STEP);
		ballsSlidersAndAttachments[i].radiusSlider.setLookAndFeel(&this->m_costumeDialLAF);
		ballsSlidersAndAttachments[i].radiusSlider.toFront(false);
		ballsSlidersAndAttachments[i].radiusSlider.setLookAndFeel(&this->m_costumeDialLAF);
		content.addChildComponent(ballsSlidersAndAttachments[i].radiusSlider);

		// X Velocity Slider
		ballsSlidersAndAttachments[i].xVelocitySlider.setBounds(BALL_X_VELOCITY_KNOB_BOUNDS);
		ballsSlidersAndAttachments[i].xVelocitySlider.setValue(BALL_X_VELOCITY_DEFAULT);
		ballsSlidersAndAttachments[i].xVelocitySlider.setSliderStyle(juce::Slider::MayT_TempoKnob);
		ballsSlidersAndAttachments[i].xVelocitySlider.setDoubleClickReturnValue(true, BALL_X_VELOCITY_DOUBLE_CLICK_VALUE);
		ballsSlidersAndAttachments[i].xVelocitySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballsSlidersAndAttachments[i].xVelocitySlider.setRange(BALL_X_VELOCITY_MIN, BALL_X_VELOCITY_MAX, BALL_X_VELOCITY_STEP);
		ballsSlidersAndAttachments[i].xVelocitySlider.toFront(false);
		ballsSlidersAndAttachments[i].xVelocitySlider.setLookAndFeel(&this->m_costumeDialLAF);
		content.addChildComponent(ballsSlidersAndAttachments[i].xVelocitySlider);

		// Y Velocity Slider
		ballsSlidersAndAttachments[i].yVelocitySlider.setBounds(BALL_Y_VELOCITY_KNOB_BOUNDS);
		ballsSlidersAndAttachments[i].yVelocitySlider.setValue(BALL_Y_VELOCITY_DEFAULT);
		ballsSlidersAndAttachments[i].yVelocitySlider.setSliderStyle(juce::Slider::MayT_TempoKnob);
		ballsSlidersAndAttachments[i].yVelocitySlider.setDoubleClickReturnValue(true, BALL_Y_VELOCITY_DOUBLE_CLICK_VALUE);
		ballsSlidersAndAttachments[i].yVelocitySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballsSlidersAndAttachments[i].yVelocitySlider.setRange(BALL_Y_VELOCITY_MIN, BALL_Y_VELOCITY_MAX, BALL_Y_VELOCITY_STEP);
		ballsSlidersAndAttachments[i].yVelocitySlider.toFront(false);
		ballsSlidersAndAttachments[i].yVelocitySlider.setLookAndFeel(&this->m_costumeDialLAF);
		content.addChildComponent(ballsSlidersAndAttachments[i].yVelocitySlider); 

		// xVelocityInverter
		ballsSlidersAndAttachments[i].xVelocityInverter.setBounds(BALL_X_VELOCITY_INVERTER_BOUNDS);
		ballsSlidersAndAttachments[i].xVelocityInverter.setToggleState(false, juce::dontSendNotification);
		ballsSlidersAndAttachments[i].xVelocityInverter.setLookAndFeel(&this->m_costumeVelocityInverterLAF);
		content.addChildComponent(ballsSlidersAndAttachments[i].xVelocityInverter);

		// yVelocityInverter
		ballsSlidersAndAttachments[i].yVelocityInverter.setBounds(BALL_Y_VELOCITY_INVERTER_BOUNDS);
		ballsSlidersAndAttachments[i].yVelocityInverter.setToggleState(false, juce::dontSendNotification);
		ballsSlidersAndAttachments[i].yVelocityInverter.setLookAndFeel(&this->m_costumeVelocityInverterLAF);
		content.addChildComponent(ballsSlidersAndAttachments[i].yVelocityInverter);

		// Delay Amount Slider
		ballEffectsSlidersAndAttachments[i].delayAmountSlider.setBounds(DELAY_AMOUNT_SLIDER_BOUNDS);
		ballEffectsSlidersAndAttachments[i].delayAmountSlider.setValue(DELAY_AMOUNT_DEFAULT);
		ballEffectsSlidersAndAttachments[i].delayAmountSlider.setSliderStyle(juce::Slider::LinearHorizontal);
		ballEffectsSlidersAndAttachments[i].delayAmountSlider.setDoubleClickReturnValue(true, DELAY_AMOUNT_DEFAULT);
		ballEffectsSlidersAndAttachments[i].delayAmountSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballEffectsSlidersAndAttachments[i].delayAmountSlider.setRange(DELAY_AMOUNT_MIN, DELAY_AMOUNT_MAX, DELAY_AMOUNT_STEP);
		ballEffectsSlidersAndAttachments[i].delayAmountSlider.setLookAndFeel(&this->m_costumeDelayAmountLAF);
		ballEffectsSlidersAndAttachments[i].delayAmountSlider.toFront(false);
		content.addChildComponent(ballEffectsSlidersAndAttachments[i].delayAmountSlider);
		
		// Delay Feedback Slider
		ballEffectsSlidersAndAttachments[i].delayFeedbackSlider.setBounds(DELAY_FEEDBACK_SLIDER_BOUNDS);
		ballEffectsSlidersAndAttachments[i].delayFeedbackSlider.setValue(DELAY_FEEDBACK_DEFAULT);
		ballEffectsSlidersAndAttachments[i].delayFeedbackSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		ballEffectsSlidersAndAttachments[i].delayFeedbackSlider.setDoubleClickReturnValue(true, DELAY_FEEDBACK_DEFAULT);
		ballEffectsSlidersAndAttachments[i].delayFeedbackSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballEffectsSlidersAndAttachments[i].delayFeedbackSlider.setRange(DELAY_FEEDBACK_MIN, DELAY_FEEDBACK_MAX, DELAY_FEEDBACK_STEP);
		ballEffectsSlidersAndAttachments[i].delayFeedbackSlider.setLookAndFeel(&this->m_costumeDelayFeedbackLAF);
		ballEffectsSlidersAndAttachments[i].delayFeedbackSlider.toFront(false);
		content.addChildComponent(ballEffectsSlidersAndAttachments[i].delayFeedbackSlider);

		// Delay Rate Slider
		ballEffectsSlidersAndAttachments[i].delayRateComboBox.setBounds(DELAY_RATE_SLIDER_BOUNDS);
		ballEffectsSlidersAndAttachments[i].delayRateComboBox.addItem("4", DELAY_RATE_4);
		ballEffectsSlidersAndAttachments[i].delayRateComboBox.addItem("2", DELAY_RATE_2);
		ballEffectsSlidersAndAttachments[i].delayRateComboBox.addItem("1", DELAY_RATE_1);
		ballEffectsSlidersAndAttachments[i].delayRateComboBox.addItem("1/2", DELAY_RATE_0_5);
		ballEffectsSlidersAndAttachments[i].delayRateComboBox.addItem("1/4", DELAY_RATE_0_25);
		ballEffectsSlidersAndAttachments[i].delayRateComboBox.addItem("1/8", DELAY_RATE_0_125);
		ballEffectsSlidersAndAttachments[i].delayRateComboBox.setSelectedId(DELAY_RATE_DEFAULT, juce::dontSendNotification);
		ballEffectsSlidersAndAttachments[i].delayRateComboBox.setColour(juce::ComboBox::textColourId, BUTTON_TEXT_COLOUR);
		ballEffectsSlidersAndAttachments[i].delayRateComboBox.setLookAndFeel(&this->m_costumeComboBoxLAF);
		ballEffectsSlidersAndAttachments[i].delayRateComboBox.toFront(false);
		content.addChildComponent(ballEffectsSlidersAndAttachments[i].delayRateComboBox);

		// Delay Note Movement ComboBox
		ballEffectsSlidersAndAttachments[i].delayNoteMovementComboBox.setBounds(DELAY_NOTE_MOVEMENT_COMBOBOX_BOUNDS);
		ballEffectsSlidersAndAttachments[i].delayNoteMovementComboBox.addItem("Same note", DELAY_NOTE_MOVEMENT_SAME);
		ballEffectsSlidersAndAttachments[i].delayNoteMovementComboBox.addItem("Up", DELAY_NOTE_MOVEMENT_UP);
		ballEffectsSlidersAndAttachments[i].delayNoteMovementComboBox.addItem("Down", DELAY_NOTE_MOVEMENT_DOWN);
		ballEffectsSlidersAndAttachments[i].delayNoteMovementComboBox.addItem("Up and Down", DELAY_NOTE_MOVEMENT_UP_DOWN);
		ballEffectsSlidersAndAttachments[i].delayNoteMovementComboBox.addItem("Down and Up", DELAY_NOTE_MOVEMENT_DOWN_UP);
		ballEffectsSlidersAndAttachments[i].delayNoteMovementComboBox.addItem("Random", DELAY_NOTE_MOVEMENT_RANDOM);
		ballEffectsSlidersAndAttachments[i].delayNoteMovementComboBox.setSelectedId(DELAY_NOTE_MOVEMENT_DEFAULT, juce::dontSendNotification);
		ballEffectsSlidersAndAttachments[i].delayNoteMovementComboBox.setColour(juce::ComboBox::textColourId, BUTTON_TEXT_COLOUR);
		ballEffectsSlidersAndAttachments[i].delayNoteMovementComboBox.setLookAndFeel(&this->m_costumeComboBoxLAF);
		ballEffectsSlidersAndAttachments[i].delayNoteMovementComboBox.toFront(false);
		content.addChildComponent(ballEffectsSlidersAndAttachments[i].delayNoteMovementComboBox);
	}

	// Set initial visibility for ball 0
	ballsSlidersAndAttachments[0].xSlider.setVisible(true);
	ballsSlidersAndAttachments[0].ySlider.setVisible(true);
	ballsSlidersAndAttachments[0].angleSlider.setVisible(true);
	ballsSlidersAndAttachments[0].radiusSlider.setVisible(true);
	ballsSlidersAndAttachments[0].velocitySlider.setVisible(true);
	ballsSlidersAndAttachments[0].xVelocitySlider.setVisible(false);
	ballsSlidersAndAttachments[0].yVelocitySlider.setVisible(false);
	ballsSlidersAndAttachments[0].xVelocityInverter.setVisible(false);
	ballsSlidersAndAttachments[0].yVelocityInverter.setVisible(false);
	ballEffectsSlidersAndAttachments[0].delayAmountSlider.setVisible(true);
	ballEffectsSlidersAndAttachments[0].delayFeedbackSlider.setVisible(true);
	ballEffectsSlidersAndAttachments[0].delayRateComboBox.setVisible(true);
	ballEffectsSlidersAndAttachments[0].delayNoteMovementComboBox.setVisible(true);

	// Start/Stop Button
	startStopButton.setButtonText("Start");
	startStopButton.onClick = [this]()
		{
			if ((GUIBalls[0]->active == false) && (GUIBalls[1]->active == false) && (GUIBalls[2]->active == false))
			{
				startStopButton.setButtonText("Start"); // good habbit or just trauma?
				return; // do not start if no balls are active
			}
			if (auto* toggleStateParam = audioProcessor.valueTreeState.getParameter("toggleState"))
			{
				bool changeTo = !toggleStateParam->getValue();
				toggleStateParam->setValueNotifyingHost(changeTo);
			}
			if (audioProcessor.getAreBallsMoving() == true)
			{
				startStopButton.setButtonText("Stop");
			}
			else
			{
				startStopButton.setButtonText("Start");
			}
		};
	startStopButton.setBounds(START_STOP_BUTTON_BOUNDS);
	startStopButton.setColour(juce::TextButton::ColourIds::buttonColourId, BUTTON_BG_COLOUR);
	startStopButton.setColour(juce::TextButton::ColourIds::textColourOffId, BUTTON_TEXT_COLOUR);
	content.addAndMakeVisible(startStopButton);

	// Add/Remove Ball Button
	addRemoveBallButton.setButtonText("Add");
	addRemoveBallButton.onClick = [this]()
		{
			if (audioProcessor.getAreBallsMoving() == true)
			{
				return; // Do not add/remove balls while the pit is playing
			}
			std::string BallActivationId = "BallActivation" + std::to_string(this->currentBallFocused);
			auto* activationParam = audioProcessor.valueTreeState.getParameter(BallActivationId);
			if (GUIBalls[this->currentBallFocused]->active == true)
			{
				addRemoveBallButton.setButtonText("Add");
				// balls[this->currentBallFocused]->setActive(false);
				if (activationParam) { activationParam->setValueNotifyingHost(false); }
			}
			else
			{
				addRemoveBallButton.setButtonText("Remove");
				// balls[this->currentBallFocused]->setActive(true);
				if (activationParam) { activationParam->setValueNotifyingHost(true); }
			}
		};
	addRemoveBallButton.setBounds(ADD_REMOVE_BUTTON_BOUNDS);
	addRemoveBallButton.setColour(juce::TextButton::ColourIds::buttonColourId, BUTTON_BG_COLOUR);
	addRemoveBallButton.setColour(juce::TextButton::ColourIds::textColourOffId, BUTTON_TEXT_COLOUR);
	content.addAndMakeVisible(addRemoveBallButton);

	// Edge Phase Slider
	edgePhaseSlider.setBounds(EDGE_PHASE_SLIDER_BOUNDS);
	edgePhaseSlider.setSliderStyle(juce::Slider::SliderStyle::MayT_PhaseKnob);
	edgePhaseSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	edgePhaseSlider.setDoubleClickReturnValue(true, PHASE_DOUBLE_CLICK_VALUE);
	edgePhaseSlider.setRange(EDGE_PHASE_MIN, EDGE_PHASE_MAX, EDGE_PHASE_STEP);
	edgePhaseSlider.setValue(EDGE_PHASE_DEFAULT);
	edgePhaseSlider.setLookAndFeel(&m_costumeDialLAF);
	content.addAndMakeVisible(edgePhaseSlider);

	// Edge Denominator Slider
	edgeDenomenatorSlider.setBounds(EDGE_DENOMENATOR_SLIDER_BOUNDS);
	edgeDenomenatorSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
	edgeDenomenatorSlider.setLookAndFeel(&this->m_costumeDialLAF);
	edgeDenomenatorSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	edgeDenomenatorSlider.setRange(EDGE_DENOMINATOR_MIN, EDGE_DENOMINATOR_MAX, EDGE_DENOMINATOR_STEP);
	edgeDenomenatorSlider.setValue(EDGE_DENOMINATOR_DEFAULT);
	content.addAndMakeVisible(edgeDenomenatorSlider);

	// Edge Range Slider
	edgeRangeSlider.setBounds(RANGE_SLIDER_BOUNDS);
	edgeRangeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
	edgeRangeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	edgeRangeSlider.setRange(EDGE_RANGE_MIN, EDGE_RANGE_MAX, EDGE_RANGE_STEP);
	edgeRangeSlider.setValue(EDGE_RANGE_DEFAULT);
	edgeRangeSlider.setLookAndFeel(&this->m_costumeDialLAF);
	content.addAndMakeVisible(edgeRangeSlider);

	// Scale Choice ComboBox
	scaleChoiceComboBox.setBounds(SCALE_COMBOBOX_BOUNDS);
	scaleChoiceComboBox.addItem("Chromatic", SCALE_CHROMATIC);
	scaleChoiceComboBox.addItem("Octatonic", SCALE_OCTATONIC);
	scaleChoiceComboBox.addItem("Dominant Diminished", SCALE_DOMINANT_DIMINISHED);
	scaleChoiceComboBox.addItem("Diminished", SCALE_DIMINISHED);
	scaleChoiceComboBox.addItem("Major", SCALE_MAJOR);
	scaleChoiceComboBox.addItem("Minor", SCALE_MINOR);
	scaleChoiceComboBox.addItem("Melodic Minor", SCALE_MELODIC_MINOR);
	scaleChoiceComboBox.addItem("Harmonic Minor", SCALE_HARMONIC_MINOR);
	scaleChoiceComboBox.addItem("Gypsy", SCALE_GYPSY);
	scaleChoiceComboBox.addItem("Symetrical", SCALE_SYMMETRICAL);
	scaleChoiceComboBox.addItem("Enigmatic", SCALE_ENIGMATIC);
	scaleChoiceComboBox.addItem("Arabian", SCALE_ARABIAN);
	scaleChoiceComboBox.addItem("Hungarian", SCALE_HUNGARIAN);
	scaleChoiceComboBox.addItem("Whole Tone", SCALE_WHOLE_TONE);
	scaleChoiceComboBox.addItem("Augmented", SCALE_AUGMENTED);
	scaleChoiceComboBox.addItem("Blues Major", SCALE_BLUES_MAJOR);
	scaleChoiceComboBox.addItem("Blues Minor", SCALE_BLUES_MINOR);
	scaleChoiceComboBox.addItem("Pentatonic", SCALE_PENTATONIC);
	scaleChoiceComboBox.addItem("Minor Pentatonic", SCALE_MINOR_PENTATONIC);
	scaleChoiceComboBox.setSelectedId(SCALE_DEFAULT);
	scaleChoiceComboBox.setColour(juce::ComboBox::textColourId, BUTTON_TEXT_COLOUR);
	scaleChoiceComboBox.setLookAndFeel(&m_costumeComboBoxLAF);
	content.addAndMakeVisible(scaleChoiceComboBox);

	// Root Note ComboBox
	rootNoteComboBox.setBounds(ROOT_NOTE_COMBOBOX_BOUNDS);
	rootNoteComboBox.addItem("C", ROOT_NOTE_C);
	rootNoteComboBox.addItem("C#", ROOT_NOTE_CSHARP);
	rootNoteComboBox.addItem("D", ROOT_NOTE_D);
	rootNoteComboBox.addItem("D#", ROOT_NOTE_DSHARP);
	rootNoteComboBox.addItem("E", ROOT_NOTE_E);
	rootNoteComboBox.addItem("F", ROOT_NOTE_F);
	rootNoteComboBox.addItem("F#", ROOT_NOTE_FSHARP);
	rootNoteComboBox.addItem("G", ROOT_NOTE_G);
	rootNoteComboBox.addItem("G#", ROOT_NOTE_GSHARP);
	rootNoteComboBox.addItem("A", ROOT_NOTE_A);
	rootNoteComboBox.addItem("A#", ROOT_NOTE_ASHARP);
	rootNoteComboBox.addItem("B", ROOT_NOTE_B);
	rootNoteComboBox.setSelectedId(ROOT_NOTE_DEFAULT);
	rootNoteComboBox.setColour(juce::ComboBox::textColourId, BUTTON_TEXT_COLOUR);
	rootNoteComboBox.setLookAndFeel(&m_costumeComboBoxLAF);
	content.addAndMakeVisible(rootNoteComboBox);

	// Edge Type ComboBox
	edgeTypeComboBox.setBounds(EDGE_TYPE_COMBOBOX_BOUNDS);
	edgeTypeComboBox.addItem("Cyclic up", EDGE_TYPE_CYCLIC_UP);
	edgeTypeComboBox.addItem("Cyclic down", EDGE_TYPE_CYCLIC_DOWN);
	edgeTypeComboBox.addItem("Ping pong", EDGE_TYPE_PING_PONG);
	edgeTypeComboBox.addItem("Random", EDGE_TYPE_RANDOM);
	edgeTypeComboBox.setSelectedId(EDGE_TYPE_DEFAULT);
	edgeTypeComboBox.setColour(juce::ComboBox::textColourId, BUTTON_TEXT_COLOUR);
	edgeTypeComboBox.setLookAndFeel(&m_costumeComboBoxLAF);
	content.addAndMakeVisible(edgeTypeComboBox);
	edgeTypeComboBox.addListener(this); // SPACIAL

	// Balls Positioning Type ComboBox
	ballsPositioningTypeComboBox.setBounds(BALLS_POSITIONING_TYPE_COMBOBOX_BOUNDS);
	ballsPositioningTypeComboBox.addItem("Chaos", BALLS_POSITIONING_CHAOS);
	ballsPositioningTypeComboBox.addItem("By Tempo", BALLS_POSITIONING_BY_TEMPO);
	ballsPositioningTypeComboBox.setSelectedId(BALLS_POSITIONING_DEFAULT);
	ballsPositioningTypeComboBox.setColour(juce::ComboBox::textColourId, BUTTON_TEXT_COLOUR);
	ballsPositioningTypeComboBox.setLookAndFeel(&m_costumeComboBoxLAF);
	content.addAndMakeVisible(ballsPositioningTypeComboBox);
	ballsPositioningTypeComboBox.addListener(this); // SPACIAL

	// Snap To Grid Button
	snapToGridButton.setBounds(SNAP_TO_GRID_BUTTON_BOUNDS);
	snapToGridButton.setToggleState(false, juce::dontSendNotification);
	snapToGridButton.setLookAndFeel(&this->m_costumeToggleButoonLAF);
	snapToGridButton.onClick = [this]()
		{
			if (snapToGridButton.getToggleState() == true)
			{
				changeXAndYToSnapToGrid();
			}
			else
			{
				changeXAndYToFree();
			}
		};
	content.addAndMakeVisible(snapToGridButton);

	// Collision Button
	collisionButton.setBounds(COLLISION_BUTTON_BOUNDS);
	collisionButton.setToggleState(false, juce::dontSendNotification);
	collisionButton.setLookAndFeel(&this->m_costumeCollisionLAF);
	content.addAndMakeVisible(collisionButton);

	// Quantization Slider
	quantizationSlider.setBounds(QUANTIZATION_KNOB_BOUNDS);
	quantizationSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	quantizationSlider.setLookAndFeel(&this->m_costumeDialLAF);
	quantizationSlider.setDoubleClickReturnValue(true, QUANTIZATION_DOUBLE_CLICK_VALUE);
	quantizationSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	quantizationSlider.setValue(QUANTIZATION_DEFAULT);
	quantizationSlider.setRange(QUANTIZATION_MIN, QUANTIZATION_MAX, QUANTIZATION_STEP);
	content.addAndMakeVisible(quantizationSlider);

	// Quantization Division ComboBox
	quantizationDivisionComboBox.setBounds(QUANTIZATION_DIVISION_COMBOBOX_BOUNDS);
	quantizationDivisionComboBox.addItem("1/32", QUANTIZATION_DIV_1_32);
	quantizationDivisionComboBox.addItem("1/16", QUANTIZATION_DIV_1_16);
	quantizationDivisionComboBox.addItem("1/8", QUANTIZATION_DIV_1_8);
	quantizationDivisionComboBox.addItem("1/4", QUANTIZATION_DIV_1_4);
	quantizationDivisionComboBox.setSelectedId(QUANTIZATION_DIV_DEFAULT);
	quantizationDivisionComboBox.setColour(juce::ComboBox::textColourId, BUTTON_TEXT_COLOUR);
	quantizationDivisionComboBox.setLookAndFeel(&m_costumeComboBoxLAF);
	content.addAndMakeVisible(quantizationDivisionComboBox);

	// volumeVariation Slider
	volumeVariationSlider.setBounds(VOLUME_VARIATION_KNOB_BOUNDS);
	volumeVariationSlider.setSliderStyle(juce::Slider::SliderStyle::MayT_VariationKnob);
	volumeVariationSlider.setLookAndFeel(&this->m_costumeDialLAF);
	volumeVariationSlider.setDoubleClickReturnValue(true, VOLUME_VARIATION_DOUBLE_CLICK_VALUE);
	volumeVariationSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	volumeVariationSlider.setValue(VOLUME_VARIATION_DEFAULT);
	volumeVariationSlider.setRange(VOLUME_VARIATION_MIN, VOLUME_VARIATION_MAX, VOLUME_VARIATION_STEP);
	content.addAndMakeVisible(volumeVariationSlider);

	// size percantage ComboBox
	sizePercentageComboBox.setBounds(SIZE_PERCENTAGE_COMBOBOX_BOUNDS);
	sizePercentageComboBox.addItem("100%", 1);
	sizePercentageComboBox.addItem("125%", 2);
	sizePercentageComboBox.addItem("150%", 3);
	sizePercentageComboBox.setSelectedId(SIZE_PERCENTAGE_DEFAULT);
	sizePercentageComboBox.setColour(juce::ComboBox::textColourId, BUTTON_TEXT_COLOUR);
	sizePercentageComboBox.setLookAndFeel(&m_costumeComboBoxLAF);
	content.addAndMakeVisible(sizePercentageComboBox);
	sizePercentageComboBox.addListener(this); // SPACIAL

	// Probability Slider
	probabilitySlider.setBounds(PROBABILITY_SLIDER_BOUNDS);
	probabilitySlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
	probabilitySlider.setLookAndFeel(&this->m_costumeHorizontalSliderLAF);
	probabilitySlider.setDoubleClickReturnValue(true, PROBABILITY_DOUBLE_CLICK_VALUE);
	probabilitySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	probabilitySlider.setValue(PROBABILITY_DEFAULT);
	probabilitySlider.setRange(PROBABILITY_MIN, PROBABILITY_MAX, PROBABILITY_STEP);
	content.addAndMakeVisible(probabilitySlider);


	// Preset Manager Button
	openPresetManager.setButtonText("Presets");
	openPresetManager.setBounds(PRESET_MANAGER_MENU_BUTTON_BOUNDS);
	openPresetManager.onClick = [this]()
		{
			if (presetPanel.isVisible())
			{
				presetPanel.setVisible(false);
			}
			else
			{
				presetPanel.setVisible(true);
				presetPanel.toFront(true); // Bring the presetPanel to the front
			}
		};
	openPresetManager.setColour(juce::TextButton::ColourIds::buttonColourId, BUTTON_BG_COLOUR);
	openPresetManager.setColour(juce::TextButton::ColourIds::textColourOffId, BUTTON_TEXT_COLOUR);
	content.addAndMakeVisible(openPresetManager);
}

//==============================================================================

static int getCurrentRectSize(int currentIndex, int noteRectSize)
{
	jassert((noteRectSize > 0) && (currentIndex > (-1)));

	int endOfColor = currentIndex + noteRectSize;

	if ((currentIndex < 392) && (endOfColor > 392))
	{
		return 392 - currentIndex;
	}
	else if ((currentIndex < 784) && (endOfColor > 784))
	{
		return 784 - currentIndex;
	}
	else if ((currentIndex < 1176) && (endOfColor > 1176))
	{
		return 1176 - currentIndex;
	}
	else if ((currentIndex < 1568) && (endOfColor > 1568))
	{
		return 1568 - currentIndex;
	}

	return -1; // no need
}

static void drawSingleRect(juce::Graphics& g, int index, int rectSizeToDraw)
{
	if (index < 392)
	{
		g.fillRect(PIT_MIN_X + 1, PIT_MIN_Y + 1 + (index % 392), PIT_EDGE_THICKNESS, rectSizeToDraw);
	}
	else if ((index >= 392) && (index < 784))
	{
		g.fillRect(PIT_MIN_X + 2 + (index % 392), PIT_MAX_Y - 3, rectSizeToDraw, PIT_EDGE_THICKNESS);
	}
	else if ((index >= 784) && (index < 1176))
	{
		g.fillRect(PIT_MAX_X - 3, PIT_MAX_Y - 1 - (index % 392) - rectSizeToDraw, PIT_EDGE_THICKNESS, rectSizeToDraw);
	}
	else if ((index >= 1176) && (index < 1568))
	{
		g.fillRect(PIT_MAX_X - 1 - rectSizeToDraw - (index % 392), PIT_MIN_Y + 1, rectSizeToDraw, PIT_EDGE_THICKNESS);
	}
}

static void drawSingleRectDivider(juce::Graphics& g, int dividerSize, int currentIndex)
{
	if (currentIndex < 392)
	{
		currentIndex = currentIndex - (dividerSize / 2);
		g.fillRect(PIT_MIN_X, PIT_MIN_Y + 1 + (currentIndex % 392), PIT_EDGE_THICKNESS + 1, dividerSize);
	}
	else if ((currentIndex >= 392) && (currentIndex < 784))
	{
		g.fillRect(PIT_MIN_X + (currentIndex % 392), PIT_MAX_Y - 3, dividerSize, PIT_EDGE_THICKNESS + 1);
	}
	else if ((currentIndex >= 784) && (currentIndex < 1176))
	{
		g.fillRect(PIT_MAX_X - 3, PIT_MAX_Y - 1 - (currentIndex % 392) - dividerSize, PIT_EDGE_THICKNESS + 1, dividerSize);
	}
	else if ((currentIndex >= 1176) && (currentIndex < 1568))
	{
		g.fillRect(PIT_MAX_X - 1 - dividerSize - (currentIndex % 392), PIT_MIN_Y, dividerSize, PIT_EDGE_THICKNESS + 1);
	}
}

static void drawRectDividers(juce::Graphics& g, int numOfSplits, int currentPhase)
{
	if (numOfSplits == 1)
		return;

	g.setColour(juce::Colours::white);
	int currentSplitPos = 0;
	int dividerSize = 4; // must be even
	jassert((numOfSplits > 0) && (dividerSize % 2 == 0));

	for (int i = 0; i < numOfSplits; i++)
	{
		currentSplitPos = (static_cast<int>(i * 1568 / numOfSplits) + currentPhase) % 1568;
		drawSingleRectDivider(g, dividerSize, currentSplitPos);
	}
}

void BallPitAudioProcessorEditor::drawPitEdge(juce::Graphics& g, juce::Colour* edgeColors)
{
	int numOfSplits = static_cast<int>(audioProcessor.valueTreeState.getRawParameterValue("edgeDenomenator")->load());
	int numOfColors = static_cast<int>(audioProcessor.valueTreeState.getRawParameterValue("edgeRange")->load());
	int currentPhase = juce::jmap<int>(audioProcessor.valueTreeState.getRawParameterValue("edgePhase")->load(), 0, 360, 0, 1567);
	if (currentPhase == 1567) { currentPhase = 0; } // weird but does the job.....
	const int* pointerToAbstractedEdgeColors = audioProcessor.abstractedEdgeColors.get();
	int noteRectSize = 1568 / numOfSplits;
	int reminder = 1568 % numOfSplits;
	int index = currentPhase;
	int currentRectSize = -1;
	g.setColour(edgeColors[pointerToAbstractedEdgeColors[index]]);

	for (int i = 0; i < numOfSplits;)
	{
		do
		{
			currentRectSize = getCurrentRectSize(index, noteRectSize);
			
			if (currentRectSize == -1)
			{
				drawSingleRect(g, index, noteRectSize);
				index = (index + noteRectSize) % 1568;
				if ((index > 1560) && (reminder != 0))
				{
					drawSingleRect(g, index, reminder);
					index = (index + reminder) % 1568;
					reminder = 0;
				}
				i++;
			}
			else
			{
				drawSingleRect(g, index, currentRectSize);
				index = (index + currentRectSize) % 1568;
				noteRectSize -= currentRectSize;
			}
		} while (currentRectSize > 0);

		noteRectSize = 1568 / numOfSplits;
		g.setColour(edgeColors[pointerToAbstractedEdgeColors[index]]);
	}

	if (numOfColors > 1)
	{
		drawRectDividers(g, numOfSplits, currentPhase);
	}

	if (reminder != 0) 
	{
		g.setColour(edgeColors[pointerToAbstractedEdgeColors[index]]);
		g.fillRect(PIT_MIN_X + 3, PIT_MIN_Y, reminder, 4);
	}
}

void BallPitAudioProcessorEditor::drawBall(juce::Graphics& g, const BallGUIEssentials currentBall) const
{
	juce::Colour ballColor;
	switch (currentBall.ballIndex)
	{
		case 0:
			ballColor = BALL_1_COLOUR;
			break;
		case 1:
			ballColor = BALL_2_COLOUR;
			break;
		case 2:
			ballColor = BALL_3_COLOUR;
			break;
	default:
		break;
	}

	float mouseOnMagnifier = (ballBeingDragged.first!= (int)MOUSE_NOT_IN_BALL) ? 6.0f : 0.0f;

	//black outline ball
	float x = currentBall.x;
 	float y = currentBall.y;
	float radius = currentBall.radius;
	double angle = currentBall.angle;
	double velocity = currentBall.velocity;

	g.setColour(juce::Colours::black);
	g.fillEllipse(x - radius - 1.0 - mouseOnMagnifier,
				  y - radius - 1.0 - mouseOnMagnifier,
				  radius * 2.0f + 2.0 + mouseOnMagnifier * 2,
				  radius * 2.0f + 2.0 + mouseOnMagnifier * 2);
	// colored ball
	g.setColour(ballColor);
	g.fillEllipse(x - radius - mouseOnMagnifier,
				  y - radius - mouseOnMagnifier,
				  radius * 2.0f + mouseOnMagnifier * 2,
				  radius * 2.0f + mouseOnMagnifier * 2);

	float angleInRadians = angle * (PI / 180.0f);
	float radiusRatio = pow(radius, 1.2);
	float arrowLenghtMultiplierByVelocity = 0.0f;
	if (currentBall.ballSpeedType == 1) // chaos
	{
		angleInRadians = (angle - 90.0f) * (PI / 180.0f);
		arrowLenghtMultiplierByVelocity = juce::jmap<float>(velocity, 0.0, 2000.0, 0.0, 30.0);
	}
	else // byTempo
	{
		arrowLenghtMultiplierByVelocity = juce::jmap<float>(velocity, 0.0, 8300.0, 0.0, 30.0);
	}

	if (currentBall.isMoving == false) // draw direction and length arrow and also delay balls
	{
		float startX = x + (5.0f + mouseOnMagnifier + radius) * cos(angleInRadians);
		float endX = x + (20.0f + mouseOnMagnifier + radiusRatio + arrowLenghtMultiplierByVelocity) * cos(angleInRadians);
		float startY = y + (5.0f + mouseOnMagnifier + radius) * sin(angleInRadians);
		float endY = y + (20.0f + mouseOnMagnifier + radiusRatio + arrowLenghtMultiplierByVelocity) * sin(angleInRadians);
		float arrowThickness = (2.5f + (radius / 6.0f));
		float arrowHeadWidth = (6.0f + (radius / 3.0f) + arrowLenghtMultiplierByVelocity / 3.0);
		float arrowHeadLength = (8.0f + (radius / 8.0f) + arrowLenghtMultiplierByVelocity / 3.0);
		juce::Line<float> line(startX, startY, endX, endY);
		startX = x + (3.0f + mouseOnMagnifier + radius) * cos(angleInRadians);
		endX = x + (24.0f + mouseOnMagnifier + radiusRatio + arrowLenghtMultiplierByVelocity) * cos(angleInRadians);
		startY = y + (3.0f + mouseOnMagnifier + radius) * sin(angleInRadians);
		endY = y + (24.0f + mouseOnMagnifier + radiusRatio + arrowLenghtMultiplierByVelocity) * sin(angleInRadians);
		juce::Line<float> thickLine(startX, startY, endX, endY);

		g.setColour(juce::Colours::black);
		g.drawArrow(thickLine, arrowThickness + 3.0f, arrowHeadWidth + 3.0f, arrowHeadLength + 5.0f);
		g.setColour(ballColor);
		g.drawArrow(line, arrowThickness, arrowHeadWidth, arrowHeadLength);

		// ----------- draw delay balls -----------
		if (currentBall.delayAmount <= 0)
		{
			return;
		}

		for (int i = 1; i < (currentBall.delayAmount + 1); i++)
		{
			float delaySizeRatio = pow(0.7f, i);
			float delayBallRadius = radius * delaySizeRatio;
			float delayBallX = x - (2.0f * radius * i + (currentBall.delayRate * i)) * cos(angleInRadians);
			float delayBallY = y - (2.0f * radius * i + (currentBall.delayRate * i)) * sin(angleInRadians);
			g.setColour(ballColor.withAlpha(0.1f + currentBall.delayFeedback * delaySizeRatio));
			g.fillEllipse(delayBallX - delayBallRadius, delayBallY - delayBallRadius, 
						  delayBallRadius * 2.0f, delayBallRadius * 2.0f);
		}
	}
	else
	{
		for (int i = 1; i < (currentBall.delayAmount + 1); i++)
		{
			float delaySizeRatio = pow(0.7f, i);
			float delayBallRadius = radius * delaySizeRatio;
			g.setColour(ballColor.withAlpha(0.1f + currentBall.delayFeedback * delaySizeRatio));
			g.fillEllipse(currentBall.delayPoints[i-1].getX() - delayBallRadius,
						  currentBall.delayPoints[i-1].getY() - delayBallRadius,
						  delayBallRadius * 2.0f,
						  delayBallRadius * 2.0f);
		}
	}
}

void BallPitAudioProcessorEditor::paint(juce::Graphics& g)
{
	g.fillAll(juce::Colours::black);

	g.addTransform(juce::AffineTransform::scale(sizePercentage));
	// draw background 
	if (backgroundDrawable != nullptr)
	{
		backgroundDrawable->setBounds(getLocalBounds() / sizePercentage);
		backgroundDrawable->draw(g, 1.0f);
	}

	// draw tabs
	if (tabsDrawable != nullptr)
	{
		juce::Rectangle<int> originalBounds = tabsDrawable->getBounds();
		float scaleX = BALLS_TABS_BOUNDS.getWidth() / static_cast<float>(originalBounds.getWidth());
		float scaleY = BALLS_TABS_BOUNDS.getHeight() / static_cast<float>(originalBounds.getHeight());
		juce::AffineTransform transform = juce::AffineTransform::scale(scaleX, scaleY).translated(BALLS_TABS_BOUNDS.getX(), BALLS_TABS_BOUNDS.getY());
		tabsDrawable->draw(g, 1.0f, transform);
	}

	// draw ballzzz
	for (int i = 0; i < 3; i++)
	{
		if (GUIBalls[i]->active == true)
		{
			if (std::isnan(GUIBalls[i]->x) || GUIBalls[i]->x < PIT_MIN_X || GUIBalls[i]->x > PIT_MAX_X)
			{
				// ball->setPosition(PIT_MIN_X + GUIBalls[i]->radius, GUIBalls[i]->y);
				GUIBalls[i]->x = PIT_MIN_X + GUIBalls[i]->radius;
				GUIBalls[i]->y = GUIBalls[i]->y;
				ballsSlidersAndAttachments[GUIBalls[i]->ballIndex].xSlider.setValue(PIT_MIN_X + GUIBalls[i]->radius, juce::sendNotification);
			}
			if (std::isnan(GUIBalls[i]->y) || GUIBalls[i]->y < PIT_MIN_Y || GUIBalls[i]->y > PIT_MAX_Y)
			{
				// ball->setPosition(GUIBalls[i]->x, PIT_MIN_Y + GUIBalls[i]->radius);
				GUIBalls[i]->x = GUIBalls[i]->x;
				GUIBalls[i]->y = PIT_MIN_Y + GUIBalls[i]->radius;
				ballsSlidersAndAttachments[GUIBalls[i]->ballIndex].ySlider.setValue(PIT_MIN_Y + GUIBalls[i]->radius, juce::sendNotification);
			}
			drawBall(g, *GUIBalls[i]);
		}
	}

	displayKnobsByTab();
	presetPanel.setPluginBounds(getLocalBounds());
	drawPitEdge(g, edgeColors);

	// draw pit corner "polls"
	g.setColour(juce::Colours::silver);
	g.fillEllipse(PIT_CORNER_TL);
	g.fillEllipse(PIT_CORNER_TR);
	g.fillEllipse(PIT_CORNER_BL);
	g.fillEllipse(PIT_CORNER_BR);

	drawTranspsoe(g);
}

void BallPitAudioProcessorEditor::resized()
{
	content.setBounds(0, 0, APP_WINDOW_WIDTH, APP_WINDOW_HIGHT); // or whatever your original design size was
}

void BallPitAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
	if (comboBoxThatHasChanged == &ballsPositioningTypeComboBox)
	{
		int ballsPosType = comboBoxThatHasChanged->getSelectedId();

		if (ballsPosType == 1)
		{
			auto backgroundSVGFile = juce::MemoryInputStream(BinaryData::ByKnobs_svg, BinaryData::ByKnobs_svgSize, false);
			backgroundDrawable = juce::Drawable::createFromImageDataStream(backgroundSVGFile);
		}
		else if (ballsPosType == 2)
		{
			auto backgroundSVGFile = juce::MemoryInputStream(BinaryData::ByTempo_svg, BinaryData::ByTempo_svgSize, false);
			backgroundDrawable = juce::Drawable::createFromImageDataStream(backgroundSVGFile);
		}
		
		repaint();
	}
	else if (comboBoxThatHasChanged == &sizePercentageComboBox)
	{
		int selectedSize = sizePercentageComboBox.getSelectedItemIndex();
		if (selectedSize == 1)
		{
			this->sizePercentage = SIZE_PERCENTAGE_125;
		}
		else if (selectedSize == 2)
		{
			this->sizePercentage = SIZE_PERCENTAGE_150;
		}
		else
		{
			this->sizePercentage = SIZE_PERCENTAGE_100;
		}

		content.setTransform(juce::AffineTransform::scale(sizePercentage));
		setSize((int)(APP_WINDOW_WIDTH * sizePercentage), (int)(APP_WINDOW_HIGHT * sizePercentage));
		resized();
	}
}

void BallPitAudioProcessorEditor::timerCallback() 
{
	auto snapshot = audioProcessor.latestBallsSnapshot.load(std::memory_order_acquire);
	if (snapshot) 
	{
    	for (const auto& ball : *snapshot) 
		{
			GUIBalls[ball.ballIndex]->ballIndex = ball.ballIndex;
        	GUIBalls[ball.ballIndex]->x = ball.x;
		 	GUIBalls[ball.ballIndex]->y = ball.y;
			GUIBalls[ball.ballIndex]->radius = ball.radius;
		 	GUIBalls[ball.ballIndex]->angle = ball.angle;
		 	GUIBalls[ball.ballIndex]->velocity = ball.velocity;
			GUIBalls[ball.ballIndex]->active = ball.active;
		 	GUIBalls[ball.ballIndex]->isMoving = ball.isMoving;
			GUIBalls[ball.ballIndex]->ballSpeedType = ball.ballSpeedType;
		 	GUIBalls[ball.ballIndex]->delayAmount = ball.delayAmount;
		 	GUIBalls[ball.ballIndex]->delayFeedback = ball.delayFeedback;
		 	GUIBalls[ball.ballIndex]->delayRate = ball.delayRate;
		 	GUIBalls[ball.ballIndex]->delayNoteMovement = ball.delayNoteMovement;
			GUIBalls[ball.ballIndex]->delayPoints[0] = ball.delayPoints[0];
			GUIBalls[ball.ballIndex]->delayPoints[1] = ball.delayPoints[1];
			GUIBalls[ball.ballIndex]->delayPoints[2] = ball.delayPoints[2];
    	}
	}
	repaint();
}

void BallPitAudioProcessorEditor::changeListenerCallback(juce::ChangeBroadcaster* source)
{
	if (source == &audioProcessor)
	{
		juce::Atomic<bool> isPlaying = audioProcessor.getIsPlaying();
		this->startStopButton.setToggleState(isPlaying.get(), juce::dontSendNotification);
		this->startStopButton.onClick();
	}
}

void BallPitAudioProcessorEditor::changeXAndYToSnapToGrid()
{
	lastBall1Position = { ballsSlidersAndAttachments[0].xSlider.getValue(), ballsSlidersAndAttachments[0].ySlider.getValue() };
	lastBall2Position = { ballsSlidersAndAttachments[1].xSlider.getValue(), ballsSlidersAndAttachments[1].ySlider.getValue() };
	lastBall3Position = { ballsSlidersAndAttachments[2].xSlider.getValue(), ballsSlidersAndAttachments[2].ySlider.getValue() };

	for (int i = 0; i < 3; i++)
	{
		ballsSlidersAndAttachments[i].xSlider.setRange(BALL_X_SLIDER_MIN, BALL_X_SLIDER_MAX, PIT_GRID_FACTOR);
		ballsSlidersAndAttachments[i].ySlider.setRange(BALL_Y_SLIDER_MIN, BALL_Y_SLIDER_MAX, PIT_GRID_FACTOR);

		std::string ballXId = "ballX" + std::to_string(i);
		std::string ballYId = "ballY" + std::to_string(i);

		float currentX = audioProcessor.valueTreeState.getRawParameterValue(ballXId)->load() - PIT_MIN_X;
		float currentY = audioProcessor.valueTreeState.getRawParameterValue(ballYId)->load() - PIT_MIN_Y;
		float snappedX = std::round(currentX / PIT_GRID_FACTOR);
		float snappedY = std::round(currentY / PIT_GRID_FACTOR);

		snappedX = juce::jlimit(0.0f, 9.0f, snappedX);
		snappedY = juce::jlimit(0.0f, 9.0f, snappedY);

		ballsSlidersAndAttachments[i].xSlider.setValue(snappedX, juce::sendNotification);
		ballsSlidersAndAttachments[i].ySlider.setValue(snappedY, juce::sendNotification);

		if (auto* xParam = audioProcessor.valueTreeState.getParameter(ballXId))
		{
			xParam->setValueNotifyingHost((snappedX * PIT_GRID_FACTOR) / PIT_WIDTH); // Scale to normalized 0-1 range
		}

		if (auto* yParam = audioProcessor.valueTreeState.getParameter(ballYId))
		{
			yParam->setValueNotifyingHost((snappedY * PIT_GRID_FACTOR) / PIT_WIDTH); // Scale to normalized 0-1 range
		}
	}
}

void BallPitAudioProcessorEditor::changeXAndYToFree()
{
	constexpr float gridFactor = 10.15f; // Scaling factor for snapping

	for (int i = 0; i < 3; i++)
	{
		ballsSlidersAndAttachments[i].xSlider.setRange(BALL_X_SLIDER_MIN, BALL_X_SLIDER_MAX, 10.15f);
		ballsSlidersAndAttachments[i].ySlider.setRange(BALL_Y_SLIDER_MIN, BALL_Y_SLIDER_MAX, 10.15f);

		std::string ballXId = "ballX" + std::to_string(i);
		std::string ballYId = "ballY" + std::to_string(i);

		float currentX = audioProcessor.valueTreeState.getRawParameterValue(ballXId)->load() - PIT_MIN_X;
		float currentY = audioProcessor.valueTreeState.getRawParameterValue(ballYId)->load() - PIT_MIN_Y;
		ballsSlidersAndAttachments[i].xSlider.setValue(currentX, juce::sendNotification);
		ballsSlidersAndAttachments[i].ySlider.setValue(currentY, juce::sendNotification);
		if (auto* xParam = audioProcessor.valueTreeState.getParameter(ballXId))
		{
			xParam->setValueNotifyingHost(currentX / PIT_WIDTH); // Scale to normalized 0-1 range
		}

		if (auto* yParam = audioProcessor.valueTreeState.getParameter(ballYId))
		{
			yParam->setValueNotifyingHost(currentY / PIT_WIDTH); // Scale to normalized 0-1 range
		}
	}
}

static float isMouseInsideBall(juce::Point<float> mousePosition, BallGUIEssentials currentBall, float sizePercentage)
{
	juce::Point<float> ballPosition(currentBall.x * sizePercentage, currentBall.y * sizePercentage);
	float distance = mousePosition.getDistanceFrom(ballPosition);
	return (distance <= currentBall.radius) ? distance : MOUSE_NOT_IN_BALL;
}

static bool isMouseOverEdgeDice(const juce::MouseEvent& event, float sizePercentage)
{
	if (event.position.x > EDGE_RANDOM_DICE_MIN_X * sizePercentage &&
		event.position.x < EDGE_RANDOM_DICE_MAX_X * sizePercentage &&
		event.position.y > EDGE_RANDOM_DICE_MIN_Y * sizePercentage &&
		event.position.y < EDGE_RANDOM_DICE_MAX_Y * sizePercentage)
	{
		return true;
	}

	return false;
}

static bool isMouseOverScaleDice(const juce::MouseEvent& event, float sizePercentage)
{
	if (event.position.x > SCALE_RANDOM_DICE_MIN_X * sizePercentage &&
		event.position.x < SCALE_RANDOM_DICE_MAX_X * sizePercentage &&
		event.position.y > SCALE_RANDOM_DICE_MIN_Y * sizePercentage &&
		event.position.y < SCALE_RANDOM_DICE_MAX_Y * sizePercentage)
	{
		return true;
	}

	return false;
}

static int isMouseOverTab(const juce::MouseEvent& event, float sizePercentage)
{
	if (event.position.x > BALLS_TAB_0_BOUNDS.getX() * sizePercentage &&
		event.position.x < (BALLS_TAB_0_BOUNDS.getX() + BALLS_TAB_0_BOUNDS.getWidth()) * sizePercentage)
	{
		if (event.position.y > BALLS_TAB_0_BOUNDS.getY() * sizePercentage &&
			event.position.y < (BALLS_TAB_0_BOUNDS.getY() + BALLS_TAB_0_BOUNDS.getHeight()) * sizePercentage)
		{
			return 0;
		}
		else if (event.position.y > BALLS_TAB_1_BOUNDS.getY() * sizePercentage &&
				 event.position.y < (BALLS_TAB_1_BOUNDS.getY() + BALLS_TAB_1_BOUNDS.getHeight()) * sizePercentage)
		{
			return 1;
		}
		else if (event.position.y > BALLS_TAB_2_BOUNDS.getY() * sizePercentage &&
				 event.position.y < (BALLS_TAB_2_BOUNDS.getY() + BALLS_TAB_2_BOUNDS.getHeight()) * sizePercentage)
		{
			return 2;
		}
		else
		{
			return MOUSE_NOT_IN_TAB;
		}
	}

	return MOUSE_NOT_IN_TAB;
}

static int isMouseOverTranspose(const juce::MouseEvent& event, float sizePercentage)
{
	if (event.position.y > TRANSPOSE_RECTANGLE_BOUNDS.getY() * sizePercentage &&
		event.position.y < (TRANSPOSE_RECTANGLE_BOUNDS.getY() + TRANSPOSE_RECTANGLE_BOUNDS.getHeight()) * sizePercentage)
	{
		if (event.position.x > TRANSPOSE_RECTANGLE_BOUNDS.getX() * sizePercentage &&
			event.position.x <= (TRANSPOSE_RECTANGLE_BOUNDS.getX() + 30) * sizePercentage)
		{
			return 0;
		}
		else if (event.position.x > (TRANSPOSE_RECTANGLE_BOUNDS.getX() + 30) * sizePercentage &&
				 event.position.x <= (TRANSPOSE_RECTANGLE_BOUNDS.getX() + 60) * sizePercentage)
		{
			return 1;
		}
		else if (event.position.x > (TRANSPOSE_RECTANGLE_BOUNDS.getX() + 60) * sizePercentage &&
				 event.position.x <= (TRANSPOSE_RECTANGLE_BOUNDS.getX() + 90) * sizePercentage)
		{
			return 2;
		}
		else if (event.position.x > (TRANSPOSE_RECTANGLE_BOUNDS.getX() + 90) * sizePercentage &&
				 event.position.x <= (TRANSPOSE_RECTANGLE_BOUNDS.getX() + 120) * sizePercentage)
		{
			return 3;
		}
		else if (event.position.x > (TRANSPOSE_RECTANGLE_BOUNDS.getX() + 120) * sizePercentage &&
				 event.position.x <= (TRANSPOSE_RECTANGLE_BOUNDS.getX() + 150) * sizePercentage)
		{
			return 4;
		}
	}

	return -1;
}

void BallPitAudioProcessorEditor::mouseMove(const juce::MouseEvent& event)
{
	float result = MOUSE_NOT_IN_BALL;
	for (int i = 0; i < 3; i++)
	{
		result = isMouseInsideBall(event.position, *GUIBalls[i], sizePercentage);
		if (result != MOUSE_NOT_IN_BALL)
		{
			ballBeingDragged.first = i;
			ballBeingDragged.second = result;
			return;
		}
		else
		{
			// TODO
		}
	}
	ballBeingDragged.first = (int)MOUSE_NOT_IN_BALL;
	ballBeingDragged.second = MOUSE_NOT_IN_BALL;
	
	if (isMouseOverEdgeDice(event, sizePercentage))
	{
		mouseOverEdgeDice = true;
		return;
	}
	mouseOverEdgeDice = false;

	if (isMouseOverScaleDice(event, sizePercentage))
	{
		mouseOverScaleDice = true;
		return;
	}
	mouseOverScaleDice = false;

	mouseOverTab = isMouseOverTab(event, sizePercentage);
	if (mouseOverTab != MOUSE_NOT_IN_TAB)
	{
		return;
	}
	mouseOverTab = MOUSE_NOT_IN_TAB;

	mouseOverTranspose = isMouseOverTranspose(event, sizePercentage);
	if (mouseOverTranspose != -1)
	{
		return;
	}
	mouseOverTranspose = -1;
}

void BallPitAudioProcessorEditor::mouseDown(const juce::MouseEvent& event)
{
	if (ballBeingDragged.first > (int)MOUSE_NOT_IN_BALL)
	{
		if (GUIBalls[ballBeingDragged.first]->active == true)
		{
			currentBallFocused = ballBeingDragged.first;
			setChosenTabIndex(currentBallFocused);
			if (currentBallFocused == 0)
			{
				auto tabsSVG = juce::MemoryInputStream(BinaryData::Ball_tab_0_svg, BinaryData::Ball_tab_0_svgSize, false);
				tabsDrawable = juce::Drawable::createFromImageDataStream(tabsSVG);
			}
			else if (currentBallFocused == 1)
			{
				auto tabsSVG = juce::MemoryInputStream(BinaryData::Ball_tab_1_svg, BinaryData::Ball_tab_1_svgSize, false);
				tabsDrawable = juce::Drawable::createFromImageDataStream(tabsSVG);
			}
			else if (currentBallFocused == 2)
			{
				auto tabsSVG = juce::MemoryInputStream(BinaryData::Ball_tab_2_svg, BinaryData::Ball_tab_2_svgSize, false);
				tabsDrawable = juce::Drawable::createFromImageDataStream(tabsSVG);
			}
			mouseIsDragging = true;
		}
	}
	else if (mouseOverEdgeDice == true)
	{
		edgeTypeComboBox.setSelectedId(4);
		auto* param = this->audioProcessor.valueTreeState.getRawParameterValue("edgeType");
		if (param != nullptr)
		{
			*param = 3; // random - 1 --> 4 - 1 = 3
		}
		this->audioProcessor.setWasGUIUpdated(true);
		// pit.setEdgeTypeToRandom();
	}
	else if (mouseOverScaleDice == true)
	{
		int randomScaleChoice = (rand() % 19) + 1;
		scaleChoiceComboBox.setSelectedId(randomScaleChoice); // 1 - 19
		auto* scaleChoiceParam = this->audioProcessor.valueTreeState.getRawParameterValue("scaleChoice");
		if (scaleChoiceParam != nullptr)
		{
			*scaleChoiceParam = randomScaleChoice;
		}
		int randomRootNote = (rand() % 12) + 1;
		rootNoteComboBox.setSelectedId(randomRootNote); // 1 - 12
		auto* rootNoteParam = this->audioProcessor.valueTreeState.getRawParameterValue("rootNote");
		if (rootNoteParam != nullptr)
		{
			*rootNoteParam = randomRootNote;
		}
		this->audioProcessor.setWasGUIUpdated(true);
	}
	else if (mouseOverTab != MOUSE_NOT_IN_TAB)
	{
		jassert(mouseOverTab >= 0 && mouseOverTab < 4);
		currentBallFocused = mouseOverTab;
		setChosenTabIndex(currentBallFocused);
		displayKnobsByTab();

		if (currentBallFocused == 0)
		{
			auto tabsSVG = juce::MemoryInputStream(BinaryData::Ball_tab_0_svg, BinaryData::Ball_tab_0_svgSize, false);
			tabsDrawable = juce::Drawable::createFromImageDataStream(tabsSVG);
		}
		else if (currentBallFocused == 1)
		{
			auto tabsSVG = juce::MemoryInputStream(BinaryData::Ball_tab_1_svg, BinaryData::Ball_tab_1_svgSize, false);
			tabsDrawable = juce::Drawable::createFromImageDataStream(tabsSVG);
		}
		else if (currentBallFocused == 2)
		{
			auto tabsSVG = juce::MemoryInputStream(BinaryData::Ball_tab_2_svg, BinaryData::Ball_tab_2_svgSize, false);
			tabsDrawable = juce::Drawable::createFromImageDataStream(tabsSVG);
		}
		mouseOverTab = MOUSE_NOT_IN_TAB;
	}
	else if (mouseOverTranspose != -1)
	{
		switch (mouseOverTranspose)
		{
		case 0:
			this->transpose = -24;
			break;
		case 1:
			this->transpose = -12;
			break;
		case 2:
			this->transpose = 0;
			break;
		case 3:
			this->transpose = 12;
			break;
		case 4:
			this->transpose = 24;
			break;
		default:
			this->transpose = 0; // Default to 0 if an unexpected value is encountered
			break;
		}

		if (auto* transposeParam = audioProcessor.valueTreeState.getParameter("transpose"))
		{
			auto& range = transposeParam->getNormalisableRange();
			float normalizedValue = range.convertTo0to1((float)this->transpose);
			transposeParam->setValueNotifyingHost(normalizedValue);
		}

		repaint();
		return;
	}
}

void BallPitAudioProcessorEditor::mouseDrag(const juce::MouseEvent& event)
{
	if ((mouseIsDragging) && (ballBeingDragged.first > (int)MOUSE_NOT_IN_BALL))
	{
		ballsSlidersAndAttachments[ballBeingDragged.first].xSlider.setValue(event.position.getX() / sizePercentage, juce::sendNotification);
		ballsSlidersAndAttachments[ballBeingDragged.first].ySlider.setValue(event.position.getY() / sizePercentage, juce::sendNotification);

		repaint();
	}
}

void BallPitAudioProcessorEditor::mouseUp(const juce::MouseEvent&)
{
	mouseIsDragging = false;
}

bool BallPitAudioProcessorEditor::keyPressed(const juce::KeyPress& key, juce::Component*)
{
	juce::UndoManager& m_undoManager = this->audioProcessor.getUndoManager();

	if (key == juce::KeyPress('z', juce::ModifierKeys::ctrlModifier, 0))
	{
		// Ctrl+Z (Undo)
		m_undoManager.undo();
		return true; // key handled
	}

	if (key == juce::KeyPress('z', juce::ModifierKeys::ctrlModifier | juce::ModifierKeys::shiftModifier, 0))
	{
		// Ctrl+Shift+Z (Redo)
		m_undoManager.redo();
		return true;
	}

	return false; // key not handled
}

void BallPitAudioProcessorEditor::drawTranspsoe(juce::Graphics& g)
{
	int chosenTranspose = 2;
	switch (this->transpose)
	{
	case -24:
		chosenTranspose = 0;
		break;
	case -12:
		chosenTranspose = 1;
		break;
	case 0:
		chosenTranspose = 2;
		break;
	case 12:
		chosenTranspose = 3;
		break;
	case 24:
		chosenTranspose = 4;
		break;
	default:
		chosenTranspose = 2; // Default to 0 if an unexpected value is encountered
		break;
	}

	g.setColour(BUTTON_BG_COLOUR.withAlpha(0.6f));
	g.fillRect((655 + 30 * chosenTranspose), 127, 29, 19);
	g.setColour(BUTTON_TEXT_COLOUR);
	g.drawText("-24", 656, 128, 28, 19, juce::Justification::centred);
	g.drawText("-12", 686, 128, 28, 19, juce::Justification::centred);
	g.drawText("0",   716, 128, 28, 19, juce::Justification::centred);
	g.drawText("12",  746, 128, 28, 19, juce::Justification::centred);
	g.drawText("24",  776, 128, 28, 19, juce::Justification::centred);
}