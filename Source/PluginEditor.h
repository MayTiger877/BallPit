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

	juce::Slider xSlider, ySlider, radiusSlider, velocitySlider, angleSlider, xVelocitySlider, yVelocitySlider;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> xAttachment, yAttachment, radiusAttachment, velocityAttachment, angleAttachment, xVelocityAttachment, yVelocityAttachment;
};

//-------------------------------------------------------------------------------------------

class BallPitAudioProcessorEditor  : public juce::AudioProcessorEditor, 
									 public juce::Timer, 
									 private juce::ChangeListener
{
public:
	BallPitAudioProcessorEditor (BallPitAudioProcessor&);
	~BallPitAudioProcessorEditor() override;

	//==============================================================================
	void paint (juce::Graphics&) override;
	void resized() override;
	void timerCallback() override;
	void loadFromProcessorState();
	void changeListenerCallback(juce::ChangeBroadcaster* source) override;

private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	BallPitAudioProcessor& audioProcessor;

	std::unique_ptr<juce::Drawable> drawable;

	BallSlidersAndAttachments ballsSlidersAndAttachments[3];
	void initiateComponents();
	void displayKnobsByTab();

	void changeXAndYToSnapToGrid();
	void changeXAndYToFree();

	juce::TextButton startStopButton;
	juce::TextButton addRemoveBallButton;

	juce::Slider edgePhaseSlider, edgeDenomenatorSlider, edgeRangeSlider;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> edgePhaseAttachment, edgeDenomenatorAttachment, edgeRangeAttachment;
	
	std::unique_ptr<juce::TabbedComponent> tabs;

	juce::ComboBox scaleChoiceComboBox;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> scaleChoiceAttachment;

	juce::ComboBox rootNoteComboBox;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> rootNoteAttachment;

	//juce::ComboBox scaleModeComboBox; TODO
	
	juce::ComboBox ballsPositioningTypeComboBox;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> ballsPositioningTypeAttachment;

	juce::ToggleButton snapToGridButton; // TODO - finish
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> snapToGridAttachment;

	juce::ToggleButton collisionButton;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> collisionAttachment;


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

	juce::ValueTree newGUIState;

	juce::TextEditor logBox;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BallPitAudioProcessorEditor)
};
