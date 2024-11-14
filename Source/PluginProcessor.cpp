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
					   ), midiBuffer(), pit(), valueTreeState(*this, nullptr, juce::Identifier("BallPitParams"), createParameters())
#endif
{
	this->isGUIUploaded = false;
	this->GUIState = juce::ValueTree("GUIState");

	// ball 1
	auto ball1 = std::make_unique<Ball>(0, 50.0f, 200.0f, 10.0f, 10.0f, 6.0f);
	ball1->setActive(true);
	auto midiListener1 = std::make_unique<BallEdgeEventListener>(midiBuffer);
	auto collisionListener1 = std::make_unique<BallCollideEventListener>(midiBuffer);
	ball1->setBallEdgeEventListener(midiListener1.get());
	ball1->setBallCollideEventListener(collisionListener1.get());
	pit.addBall(std::move(ball1));
	listeners.push_back(std::move(midiListener1));
	listeners.push_back(std::move(collisionListener1));

	// ball 2
	auto ball2 = std::make_unique<Ball>(1, 180.0f, 200.0f, 20.0f, 15.0f, 2.0f);
	ball2->setActive(false);
	auto midiListener2 = std::make_unique<BallEdgeEventListener>(midiBuffer);
	auto collisionListener2 = std::make_unique<BallCollideEventListener>(midiBuffer);
	ball2->setBallEdgeEventListener(midiListener2.get());
	ball2->setBallCollideEventListener(collisionListener2.get());
	pit.addBall(std::move(ball2));
	listeners.push_back(std::move(midiListener2));
	listeners.push_back(std::move(collisionListener2));

	// ball 3
	auto ball3 = std::make_unique<Ball>(2, 310.0f, 200.0f, 20.0f, 1.0f, 1.0f);
	ball3->setActive(false);
	auto midiListener3 = std::make_unique<BallEdgeEventListener>(midiBuffer);
	auto collisionListener3 = std::make_unique<BallCollideEventListener>(midiBuffer);
	ball3->setBallEdgeEventListener(midiListener3.get());
	ball3->setBallCollideEventListener(collisionListener3.get());
	pit.addBall(std::move(ball3));
	listeners.push_back(std::move(midiListener3));
	listeners.push_back(std::move(collisionListener3));
	
	getUpdatedBallParams();
	
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
	m_sampleRate = sampleRate;
	m_samplesPerBlock = samplesPerBlock;
	pit.setSampleRate(sampleRate);
	midiBuffer.clear();
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

void BallPitAudioProcessor::getUpdatedBallParams()
{
	for (int i = 0; i < 3; i++)
	{
		if (pit.getBalls()[i]->isActive() == false)	
			continue;

		std::string ballXId = "ballX" + std::to_string(i);
		std::string ballYId = "ballY" + std::to_string(i);
		std::string ballRadiusId = "ballRadius" + std::to_string(i);
		std::string ballVelocityId = "ballVelocity" + std::to_string(i);
		std::string ballAngleId = "ballAngle" + std::to_string(i);

		float x = valueTreeState.getRawParameterValue(ballXId)->load();
		float y = valueTreeState.getRawParameterValue(ballYId)->load();
		float radius = valueTreeState.getRawParameterValue(ballRadiusId)->load();
		float velocity = valueTreeState.getRawParameterValue(ballVelocityId)->load();
		float angle = valueTreeState.getRawParameterValue(ballAngleId)->load();

		pit.setBallParams(i, x, y, radius, velocity, angle);
	}
	pit.setBallsEdgeNotes();
}

void BallPitAudioProcessor::getUpdatedEdgeParams()
{
	int scaleChoice = valueTreeState.getRawParameterValue("scaleChoice")->load();
	int rootNote = 59 + valueTreeState.getRawParameterValue("rootNote")->load(); // 59 is the offset for starting the parameter at 1 to reach middle C = 60
	pit.setEdgeScale(static_cast<Scale::ScaleKinds>(scaleChoice), rootNote, 0);
	pit.setEdgeParams(valueTreeState.getRawParameterValue("edgePhase")->load(), 
					  valueTreeState.getRawParameterValue("edgeDenomenator")->load(),
					  valueTreeState.getRawParameterValue("edgeRange")->load());
}

void BallPitAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	if (auto* playhead = getPlayHead())
	{
		juce::AudioPlayHead::CurrentPositionInfo positionInfo;
		if (playhead->getCurrentPosition(positionInfo))
		{
			double bpm = positionInfo.bpm;
			double effectiveFrameRate = positionInfo.frameRate.getEffectiveRate();
			
			DBG("Current BPM: " << bpm);
			DBG("Current Frame Rate: " << effectiveFrameRate);
			
			if (bpm > 0 && effectiveFrameRate > 0)
			{
				double secondsPerBeat = 60.0 / bpm;
				const double pitWidth = 390.0;
				double speedPerFrame = pitWidth / (effectiveFrameRate * secondsPerBeat);
			}
		}
	}

	buffer.clear();
	if (this->pit.isBallsMoving() == false)
	{
		getUpdatedBallParams();
		getUpdatedEdgeParams();
	}
	else
	{
		pit.update();
	}
	// TODO - check this function again after all the changes
	juce::MidiBuffer::Iterator it(midiBuffer);
	juce::MidiMessage message;
	int samplePosition;

	for (auto pendingIt = pendingEvents.begin(); pendingIt != pendingEvents.end();)
	{
		if (pendingIt->samplePosition < m_samplesPerBlock)
		{
			DBG(pendingIt->message.getDescription() << "INSIDE MIDI BUFFER");
			midiMessages.addEvent(pendingIt->message, pendingIt->samplePosition);
			pendingIt = pendingEvents.erase(pendingIt);
		}
		else
		{
			pendingIt->samplePosition -= static_cast<int>(m_samplesPerBlock);
			++pendingIt;
		}
	}

	while (it.getNextEvent(message, samplePosition))
	{
		if (message.isNoteOn())
		{
			DBG(message.getDescription() << "NOTE ON");
			midiMessages.addEvent(message, samplePosition);
			continue;
		}

		if (samplePosition < m_samplesPerBlock)
		{
			DBG(message.getDescription() << "INSIDE MIDI BUFFER");
			midiMessages.addEvent(message, samplePosition);
		}
		else
		{
			DBG(message.getDescription() << "SENT PENDING");
			int newSamplePosition = samplePosition - static_cast<int>(m_samplesPerBlock);
			pendingEvents.push_back({message, newSamplePosition});
		}
	}

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
	
	std::unique_ptr<juce::XmlElement> xml(valueTreeState.copyState().createXml());
	copyXmlToBinary(*xml, destData);

}

void BallPitAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
	// You should use this method to restore your parameters from this memory block,
	// whose contents will have been created by the getStateInformation() call.
	
	if (auto xml = getXmlFromBinary(data, sizeInBytes))
	{
		valueTreeState.replaceState(juce::ValueTree::fromXml(*xml));
	}
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
	return new BallPitAudioProcessor();
}

Pit& BallPitAudioProcessor::getPit()
{
	return pit;
}

juce::StringArray getScaleOptions()
{
	return { "Chromatic", "Octatonic", "Dominant Diminished", "Diminished", "Major", "Minor",
			 "Melodic Minor", "Harmonic Minor", "Gypsy", "Symmetrical", "Enigmatic", "Arabian",
			 "Hungarian", "Whole Tone", "Augmented", "Blues Major", "Blues Minor",
			 "Pentatonic", "Minor Pentatonic" };
}

juce::AudioProcessorValueTreeState::ParameterLayout BallPitAudioProcessor::createParameters()
{
	juce::AudioProcessorValueTreeState::ParameterLayout params;

	for (int i = 0; i < 3; i++)
	{
		std::string ballXId = "ballX" + std::to_string(i);
		std::string ballYId = "ballY" + std::to_string(i);
		std::string ballRadiusId = "ballRadius" + std::to_string(i);
		std::string ballVelocityId = "ballVelocity" + std::to_string(i);
		std::string ballAngleId = "ballAngle" + std::to_string(i);

		params.add(std::make_unique<juce::AudioParameterFloat>(ballXId, "Ball X", 0.0f, 390.0f, 10.0f));
		params.add(std::make_unique<juce::AudioParameterFloat>(ballYId, "Ball Y", 0.0f, 390.0f, 10.0f));
		params.add(std::make_unique<juce::AudioParameterFloat>(ballRadiusId, "Radius", 5.0f, 25.0f, 0.5f));
		params.add(std::make_unique<juce::AudioParameterFloat>(ballVelocityId, "Velocity", 0.0f, 10.0f, 0.5f));
		params.add(std::make_unique<juce::AudioParameterFloat>(ballAngleId, "Angle", 0.0f, 360.0f, 1.0f));
	}

	std::string edgePhaseId = "edgePhase";
	params.add(std::make_unique<juce::AudioParameterFloat>(edgePhaseId, "Edge Phase", 0.0f, 360.0f, 0.0f));

	std::string edgeDenomenatorId = "edgeDenomenator";
	params.add(std::make_unique<juce::AudioParameterFloat>(edgeDenomenatorId, "Edge Denomenator", 1.0f, 8.0f, 1.0f));

	std::string edgeRangeId = "edgeRange";
	params.add(std::make_unique<juce::AudioParameterFloat>(edgeRangeId, "Edge Range", 1.0f, 8.0f, 1.0f));

	std::string scaleKindId = "scaleChoice";
	params.add(std::make_unique<juce::AudioParameterChoice>(scaleKindId, "Scale", getScaleOptions(), 5));
	
	std::string rootNoteId = "rootNote";
	params.add(std::make_unique<juce::AudioParameterInt>(rootNoteId, "Root Note", 0, 11, 0));

	return params;
}

void BallPitAudioProcessor::saveGUIState(juce::ValueTree &newGUIState)
{
	this->GUIState.copyPropertiesFrom(newGUIState, nullptr);
}