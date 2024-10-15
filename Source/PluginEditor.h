/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Pit.h"

//==============================================================================
/**
*/

typedef struct 
{
	juce::Slider xSlider, ySlider, radiusSlider, velocitySlider, angleSlider;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> xAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> yAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> radiusAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> velocityAttachment;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> angleAttachment;
}BallSlidersAndAttachments;

//-------------------------------------------------------------------------------------------

class BallPitAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::Timer
{
public:
	BallPitAudioProcessorEditor (BallPitAudioProcessor&);
	~BallPitAudioProcessorEditor() override;

	//==============================================================================
	void paint (juce::Graphics&) override;
	void resized() override;
	void timerCallback() override;

private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	BallPitAudioProcessor& audioProcessor;

	std::unique_ptr<juce::Drawable> drawable;

	BallSlidersAndAttachments ballsSlidersAndAttachments[3];
	void initiateComponents();

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BallPitAudioProcessorEditor)
};
