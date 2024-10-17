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
	//auto svgFile = juce::File("C:/Users/97252/Desktop/computer_science/project/BallPit/Resources/LayOut.svg");
	auto svgFile = juce::File("D:/Computer_Science/project/BallPit/Resources/LayOut.svg");
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
	initiateComponents();
}

BallPitAudioProcessorEditor::~BallPitAudioProcessorEditor()
{
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
		ballsSlidersAndAttachments[i].xSlider.setValue(250.0f);
		ballsSlidersAndAttachments[i].xSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		ballsSlidersAndAttachments[i].xSlider.setDoubleClickReturnValue(true, 0.0f);
		ballsSlidersAndAttachments[i].xSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballsSlidersAndAttachments[i].xSlider.setRange(0.0f, 390.0f, 10.0f);
		ballsSlidersAndAttachments[i].xSlider.toFront(false);
		addChildComponent(ballsSlidersAndAttachments[i].xSlider);

		ballsSlidersAndAttachments[i].ySlider.setBounds(744, 65, 65, 65);
		ballsSlidersAndAttachments[i].ySlider.setValue(250.0f);
		ballsSlidersAndAttachments[i].ySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		ballsSlidersAndAttachments[i].ySlider.setDoubleClickReturnValue(true, 0.0f);
		ballsSlidersAndAttachments[i].ySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballsSlidersAndAttachments[i].ySlider.setRange(0.0f, 390.0f, 10.0f);
		ballsSlidersAndAttachments[i].ySlider.toFront(false);
		addChildComponent(ballsSlidersAndAttachments[i].ySlider);

		ballsSlidersAndAttachments[i].angleSlider.setBounds(656, 115, 65, 65);
		ballsSlidersAndAttachments[i].angleSlider.setValue(6.0f);
		ballsSlidersAndAttachments[i].angleSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		ballsSlidersAndAttachments[i].angleSlider.setDoubleClickReturnValue(true, 3.0f);
		ballsSlidersAndAttachments[i].angleSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballsSlidersAndAttachments[i].angleSlider.setRotaryParameters(0, 2*3.14159265358979323846f, false);
		ballsSlidersAndAttachments[i].angleSlider.setRange(0.0f, 360.0f, 1.0f);
		ballsSlidersAndAttachments[i].angleSlider.toFront(false);
		addChildComponent(ballsSlidersAndAttachments[i].angleSlider);

		ballsSlidersAndAttachments[i].velocitySlider.setBounds(656, 165, 65, 65);
		ballsSlidersAndAttachments[i].velocitySlider.setValue(10.0f);
		ballsSlidersAndAttachments[i].velocitySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		ballsSlidersAndAttachments[i].velocitySlider.setDoubleClickReturnValue(true, 3.0f);
		ballsSlidersAndAttachments[i].velocitySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballsSlidersAndAttachments[i].velocitySlider.setRange(0.0f, 10.0f, 0.5f);
		ballsSlidersAndAttachments[i].velocitySlider.toFront(false);
		addChildComponent(ballsSlidersAndAttachments[i].velocitySlider);

		ballsSlidersAndAttachments[i].radiusSlider.setBounds(656, 215, 65, 65);
		ballsSlidersAndAttachments[i].radiusSlider.setValue(10.0f);
		ballsSlidersAndAttachments[i].radiusSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		ballsSlidersAndAttachments[i].radiusSlider.setDoubleClickReturnValue(true, 5.0f);
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
		audioProcessor.getPit().toggleBallMovement();
		if (audioProcessor.getPit().isBallsMoving())
			startStopButton.setButtonText("Stop");
		else
			startStopButton.setButtonText("Start");
	};
	startStopButton.setBounds(470, 290, 140, 40);
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
	addRemoveBallButton.setBounds(620, 290, 140, 40);
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
}

void BallPitAudioProcessorEditor::resized()
{
}

void BallPitAudioProcessorEditor::timerCallback() 
{
	repaint();
}