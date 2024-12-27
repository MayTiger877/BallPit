/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BallPitAudioProcessorEditor::BallPitAudioProcessorEditor (BallPitAudioProcessor& p)
	: AudioProcessorEditor (&p), presetPanel(p.getPresetManager()), audioProcessor (p)
{
	//auto svgFile = juce::File("C:/Users/97252/Desktop/computer_science/project/BallPit/Resources/LayOut.svg"); //laptop
	//auto svgFile = juce::File("D:/Computer_Science/project/BallPit/Resources/LayOut.svg"); //bialik
	auto svgFile = juce::File("D:/Plugin Laboratory/BallPit/Resources/LayOut.svg"); //haifa

	std::unique_ptr<juce::XmlElement> svgXml(juce::XmlDocument::parse(svgFile));
	if (svgXml != nullptr) { drawable = juce::Drawable::createFromSVG(*svgXml); }

	// Tabs
	tabs.reset(new juce::TabbedComponent(juce::TabbedButtonBar::TabsAtTop));
	for (int i = 0; i < 3; ++i)
	{
		auto ballControlComponent = std::make_unique<BallSlidersAndAttachments>();
		tabs->addTab("Ball " + std::to_string(i + 1), juce::Colours::grey, ballControlComponent.release(), true);
	}
	tabs->setBounds(407, 10, 421, 398);
	addAndMakeVisible(tabs.get());
	

	setSize(836, 754); // TODO - set back to original (836, 654) after removing logger
	startTimerHz(60);

	this->newGUIState = juce::ValueTree("newGUIState");

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

	std::string ballsPositioningTypeID = "ballsPositioningType";
	ballsPositioningTypeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.valueTreeState, ballsPositioningTypeID, ballsPositioningTypeComboBox);

	std::string snapToGridID = "snapToGrid";
	snapToGridAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.valueTreeState, snapToGridID, snapToGridButton);

	std::string collisionID = "collision";
	collisionAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(audioProcessor.valueTreeState, collisionID, collisionButton);
	
	audioProcessor.addChangeListener(this); // Register as listener

	logBox.setMultiLine(true);
	logBox.setReadOnly(true);
	logBox.setScrollbarsShown(true);
	logBox.setFont(juce::Font(16.0f));
	logBox.setColour(juce::TextEditor::backgroundColourId, juce::Colours::black);
	logBox.setColour(juce::TextEditor::textColourId, juce::Colours::white);
	logBox.setColour(juce::TextEditor::highlightColourId, juce::Colours::yellow);
	addAndMakeVisible(logBox);

	addChildComponent(&presetPanel);

	initiateComponents();
}

BallPitAudioProcessorEditor::~BallPitAudioProcessorEditor()
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

		this->newGUIState.setProperty(juce::Identifier(ballXId), audioProcessor.valueTreeState.getRawParameterValue(ballXId)->load(), nullptr);
		this->newGUIState.setProperty(juce::Identifier(ballYId), audioProcessor.valueTreeState.getRawParameterValue(ballYId)->load(), nullptr);
		this->newGUIState.setProperty(juce::Identifier(ballRadiusId), audioProcessor.valueTreeState.getRawParameterValue(ballRadiusId)->load(), nullptr);
		this->newGUIState.setProperty(juce::Identifier(ballVelocityId), audioProcessor.valueTreeState.getRawParameterValue(ballVelocityId)->load(), nullptr);
		this->newGUIState.setProperty(juce::Identifier(ballAngleId), audioProcessor.valueTreeState.getRawParameterValue(ballAngleId)->load(), nullptr);
		this->newGUIState.setProperty(juce::Identifier(ballXVelocityId), audioProcessor.valueTreeState.getRawParameterValue(ballXVelocityId)->load(), nullptr);
		this->newGUIState.setProperty(juce::Identifier(ballYVelocityId), audioProcessor.valueTreeState.getRawParameterValue(ballYVelocityId)->load(), nullptr);
	}

	this->newGUIState.setProperty("edgePhase", audioProcessor.valueTreeState.getRawParameterValue("edgePhase")->load(), nullptr);
	this->newGUIState.setProperty("edgeDenomenator", audioProcessor.valueTreeState.getRawParameterValue("edgeDenomenator")->load(), nullptr);
	this->newGUIState.setProperty("edgeRange", audioProcessor.valueTreeState.getRawParameterValue("edgeRange")->load(), nullptr);
	this->newGUIState.setProperty("scaleChoice", audioProcessor.valueTreeState.getRawParameterValue("scaleChoice")->load(), nullptr);
	this->newGUIState.setProperty("rootNote", audioProcessor.valueTreeState.getRawParameterValue("rootNote")->load(), nullptr);
	this->newGUIState.setProperty("ballsPositioningType", audioProcessor.valueTreeState.getRawParameterValue("ballsPositioningType")->load(), nullptr);
	this->newGUIState.setProperty("snapToGrid", audioProcessor.valueTreeState.getRawParameterValue("snapToGrid")->load(), nullptr);
	this->newGUIState.setProperty("collision", audioProcessor.valueTreeState.getRawParameterValue("collision")->load(), nullptr);

	audioProcessor.saveGUIState(this->newGUIState);
	audioProcessor.updateGUIFlag(false);
	audioProcessor.removeChangeListener(this);
}

void BallPitAudioProcessorEditor::loadFromProcessorState()
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
	ballsPositioningTypeComboBox.setSelectedItemIndex(GUIState.getProperty("ballsPositioningType"), juce::dontSendNotification);
	snapToGridButton.setToggleState(GUIState.getProperty("snapToGrid"), juce::dontSendNotification);
	collisionButton.setToggleState(GUIState.getProperty("collision"), juce::dontSendNotification);
}

void BallPitAudioProcessorEditor::displayKnobsByTab()
{
	int ballsPosType = 1 + audioProcessor.valueTreeState.getRawParameterValue("ballsPositioningType")->load(); // 1 is offset
	int currentTab = tabs->getCurrentTabIndex();
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
	
	if (audioProcessor.getPit().getBalls()[tabs->getCurrentTabIndex()]->isActive() == true)
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
		ballsSlidersAndAttachments[i].xSlider.setBounds(656, 65, 65, 65);
		ballsSlidersAndAttachments[i].xSlider.setValue(75.0f + (i * 130.0f));
		ballsSlidersAndAttachments[i].xSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		ballsSlidersAndAttachments[i].xSlider.setDoubleClickReturnValue(true, 50.0f);
		ballsSlidersAndAttachments[i].xSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballsSlidersAndAttachments[i].xSlider.setRange(0.0f, 390.0f, 10.0f);
		ballsSlidersAndAttachments[i].xSlider.toFront(false);
		addChildComponent(ballsSlidersAndAttachments[i].xSlider);

		ballsSlidersAndAttachments[i].ySlider.setBounds(744, 65, 65, 65);
		ballsSlidersAndAttachments[i].ySlider.setValue(200.0f);
		ballsSlidersAndAttachments[i].ySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		ballsSlidersAndAttachments[i].ySlider.setDoubleClickReturnValue(true, 50.0f);
		ballsSlidersAndAttachments[i].ySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballsSlidersAndAttachments[i].ySlider.setRange(0.0f, 390.0f, 10.0f);
		ballsSlidersAndAttachments[i].ySlider.toFront(false);
		addChildComponent(ballsSlidersAndAttachments[i].ySlider);

		ballsSlidersAndAttachments[i].angleSlider.setBounds(656, 115, 65, 65);
		ballsSlidersAndAttachments[i].angleSlider.setValue(0.0f + (i * 60.0f));
		ballsSlidersAndAttachments[i].angleSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		ballsSlidersAndAttachments[i].angleSlider.setDoubleClickReturnValue(true, 0.0f);
		ballsSlidersAndAttachments[i].angleSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballsSlidersAndAttachments[i].angleSlider.setRotaryParameters(0, 2*juce::MathConstants<double>::pi, false);
		ballsSlidersAndAttachments[i].angleSlider.setRange(0.0f, 360.0f, 1.0f);
		ballsSlidersAndAttachments[i].angleSlider.toFront(false);
		addChildComponent(ballsSlidersAndAttachments[i].angleSlider);

		ballsSlidersAndAttachments[i].velocitySlider.setBounds(744, 115, 65, 65);
		ballsSlidersAndAttachments[i].velocitySlider.setValue(5.0f);
		ballsSlidersAndAttachments[i].velocitySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		ballsSlidersAndAttachments[i].velocitySlider.setDoubleClickReturnValue(true, 3.0f);
		ballsSlidersAndAttachments[i].velocitySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballsSlidersAndAttachments[i].velocitySlider.setRange(0.0f, 10.0f, 0.5f);
		ballsSlidersAndAttachments[i].velocitySlider.toFront(false);
		addChildComponent(ballsSlidersAndAttachments[i].velocitySlider);

		ballsSlidersAndAttachments[i].radiusSlider.setBounds(656, 165, 65, 65);
		ballsSlidersAndAttachments[i].radiusSlider.setValue(10.0f);
		ballsSlidersAndAttachments[i].radiusSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		ballsSlidersAndAttachments[i].radiusSlider.setDoubleClickReturnValue(true, 10.0f);
		ballsSlidersAndAttachments[i].radiusSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballsSlidersAndAttachments[i].radiusSlider.setRange(5.0f, 25.0f, 0.5f);
		ballsSlidersAndAttachments[i].radiusSlider.toFront(false);
		addChildComponent(ballsSlidersAndAttachments[i].radiusSlider);

		ballsSlidersAndAttachments[i].xVelocitySlider.setBounds(656, 115, 65, 65);
		ballsSlidersAndAttachments[i].xVelocitySlider.setValue(1.0f);
		ballsSlidersAndAttachments[i].xVelocitySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		ballsSlidersAndAttachments[i].xVelocitySlider.setDoubleClickReturnValue(true, 1.0f);
		ballsSlidersAndAttachments[i].xVelocitySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballsSlidersAndAttachments[i].xVelocitySlider.setRange(0.0f, 10.0f, 1.0f);
		ballsSlidersAndAttachments[i].xVelocitySlider.toFront(false);
		ballsSlidersAndAttachments[i].xVelocitySlider.onValueChange = [this, i]()
		{
			ballsSlidersAndAttachments[i].xVelocitySlider.setValue(
				static_cast<int>(ballsSlidersAndAttachments[i].xVelocitySlider.getValue() + 0.5),
				juce::dontSendNotification
			);
		};
		addChildComponent(ballsSlidersAndAttachments[i].xVelocitySlider);

		ballsSlidersAndAttachments[i].yVelocitySlider.setBounds(744, 115, 65, 65);
		ballsSlidersAndAttachments[i].yVelocitySlider.setValue(1.0f);
		ballsSlidersAndAttachments[i].yVelocitySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		ballsSlidersAndAttachments[i].yVelocitySlider.setDoubleClickReturnValue(true, 1.0f);
		ballsSlidersAndAttachments[i].yVelocitySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballsSlidersAndAttachments[i].yVelocitySlider.setRange(0.0f, 10.0f, 1.0f);
		ballsSlidersAndAttachments[i].yVelocitySlider.toFront(false);
		ballsSlidersAndAttachments[i].yVelocitySlider.onValueChange = [this, i]()
		{
			ballsSlidersAndAttachments[i].yVelocitySlider.setValue(
				static_cast<int>(ballsSlidersAndAttachments[i].yVelocitySlider.getValue() + 0.5),
				juce::dontSendNotification
			);
		};
		addChildComponent(ballsSlidersAndAttachments[i].yVelocitySlider);
	}
	
	ballsSlidersAndAttachments[0].xSlider.setVisible(true);
	ballsSlidersAndAttachments[0].ySlider.setVisible(true);
	ballsSlidersAndAttachments[0].angleSlider.setVisible(true);
	ballsSlidersAndAttachments[0].radiusSlider.setVisible(true);
	ballsSlidersAndAttachments[0].velocitySlider.setVisible(true);
	ballsSlidersAndAttachments[0].xVelocitySlider.setVisible(false);
	ballsSlidersAndAttachments[0].yVelocitySlider.setVisible(false);
	
	startStopButton.setButtonText("Start");
	startStopButton.onClick = [this]()
	{
		audioProcessor.getPit().checkBallsPositionBeforeStart();
		audioProcessor.getPit().toggleBallMovement();
		if (audioProcessor.getPit().areBallsMoving())
			startStopButton.setButtonText("Stop");
		else
			startStopButton.setButtonText("Start");
	};
	startStopButton.setBounds(420, 350, 140, 40);
	addAndMakeVisible(startStopButton);

	addRemoveBallButton.setButtonText("Add");
	addRemoveBallButton.onClick = [this]()
	{
		if (audioProcessor.getPit().getBalls()[tabs->getCurrentTabIndex()]->isActive() == true)
		{
			addRemoveBallButton.setButtonText("Add");
			audioProcessor.getPit().getBalls()[tabs->getCurrentTabIndex()]->setActive(false);
		}
		else
		{
			addRemoveBallButton.setButtonText("Remove");
			audioProcessor.getPit().getBalls()[tabs->getCurrentTabIndex()]->setActive(true);
		}
	};
	addRemoveBallButton.setBounds(580, 350, 140, 40);
	addAndMakeVisible(addRemoveBallButton);

	edgePhaseSlider.setBounds(200, 475, 100, 30);
	edgePhaseSlider.setSliderStyle(juce::Slider::SliderStyle::IncDecButtons);
	edgePhaseSlider.setTextBoxStyle(juce::Slider::TextBoxRight, true, 50, 30);
	edgePhaseSlider.setRange(0, 360, 10);
	edgePhaseSlider.setValue(0);
	addAndMakeVisible(edgePhaseSlider);
	
	edgeDenomenatorSlider.setBounds(200, 540, 100, 30);
	edgeDenomenatorSlider.setSliderStyle(juce::Slider::SliderStyle::IncDecButtons);
	edgeDenomenatorSlider.setTextBoxStyle(juce::Slider::TextBoxRight, true, 50, 30);
	edgeDenomenatorSlider.setRange(1, 8, 1);
	edgeDenomenatorSlider.setValue(1);
	addAndMakeVisible(edgeDenomenatorSlider);

	edgeRangeSlider.setBounds(630, 540, 100, 30);
	edgeRangeSlider.setSliderStyle(juce::Slider::SliderStyle::IncDecButtons);
	edgeRangeSlider.setTextBoxStyle(juce::Slider::TextBoxRight, true, 50, 30);
	edgeRangeSlider.setRange(1, 8, 1);
	edgeRangeSlider.setValue(2);
	addAndMakeVisible(edgeRangeSlider);

	scaleChoiceComboBox.setBounds(630, 475, 100, 30);
	scaleChoiceComboBox.addItem("Chromatic", 1);
	scaleChoiceComboBox.addItem("Octatonic", 2);
	scaleChoiceComboBox.addItem("Dominant Diminished", 3);
	scaleChoiceComboBox.addItem("Diminished", 4);
	scaleChoiceComboBox.addItem("Major", 5);
	scaleChoiceComboBox.addItem("Minor", 6);
	scaleChoiceComboBox.addItem("Melodic Minor", 7);
	scaleChoiceComboBox.addItem("Harmonic Minor", 8);
	scaleChoiceComboBox.addItem("Gypsy", 9);
	scaleChoiceComboBox.addItem("Symetrical", 10);
	scaleChoiceComboBox.addItem("Enigmatic", 11);
	scaleChoiceComboBox.addItem("Arabian", 12);
	scaleChoiceComboBox.addItem("Hungarian", 13);
	scaleChoiceComboBox.addItem("Whole Tone", 14);
	scaleChoiceComboBox.addItem("Augmented", 15);
	scaleChoiceComboBox.addItem("Blues Major", 16);
	scaleChoiceComboBox.addItem("Blues Minor", 17);
	scaleChoiceComboBox.addItem("Pentatonic", 18);
	scaleChoiceComboBox.addItem("Minor Pentatonic", 19);
	scaleChoiceComboBox.setSelectedId(5);
	addAndMakeVisible(scaleChoiceComboBox);

	rootNoteComboBox.setBounds(630, 590, 100, 30);
	rootNoteComboBox.addItem("C",  1);
	rootNoteComboBox.addItem("C#", 2);
	rootNoteComboBox.addItem("D",  3);
	rootNoteComboBox.addItem("D#", 4);
	rootNoteComboBox.addItem("E",  5);
	rootNoteComboBox.addItem("F",  6);
	rootNoteComboBox.addItem("F#", 7);
	rootNoteComboBox.addItem("G",  8);
	rootNoteComboBox.addItem("G#", 9);
	rootNoteComboBox.addItem("A",  10);
	rootNoteComboBox.addItem("A#", 11);
	rootNoteComboBox.addItem("B",  12);
	rootNoteComboBox.setSelectedId(1);
	addAndMakeVisible(rootNoteComboBox);

	ballsPositioningTypeComboBox.setBounds(550, 120, 100, 30);
	ballsPositioningTypeComboBox.addItem("Chaos", 1);
	ballsPositioningTypeComboBox.addItem("By Tempo", 2);
	ballsPositioningTypeComboBox.setSelectedId(1);
	addAndMakeVisible(ballsPositioningTypeComboBox);

	snapToGridButton.setBounds(670, 45, 100, 30);
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

	collisionButton.setBounds(550, 45, 100, 30);
	collisionButton.setToggleState(true, juce::dontSendNotification);
	collisionButton.setButtonText("Collision");
	addAndMakeVisible(collisionButton);

	openPresetManager.setButtonText("Presets");
	openPresetManager.setBounds(670, 10, 100, 30);
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
	addAndMakeVisible(openPresetManager);
}

//==============================================================================

void BallPitAudioProcessorEditor::paint(juce::Graphics& g)
{
	g.fillAll(juce::Colours::black);

	// draw background 
	if (drawable != nullptr)
	{
		drawable->setBounds(getLocalBounds());
		drawable->draw(g, 1.0f);
	}
	
	if (audioProcessor.getIsGUIUploaded() == false)
	{
		audioProcessor.updateGUIFlag(true);
		loadFromProcessorState();
	}

	// draw balls
	const auto& balls = audioProcessor.getPit().getBalls();
	int color = 0;
	for (const auto& ball : balls) 
	{
		if (color == 0) { g.setColour(juce::Colours::blue); }
		if (color == 1) { g.setColour(juce::Colours::crimson); }
		if (color == 2) { g.setColour(juce::Colours::orange); }
		color++;
		if (ball->isActive() == true)
		{
			g.fillEllipse(ball->getX() - ball->getRadius(),
						  ball->getY() - ball->getRadius(),
						  ball->getRadius() * 2.0f,
						  ball->getRadius() * 2.0f);
		}
	}

	displayKnobsByTab();
	presetPanel.setPluginBounds(getLocalBounds());
	audioProcessor.getPit().drawPitEdge(g, edgeColors);
}

void BallPitAudioProcessorEditor::resized()
{
	logBox.setBounds(0, 654, 836, 100);
}

void BallPitAudioProcessorEditor::timerCallback() 
{
	if (auto* logger = audioProcessor.getLogger())
	{
		auto messages = logger->getMessages();
		juce::String allMessages;

		for (const auto& message : messages)
		{
			allMessages += message + "\n";
		}

		logBox.setText(allMessages, juce::dontSendNotification);
	}
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
	// Define the snapping factor
	constexpr float gridFactor = 48.75f; // Scaling factor for snapping

	for (int i = 0; i < 3; i++)
	{
		// Set the sliders to the snap-to-grid range
		ballsSlidersAndAttachments[i].xSlider.setRange(0.0f, 390.0f, 48.75f);
		ballsSlidersAndAttachments[i].ySlider.setRange(0.0f, 390.0f, 48.75f);

		// Get parameter IDs
		std::string ballXId = "ballX" + std::to_string(i);
		std::string ballYId = "ballY" + std::to_string(i);

		// Get the current parameter values
		float currentX = audioProcessor.valueTreeState.getRawParameterValue(ballXId)->load() - 12;
		float currentY = audioProcessor.valueTreeState.getRawParameterValue(ballYId)->load() - 12;

		// Snap values to the nearest grid step based on the scaling factor
		float snappedX = std::round(currentX / gridFactor);
		float snappedY = std::round(currentY / gridFactor);

		// Clamp the snapped values within the slider range
		snappedX = juce::jlimit(0.0f, 9.0f, snappedX);
		snappedY = juce::jlimit(0.0f, 9.0f, snappedY);

		// Update the sliders
		ballsSlidersAndAttachments[i].xSlider.setValue(snappedX, juce::sendNotification);
		ballsSlidersAndAttachments[i].ySlider.setValue(snappedY, juce::sendNotification);

		// Update the parameters and notify the host
		if (auto* xParam = audioProcessor.valueTreeState.getParameter(ballXId))
		{
			xParam->setValueNotifyingHost((snappedX * gridFactor) / 390.0f); // Scale to normalized 0-1 range
		}

		if (auto* yParam = audioProcessor.valueTreeState.getParameter(ballYId))
		{
			yParam->setValueNotifyingHost((snappedY * gridFactor) / 390.0f); // Scale to normalized 0-1 range
		}
	}
}


void BallPitAudioProcessorEditor::changeXAndYToFree()
{
	// Define the snapping factor
	constexpr float gridFactor = 10.0f; // Scaling factor for snapping

	for (int i = 0; i < 3; i++)
	{
		// Set the sliders to the snap-to-grid range
		ballsSlidersAndAttachments[i].xSlider.setRange(0.0f, 390.0f, 10.0f);
		ballsSlidersAndAttachments[i].ySlider.setRange(0.0f, 390.0f, 10.0f);

		// Get parameter IDs
		std::string ballXId = "ballX" + std::to_string(i);
		std::string ballYId = "ballY" + std::to_string(i);

		// Get the current parameter values
		float currentX = audioProcessor.valueTreeState.getRawParameterValue(ballXId)->load();
		float currentY = audioProcessor.valueTreeState.getRawParameterValue(ballYId)->load();

		// Snap values to the nearest grid step based on the scaling factor
		float snappedX = std::round(currentX / gridFactor);
		float snappedY = std::round(currentY / gridFactor);

		// Clamp the snapped values within the slider range
		snappedX = juce::jlimit(0.0f, 39.0f, snappedX);
		snappedY = juce::jlimit(0.0f, 39.0f, snappedY);

		// Update the sliders
		ballsSlidersAndAttachments[i].xSlider.setValue(snappedX, juce::sendNotification);
		ballsSlidersAndAttachments[i].ySlider.setValue(snappedY, juce::sendNotification);

		// Update the parameters and notify the host
		if (auto* xParam = audioProcessor.valueTreeState.getParameter(ballXId))
		{
			xParam->setValueNotifyingHost((snappedX * gridFactor) / 390.0f); // Scale to normalized 0-1 range
		}

		if (auto* yParam = audioProcessor.valueTreeState.getParameter(ballYId))
		{
			yParam->setValueNotifyingHost((snappedY * gridFactor) / 390.0f); // Scale to normalized 0-1 range
		}
	}
}
