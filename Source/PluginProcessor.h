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

	juce::Atomic<bool> isPlaying;

	juce::ValueTree& getGUIState() { return processorGUIState; }
	void saveGUIState(juce::ValueTree &GUIState);

	Pit& getPit();

	void updateGUIFlag(bool newStatus) { wasGUIUploaded = newStatus; }
	bool getWasGUIUploaded() { return wasGUIUploaded; }

	std::atomic<double> m_bpm {DEFAULT_BPM};
	juce::AudioPlayHead::TimeSignature m_timeSignature;

	void getUpdatedBallParams();
	void getUpdatedEdgeParams();
	void setXYVelocityByTempo(float& xVelocity, float& yVelocity, float ballRadius);
	void updateQuantization();

	float getVariedNoteVelocity(int currentNoteVelocity);

	void setWasGUIUpdatedToTrue() { this->wasGUIUpdated = true; }

	Service::PresetManager& getPresetManager() { return *presetManager; }

	juce::UndoManager& getUndoManager() { return m_undoManager; }

	double getClockTimeSecond() { return clockTimeSeconds; }
	void setClockTimeSeconds(double newClockTimeSeconds) { this->clockTimeSeconds = newClockTimeSeconds; }
	void togglePlayState();

	std::atomic<double> m_sampleRate;
	std::atomic<int> m_samplesPerBlock;

	std::atomic<float> m_probability = 1.0f; // default to 100% probability

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BallPitAudioProcessor)

	Pit pit;
	juce::MidiBuffer midiBuffer;
	std::vector<std::unique_ptr<EdgeEventListener>> listeners;
	std::vector<PendingMidiEvent> pendingEvents;

	std::atomic<double> clockTimeSeconds = 0.0;

	std::atomic<float> quantizationpercent = 0.0;
	std::atomic<float> quantizationDivision = (1/32);

	std::atomic<int> samplesPerStep = 0;
	std::atomic<int> sampleCounter = 0;

	std::atomic<bool> wasGUIUploaded;
	juce::ValueTree processorGUIState;
	std::atomic<bool> wasGUIUpdated = true; // initialize to true to force the GUI to update on first load
	void parameterChanged(const juce::String& parameterID, float newValue) override;

	std::unique_ptr<Service::PresetManager> presetManager;

	juce::UndoManager m_undoManager;

	std::set<int> activeNotes;  // store MIDI note numbers

	public:
	// TODO- check that all params are here!
	std::vector<juce::String> paramIDs = {
	"ballX0", "ballY0", "ballRadius0", "ballVelocity0", "ballAngle0", "ballXVelocity0", "ballYVelocity0", "xVelocityInverter0", "yVelocityInverter0", "delayAmount0", "delayFeedback0", "delayRate0", "delayNoteMovement0",
	"ballX1", "ballY1", "ballRadius1", "ballVelocity1", "ballAngle1", "ballXVelocity1", "ballYVelocity1", "xVelocityInverter1", "yVelocityInverter1", "delayAmount1", "delayFeedback1", "delayRate1", "delayNoteMovement1",
	"ballX2", "ballY2", "ballRadius2", "ballVelocity2", "ballAngle2", "ballXVelocity2", "ballYVelocity2", "xVelocityInverter2", "yVelocityInverter2", "delayAmount2", "delayFeedback2", "delayRate2", "delayNoteMovement2",
	"edgePhase", "edgeDenomenator", "edgeRange", "scaleChoice", "rootNote", "edgeType",
	"ballsPositioningType", "snapToGrid", "collision", "quantization", "quantizationDivision", "volumeVariation", "sizePercentage", "probability", "transpose"
	};
};
