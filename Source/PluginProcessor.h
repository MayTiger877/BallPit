/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Pit.h"
#include "EdgeEventListener.h"
#include "Scales.h"
#include "Configs.h"
#include "PresetManagerBackend.h"

double velocityToInterval(int velocity);

typedef struct
{
	juce::MidiMessage message;
	int samplePosition;
}PendingMidiEvent;

//==============================================================================
/**
*/
class BallPitAudioProcessor  :  public juce::AudioProcessor, 
								public juce::ChangeBroadcaster,
								public juce::AudioProcessorValueTreeState::Listener
{
public:
	//==============================================================================
	BallPitAudioProcessor();
	~BallPitAudioProcessor() override;

	//==============================================================================
	void prepareToPlay (double sampleRate, int samplesPerBlock) override;
	void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
	bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

	void processBlock (juce::AudioBuffer<float>&, juce::MidiBuffer&) override;
	juce::AudioProcessorEditor* createEditor() override;
	bool hasEditor() const override;
	const juce::String getName() const override;

	bool acceptsMidi() const override;
	bool producesMidi() const override;
	bool isMidiEffect() const override;
	double getTailLengthSeconds() const override;
	int getNumPrograms() override;
	int getCurrentProgram() override;
	void setCurrentProgram (int index) override;
	const juce::String getProgramName (int index) override;
	void changeProgramName (int index, const juce::String& newName) override;
	void getStateInformation (juce::MemoryBlock& destData) override;
	void setStateInformation (const void* data, int sizeInBytes) override;

	//==============================================================================

	juce::AudioProcessorValueTreeState valueTreeState;
	juce::AudioProcessorValueTreeState::ParameterLayout createParameters();
	void addParamListeners(juce::AudioProcessorValueTreeState& apvts);
	juce::AudioProcessorValueTreeState& getValueTreeState() { return valueTreeState; }

	juce::ValueTree& getGUIState() { return processorGUIState; }
	void saveGUIState(juce::ValueTree &GUIState);
	
	void getUpdatedBallParams();
	void getUpdatedEdgeParams();
	void setXYVelocityByTempo(float& xVelocity, float& yVelocity, float ballRadius);
	void updateQuantization();
	
	float getVariedNoteVelocity(int currentNoteVelocity);
	
	Service::PresetManager& getPresetManager() { return *presetManager; }
	
	juce::UndoManager& getUndoManager() { return m_undoManager; }
	
	//------------------------------------------------------------------
	//getters and setters
	
	bool getToggleState();
	void setToggleState(bool newState);
	bool getIsPlaying();
	void setIsPlaying(bool newState);
	double getSampleRate() const;
	void setSampleRate(double newSampleRate);
	int getSamplesPerBlock() const;
	void setSamplesPerBlock(int newSamplesPerBlock);
	float getProbability() const;
	void setProbability(float newProbability);
	double getBPM() const;
	void setBPM(double newBPM);
	juce::AudioPlayHead::TimeSignature getTimeSignature() const;
	void setTimeSignature(juce::AudioPlayHead::TimeSignature newTimeSignature);
	float getQuantizationPercent() const;
	void setQuantizationPercent(float newQuantizationPercent);
	float getQuantizationDivision() const;
	void setQuantizationDivision(float newQuantizationDivision);
	int getSamplesPerStep() const;
	void setSamplesPerStep(int newSamplesPerStep);
	int getSampleCounter() const;
	void setSampleCounter(int newSampleCounter);
	// double getClockTimeSeconds() const;
	// void setClockTimeSeconds(double newClockTimeSeconds);
	bool getWasGUIUpdated() const;
	void setWasGUIUpdated(bool newStatus);
	bool getWasGUIUploaded();
	void setWasGUIUploaded(bool newStatus);
	bool getAreBallsMoving();
	void setAreBallsMoving(bool newState);

	// Thread safe methods
	std::shared_ptr<const std::vector<BallGUIEssentials>> getBallsSnapshot() const;
	void updateBallsSnapshot(std::shared_ptr<const std::vector<BallGUIEssentials>> newSnapshot);
	
	std::shared_ptr<std::vector<int>> getAbstractedEdgeColors() const;
	void updateAbstractedEdgeColors(std::shared_ptr<std::vector<int>> newColors);

	//==============================================================================
	
private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BallPitAudioProcessor)
	
	Pit pit;
	juce::MidiBuffer midiBuffer;
	std::vector<std::unique_ptr<EdgeEventListener>> listeners;
	std::vector<PendingMidiEvent> pendingEvents;
	
	juce::Atomic<bool> toggleState = false;
	juce::Atomic<bool> isPlaying = false;
	juce::Atomic<bool> areBallsMoving = false; // to track if balls are moving

	juce::Atomic<double> m_sampleRate;
	juce::Atomic<int> m_samplesPerBlock;
	
	juce::Atomic<float> m_probability = 1.0f; // default to 100% probability
	
	double clockTimeSeconds = 0.0;

	juce::Atomic<double> m_bpm {DEFAULT_BPM};
	juce::Atomic<juce::AudioPlayHead::TimeSignature> m_timeSignature;
	
	juce::Atomic<float> quantizationpercent = 0.0;
	juce::Atomic<float> quantizationDivision = (1/32);

	juce::Atomic<int> samplesPerStep = 0;
	juce::Atomic<int> sampleCounter = 0;

	juce::Atomic<bool> wasGUIUploaded;
	juce::ValueTree processorGUIState;
	juce::Atomic<bool> wasGUIUpdated = true; // initialize to true to force the GUI to update on first load
	void parameterChanged(const juce::String& parameterID, float newValue) override;

	std::unique_ptr<Service::PresetManager> presetManager;

	juce::UndoManager m_undoManager;

	std::set<int> activeNotes;  // store MIDI note numbers

	std::vector<BallGUIEssentials> latestBallsSnapshot = {
		{0.0f, 0.0f, 0.0f, 0.0, 0.0, false, 0, false, 0, 0, 0.0f, 0.0f, 0},
		{0.0f, 0.0f, 0.0f, 0.0, 0.0, false, 1, false, 1, 1, 0.0f, 0.0f, 1},
		{0.0f, 0.0f, 0.0f, 0.0, 0.0, false, 2, false, 2, 2, 0.0f, 0.0f, 2}
	};
	std::vector<int> abstractedEdgeColors;

	std::shared_ptr<const std::vector<BallGUIEssentials>> latestBallsSnapshotPointer;
	mutable juce::CriticalSection ballsSnapshotPointerLock;
	
	std::shared_ptr<std::vector<int>> abstractedEdgeColorsPointer;
	mutable juce::CriticalSection edgeColorsPointerLock;
	
public:
	// TODO- check that all params are here!
	std::vector<juce::String> paramIDs = {
		"ballX0", "ballY0", "ballRadius0", "ballVelocity0", "ballAngle0", "ballXVelocity0", "BallActivation0" "ballYVelocity0", "xVelocityInverter0", "yVelocityInverter0", "delayAmount0", "delayFeedback0", "delayRate0", "delayNoteMovement0",
		"ballX1", "ballY1", "ballRadius1", "ballVelocity1", "ballAngle1", "ballXVelocity1", "BallActivation1" "ballYVelocity1", "xVelocityInverter1", "yVelocityInverter1", "delayAmount1", "delayFeedback1", "delayRate1", "delayNoteMovement1",
		"ballX2", "ballY2", "ballRadius2", "ballVelocity2", "ballAngle2", "ballXVelocity2", "BallActivation2" "ballYVelocity2", "xVelocityInverter2", "yVelocityInverter2", "delayAmount2", "delayFeedback2", "delayRate2", "delayNoteMovement2",
		"edgePhase", "edgeDenomenator", "edgeRange", "scaleChoice", "rootNote", "edgeType",
		"ballsPositioningType", "snapToGrid", "collision", "quantization", "quantizationDivision", "volumeVariation", "sizePercentage", "probability", "transpose",
		"toggleState"
	};
};
