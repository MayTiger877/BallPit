/*
  ==============================================================================

	This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"
#include <chrono>

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
	this->wasGUIUploaded = false;
	this->processorGUIState = juce::ValueTree("GUIState");

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
	this->isDAWPlaying = false;

	valueTreeState.state.setProperty(Service::PresetManager::presetNameProperty, "", nullptr);
	valueTreeState.state.setProperty("version", ProjectInfo::versionString, nullptr);
	addParamListeners(valueTreeState);

	presetManager = std::make_unique<Service::PresetManager>(valueTreeState);
}

BallPitAudioProcessor::~BallPitAudioProcessor()
{
	valueTreeState.removeParameterListener("someParameter", this);
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

	lastPPQPosition = 0.0;
	stepPPQIncrement = 0.0;
	samplesPerStep = static_cast<int>(sampleRate / SECONDS_IN_MINUTE);
	sampleCounter = 0;
	m_timeSignature.numerator = DEFAULT_TIME_SIGNATURE_NUMERATOR;
	m_timeSignature.denominator = DEFAULT_TIME_SIGNATURE_DENOMINATOR;

	clockTimeSeconds = 0.0;
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

double velocityToInterval(int velocity)
{
	switch (velocity)
	{
	case 0:
		return 0.0;
	case 1:
		return 4.0;
	case 2:
		return 2.0;
	case 3:
		return 1.25;
	case 4:
		return 1.0;
	case 5:
		return 0.75;
	case 6:
		return 0.5;
	case 7:
		return 0.33;
	case 8:
		return 0.25;
	case 9:
		return 0.166;
	case 10:
		return 0.125;
	default:
		break;
	}
}

// enterd every 1/60 seconds (60Hz)
void BallPitAudioProcessor::setXYVelocityByTempo(float& xVelocity, float& yVelocity, float ballRadius)
{
	if (m_bpm > 0)
	{
		float beatsPerSecond = m_bpm / SECONDS_IN_MINUTE;
		double pitWidth = (390.0 - (2.0 * ballRadius));
		float distancePerUpdate = pitWidth * beatsPerSecond;
		double denomeratorRatio = 4.0 / static_cast<double>(m_timeSignature.denominator);
		float effectiveVelocity = (distancePerUpdate / VISUAL_FRAMES_PER_SECOND) / denomeratorRatio;

		double deviation = velocityToInterval(static_cast<int>(xVelocity));
		xVelocity = (deviation != 0) ? (effectiveVelocity / deviation) : 0.0f;

		deviation = velocityToInterval(static_cast<int>(yVelocity));
		yVelocity = (deviation != 0) ? (effectiveVelocity / deviation) : 0.0f;
	}
}

static void getAngleAndVelocity(float& angle, float& velocity, float xVelocity, float yVelocity)
{
	velocity = sqrt(pow(xVelocity, 2) + pow(yVelocity, 2));
	angle = atan2(yVelocity, xVelocity) * 180 / juce::MathConstants<float>::pi;
}

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
		std::string ballXVelocityId = "ballXVelocity" + std::to_string(i);
		std::string ballYVelocityId = "ballYVelocity" + std::to_string(i);

		float x = valueTreeState.getRawParameterValue(ballXId)->load();
		float y = valueTreeState.getRawParameterValue(ballYId)->load();
		float radius = valueTreeState.getRawParameterValue(ballRadiusId)->load();
		float velocity = valueTreeState.getRawParameterValue(ballVelocityId)->load();
		float angle = valueTreeState.getRawParameterValue(ballAngleId)->load();
		float xVelocity = valueTreeState.getRawParameterValue(ballXVelocityId)->load();
		float yVelocity = valueTreeState.getRawParameterValue(ballYVelocityId)->load();

		int ballsPosType = 1 + this->valueTreeState.getRawParameterValue("ballsPositioningType")->load(); // 1 is offset
		switch (ballsPosType)
		{
			case 1: // chaos
			{
				break;
			}
			case 2: // by tempo
			{
				setXYVelocityByTempo(xVelocity, yVelocity, radius);
				getAngleAndVelocity(angle, velocity, xVelocity, yVelocity);
				break;
			}
		}
		pit.setBallParams(i, x, y, radius, velocity, angle, ballsPosType);
	}
	pit.setCollision(static_cast<bool>(valueTreeState.getRawParameterValue("collision")->load()));
}

void BallPitAudioProcessor::getUpdatedEdgeParams()
{
	int scaleChoice = valueTreeState.getRawParameterValue("scaleChoice")->load();
	int rootNote = 59 + valueTreeState.getRawParameterValue("rootNote")->load(); // 59 is the offset for starting the parameter at 1 to reach middle C = 60
	pit.setEdgeScale(static_cast<Scale::ScaleKinds>(scaleChoice), rootNote, 0);
	pit.setEdgeParams(valueTreeState.getRawParameterValue("edgePhase")->load(),
					  valueTreeState.getRawParameterValue("edgeDenomenator")->load(),
					  valueTreeState.getRawParameterValue("edgeRange")->load(),
					  valueTreeState.getRawParameterValue("edgeType")->load());
	pit.setBallsEdgeNotes();
}

void BallPitAudioProcessor::updateQuantization()
{
	quantizationpercent = valueTreeState.getRawParameterValue("quantization")->load();
	int quantizationDivisionChoice = valueTreeState.getRawParameterValue("quantizationDivision")->load();
	switch (quantizationDivisionChoice)
	{
	case 0:
		quantizationDivision = (1.0 / 32.0);
		break;
	case 1:
		quantizationDivision = (1.0 / 16.0);
		break;
	case 2:
		quantizationDivision = (1.0 / 8.0);
		break;
	case 3:
		quantizationDivision = (1.0 / 4.0);
		break;
	default:
		quantizationDivision = (1.0 / 32.0);
		break;
	}
}

void BallPitAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	buffer.clear();
	bool tickPassed = false;

	if (auto* playhead = getPlayHead())
	{
		juce::Optional<juce::AudioPlayHead::PositionInfo> newPositionInfo = playhead->getPosition();
		if (newPositionInfo.hasValue())
		{
			auto bpm = newPositionInfo->getBpm();
			m_bpm = bpm.hasValue() ? (*bpm) : DEFAULT_BPM;

			auto timeSig = newPositionInfo->getTimeSignature();
			if (timeSig.hasValue())
			{
				m_timeSignature.numerator = (*timeSig).numerator;
				m_timeSignature.denominator = (*timeSig).denominator;
			}

			this->isDAWPlaying = newPositionInfo->getIsPlaying();
			if (this->isPlaying.exchange(this->isDAWPlaying) != this->isDAWPlaying)
			{
				sendChangeMessage(); // Notify the editor of a state change
			}

			auto ppqPosition = newPositionInfo->getPpqPosition();
			m_ppqPos = ppqPosition.hasValue() ? (*ppqPosition) : 0.0;
			if (m_bpm > 0.0 && m_ppqPos > 0.0)
			{
				const double secondsPerBeat = SECONDS_IN_MINUTE / m_bpm;
				stepPPQIncrement = 1.0 / secondsPerBeat / SECONDS_IN_MINUTE;

				sampleCounter += buffer.getNumSamples();
				if (sampleCounter >= samplesPerStep)
				{
					sampleCounter -= samplesPerStep;

					if (std::floor(m_ppqPos) != std::floor(lastPPQPosition))
					{
						tickPassed = true;
					}

					lastPPQPosition += stepPPQIncrement;
				}
			}
		}
	}

	if (wasGUIUpdated == true)
	{
		getUpdatedEdgeParams();
		updateQuantization();
		wasGUIUpdated = false;
	}
	if ((this->isDAWPlaying == false) && (this->pit.areBallsMoving() == false))
	{
		clockTimeSeconds = 0.0;
		getUpdatedBallParams();
	}
	else 
	{
		clockTimeSeconds += buffer.getNumSamples() / this->m_sampleRate;

		if (this->isDAWPlaying == true)
		{
			// TODO : figure out if need the START button and how to sync it with the DAW
			/*if (pit.areBallsMoving())
			{
				pit.toggleBallMovement();
			}*/

			if (tickPassed == true)
			{
				tickPassed = false;
				pit.update();
			}
		}
		else if (this->pit.areBallsMoving() == true)
		{
			pit.update();
		}
	}

	int samplePosition;
	double secondsPerBeat = (60.0 / m_bpm);
	double secondsPerDivision = (secondsPerBeat * quantizationDivision / 4);
	double samplesPerDivision = (secondsPerDivision * m_sampleRate);

	float temp = std::ceil(this->clockTimeSeconds / secondsPerDivision);
	int nextQuantizedSamplePos = (static_cast<int>(temp * secondsPerDivision * m_sampleRate));

	for (auto pendingIt = pendingEvents.begin(); pendingIt != pendingEvents.end();)
	{
		if (pendingIt->samplePosition < m_samplesPerBlock)
		{
			if (pendingIt->message.isNoteOn())
			{
				midiMessages.addEvent(pendingIt->message, pendingIt->samplePosition);
				juce::MidiMessage noteOff = juce::MidiMessage::noteOff(pendingIt->message.getChannel(), pendingIt->message.getNoteNumber());
				int noteDurationSamples = static_cast<int>(0.2 * m_sampleRate); // 200ms note duration
				if ((pendingIt->samplePosition + noteDurationSamples) < m_samplesPerBlock)
				{
					midiMessages.addEvent(noteOff, pendingIt->samplePosition + noteDurationSamples);
					pendingIt = pendingEvents.erase(pendingIt);
				}
				else
				{
					pendingEvents.push_back({ noteOff, (pendingIt->samplePosition + noteDurationSamples - static_cast<int>(m_samplesPerBlock)) });
				}
			}
			else if (pendingIt->message.isNoteOff())
			{
				midiMessages.addEvent(pendingIt->message, pendingIt->samplePosition);
				pendingIt = pendingEvents.erase(pendingIt);
			}
		}
		else
		{
			pendingIt->samplePosition -= static_cast<int>(m_samplesPerBlock);
			++pendingIt;
		}
	}

	for (const auto metadata : midiBuffer)
	{
		auto msg = metadata.getMessage();
		if (msg.isNoteOn())
		{
			samplePosition = metadata.samplePosition;
			int realTimeSamplePosition = (clockTimeSeconds * m_sampleRate) + samplePosition;
			int quantizationDelta = ((nextQuantizedSamplePos - realTimeSamplePosition) * quantizationpercent);
			int finalSamplePos = (quantizationDelta + samplePosition);
			if (finalSamplePos < m_samplesPerBlock)
			{
				midiMessages.addEvent(msg, finalSamplePos);
				juce::MidiMessage noteOff = juce::MidiMessage::noteOff(msg.getChannel(), msg.getNoteNumber());
				int noteDurationSamples = static_cast<int>(0.2 * m_sampleRate); // 200ms note duration
				if ((finalSamplePos + noteDurationSamples) < m_samplesPerBlock)
				{
					midiMessages.addEvent(noteOff, finalSamplePos + noteDurationSamples);
				}
				else
				{
					pendingEvents.push_back({ noteOff, (finalSamplePos + noteDurationSamples - static_cast<int>(m_samplesPerBlock)) });
				}
			}
			else
			{
				pendingEvents.push_back({ msg, (finalSamplePos - static_cast<int>(m_samplesPerBlock)) });
			}
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

juce::StringArray getEdgeTypes()
{
	return { "Cyclic up", "Cyclic down" };
}

juce::StringArray getBallsPositioningTypes()
{
	return { "Chaos", "By Tempo" };
}

juce::StringArray getQuantizationDivisionTypes()
{
	return { "1/32", "1/16", "1/8", "1/4" };
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
		std::string ballXVelocityId = "ballXVelocity" + std::to_string(i);
		std::string ballYVelocityId = "ballYVelocity" + std::to_string(i);

		params.add(std::make_unique<juce::AudioParameterFloat>(ballXId, "Ball X", BALL_X_SLIDER_MIN, BALL_X_SLIDER_MAX, 10.0f));
		params.add(std::make_unique<juce::AudioParameterFloat>(ballYId, "Ball Y", BALL_Y_SLIDER_MIN, BALL_Y_SLIDER_MAX, 10.0f));
		params.add(std::make_unique<juce::AudioParameterFloat>(ballRadiusId, "Radius", 5.0f, 25.0f, 0.5f));
		params.add(std::make_unique<juce::AudioParameterFloat>(ballVelocityId, "Velocity", 0.0f, 10.0f, 0.5f));
		params.add(std::make_unique<juce::AudioParameterFloat>(ballAngleId, "Angle", 0.0f, 360.0f, 1.0f));
		params.add(std::make_unique<juce::AudioParameterFloat>(ballXVelocityId, "XVelocity", 0.0f, 10.0f, 1.0f));
		params.add(std::make_unique<juce::AudioParameterFloat>(ballYVelocityId, "YVelocity", 0.0f, 10.0f, 1.0f));
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

	std::string edgeTypeID = "edgeType";
	params.add(std::make_unique<juce::AudioParameterChoice>(edgeTypeID, "Edge Type", getEdgeTypes(), 0));

	std::string ballsPositioningTypeId = "ballsPositioningType";
	params.add(std::make_unique<juce::AudioParameterChoice>(ballsPositioningTypeId, "BallsPositioning", getBallsPositioningTypes(), 0));

	std::string snapToGridId = "snapToGrid";
	params.add(std::make_unique<juce::AudioParameterBool>(snapToGridId, "Snap To Grid", false));

	std::string collisionId = "collision";
	params.add(std::make_unique<juce::AudioParameterBool>(collisionId, "Collision", true));

	std::string quantizationId = "quantization";
	params.add(std::make_unique<juce::AudioParameterBool>(quantizationId, "Quantization", true));

	std::string quantizationDivisionID = "quantizationDivision";
	params.add(std::make_unique<juce::AudioParameterChoice>(quantizationDivisionID, "Quantization Division", getQuantizationDivisionTypes(), 1));
	
	return params;
}

void BallPitAudioProcessor::addParamListeners(juce::AudioProcessorValueTreeState& apvts)
{	
	for (const auto& paramID : paramIDs)
	{
		if (apvts.getParameter(paramID) != nullptr)
		{
			apvts.addParameterListener(paramID, this);
		}
	}
}

void BallPitAudioProcessor::saveGUIState(juce::ValueTree &currentGUIState)
{
	processorGUIState.copyPropertiesFrom(currentGUIState, nullptr);
}

void BallPitAudioProcessor::parameterChanged(const juce::String& parameterID, float newValue)
{
	wasGUIUpdated = true;
}