/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BallPitAudioProcessorEditor::BallPitAudioProcessorEditor (BallPitAudioProcessor& p)
	: AudioProcessorEditor (&p), presetPanel(p.getPresetManager()), audioProcessor (p), ballBeingDragged(MOUSE_NOT_IN_BALL, MOUSE_NOT_IN_BALL)
{
	//auto backgroundSVGFile = juce::File("C:/Users/97252/Desktop/computer_science/project/BallPit/Resources/ByKnobs.svg"); //laptop
	//auto backgroundSVGFile = juce::File("D:/Computer_Science/project/BallPit/Resources/ByKnobs.svg"); //bialik
	auto backgroundSVGFile = juce::File("D:/Plugin Laboratory/BallPit/Resources/ByKnobs.svg"); //haifa

	//auto tabsSVGFile = juce::File("C:/Users/97252/Desktop/computer_science/project/BallPit/Resources/Ball_tab_0.svg"); //laptop
	//auto tabsSVGFile = juce::File("D:/Computer_Science/project/BallPit/Resources/Ball_tab_0.svg"); //bialik
	auto tabsSVGFile = juce::File("D:/Plugin Laboratory/BallPit/Resources/Ball_tab_0.svg"); //haifa

	std::unique_ptr<juce::XmlElement> backgroundSVG(juce::XmlDocument::parse(backgroundSVGFile));
	if (backgroundSVG != nullptr) { backgroundDrawable = juce::Drawable::createFromSVG(*backgroundSVG); }
	
	std::unique_ptr<juce::XmlElement> tabsSVG(juce::XmlDocument::parse(tabsSVGFile));
	if (tabsSVG != nullptr) { tabsDrawable = juce::Drawable::createFromSVG(*tabsSVG); }

	setSize(APP_WINDOW_WIDTH, APP_WINDOW_HIGHT);
	startTimerHz(VISUAL_FRAMES_PER_SECOND);

	GUIState = juce::ValueTree("GUIState");

	// Configure and attach parameters and sliders
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

	audioProcessor.addChangeListener(this); // Register as listener

	addChildComponent(&presetPanel);

	initiateComponents();

	if (audioProcessor.getWasGUIUploaded() == false) // save default state only at first time gui is built
	{
		saveGUIState();
		audioProcessor.updateGUIFlag(true);
	}
	else
	{
		loadGUIState();
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
	}
	scaleChoiceComboBox.setLookAndFeel(nullptr);
	edgeTypeComboBox.setLookAndFeel(nullptr);
	rootNoteComboBox.setLookAndFeel(nullptr);
	ballsPositioningTypeComboBox.setLookAndFeel(nullptr);
	quantizationDivisionComboBox.setLookAndFeel(nullptr);
	quantizationSlider.setLookAndFeel(nullptr);

	edgeDenomenatorSlider.setLookAndFeel(nullptr);
	edgeRangeSlider.setLookAndFeel(nullptr);
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

		GUIState.setProperty(juce::Identifier(ballXId), ballsSlidersAndAttachments[i].xSlider.getValue(), nullptr);
		GUIState.setProperty(juce::Identifier(ballYId), ballsSlidersAndAttachments[i].ySlider.getValue(), nullptr);
		GUIState.setProperty(juce::Identifier(ballRadiusId), ballsSlidersAndAttachments[i].radiusSlider.getValue(), nullptr);
		GUIState.setProperty(juce::Identifier(ballVelocityId), ballsSlidersAndAttachments[i].velocitySlider.getValue(), nullptr);
		GUIState.setProperty(juce::Identifier(ballAngleId), ballsSlidersAndAttachments[i].angleSlider.getValue(), nullptr);
		GUIState.setProperty(juce::Identifier(ballXVelocityId), ballsSlidersAndAttachments[i].xVelocitySlider.getValue(), nullptr);
		GUIState.setProperty(juce::Identifier(ballYVelocityId), ballsSlidersAndAttachments[i].yVelocitySlider.getValue(), nullptr);
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

		ballsSlidersAndAttachments[i].xSlider.setValue(GUIState.getProperty(juce::Identifier(ballXId)), juce::dontSendNotification);
		ballsSlidersAndAttachments[i].ySlider.setValue(GUIState.getProperty(juce::Identifier(ballYId)), juce::dontSendNotification);
		ballsSlidersAndAttachments[i].radiusSlider.setValue(GUIState.getProperty(juce::Identifier(ballRadiusId)), juce::dontSendNotification);
		ballsSlidersAndAttachments[i].velocitySlider.setValue(GUIState.getProperty(juce::Identifier(ballVelocityId)), juce::dontSendNotification);
		ballsSlidersAndAttachments[i].angleSlider.setValue(GUIState.getProperty(juce::Identifier(ballAngleId)), juce::dontSendNotification);
		ballsSlidersAndAttachments[i].xVelocitySlider.setValue(GUIState.getProperty(juce::Identifier(ballXVelocityId)), juce::dontSendNotification);
		ballsSlidersAndAttachments[i].yVelocitySlider.setValue(GUIState.getProperty(juce::Identifier(ballYVelocityId)), juce::dontSendNotification);
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
}

void BallPitAudioProcessorEditor::displayKnobsByTab()
{
	int ballsPosType = 1 + audioProcessor.valueTreeState.getRawParameterValue("ballsPositioningType")->load(); // 1 is offset
	int currentTab = this->currentBallFocused;
	int otherTab1 = (currentTab + 1) % 3;
	int otherTab2 = (currentTab + 2) % 3;
	std::string ballXVelocityID = "ballXVelocity" + std::to_string(currentTab);
	std::string ballYVelocityID = "ballYVelocity" + std::to_string(currentTab);
	
	ballsSlidersAndAttachments[currentTab].xSlider.setVisible(true);
	ballsSlidersAndAttachments[currentTab].ySlider.setVisible(true);
	ballsSlidersAndAttachments[currentTab].radiusSlider.setVisible(true);
	switch (ballsPosType)
	{
		case 1: // chaos
		{
			ballsSlidersAndAttachments[currentTab].angleSlider.setVisible(true);
			ballsSlidersAndAttachments[currentTab].velocitySlider.setVisible(true);
			ballsSlidersAndAttachments[currentTab].xVelocitySlider.setVisible(false);
			ballsSlidersAndAttachments[currentTab].yVelocitySlider.setVisible(false);
			break;
		}
		case 2: // by tempo
		{
			ballsSlidersAndAttachments[currentTab].angleSlider.setVisible(false);
			ballsSlidersAndAttachments[currentTab].velocitySlider.setVisible(false);
			ballsSlidersAndAttachments[currentTab].xVelocitySlider.setVisible(true);
			ballsSlidersAndAttachments[currentTab].yVelocitySlider.setVisible(true);
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

	ballsSlidersAndAttachments[otherTab2].xSlider.setVisible(false);
	ballsSlidersAndAttachments[otherTab2].ySlider.setVisible(false);
	ballsSlidersAndAttachments[otherTab2].angleSlider.setVisible(false);
	ballsSlidersAndAttachments[otherTab2].radiusSlider.setVisible(false);
	ballsSlidersAndAttachments[otherTab2].velocitySlider.setVisible(false);
	ballsSlidersAndAttachments[otherTab2].xVelocitySlider.setVisible(false);
	ballsSlidersAndAttachments[otherTab2].yVelocitySlider.setVisible(false);
	
	if (audioProcessor.getPit().getBalls()[this->currentBallFocused]->isActive() == true)
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
		addChildComponent(ballsSlidersAndAttachments[i].xSlider);

		// Y Slider
		ballsSlidersAndAttachments[i].ySlider.setBounds(BALL_Y_KNOB_BOUNDS);
		ballsSlidersAndAttachments[i].ySlider.setValue(BALL_Y_DEFAULT);
		ballsSlidersAndAttachments[i].ySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		ballsSlidersAndAttachments[i].ySlider.setDoubleClickReturnValue(true, BALL_Y_DEFAULT);
		ballsSlidersAndAttachments[i].ySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballsSlidersAndAttachments[i].ySlider.setRange(BALL_Y_SLIDER_MIN, BALL_Y_SLIDER_MAX, BALL_Y_SLIDER_STEP);
		ballsSlidersAndAttachments[i].ySlider.toFront(false);
		ballsSlidersAndAttachments[i].ySlider.setLookAndFeel(&this->m_costumeDialLAF);
		addChildComponent(ballsSlidersAndAttachments[i].ySlider);

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
		addChildComponent(ballsSlidersAndAttachments[i].angleSlider);

		// Velocity Slider
		ballsSlidersAndAttachments[i].velocitySlider.setBounds(BALL_SPEED_KNOB_BOUNDS);
		ballsSlidersAndAttachments[i].velocitySlider.setValue(BALL_VELOCITY_DEFAULT);
		ballsSlidersAndAttachments[i].velocitySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		ballsSlidersAndAttachments[i].velocitySlider.setDoubleClickReturnValue(true, BALL_VELOCITY_DOUBLE_CLICK_VALUE);
		ballsSlidersAndAttachments[i].velocitySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballsSlidersAndAttachments[i].velocitySlider.setRange(BALL_VELOCITY_MIN, BALL_VELOCITY_MAX, BALL_VELOCITY_STEP);
		ballsSlidersAndAttachments[i].velocitySlider.toFront(false);
		ballsSlidersAndAttachments[i].velocitySlider.setLookAndFeel(&this->m_costumeDialLAF);
		addChildComponent(ballsSlidersAndAttachments[i].velocitySlider);

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
		addChildComponent(ballsSlidersAndAttachments[i].radiusSlider);

		// X Velocity Slider
		ballsSlidersAndAttachments[i].xVelocitySlider.setBounds(BALL_X_VELOCITY_KNOB_BOUNDS);
		ballsSlidersAndAttachments[i].xVelocitySlider.setValue(BALL_X_VELOCITY_DEFAULT);
		ballsSlidersAndAttachments[i].xVelocitySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		ballsSlidersAndAttachments[i].xVelocitySlider.setDoubleClickReturnValue(true, BALL_X_VELOCITY_DOUBLE_CLICK_VALUE);
		ballsSlidersAndAttachments[i].xVelocitySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballsSlidersAndAttachments[i].xVelocitySlider.setRange(BALL_X_VELOCITY_MIN, BALL_X_VELOCITY_MAX, BALL_X_VELOCITY_STEP);
		ballsSlidersAndAttachments[i].xVelocitySlider.toFront(false);
		ballsSlidersAndAttachments[i].xVelocitySlider.setLookAndFeel(&this->m_costumeDialLAF);
		ballsSlidersAndAttachments[i].xVelocitySlider.onValueChange = [this, i]()
			{
				ballsSlidersAndAttachments[i].xVelocitySlider.setValue(
					static_cast<int>(ballsSlidersAndAttachments[i].xVelocitySlider.getValue() + 0.5),
					juce::dontSendNotification
				);
			};
		addChildComponent(ballsSlidersAndAttachments[i].xVelocitySlider);

		// Y Velocity Slider
		ballsSlidersAndAttachments[i].yVelocitySlider.setBounds(BALL_Y_VELOCITY_KNOB_BOUNDS);
		ballsSlidersAndAttachments[i].yVelocitySlider.setValue(BALL_Y_VELOCITY_DEFAULT);
		ballsSlidersAndAttachments[i].yVelocitySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		ballsSlidersAndAttachments[i].yVelocitySlider.setDoubleClickReturnValue(true, BALL_Y_VELOCITY_DOUBLE_CLICK_VALUE);
		ballsSlidersAndAttachments[i].yVelocitySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballsSlidersAndAttachments[i].yVelocitySlider.setRange(BALL_Y_VELOCITY_MIN, BALL_Y_VELOCITY_MAX, BALL_Y_VELOCITY_STEP);
		ballsSlidersAndAttachments[i].yVelocitySlider.toFront(false);
		ballsSlidersAndAttachments[i].yVelocitySlider.setLookAndFeel(&this->m_costumeDialLAF);
		ballsSlidersAndAttachments[i].yVelocitySlider.onValueChange = [this, i]()
			{
				ballsSlidersAndAttachments[i].yVelocitySlider.setValue(
					static_cast<int>(ballsSlidersAndAttachments[i].yVelocitySlider.getValue() + 0.5),
					juce::dontSendNotification
				);
			};
		addChildComponent(ballsSlidersAndAttachments[i].yVelocitySlider);
	}

	// Set initial visibility for ball 0
	ballsSlidersAndAttachments[0].xSlider.setVisible(true);
	ballsSlidersAndAttachments[0].ySlider.setVisible(true);
	ballsSlidersAndAttachments[0].angleSlider.setVisible(true);
	ballsSlidersAndAttachments[0].radiusSlider.setVisible(true);
	ballsSlidersAndAttachments[0].velocitySlider.setVisible(true);
	ballsSlidersAndAttachments[0].xVelocitySlider.setVisible(false);
	ballsSlidersAndAttachments[0].yVelocitySlider.setVisible(false);

	// Start/Stop Button
	startStopButton.setButtonText("Start");
	startStopButton.onClick = [this]()
		{
			audioProcessor.togglePlayState();
			if (audioProcessor.getPit().areBallsMoving())
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
	addAndMakeVisible(startStopButton);

	// Add/Remove Ball Button
	addRemoveBallButton.setButtonText("Add");
	addRemoveBallButton.onClick = [this]()
		{
			if (audioProcessor.getPit().getBalls()[this->currentBallFocused]->isActive() == true)
			{
				addRemoveBallButton.setButtonText("Add");
				audioProcessor.getPit().getBalls()[this->currentBallFocused]->setActive(false);
			}
			else
			{
				addRemoveBallButton.setButtonText("Remove");
				audioProcessor.getPit().getBalls()[this->currentBallFocused]->setActive(true);
			}
		};
	addRemoveBallButton.setBounds(ADD_REMOVE_BUTTON_BOUNDS);
	addRemoveBallButton.setColour(juce::TextButton::ColourIds::buttonColourId, BUTTON_BG_COLOUR);
	addAndMakeVisible(addRemoveBallButton);

	// Edge Phase Slider
	edgePhaseSlider.setBounds(EDGE_PHASE_SLIDER_BOUNDS);
	edgePhaseSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
	edgePhaseSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	edgePhaseSlider.setRange(EDGE_PHASE_MIN, EDGE_PHASE_MAX, EDGE_PHASE_STEP);
	edgePhaseSlider.setValue(EDGE_PHASE_DEFAULT);
	addAndMakeVisible(edgePhaseSlider);

	// Edge Denominator Slider
	edgeDenomenatorSlider.setBounds(EDGE_DENOMENATOR_SLIDER_BOUNDS);
	edgeDenomenatorSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
	edgeDenomenatorSlider.setLookAndFeel(&this->m_costumeDialLAF);
	edgeDenomenatorSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	edgeDenomenatorSlider.setRange(EDGE_DENOMINATOR_MIN, EDGE_DENOMINATOR_MAX, EDGE_DENOMINATOR_STEP);
	edgeDenomenatorSlider.setValue(EDGE_DENOMINATOR_DEFAULT);
	addAndMakeVisible(edgeDenomenatorSlider);

	// Edge Range Slider
	edgeRangeSlider.setBounds(RANGE_SLIDER_BOUNDS);
	edgeRangeSlider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
	edgeRangeSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	edgeRangeSlider.setRange(EDGE_RANGE_MIN, EDGE_RANGE_MAX, EDGE_RANGE_STEP);
	edgeRangeSlider.setValue(EDGE_RANGE_DEFAULT);
	edgeRangeSlider.setLookAndFeel(&this->m_costumeDialLAF);
	addAndMakeVisible(edgeRangeSlider);

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
	scaleChoiceComboBox.setLookAndFeel(&m_costumeComboBoxLAF);
	addAndMakeVisible(scaleChoiceComboBox);

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
	rootNoteComboBox.setLookAndFeel(&m_costumeComboBoxLAF);
	addAndMakeVisible(rootNoteComboBox);

	// Edge Type ComboBox
	edgeTypeComboBox.setBounds(EDGE_TYPE_COMBOBOX_BOUNDS);
	edgeTypeComboBox.addItem("Cyclic up", EDGE_TYPE_CYCLIC_UP);
	edgeTypeComboBox.addItem("Cyclic down", EDGE_TYPE_CYCLIC_DOWN);
	edgeTypeComboBox.addItem("Ping pong", EDGE_TYPE_PING_PONG);
	edgeTypeComboBox.addItem("Random", EDGE_TYPE_RANDOM);
	edgeTypeComboBox.setSelectedId(EDGE_TYPE_DEFAULT);
	edgeTypeComboBox.setLookAndFeel(&m_costumeComboBoxLAF);
	addAndMakeVisible(edgeTypeComboBox);
	edgeTypeComboBox.addListener(this); // SPACIAL

	// Balls Positioning Type ComboBox
	ballsPositioningTypeComboBox.setBounds(BALLS_POSITIONING_TYPE_COMBOBOX_BOUNDS);
	ballsPositioningTypeComboBox.addItem("Chaos", BALLS_POSITIONING_CHAOS);
	ballsPositioningTypeComboBox.addItem("By Tempo", BALLS_POSITIONING_BY_TEMPO);
	ballsPositioningTypeComboBox.setSelectedId(BALLS_POSITIONING_DEFAULT);
	ballsPositioningTypeComboBox.setLookAndFeel(&m_costumeComboBoxLAF);
	addAndMakeVisible(ballsPositioningTypeComboBox);
	ballsPositioningTypeComboBox.addListener(this); // SPACIAL

	// Snap To Grid Button
	snapToGridButton.setBounds(SNAP_TO_GRID_BUTTON_BOUNDS);
	snapToGridButton.setToggleState(false, juce::dontSendNotification);
	snapToGridButton.setButtonText("Snap To Grid");
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
	addAndMakeVisible(snapToGridButton);

	// Collision Button
	collisionButton.setBounds(COLLISION_BUTTON_BOUNDS);
	collisionButton.setToggleState(true, juce::dontSendNotification);
	collisionButton.setButtonText("Collision");
	addAndMakeVisible(collisionButton);

	// Quantization Slider
	quantizationSlider.setBounds(QUANTIZATION_KNOB_BOUNDS);
	quantizationSlider.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
	quantizationSlider.setLookAndFeel(&this->m_costumeDialLAF);
	quantizationSlider.setDoubleClickReturnValue(true, QUANTIZATION_DOUBLE_CLICK_VALUE);
	quantizationSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
	quantizationSlider.setValue(QUANTIZATION_DEFAULT);
	quantizationSlider.setRange(QUANTIZATION_MIN, QUANTIZATION_MAX, QUANTIZATION_STEP);
	addAndMakeVisible(quantizationSlider);

	// Quantization Division ComboBox
	quantizationDivisionComboBox.setBounds(QUANTIZATION_DIVISION_COMBOBOX_BOUNDS);
	quantizationDivisionComboBox.addItem("1/32", QUANTIZATION_DIV_1_32);
	quantizationDivisionComboBox.addItem("1/16", QUANTIZATION_DIV_1_16);
	quantizationDivisionComboBox.addItem("1/8", QUANTIZATION_DIV_1_8);
	quantizationDivisionComboBox.addItem("1/4", QUANTIZATION_DIV_1_4);
	quantizationDivisionComboBox.setSelectedId(QUANTIZATION_DIV_DEFAULT);
	
	quantizationDivisionComboBox.setLookAndFeel(&m_costumeComboBoxLAF);
	addAndMakeVisible(quantizationDivisionComboBox);

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
	addAndMakeVisible(openPresetManager);
}

//==============================================================================

void BallPitAudioProcessorEditor::paint(juce::Graphics& g)
{
	g.fillAll(juce::Colours::black);

	// draw background 
	if (backgroundDrawable != nullptr)
	{
		backgroundDrawable->setBounds(getLocalBounds());
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
	const auto& balls = audioProcessor.getPit().getBalls();
	for (const auto& ball : balls)
	{
		if (ball->isActive() == true)
		{
			if (std::isnan(ball->getX()) || ball->getX() < PIT_MIN_X || ball->getX() > PIT_MAX_X)
			{
				ball->setPosition(PIT_MIN_X + ball->getRadius(), ball->getY());
				ballsSlidersAndAttachments[ball->getBallIndex()].xSlider.setValue(PIT_MIN_X + ball->getRadius(), juce::sendNotification);
			}
			if (std::isnan(ball->getY()) || ball->getY() < PIT_MIN_Y || ball->getY() > PIT_MAX_Y)
			{
				ball->setPosition(ball->getX(), PIT_MIN_Y + ball->getRadius());
				ballsSlidersAndAttachments[ball->getBallIndex()].ySlider.setValue(PIT_MIN_Y + ball->getRadius(), juce::sendNotification);
			}
			ball->draw(g);
		}
	}

	displayKnobsByTab();
	presetPanel.setPluginBounds(getLocalBounds());
	audioProcessor.getPit().drawPitEdge(g, edgeColors);

	// draw pit corner "polls"
	g.setColour(juce::Colours::silver);
	g.fillEllipse(PIT_CORNER_TL);
	g.fillEllipse(PIT_CORNER_TR);
	g.fillEllipse(PIT_CORNER_BL);
	g.fillEllipse(PIT_CORNER_BR);
}

void BallPitAudioProcessorEditor::resized()
{
}

void BallPitAudioProcessorEditor::comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged)
{
	if (comboBoxThatHasChanged == &ballsPositioningTypeComboBox)
	{
		int ballsPosType = comboBoxThatHasChanged->getSelectedId();
		juce::File svgFile;

		if (ballsPosType == 1)
		{
			svgFile = juce::File("D:/Plugin Laboratory/BallPit/Resources/ByKnobs.svg");
			//svgFile = juce::File("C:/Users/97252/Desktop/computer_science/project/BallPit/Resources/ByKnobs.svg"); //laptop

		}
		else if (ballsPosType == 2)
		{
			svgFile = juce::File("D:/Plugin Laboratory/BallPit/Resources/ByTempo.svg");
			//svgFile = juce::File("C:/Users/97252/Desktop/computer_science/project/BallPit/Resources/ByTempo.svg"); //laptop
		}

		if (svgFile.existsAsFile())
		{
			std::unique_ptr<juce::XmlElement> svgXml(juce::XmlDocument::parse(svgFile));
			if (svgXml != nullptr) { backgroundDrawable = juce::Drawable::createFromSVG(*svgXml); }
		}

		repaint();
	}
}

void BallPitAudioProcessorEditor::timerCallback() 
{
	repaint();
}

void BallPitAudioProcessorEditor::changeListenerCallback(juce::ChangeBroadcaster* source)
{
	if (source == &audioProcessor)
	{
		juce::Atomic<bool> isPlaying = audioProcessor.isPlaying.get();
		this->startStopButton.setToggleState(isPlaying.get(), juce::dontSendNotification);
		this->startStopButton.onClick();
	}
}

void BallPitAudioProcessorEditor::changeXAndYToSnapToGrid()
{
	lastBall1Position = { ballsSlidersAndAttachments[0].xSlider.getValue(), ballsSlidersAndAttachments[0].ySlider.getValue() };
	lastBall2Position = { ballsSlidersAndAttachments[1].xSlider.getValue(), ballsSlidersAndAttachments[1].ySlider.getValue() };
	lastBall3Position = { ballsSlidersAndAttachments[2].xSlider.getValue(), ballsSlidersAndAttachments[2].ySlider.getValue() };

	constexpr float gridFactor = 48.75f; // scaling factor for snapping-- 390 = 48.75px * 8 parts

	for (int i = 0; i < 3; i++)
	{
		ballsSlidersAndAttachments[i].xSlider.setRange(BALL_X_SLIDER_MIN, BALL_X_SLIDER_MAX, 48.75f);
		ballsSlidersAndAttachments[i].ySlider.setRange(BALL_Y_SLIDER_MIN, BALL_Y_SLIDER_MAX, 48.75f);

		std::string ballXId = "ballX" + std::to_string(i);
		std::string ballYId = "ballY" + std::to_string(i);

		float currentX = audioProcessor.valueTreeState.getRawParameterValue(ballXId)->load() - PIT_MIN_X;
		float currentY = audioProcessor.valueTreeState.getRawParameterValue(ballYId)->load() - PIT_MIN_Y;
		float snappedX = std::round(currentX / gridFactor);
		float snappedY = std::round(currentY / gridFactor);

		snappedX = juce::jlimit(0.0f, 9.0f, snappedX);
		snappedY = juce::jlimit(0.0f, 9.0f, snappedY);

		ballsSlidersAndAttachments[i].xSlider.setValue(snappedX, juce::sendNotification);
		ballsSlidersAndAttachments[i].ySlider.setValue(snappedY, juce::sendNotification);

		if (auto* xParam = audioProcessor.valueTreeState.getParameter(ballXId))
		{
			xParam->setValueNotifyingHost((snappedX * gridFactor) / PIT_WIDTH); // Scale to normalized 0-1 range
		}

		if (auto* yParam = audioProcessor.valueTreeState.getParameter(ballYId))
		{
			yParam->setValueNotifyingHost((snappedY * gridFactor) / PIT_WIDTH); // Scale to normalized 0-1 range
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

static bool isMouseOverEdgeDice(const juce::MouseEvent& event)
{
	if (event.position.x > EDGE_RANDOM_DICE_MIN_X &&
		event.position.x < EDGE_RANDOM_DICE_MAX_X &&
		event.position.y > EDGE_RANDOM_DICE_MIN_Y &&
		event.position.y < EDGE_RANDOM_DICE_MAX_Y)
	{
		return true;
	}

	return false;
}

static bool isMouseOverScaleDice(const juce::MouseEvent& event)
{
	if (event.position.x > SCALE_RANDOM_DICE_MIN_X &&
		event.position.x < SCALE_RANDOM_DICE_MAX_X &&
		event.position.y > SCALE_RANDOM_DICE_MIN_Y &&
		event.position.y < SCALE_RANDOM_DICE_MAX_Y)
	{
		return true;
	}

	return false;
}

static int isMouseOverTab(const juce::MouseEvent& event)
{
	if (event.position.x > BALLS_TAB_0_BOUNDS.getX() &&
		event.position.x < BALLS_TAB_0_BOUNDS.getX() + BALLS_TAB_0_BOUNDS.getWidth())
	{
		if (event.position.y > BALLS_TAB_0_BOUNDS.getY() &&
			event.position.y < BALLS_TAB_0_BOUNDS.getY() + BALLS_TAB_0_BOUNDS.getHeight())
		{
			return 0;
		}
		else if (event.position.y > BALLS_TAB_1_BOUNDS.getY() &&
				 event.position.y < BALLS_TAB_1_BOUNDS.getY() + BALLS_TAB_1_BOUNDS.getHeight())
		{
			return 1;
		}
		else if (event.position.y > BALLS_TAB_2_BOUNDS.getY() &&
				 event.position.y < BALLS_TAB_2_BOUNDS.getY() + BALLS_TAB_2_BOUNDS.getHeight())
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

void BallPitAudioProcessorEditor::mouseMove(const juce::MouseEvent& event)
{
	float result = MOUSE_NOT_IN_BALL;
	for (const auto& ball : this->audioProcessor.getPit().getBalls())
	{
		result = ball->isMouseInsineBall(event.position);
		if (result != MOUSE_NOT_IN_BALL)
		{
			ballBeingDragged.first = ball->getBallIndex();
			ballBeingDragged.second = result;
			this->audioProcessor.getPit().getBalls()[ball->getBallIndex()]->setIsMouseOverBall(true);
			return;
		}
		else
		{
			this->audioProcessor.getPit().getBalls()[ball->getBallIndex()]->setIsMouseOverBall(false);
		}
	}
	ballBeingDragged.first = MOUSE_NOT_IN_BALL;
	ballBeingDragged.second = MOUSE_NOT_IN_BALL;
	
	if (isMouseOverEdgeDice(event))
	{
		mouseOverEdgeDice = true;
		return;
	}
	mouseOverEdgeDice = false;

	if (isMouseOverScaleDice(event))
	{
		mouseOverScaleDice = true;
		return;
	}
	mouseOverScaleDice = false;

	mouseOverTab = isMouseOverTab(event);
	if (mouseOverTab != MOUSE_NOT_IN_TAB)
	{
		return;
	}
	mouseOverTab = MOUSE_NOT_IN_TAB;
}

void BallPitAudioProcessorEditor::mouseDown(const juce::MouseEvent& event)
{
	if (ballBeingDragged.first > MOUSE_NOT_IN_BALL)
	{
		if (this->audioProcessor.getPit().getBalls()[ballBeingDragged.first]->isActive() == true)
		{
			mouseIsDragging = true;
		}
	}
	else if (mouseOverEdgeDice == true)
	{
		edgeTypeComboBox.setSelectedId(4);
		auto* param = this->audioProcessor.valueTreeState.getRawParameterValue("edgeType");
		if (param != nullptr)
		{
			*param = 2;
		}
		this->audioProcessor.getPit().setEdgeTypeToRandom();
		this->audioProcessor.setWasGUIUpdatedToTrue();
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
		this->audioProcessor.getPit().setEdgeTypeToRandom();
		this->audioProcessor.setWasGUIUpdatedToTrue();
	}
	else if (mouseOverTab != MOUSE_NOT_IN_TAB)
	{
		jassert(mouseOverTab >= 0 && mouseOverTab < 4);
		currentBallFocused = mouseOverTab;
		displayKnobsByTab();

		//std::string ballTabId = "C:/Users/97252/Desktop/computer_science/project/BallPit/Resources/Ball_tab_" + std::to_string(currentBallFocused) + ".svg";
		//std::string ballTabId = "D:/Computer_Science/project/BallPit/Resources/Ball_tab_" + std::to_string(currentBallFocused) + ".svg";
		std::string ballTabId = "D:/Plugin Laboratory/BallPit/Resources/Ball_tab_" + std::to_string(currentBallFocused) + ".svg";
		auto tabsSVGFile = juce::File(ballTabId);
		std::unique_ptr<juce::XmlElement> tabsSVG(juce::XmlDocument::parse(tabsSVGFile));
		if (tabsSVG != nullptr) { tabsDrawable = juce::Drawable::createFromSVG(*tabsSVG); }

		mouseOverTab = MOUSE_NOT_IN_TAB;
	}
}

void BallPitAudioProcessorEditor::mouseDrag(const juce::MouseEvent& event)
{
	if ((mouseIsDragging) && (ballBeingDragged.first > MOUSE_NOT_IN_BALL))
	{
		ballsSlidersAndAttachments[ballBeingDragged.first].xSlider.setValue(event.position.getX(), juce::sendNotification);
		ballsSlidersAndAttachments[ballBeingDragged.first].ySlider.setValue(event.position.getY(), juce::sendNotification);

		repaint();
	}
}

void BallPitAudioProcessorEditor::mouseUp(const juce::MouseEvent&)
{
	mouseIsDragging = false;
}
