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
	jassert(velocity >= 0 && velocity <= 10);
	switch (velocity)
	{
	case 1: return 4.0;
	case 2: return 2.0;
	case 3: return 5.0 / 4.0;
	case 4: return 1.0;
	case 5: return 3.0 / 4.0;
	case 6: return 1.0 / 2.0;
	case 7: return 1.0 / 3.0;
	case 8: return 1.0 / 4.0;
	case 9: return 1.0 / 6.0;
	case 10: return 1.0 / 8.0;
	default: return 1.0;     // Safe default (1 beat)
	}
}

void BallPitAudioProcessor::setXYVelocityByTempo(float& xVelocity, float& yVelocity, float ballRadius)
{
	if (m_bpm <= 0)
		return;

	/*float beatsPerSecond = static_cast<double>(m_bpm) / SECONDS_IN_MINUTE;
	double pitWidth = static_cast<double>(PIT_WIDTH - (2 * PIT_INNER_DIFF) - (2.0 * ballRadius));
	float distancePerUpdate = pitWidth * beatsPerSecond;
	double denomeratorRatio = 4.0 / static_cast<double>(m_timeSignature.denominator);
	float effectiveVelocity = (distancePerUpdate / static_cast<double>(VISUAL_FRAMES_PER_SECOND)) / denomeratorRatio;*/
	
	const float beatsPerSecond = static_cast<float>(m_bpm) / SECONDS_IN_MINUTE;
	const double pitWidth = static_cast<double>(PIT_WIDTH - (2 * PIT_INNER_DIFF) - (2.0 * ballRadius));
	const float distancePerSecond = static_cast<float>(pitWidth * beatsPerSecond);
	const double denominatorRatio = 4.0 / static_cast<double>(m_timeSignature.denominator);
	const float effectiveVelocityPerSecond = distancePerSecond / static_cast<float>(denominatorRatio);

	if (std::abs(xVelocity) > 0.0001)
	{
		double deviation = velocityToInterval(static_cast<int>(xVelocity));
		jassert(deviation > 0.0001);
		xVelocity = (effectiveVelocityPerSecond / deviation);
	}

	if (std::abs(yVelocity) > 0.0001)
	{
		double deviation = velocityToInterval(static_cast<int>(yVelocity));
		jassert(deviation > 0.0001);
		yVelocity = (effectiveVelocityPerSecond / deviation);
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
	pit.setEdgeScale(static_cast<Scale::ScaleKinds>(scaleChoice), rootNote);
	pit.setEdgeParams(valueTreeState.getRawParameterValue("edgePhase")->load(),
					  valueTreeState.getRawParameterValue("edgeDenomenator")->load(),
					  valueTreeState.getRawParameterValue("edgeRange")->load(),
					  valueTreeState.getRawParameterValue("edgeType")->load() + 1); // +1 is for the edgetype offset
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

float BallPitAudioProcessor::getVariedNoteVelocity(int currentNoteVelocity)
{
	float volumeVariation = valueTreeState.getRawParameterValue("volumeVariation")->load();
	int velocityToAdd = (rand() % 100) - 50;
	velocityToAdd = std::round((velocityToAdd * volumeVariation));

	return (float)jmin((velocityToAdd + currentNoteVelocity), 127);
}

void BallPitAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
	buffer.clear();
	double timePassed = buffer.getNumSamples() / this->m_sampleRate;
	juce::Optional<juce::AudioPlayHead::PositionInfo> newPositionInfo;
	if (auto* playhead = getPlayHead())
	{
		newPositionInfo = playhead->getPosition();
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
		}
	}

	if (wasGUIUpdated == true)
	{
		getUpdatedEdgeParams();
		updateQuantization();
		wasGUIUpdated = false;
	}

	if (this->pit.areBallsMoving() == false)
	{
		clockTimeSeconds = 0.0;
		getUpdatedBallParams();
	}
	else 
	{
		if (this->isPlaying.get() == true)
		{
			//auto currentPPQ = newPositionInfo->getPpqPosition();
			//if (currentPPQ.hasValue())
			//{
			//	// TODO- maybe quantize 1 time to closest ppq...
			//}
		}
		clockTimeSeconds += timePassed;
		pit.update(timePassed);
	}

	double secondsPerBeat = SECONDS_IN_MINUTE / m_bpm;
	double secondsPerDivision = (secondsPerBeat * quantizationDivision / 4.0); // 1 division = 1/quantizationDivision of a measure
	std::vector<PendingMidiEvent> eventsToAdd;

	if (!pendingEvents.empty())
	{
		for (auto pendingIt = pendingEvents.begin(); pendingIt != pendingEvents.end();)
		{
			if (pendingIt->samplePosition < m_samplesPerBlock)
			{
				if (pendingIt->message.isNoteOn())
				{
					float var = getVariedNoteVelocity(pendingIt->message.getVelocity());
					pendingIt->message.setVelocity(var/127);
					midiMessages.addEvent(pendingIt->message, pendingIt->samplePosition);
					int noteDurationSamples = static_cast<int>(NOTE_MIDI_DURATION * m_sampleRate);

					int offSample = pendingIt->samplePosition + noteDurationSamples;
					if (offSample < m_samplesPerBlock)
					{
						juce::MidiMessage noteOff = juce::MidiMessage::noteOff(pendingIt->message.getChannel(), pendingIt->message.getNoteNumber());
						midiMessages.addEvent(noteOff, offSample);
					}
					else
					{
						juce::MidiMessage noteOff = juce::MidiMessage::noteOff(pendingIt->message.getChannel(),	pendingIt->message.getNoteNumber());

						int futureSample = offSample - m_samplesPerBlock;
						eventsToAdd.push_back({ noteOff, futureSample });
					}

					pendingIt = pendingEvents.erase(pendingIt);
				}
				else if (pendingIt->message.isNoteOff())
				{
					midiMessages.addEvent(pendingIt->message, pendingIt->samplePosition);
					pendingIt = pendingEvents.erase(pendingIt);
				}
			}
			else
			{
				pendingIt->samplePosition -= m_samplesPerBlock;
				++pendingIt;
			}
		}

		if (!eventsToAdd.empty())
		{
			pendingEvents.insert(pendingEvents.end(), eventsToAdd.begin(), eventsToAdd.end());
		}
	}

	for (const auto& metadata : midiBuffer)
	{
		auto msg = metadata.getMessage();
		if (!msg.isNoteOn())
			continue;

		double messageTimeSec = clockTimeSeconds + (metadata.samplePosition / m_sampleRate);
		double quantizedTimeSec = std::ceil(messageTimeSec / secondsPerDivision) * secondsPerDivision;

		// Convert quantized time to sample position relative to block enntry time
		int quantizedSamplePosAbsolute = static_cast<int>(quantizedTimeSec * m_sampleRate);
		int quantizedSamplePosRelative = quantizedSamplePosAbsolute - static_cast<int>(clockTimeSeconds * m_sampleRate);
		int finalSamplePos = static_cast<int>((1.0 - quantizationpercent) * metadata.samplePosition + quantizationpercent * quantizedSamplePosRelative);
		
		// add volume variation
		float var = getVariedNoteVelocity(msg.getVelocity());
		msg.setVelocity(var/127);

		if (finalSamplePos < m_samplesPerBlock)
		{
			midiMessages.addEvent(msg, finalSamplePos);
		}
		else
		{
			finalSamplePos = finalSamplePos = m_samplesPerBlock;
			pendingEvents.push_back({ msg, finalSamplePos });
		}

		int noteDurationSamples = static_cast<int>(NOTE_MIDI_DURATION * m_sampleRate);
		int noteOffPos = finalSamplePos + noteDurationSamples;

		if (noteOffPos < m_samplesPerBlock)
		{
			juce::MidiMessage noteOff = juce::MidiMessage::noteOff(msg.getChannel(), msg.getNoteNumber());
			midiMessages.addEvent(noteOff, noteOffPos);
		}
		else
		{
			int futureSample = noteOffPos - m_samplesPerBlock;
			juce::MidiMessage noteOff = juce::MidiMessage::noteOff(msg.getChannel(), msg.getNoteNumber());
			pendingEvents.push_back({ noteOff, futureSample });
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

void BallPitAudioProcessor::togglePlayState()
{
	this->pit.toggleBallMovement();
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
	return { "Cyclic up", "Cyclic down", "Ping pong", "Random"};
}

juce::StringArray getBallsPositioningTypes()
{
	return { "Chaos", "By Tempo" };
}

juce::StringArray getQuantizationDivisionTypes()
{
	return { "1/32", "1/16", "1/8", "1/4" };
}

juce::StringArray getsizePercentageTypes()
{
	return { "100%", "125%", "150%"};
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

		params.add(std::make_unique<juce::AudioParameterFloat>(ballXId, "Ball X", BALL_X_SLIDER_MIN, BALL_X_SLIDER_MAX, BALL_X_SLIDER_STEP));
		params.add(std::make_unique<juce::AudioParameterFloat>(ballYId, "Ball Y", BALL_Y_SLIDER_MIN, BALL_Y_SLIDER_MAX, BALL_Y_SLIDER_STEP));
		params.add(std::make_unique<juce::AudioParameterFloat>(ballRadiusId, "Radius", BALL_RADIUS_MIN, BALL_RADIUS_MAX, BALL_RADIUS_STEP));
		params.add(std::make_unique<juce::AudioParameterFloat>(ballVelocityId, "Velocity", BALL_VELOCITY_MIN, BALL_VELOCITY_MAX, BALL_VELOCITY_STEP));
		params.add(std::make_unique<juce::AudioParameterFloat>(ballAngleId, "Angle", BALL_ANGLE_MIN, BALL_ANGLE_MAX, BALL_ANGLE_STEP));
		params.add(std::make_unique<juce::AudioParameterFloat>(ballXVelocityId, "XVelocity", BALL_X_VELOCITY_MIN, BALL_X_VELOCITY_MAX, BALL_X_VELOCITY_STEP));
		params.add(std::make_unique<juce::AudioParameterFloat>(ballYVelocityId, "YVelocity", BALL_Y_VELOCITY_MIN, BALL_Y_VELOCITY_MAX, BALL_Y_VELOCITY_STEP));
	}

	std::string edgePhaseId = "edgePhase";
	params.add(std::make_unique<juce::AudioParameterFloat>(edgePhaseId, "Edge Phase", EDGE_PHASE_MIN, EDGE_PHASE_MAX, EDGE_PHASE_STEP));

	std::string edgeDenomenatorId = "edgeDenomenator";
	params.add(std::make_unique<juce::AudioParameterFloat>(edgeDenomenatorId, "Edge Denomenator", EDGE_DENOMINATOR_MIN, EDGE_DENOMINATOR_MAX, EDGE_DENOMINATOR_STEP));

	std::string edgeRangeId = "edgeRange";
	params.add(std::make_unique<juce::AudioParameterFloat>(edgeRangeId, "Edge Range", EDGE_RANGE_MIN, EDGE_RANGE_MAX, EDGE_RANGE_STEP));

	std::string scaleKindId = "scaleChoice";
	params.add(std::make_unique<juce::AudioParameterChoice>(scaleKindId, "Scale", getScaleOptions(), SCALE_DEFAULT));
	
	std::string rootNoteId = "rootNote";
	params.add(std::make_unique<juce::AudioParameterInt>(rootNoteId, "Root Note", ROOT_NOTE_C, ROOT_NOTE_B, ROOT_NOTE_DEFAULT));

	std::string edgeTypeID = "edgeType";
	params.add(std::make_unique<juce::AudioParameterChoice>(edgeTypeID, "Edge Type", getEdgeTypes(), EDGE_TYPE_DEFAULT));

	std::string ballsPositioningTypeId = "ballsPositioningType";
	params.add(std::make_unique<juce::AudioParameterChoice>(ballsPositioningTypeId, "BallsPositioning", getBallsPositioningTypes(), BALLS_POSITIONING_DEFAULT));

	std::string snapToGridId = "snapToGrid";
	params.add(std::make_unique<juce::AudioParameterBool>(snapToGridId, "Snap To Grid", false));

	std::string collisionId = "collision";
	params.add(std::make_unique<juce::AudioParameterBool>(collisionId, "Collision", true));

	std::string quantizationId = "quantization";
	params.add(std::make_unique<juce::AudioParameterFloat>(quantizationId, "Quantization", QUANTIZATION_MIN, QUANTIZATION_MAX, QUANTIZATION_DEFAULT));

	std::string quantizationDivisionID = "quantizationDivision";
	params.add(std::make_unique<juce::AudioParameterChoice>(quantizationDivisionID, "Quantization Division", getQuantizationDivisionTypes(), QUANTIZATION_DIV_DEFAULT));

	std::string volumeVariationId = "volumeVariation";
	params.add(std::make_unique<juce::AudioParameterFloat>(volumeVariationId, "volumeVariation", VOLUME_VARIATION_MIN, VOLUME_VARIATION_MAX, VOLUME_VARIATION_DEFAULT));
	
	std::string sizePercentageID = "sizePercentage";
	params.add(std::make_unique<juce::AudioParameterChoice>(sizePercentageID, "size Percentage", getsizePercentageTypes(), SIZE_PERCANTAGE_DEFAULT));
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