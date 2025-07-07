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
#include "CostumeDial.h"
#include "CostumeComboBox.h"
#include "CostumeToggleButton.h"
#include "CostumeDelaySliders.h"

//==============================================================================
/**
*/

class BallSlidersAndAttachments : public juce::Component
{
public:
	BallSlidersAndAttachments() {}

	juce::Slider xSlider, ySlider, radiusSlider, velocitySlider, angleSlider, xVelocitySlider, yVelocitySlider;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> xAttachment, yAttachment, radiusAttachment, velocityAttachment, angleAttachment, xVelocityAttachment, yVelocityAttachment;
	juce::ToggleButton xVelocityInverter, yVelocityInverter;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> xVelocityInverterAttachment, yVelocityInverterAttachment;
};

class BallEffectsSlidersAndAttachments : public juce::Component
{
public:
	BallEffectsSlidersAndAttachments() {}

	// delay fx
	juce::Slider delayAmountSlider, delayFeedbackSlider;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> delayAmountAttachment, delayFeedbackAttachment;
	juce::ComboBox delayRateComboBox, delayNoteMovementComboBox;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> delayRateAttachment, delayNoteMovementAttachment;
};

//-------------------------------------------------------------------------------------------

class BallPitAudioProcessorEditor : public juce::AudioProcessorEditor,
	public juce::Timer,
	private juce::ChangeListener,
	public juce::KeyListener,
	public juce::ComboBox::Listener
{
public:
	BallPitAudioProcessorEditor(BallPitAudioProcessor&);
	~BallPitAudioProcessorEditor() override;

	//====================================================================================
	void paint(juce::Graphics&) override;
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
	bool mouseOverEdgeDice = false;
	bool mouseOverScaleDice = false;

	bool keyPressed(const juce::KeyPress& key, juce::Component* originatingComponent) override;
	//----------------------------------------------------------------------

private:
	// This reference is provided as a quick way for your editor to
	// access the processor object that created it.
	BallPitAudioProcessor& audioProcessor;

	std::unique_ptr<juce::Drawable> backgroundDrawable;

	BallSlidersAndAttachments ballsSlidersAndAttachments[3];
	std::unique_ptr<juce::Drawable> tabsDrawable;
	int mouseOverTab = -1;

	void configureAttachments();
	void initiateComponents();
	void displayKnobsByTab();
	void drawPitEdge(juce::Graphics& g, juce::Colour* edgeColors);
	void drawBall(juce::Graphics& g, const BallGUIEssentials currentBall) const;
	void changeXAndYToSnapToGrid();
	void changeXAndYToFree();

	int transpose = 0;
	int mouseOverTranspose = -1;
	void drawTranspsoe(juce::Graphics& g);

	juce::TextButton startStopButton;
	juce::TextButton addRemoveBallButton;

	juce::Slider edgePhaseSlider, edgeDenomenatorSlider, edgeRangeSlider;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> edgePhaseAttachment, edgeDenomenatorAttachment, edgeRangeAttachment;

	juce::Slider quantizationSlider;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> quantizationAttachment;

	juce::ComboBox quantizationDivisionComboBox;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> quantizationDivisionAttachment;
	
	int currentBallFocused = 0;

	juce::ComboBox scaleChoiceComboBox;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> scaleChoiceAttachment;

	juce::ComboBox rootNoteComboBox;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> rootNoteAttachment;

	juce::ComboBox edgeTypeComboBox;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> edgeTypeAttachment;
	
	juce::ComboBox ballsPositioningTypeComboBox;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> ballsPositioningTypeAttachment;

	juce::ToggleButton snapToGridButton;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> snapToGridAttachment;

	juce::ToggleButton collisionButton;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> collisionAttachment;

	juce::Slider volumeVariationSlider;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> volumeVariationAttachment;

	juce::ComboBox sizePercentageComboBox;
	std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> sizePercentageAttachment;

	juce::Slider probabilitySlider;
	std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> probabilityAttachment;

	BallEffectsSlidersAndAttachments ballEffectsSlidersAndAttachments[3];
	
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

	MyCostumeDial m_costumeDialLAF;
	MyCostumeComboBox m_costumeComboBoxLAF;
	MyCostumeSnapToGridLAF m_costumeToggleButoonLAF;
	MyCostumeCollisionLAF m_costumeCollisionLAF;
	MyCostumeVelocityInverterLAF m_costumeVelocityInverterLAF;
	MyCostumeDelayAmountLAF m_costumeDelayAmountLAF;
	MyCostumeDelayFeedbackLAF m_costumeDelayFeedbackLAF;
	CostumeHorizontalSlider m_costumeHorizontalSliderLAF;

	juce::Component content;
	float sizePercentage = 1.0f;

	juce::TextButton openPresetManager;
	PresetPanel presetPanel;

	std::vector<std::unique_ptr<BallGUIEssentials>> GUIBalls;

	std::shared_ptr<const std::vector<BallGUIEssentials>> ballsSnapshot;
	std::shared_ptr<const std::vector<int>> abstractedEdgeColorsSnapshot;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BallPitAudioProcessorEditor)
};
