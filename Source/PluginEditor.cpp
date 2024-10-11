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
    // Load the SVG file (replace with the actual path to your SVG file)
    auto svgFile = juce::File("C:/Users/97252/Desktop/computer_science/project/BallPit/Resources/LayOut.svg");
    //auto svgFile = juce::File("D:/Computer_Science/project/BallPit/Resources/LayOut.svg");

    std::unique_ptr<juce::XmlElement> svgXml(juce::XmlDocument::parse(svgFile));

    // Create the Drawable from the SVG XML
    if (svgXml != nullptr)
        drawable = juce::Drawable::createFromSVG(*svgXml);

    // Set the size of the plugin window
    setSize(836, 654);
    startTimerHz(60);
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

void BallPitAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void BallPitAudioProcessorEditor::timerCallback() 
{
    repaint();
}