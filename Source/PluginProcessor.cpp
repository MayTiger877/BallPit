/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
BallPitAudioProcessor::BallPitAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
	 : AudioProcessor (BusesProperties()
					 #if ! JucePlugin_IsMidiEffect
					  #if ! JucePlugin_IsSynth
					   .withInput  ("Input",  juce::AudioChannelSet::stereo(), true)
					  #endif
					   .withOutput ("Output", juce::AudioChannelSet::stereo(), true)
					 #endif
					   ), pit(), midiBuffer()
#endif
{
	// ball 1
	auto ball1 = std::make_unique<Ball>(250.0f, 250.0f, 10.0f, 10.0f, 6.0f);
	auto midiListener1 = std::make_unique<BallEdgeEventListener>(midiBuffer);
	auto collisionListener1 = std::make_unique<BallCollideEventListener>(midiBuffer);
	ball1->setBallEdgeEventListener(midiListener1.get());
	ball1->setBallCollideEventListener(collisionListener1.get());
	pit.addBall(std::move(ball1));
	listeners.push_back(std::move(midiListener1));
	listeners.push_back(std::move(collisionListener1));

	// ball 2
	auto ball2 = std::make_unique<Ball>(100.0f, 150.0f, 20.0f, 15.0f, 2.0f);
	auto midiListener2 = std::make_unique<BallEdgeEventListener>(midiBuffer);
	auto collisionListener2 = std::make_unique<BallCollideEventListener>(midiBuffer);
	ball2->setBallEdgeEventListener(midiListener2.get());
	ball2->setBallCollideEventListener(collisionListener2.get());
	pit.addBall(std::move(ball2));
	listeners.push_back(std::move(midiListener2));
	listeners.push_back(std::move(collisionListener2));

	// ball 3
	auto ball3 = std::make_unique<Ball>(100.0f, 100.0f, 20.0f, 1.0f, 6.0f);
	auto midiListener3 = std::make_unique<BallEdgeEventListener>(midiBuffer);
	auto collisionListener3 = std::make_unique<BallCollideEventListener>(midiBuffer);
	ball3->setBallEdgeEventListener(midiListener3.get());
	ball3->setBallCollideEventListener(collisionListener3.get());
	pit.addBall(std::move(ball3));
	listeners.push_back(std::move(midiListener3));
	listeners.push_back(std::move(collisionListener3));
	
}

BallPitAudioProcessor::~BallPitAudioProcessor()
{
}

//==============================================================================
const juce::String BallPitAudioProcessor::getName() const
{
	return JucePlugin_Name;
}

bool BallPitAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
	return true;
   #else
	return false;
   #endif
}

bool BallPitAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
	return true;
   #else
	return false;
   #endif
}

bool BallPitAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
	return true;
   #else
	return false;
   #endif
}

double BallPitAudioProcessor::getTailLengthSeconds() const
{
	return 0.0;
}

int BallPitAudioProcessor::getNumPrograms()
{
	return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
				// so this should be at least 1, even if you're not really implementing programs.
}

int BallPitAudioProcessor::getCurrentProgram()
{
	return 0;
}

void BallPitAudioProcessor::setCurrentProgram (int index)
{
}

const juce::String BallPitAudioProcessor::getProgramName (int index)
{
	return {};
}

void BallPitAudioProcessor::changeProgramName (int index, const juce::String& newName)
{
}

//==============================================================================
void BallPitAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
	pit.setSampleRate(this->getSampleRate());
}

void BallPitAudioProcessor::releaseResources()
{
	// When playback stops, you can use this as an opportunity to free up any
	// spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool BallPitAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
	juce::ignoreUnused (layouts);
	return true;
  #else
	// This is the place where you check if the layout is supported.
	// In this template code we only support mono or stereo.
	// Some plugin hosts, such as certain GarageBand versions, will only
	// load plugins that support stereo bus layouts.
	if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
	 && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
		return false;

	// This checks if the input layout matches the output layout
   #if ! JucePlugin_IsSynth
	if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
		return false;
   #endif

	return true;
  #endif
}
#endif

void dumpMIDIMessages(juce::MidiBuffer& midiMessages, juce::MidiBuffer *midiBufferPtr)
{
	juce::MidiBuffer midiBuffer = *midiBufferPtr;
	juce::MidiBuffer::Iterator it(midiBuffer);
	juce::MidiMessage message;
	int samplePosition;

	while (it.getNextEvent(message, samplePosition))
	{
		midiMessages.addEvent(message, samplePosition);
	}

	midiBuffer.clear();
}

void BallPitAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	buffer.clear(); 
	//pit.update();
	dumpMIDIMessages(midiMessages, &midiBuffer);
	midiBuffer.clear();
}

//==============================================================================
bool BallPitAudioProcessor::hasEditor() const
{
	return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* BallPitAudioProcessor::createEditor()
{
	return new BallPitAudioProcessorEditor (*this);
}

//==============================================================================
void BallPitAudioProcessor::getStateInformation (juce::MemoryBlock& destData)
{
	// You should use this method to store your parameters in the memory block.
	// You could do that either as raw data, or use the XML or ValueTree classes
	// as intermediaries to make it easy to save and load complex data.
}

void BallPitAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new BallPitAudioProcessor();
}

const Pit& BallPitAudioProcessor::getPit() const
{
	return pit;
}