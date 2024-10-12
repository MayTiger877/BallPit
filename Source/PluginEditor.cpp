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
		
		std::string ballSpeedXID = "ballSpeedX" + std::to_string(i);
		ballsSlidersAndAttachments[i].speedXAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTreeState, ballSpeedXID, ballsSlidersAndAttachments[i].speedXSlider);

		std::string ballSpeedYID = "ballSpeedY" + std::to_string(i);
		ballsSlidersAndAttachments[i].speedYAttachment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.valueTreeState, ballSpeedYID, ballsSlidersAndAttachments[i].speedYSlider);
	}
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
		if (color == 2) { g.setColour(juce::Colours::darkgreen); }
		color++;
		g.fillEllipse(ball->getX() - ball->getRadius(),
					  ball->getY() - ball->getRadius(),
					  ball->getRadius() * 2.0f,
					  ball->getRadius() * 2.0f);
	}
}

void BallPitAudioProcessorEditor::initiateComponents()
{
	for (int i = 0; i < 3; i++)
	{
		ballsSlidersAndAttachments[i].xSlider.setBounds(677, 96, 135, 135);
		ballsSlidersAndAttachments[i].xSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		addAndMakeVisible(ballsSlidersAndAttachments[i].xSlider);

		ballsSlidersAndAttachments[i].ySlider.setBounds(765, 96, 135, 135);
		ballsSlidersAndAttachments[i].ySlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		addAndMakeVisible(ballsSlidersAndAttachments[i].ySlider);

		ballsSlidersAndAttachments[i].radiusSlider.setBounds(10, 70, getWidth() - 20, 20);
		ballsSlidersAndAttachments[i].radiusSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		addAndMakeVisible(ballsSlidersAndAttachments[i].radiusSlider);

		ballsSlidersAndAttachments[i].speedXSlider.setBounds(10, 100, getWidth() - 20, 20);
		ballsSlidersAndAttachments[i].speedXSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		addAndMakeVisible(ballsSlidersAndAttachments[i].speedXSlider);

		ballsSlidersAndAttachments[i].speedYSlider.setBounds(10, 130, getWidth() - 20, 20);
		ballsSlidersAndAttachments[i].speedYSlider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
		addAndMakeVisible(ballsSlidersAndAttachments[i].speedYSlider);
	}
}

void BallPitAudioProcessorEditor::resized()
{
	initiateComponents();
}

void BallPitAudioProcessorEditor::timerCallback() 
{
	repaint();
}