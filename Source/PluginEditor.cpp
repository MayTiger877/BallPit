/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BallPitAudioProcessorEditor::BallPitAudioProcessorEditor (BallPitAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p), ball1(50, 50, 3, 10, 10)
{
    // Load the SVG file (replace with the actual path to your SVG file)
    auto svgFile = juce::File("C:/Users/97252/Desktop/computer_science/project/BallPit/Resources/LayOut.svg");

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

    if (drawable != nullptr)
    {
        drawable->setBounds(getLocalBounds());
        drawable->draw(g, 1.0f);
    }

    ball1.draw(g);
}

void BallPitAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}

void BallPitAudioProcessorEditor::timerCallback() {
    ball1.update();  // Update the ball's position
    repaint();      // Repaint the component to reflect the updated position
}