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
#include "PresetManagerGUI.h"

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
									 private juce::ChangeListener,
									 public juce::ComboBox::Listener
{
public:
	BallPitAudioProcessorEditor (BallPitAudioProcessor&);
	~BallPitAudioProcessorEditor() override;

	//====================================================================================
	void paint (juce::Graphics&) override;
	void resized() override;
	void timerCallback() override;
	void saveGUIState();
	void loadGUIState();
	void changeListenerCallback(juce::ChangeBroadcaster* source) override;

	void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;

	// this is foe dealing with the mouse moving the balllz----------------
	void mouseMove(const juce::MouseEvent& event) override;
	void mouseDown(const juce::MouseEvent& event) override;
	void mouseDrag(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;
	std::pair<int, float> ballBeingDragged; // first is ball index, second is mouse distance from center
	bool mouseIsDragging = false;
	bool mouseOverDice = false;
	//----------------------------------------------------------------------


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

	juce::Slider quantizationSlider;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> quantizationAttachment;

	juce::ComboBox quantizationDivisionComboBox;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> quantizationDivisionAttachment;
	
	std::unique_ptr<juce::TabbedComponent> tabs;

	juce::ComboBox scaleChoiceComboBox;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> scaleChoiceAttachment;

	juce::ComboBox rootNoteComboBox;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> rootNoteAttachment;

	juce::ComboBox edgeTypeComboBox;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> edgeTypeAttachment;

	//juce::ComboBox scaleModeComboBox; TODO
	
	juce::ComboBox ballsPositioningTypeComboBox;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> ballsPositioningTypeAttachment;

	juce::ToggleButton snapToGridButton;
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

	juce::Point<double> lastBall1Position, lastBall2Position, lastBall3Position = { 0.0, 0.0 };

	juce::ValueTree GUIState;

	juce::TextButton openPresetManager;
	PresetPanel presetPanel;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BallPitAudioProcessorEditor)
};
