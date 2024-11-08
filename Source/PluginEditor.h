/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "Pit.h"
#include "Scales.h"

//==============================================================================
/**
*/

class BallSlidersAndAttachments : public juce::Component
{
public:
	BallSlidersAndAttachments() {}

	juce::Slider xSlider, ySlider, radiusSlider, velocitySlider, angleSlider;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> xAttachment, yAttachment, radiusAttachment, velocityAttachment, angleAttachment;
};

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
	void displayKnobsByTab();

	juce::TextButton startStopButton;
	juce::TextButton addRemoveBallButton;

	juce::Slider edgePhaseSlider, edgeDenomenatorSlider, edgeRangeSlider;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> edgePhaseAttachment, edgeDenomenatorAttachment, edgeRangeAttachment;
	
	std::unique_ptr<juce::TabbedComponent> tabs;

	juce::ComboBox scaleChoiceComboBox;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> scaleChoiceAttachment;

	juce::ComboBox rootNoteComboBox;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> rootNoteAttachment;

	juce::Colour edgeColors[8] = {
		juce::Colours::red,
		juce::Colours::limegreen,
		juce::Colours::yellow,
		juce::Colours::blue,
		juce::Colours::pink,
		juce::Colours::purple,
		juce::Colours::cyan,
		juce::Colours::orange
	};

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (BallPitAudioProcessorEditor)
};
