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
	auto svgFile = juce::File("C:/Users/97252/Desktop/computer_science/project/BallPit/Resources/LayOut.svg");
	//auto svgFile = juce::File("D:/Computer_Science/project/BallPit/Resources/LayOut.svg");
	std::unique_ptr<juce::XmlElement> svgXml(juce::XmlDocument::parse(svgFile));
	if (svgXml != nullptr) { drawable = juce::Drawable::createFromSVG(*svgXml); }

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
}

void BallPitAudioProcessorEditor::initiateComponents()
{
	for (int i = 0; i < 3; i++) // TODO- add slider for each ball by tab
	{
		ballsSlidersAndAttachments[i].xSlider.setBounds(656, 65, 65, 65);
		ballsSlidersAndAttachments[i].xSlider.setValue(250.0f);
		ballsSlidersAndAttachments[i].xSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		ballsSlidersAndAttachments[i].xSlider.setDoubleClickReturnValue(true, 0.0f);
		ballsSlidersAndAttachments[i].xSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballsSlidersAndAttachments[i].xSlider.setRange(0.0f, 390.0f, 10.0f);

		ballsSlidersAndAttachments[i].ySlider.setBounds(744, 65, 65, 65);
		ballsSlidersAndAttachments[i].ySlider.setValue(250.0f);
		ballsSlidersAndAttachments[i].ySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		ballsSlidersAndAttachments[i].ySlider.setDoubleClickReturnValue(true, 0.0f);
		ballsSlidersAndAttachments[i].ySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballsSlidersAndAttachments[i].ySlider.setRange(0.0f, 390.0f, 10.0f);

		ballsSlidersAndAttachments[i].angleSlider.setBounds(656, 115, 65, 65);
		ballsSlidersAndAttachments[i].angleSlider.setValue(6.0f);
		ballsSlidersAndAttachments[i].angleSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		ballsSlidersAndAttachments[i].angleSlider.setDoubleClickReturnValue(true, 3.0f);
		ballsSlidersAndAttachments[i].angleSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballsSlidersAndAttachments[i].angleSlider.setRotaryParameters(0, 2*3.14159265358979323846f, false);
		ballsSlidersAndAttachments[i].angleSlider.setRange(0.0f, 360.0f, 1.0f);

		ballsSlidersAndAttachments[i].velocitySlider.setBounds(656, 165, 65, 65);
		ballsSlidersAndAttachments[i].velocitySlider.setValue(10.0f);
		ballsSlidersAndAttachments[i].velocitySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		ballsSlidersAndAttachments[i].velocitySlider.setDoubleClickReturnValue(true, 3.0f);
		ballsSlidersAndAttachments[i].velocitySlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballsSlidersAndAttachments[i].velocitySlider.setRange(0.0f, 10.0f, 0.5f);

		ballsSlidersAndAttachments[i].radiusSlider.setBounds(656, 215, 65, 65);
		ballsSlidersAndAttachments[i].radiusSlider.setValue(10.0f);
		ballsSlidersAndAttachments[i].radiusSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		ballsSlidersAndAttachments[i].radiusSlider.setDoubleClickReturnValue(true, 5.0f);
		ballsSlidersAndAttachments[i].radiusSlider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
		ballsSlidersAndAttachments[i].radiusSlider.setRange(5.0f, 15.0f, 0.5f);
	}
	// TODO- remove this and add the tabs
		addAndMakeVisible(ballsSlidersAndAttachments[0].xSlider);
		addAndMakeVisible(ballsSlidersAndAttachments[0].ySlider);
		addAndMakeVisible(ballsSlidersAndAttachments[0].angleSlider);
		addAndMakeVisible(ballsSlidersAndAttachments[0].radiusSlider);
		addAndMakeVisible(ballsSlidersAndAttachments[0].velocitySlider);
}

void BallPitAudioProcessorEditor::resized()
{
}

void BallPitAudioProcessorEditor::timerCallback() 
{
	repaint();
}