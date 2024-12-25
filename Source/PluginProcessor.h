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
#include "CostumeLogger.h"
#include "Configs.h"

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

	std::atomic<double> m_bpm {DEFAULT_BPM};
	double m_ppqPos = 0.0;

	juce::AudioPlayHead::TimeSignature m_timeSignatureDEBUG;

	CostumeLogger* getLogger() const { return m_logger.get(); }

	void printLoggedEvent(std::string toPrint)
	{
		if (m_logger)
			m_logger->logMessage(toPrint);
	}

	void getUpdatedBallParams();
	void getUpdatedEdgeParams();
	void setXYVelocityByTempo(float& xVelocity, float& yVelocity, float ballRadius);

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


	bool isGUIUploaded;

	std::unique_ptr<CostumeLogger> m_logger;
};
