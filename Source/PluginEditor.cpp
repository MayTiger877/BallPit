/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BallPitAudioProcessorEditor::BallPitAudioProcessorEditor (BallPitAudioProcessor& p)
	: AudioProcessorEditor (&p), audioProcessor (p)
{
	//auto svgFile = juce::File("C:/Users/97252/Desktop/computer_science/project/BallPit/Resources/LayOut.svg"); //laptop
	auto svgFile = juce::File("D:/Computer_Science/project/BallPit/Resources/LayOut.svg"); //bialik
	//auto svgFile = juce::File("D:/Plugin Laboratory/BallPit/Resources/LayOut.svg"); //haifa

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
	

	setSize(836, 654);
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

		this->newGUIState.setProperty(juce::Identifier(ballXId), audioProcessor.valueTreeState.getRawParameterValue(ballXId)->load(), nullptr);
		this->newGUIState.setProperty(juce::Identifier(ballYId), audioProcessor.valueTreeState.getRawParameterValue(ballYId)->load(), nullptr);
		this->newGUIState.setProperty(juce::Identifier(ballRadiusId), audioProcessor.valueTreeState.getRawParameterValue(ballRadiusId)->load(), nullptr);
		this->newGUIState.setProperty(juce::Identifier(ballVelocityId), audioProcessor.valueTreeState.getRawParameterValue(ballVelocityId)->load(), nullptr);
		this->newGUIState.setProperty(juce::Identifier(ballAngleId), audioProcessor.valueTreeState.getRawParameterValue(ballAngleId)->load(), nullptr);
	}

	this->newGUIState.setProperty("edgePhase", audioProcessor.valueTreeState.getRawParameterValue("edgePhase")->load(), nullptr);
	this->newGUIState.setProperty("edgeDenomenator", audioProcessor.valueTreeState.getRawParameterValue("edgeDenomenator")->load(), nullptr);
	this->newGUIState.setProperty("edgeRange", audioProcessor.valueTreeState.getRawParameterValue("edgeRange")->load(), nullptr);
	this->newGUIState.setProperty("scaleChoice", audioProcessor.valueTreeState.getRawParameterValue("scaleChoice")->load(), nullptr);
	this->newGUIState.setProperty("rootNote", audioProcessor.valueTreeState.getRawParameterValue("rootNote")->load(), nullptr);

	audioProcessor.saveGUIState(this->newGUIState);
	audioProcessor.updateGUIFlag(false);
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

		ballsSlidersAndAttachments[i].xSlider.setValue(GUIState.getProperty(juce::Identifier(ballXId)), juce::dontSendNotification);
		ballsSlidersAndAttachments[i].ySlider.setValue(GUIState.getProperty(juce::Identifier(ballYId)), juce::dontSendNotification);
		ballsSlidersAndAttachments[i].radiusSlider.setValue(GUIState.getProperty(juce::Identifier(ballRadiusId)), juce::dontSendNotification);
		ballsSlidersAndAttachments[i].velocitySlider.setValue(GUIState.getProperty(juce::Identifier(ballVelocityId)), juce::dontSendNotification);
		ballsSlidersAndAttachments[i].angleSlider.setValue(GUIState.getProperty(juce::Identifier(ballAngleId)), juce::dontSendNotification);
	}

	edgePhaseSlider.setValue(GUIState.getProperty("edgePhase"), juce::dontSendNotification);
	edgeDenomenatorSlider.setValue(GUIState.getProperty("edgeDenomenator"), juce::dontSendNotification);
	edgeRangeSlider.setValue(GUIState.getProperty("edgeRange"), juce::dontSendNotification);
	scaleChoiceComboBox.setSelectedItemIndex(GUIState.getProperty("scaleChoice"), juce::dontSendNotification);
	rootNoteComboBox.setSelectedItemIndex(GUIState.getProperty("rootNote"), juce::dontSendNotification);
}

void BallPitAudioProcessorEditor::displayKnobsByTab()
{
	int currentTab = tabs->getCurrentTabIndex();
	int otherTab1 = (currentTab + 1) % 3;
	int otherTab2 = (currentTab + 2) % 3;
	
	ballsSlidersAndAttachments[currentTab].xSlider.setVisible(true);
	ballsSlidersAndAttachments[currentTab].ySlider.setVisible(true);
	ballsSlidersAndAttachments[currentTab].angleSlider.setVisible(true);
	ballsSlidersAndAttachments[currentTab].radiusSlider.setVisible(true);
	ballsSlidersAndAttachments[currentTab].velocitySlider.setVisible(true);
	
	ballsSlidersAndAttachments[otherTab1].xSlider.setVisible(false);
	ballsSlidersAndAttachments[otherTab1].ySlider.setVisible(false);
	ballsSlidersAndAttachments[otherTab1].angleSlider.setVisible(false);
	ballsSlidersAndAttachments[otherTab1].radiusSlider.setVisible(false);
	ballsSlidersAndAttachments[otherTab1].velocitySlider.setVisible(false);
	
	ballsSlidersAndAttachments[otherTab2].xSlider.setVisible(false);
	ballsSlidersAndAttachments[otherTab2].ySlider.setVisible(false);
	ballsSlidersAndAttachments[otherTab2].angleSlider.setVisible(false);
	ballsSlidersAndAttachments[otherTab2].radiusSlider.setVisible(false);
	ballsSlidersAndAttachments[otherTab2].velocitySlider.setVisible(false);
	
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

		ballsSlidersAndAttachments[i].velocitySlider.setBounds(656, 165, 65, 65);
		ballsSlidersAndAttachments[i].velocitySlider.setValue(5.0f);
		ballsSlidersAndAttachments[i].velocitySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		ballsSlidersAndAttachments[i].velocitySlider.setDoubleClickReturnValue(true, 3.0f);
		ballsSlidersAndAttachments[i].velocitySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballsSlidersAndAttachments[i].velocitySlider.setRange(0.0f, 10.0f, 0.5f);
		ballsSlidersAndAttachments[i].velocitySlider.toFront(false);
		addChildComponent(ballsSlidersAndAttachments[i].velocitySlider);

		ballsSlidersAndAttachments[i].radiusSlider.setBounds(656, 215, 65, 65);
		ballsSlidersAndAttachments[i].radiusSlider.setValue(10.0f);
		ballsSlidersAndAttachments[i].radiusSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		ballsSlidersAndAttachments[i].radiusSlider.setDoubleClickReturnValue(true, 10.0f);
		ballsSlidersAndAttachments[i].radiusSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballsSlidersAndAttachments[i].radiusSlider.setRange(5.0f, 25.0f, 0.5f);
		ballsSlidersAndAttachments[i].radiusSlider.toFront(false);
		addChildComponent(ballsSlidersAndAttachments[i].radiusSlider);
	}
	
	ballsSlidersAndAttachments[0].xSlider.setVisible(true);
	ballsSlidersAndAttachments[0].ySlider.setVisible(true);
	ballsSlidersAndAttachments[0].angleSlider.setVisible(true);
	ballsSlidersAndAttachments[0].radiusSlider.setVisible(true);
	ballsSlidersAndAttachments[0].velocitySlider.setVisible(true);
	
	startStopButton.setButtonText("Start");
	startStopButton.onClick = [this]()
	{
		audioProcessor.getPit().checkBallsPositionBeforeStart();
		audioProcessor.getPit().toggleBallMovement();
		if (audioProcessor.getPit().isBallsMoving())
			startStopButton.setButtonText("Stop");
		else
			startStopButton.setButtonText("Start");
	};
	startStopButton.setBounds(420, 290, 140, 40);
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
	addRemoveBallButton.setBounds(580, 290, 140, 40);
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
	audioProcessor.getPit().drawPitEdge(g, edgeColors);
}

void BallPitAudioProcessorEditor::resized()
{
}

void BallPitAudioProcessorEditor::timerCallback() 
{
	repaint();
}