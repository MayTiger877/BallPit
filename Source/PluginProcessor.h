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
#include "TempoManager.h"

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
								public juce::ChangeBroadcaster
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
	juce::AudioProcessorValueTreeState& getValueTreeState() { return valueTreeState; }

	juce::Atomic<bool> isPlaying;

	juce::ValueTree GUIState;
	juce::ValueTree& getGUIState() { return GUIState; }
	void saveGUIState(juce::ValueTree &newGUIState);

	Pit& getPit();

	void updateGUIFlag(bool newStatus) { isGUIUploaded = newStatus; }
	bool getIsGUIUploaded() { return isGUIUploaded; }

	bool isHostPlaying() const { return isDAWPlaying; }

	std::atomic<double> BPM {0.0}; // debug
	std::atomic<double> FrameRate { 0.0 }; // debug
	std::atomic<double> elapsedTime { 0.0 }; // debug
	std::chrono::steady_clock::time_point lastProcessTime;

	//--------------------------------------------------------------
	juce::AudioPlayHead::PositionInfo playheadPosition_;
	double getCurrentBeatPos();
	int getBeat() { return tickState.beat; }

	TicksHolder& getTicks() { return ticks; }
	TickSettings& getState() { return settings; }

private:
	double m_sampleRate;
	double m_samplesPerBlock;
	
	Pit pit;
	juce::MidiBuffer midiBuffer;
	std::vector<std::unique_ptr<EdgeEventListener>> listeners;
	std::vector<PendingMidiEvent> pendingEvents;

	bool isDAWPlaying;

	void getUpdatedBallParams();
	void getUpdatedEdgeParams();

	bool isGUIUploaded;

	//--------------------------------------------------------------------------------

	void handlePreCount(double inputPPQ);
	double ppqEndVal;

	// audio samples bank
	TicksHolder ticks;

	// programmable assignment of samples per beat.
	// hard-coded to support upto 64.
	TickSettings settings;

	struct TickState
	{
		int currentSample = -1;
		int tickLengthInSamples = 0;
		int tickStartPosition = 0;
		int beat = 0;
		double beatPos = 0;
		juce::AudioSampleBuffer sample;
		float* refer[1];
		bool isClear = true;

		void fillTickSample(juce::MidiBuffer& bufferToFill);
		void addTickSample(juce::MidiBuffer& bufferToFill, int startPos, int endPos);
		void clear();
	} tickState;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BallPitAudioProcessor)
};
