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

    std::unique_ptr<juce::XmlElement> svgXml(juce::XmlDocument::parse(svgFile));

    // Create the Drawable from the SVG XML
    if (svgXml != nullptr)
        drawable = juce::Drawable::createFromSVG(*svgXml);

    // Set the size of the plugin window
    setSize(836, 654);
}

BallPitAudioProcessorEditor::~BallPitAudioProcessorEditor()
{
}

//==============================================================================
void BallPitAudioProcessorEditor::paint (juce::Graphics& g)
{
    // Fill the background with a solid color
    g.fillAll(juce::Colours::white);

    // Draw the SVG if it has been successfully loaded
    if (drawable != nullptr)
    {
        // Set the drawable's bounds to fit the component area
        drawable->setBounds(getLocalBounds());
        drawable->draw(g, 1.0f);  // Draw the SVG with a scale factor of 1.0
    }
}

void BallPitAudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
