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

	bool isHostPlaying() const { return isDAWPlaying; }

	std::atomic<double> m_bpm {DEFAULT_BPM};
	double m_ppqPos = 0.0;

	juce::AudioPlayHead::TimeSignature m_timeSignatureDEBUG;

	void getUpdatedBallParams();
	void getUpdatedEdgeParams();
	void setXYVelocityByTempo(float& xVelocity, float& yVelocity, float ballRadius);
	void updateBallsQuantization();

	void setWasGUIUpdatedToTrue() { this->wasGUIUpdated = true; }
	void setWasEdgeParamChangedFlagToTrue() { this->wasEdgeParamChanged = true; }

	Service::PresetManager& getPresetManager() { return *presetManager; }

private:
	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BallPitAudioProcessor)

	double m_sampleRate;
	double m_samplesPerBlock;
	
	Pit pit;
	juce::MidiBuffer midiBuffer;
	std::vector<std::unique_ptr<EdgeEventListener>> listeners;
	std::vector<PendingMidiEvent> pendingEvents;

	bool isDAWPlaying;

	double lastPPQPosition = 0.0;
	double stepPPQIncrement = 0.0;
	int samplesPerStep = 0;
	int sampleCounter = 0;
	juce::AudioPlayHead::TimeSignature m_timeSignature;

	bool wasGUIUploaded;
	juce::ValueTree processorGUIState;
	bool wasGUIUpdated = true; // initialize to true to force the GUI to update on first load
	bool wasEdgeParamChanged = true; // initialize to true to force the GUI to update on first load
	void parameterChanged(const juce::String& parameterID, float newValue) override;

	std::unique_ptr<Service::PresetManager> presetManager;

	// TODO- check that all params are here!
	std::vector<juce::String> paramIDs = {
	"ballX0", "ballY0", "ballRadius0", "ballVelocity0", "ballAngle0", "ballXVelocity0", "ballYVelocity0",
	"ballX1", "ballY1", "ballRadius1", "ballVelocity1", "ballAngle1", "ballXVelocity1", "ballYVelocity1",
	"ballX2", "ballY2", "ballRadius2", "ballVelocity2", "ballAngle2", "ballXVelocity2", "ballYVelocity2",
	"edgePhase", "edgeDenomenator", "edgeRange", "scaleChoice", "rootNote", "edgeType",
	"ballsPositioningType", "snapToGrid", "collision", "quantization"
	};
};
