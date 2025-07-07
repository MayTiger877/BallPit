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
					   ), midiBuffer(), pit(), valueTreeState(*this, &m_undoManager, juce::Identifier("BallPitParams"), createParameters())
#endif
{
	this->wasGUIUploaded.set(false);
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

	m_undoManager.setMaxNumberOfStoredUnits(20, 1);

	ballsSnapshot[0] = ball1->getBallGUINesseceities();
	ballsSnapshot[1] = ball2->getBallGUINesseceities();
	ballsSnapshot[2] = ball3->getBallGUINesseceities();

	abstractedEdgeColors = std::vector<int>(1568, 0); // Initialize with 0s

	updateBallsSnapshot();
	updateAbstractedEdgeColors();
}

BallPitAudioProcessor::~BallPitAudioProcessor()
{
	for (const auto& paramID : paramIDs)
	{
		if (this->valueTreeState.getParameter(paramID) != nullptr)
		{
			this->valueTreeState.removeParameterListener(paramID, this);
		}
	}
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

	auto ts = m_timeSignature.get();
	ts.numerator = DEFAULT_TIME_SIGNATURE_NUMERATOR;
	ts.denominator = DEFAULT_TIME_SIGNATURE_DENOMINATOR;
	m_timeSignature.set(ts);

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

float getDelayRateInSeconds(int delayRateChoice, float bpm, int sampleRate)
{
	if (bpm <= 0 || sampleRate <= 0)
		return 0.0f;

	const float secondsPerBeat = SECONDS_IN_MINUTE / bpm;
	const float secondsPerDivision = secondsPerBeat / 4.0f; // 1 division = 1/4 of a beat
	switch (delayRateChoice)
	{
	case 1: return secondsPerDivision * 4.0f; // 4
	case 2: return secondsPerDivision * 2.0f; // 2
	case 3: return secondsPerDivision * 1.0F; // 1
	case 4: return secondsPerDivision / 2.0f; // 1/2
	case 5: return secondsPerDivision / 4.0f; // 1/4
	case 6: return secondsPerDivision / 8.0f; // 1/8
	default: return secondsPerDivision;        // Default to 1 division
	}
}

void BallPitAudioProcessor::setXYVelocityByTempo(float& xVelocity, float& yVelocity, float ballRadius)
{
	if (m_bpm.get() <= 0)
		return;
	
	const float beatsPerSecond = static_cast<float>(m_bpm.get()) / SECONDS_IN_MINUTE;
	const double pitWidth = static_cast<double>(PIT_WIDTH - (2 * PIT_INNER_DIFF) - (2.0 * ballRadius));
	const float distancePerSecond = static_cast<float>(pitWidth * beatsPerSecond);
	auto ts = m_timeSignature.get();
	const double denominatorRatio = 4.0 / static_cast<double>(ts.denominator);
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
	velocity = sqrt((xVelocity * xVelocity) + (yVelocity * yVelocity));
	angle = atan2(((-1) * yVelocity), xVelocity) * (180 / PI);
}

void BallPitAudioProcessor::getUpdatedBallParams()
{
	for (int i = 0; i < 3; i++)
	{
		std::string BallActivationId = "BallActivation" + std::to_string(i);
		bool ballActivation = static_cast<bool>(valueTreeState.getRawParameterValue(BallActivationId)->load());
		pit.getBalls()[i]->setActive(ballActivation);

		if (pit.getBalls()[i]->isActive() == false)
			continue;

		std::string ballXId = "ballX" + std::to_string(i);
		std::string ballYId = "ballY" + std::to_string(i);
		std::string ballRadiusId = "ballRadius" + std::to_string(i);
		std::string ballVelocityId = "ballVelocity" + std::to_string(i);
		std::string ballAngleId = "ballAngle" + std::to_string(i);
		std::string ballXVelocityId = "ballXVelocity" + std::to_string(i);
		std::string ballYVelocityId = "ballYVelocity" + std::to_string(i);
		std::string xVelocityInverterId = "xVelocityInverter" + std::to_string(i);
		std::string yVelocityInverterId = "yVelocityInverter" + std::to_string(i);

		float x = valueTreeState.getRawParameterValue(ballXId)->load();
		float y = valueTreeState.getRawParameterValue(ballYId)->load();
		float radius = valueTreeState.getRawParameterValue(ballRadiusId)->load();
		float velocity = valueTreeState.getRawParameterValue(ballVelocityId)->load();
		float angle = valueTreeState.getRawParameterValue(ballAngleId)->load();
		float xVelocity = valueTreeState.getRawParameterValue(ballXVelocityId)->load();
		float yVelocity = valueTreeState.getRawParameterValue(ballYVelocityId)->load();
		float delayRateInSeconds = getDelayRateInSeconds(static_cast<int>(valueTreeState.getRawParameterValue("delayRate" + std::to_string(i))->load()) + 1, this->m_bpm.get(), this->m_sampleRate.get());

		DelaySettings newDelaySettings = { static_cast<int>(valueTreeState.getRawParameterValue("delayAmount" + std::to_string(i))->load()),
										   valueTreeState.getRawParameterValue("delayFeedback" + std::to_string(i))->load(),
										   delayRateInSeconds,
										   static_cast<int>(valueTreeState.getRawParameterValue("delayNoteMovement" + std::to_string(i))->load()) + 1 };

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
				float inverter = (static_cast<bool>(valueTreeState.getRawParameterValue(xVelocityInverterId)->load())) ? (-1.0) : 1.0;
				xVelocity = xVelocity * inverter;
				inverter = (static_cast<bool>(valueTreeState.getRawParameterValue(yVelocityInverterId)->load())) ? (-1.0) : 1.0;
				yVelocity = yVelocity * inverter;
				getAngleAndVelocity(angle, velocity, xVelocity, yVelocity);
				break;
			}
		}
		pit.setBallParams(i, x, y, radius, velocity, angle, ballsPosType, newDelaySettings);
	}
	pit.setCollision(static_cast<bool>(valueTreeState.getRawParameterValue("collision")->load()));
	pit.setBallsTranspose(valueTreeState.getRawParameterValue("transpose")->load());
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
	juce::Optional<juce::AudioPlayHead::PositionInfo> newPositionInfo;
	if (auto* playhead = getPlayHead())
	{
		newPositionInfo = playhead->getPosition();
		if (newPositionInfo.hasValue())
		{
			auto bpm = newPositionInfo->getBpm();
			m_bpm.set(bpm.hasValue() ? (*bpm) : DEFAULT_BPM);

			auto timeSig = newPositionInfo->getTimeSignature();
			if (timeSig.hasValue())
			{
				auto ts = m_timeSignature.get();
				ts.numerator = (*timeSig).numerator;
				ts.denominator = (*timeSig).denominator;
				m_timeSignature.set(ts);
			}
		}
	}

	if (wasGUIUpdated.get() == true)
	{
		getUpdatedEdgeParams();
		updateQuantization();
		wasGUIUpdated.set(false);
	}

	double timePassed = buffer.getNumSamples() / this->m_sampleRate.get();
	if (pit.areBallsMoving() == false)
	{
		clockTimeSeconds = 0.0;
		getUpdatedBallParams();
		for (int note : activeNotes)
		{
		    auto panicNoteOff = juce::MidiMessage::noteOff(1, note);
		    midiMessages.addEvent(panicNoteOff, m_samplesPerBlock.get() - 1);
		}
		activeNotes.clear();
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
		pit.update(timePassed, clockTimeSeconds);
	}

	int randomProbabilityDecider = (rand() % 100) + 1;
	float currentNotePlay = 1.0f;
	m_probability = valueTreeState.getRawParameterValue("probability")->load();
	if (randomProbabilityDecider > m_probability.get())
	{
		currentNotePlay = 0.0f;
	}
	double secondsPerBeat = SECONDS_IN_MINUTE / m_bpm.get();
	double secondsPerDivision = (secondsPerBeat * quantizationDivision.get() / 4.0); // 1 division = 1/quantizationDivision of a measure
	std::vector<PendingMidiEvent> eventsToAdd;

	if (!pendingEvents.empty())
	{
		for (auto pendingIt = pendingEvents.begin(); pendingIt != pendingEvents.end();)
		{
			if (pendingIt->samplePosition < (m_samplesPerBlock.get() - 1)) // -1 because i want to add the noteoff at samplePosition and 1 after the noteon
			{
				if (pendingIt->message.isNoteOn())
				{
					float var = getVariedNoteVelocity(pendingIt->message.getVelocity());
				    pendingIt->message.setVelocity((currentNotePlay * var)/127);

				    // [2] BEFORE noteOn, ensure note is turned off if it's active
				    if (activeNotes.count(pendingIt->message.getNoteNumber()) > 0)
				    {
				        int earlyOffSample = juce::jmax(0, pendingIt->samplePosition - 2);
				        juce::MidiMessage earlyNoteOff = juce::MidiMessage::noteOff(
				            pendingIt->message.getChannel(), pendingIt->message.getNoteNumber()
				        );
				        midiMessages.addEvent(earlyNoteOff, earlyOffSample);
				        activeNotes.erase(pendingIt->message.getNoteNumber());
				    }
				
				    // [3] Send the noteOn and mark it as active
				    midiMessages.addEvent(pendingIt->message, pendingIt->samplePosition + 1);
				    activeNotes.insert(pendingIt->message.getNoteNumber());
				
				    // [4] Schedule proper noteOff
				    int noteDurationSamples = static_cast<int>(NOTE_MIDI_DURATION * m_sampleRate.get());
				    int offSample = pendingIt->samplePosition + noteDurationSamples;
				    juce::MidiMessage noteOff = juce::MidiMessage::noteOff(
				        pendingIt->message.getChannel(), pendingIt->message.getNoteNumber()
				    );
				
				    if (offSample < m_samplesPerBlock.get())
				    {
				        midiMessages.addEvent(noteOff, offSample);
				    }
				    else
				    {
				        int futureSample = offSample - m_samplesPerBlock.get();
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
				pendingIt->samplePosition -= m_samplesPerBlock.get();
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

		double messageTimeSec = clockTimeSeconds + (metadata.samplePosition / m_sampleRate.get());
		double quantizedTimeSec = std::ceil(messageTimeSec / secondsPerDivision) * secondsPerDivision;

		// Convert quantized time to sample position relative to block enntry time
		int quantizedSamplePosAbsolute = static_cast<int>(quantizedTimeSec * m_sampleRate.get());
		int quantizedSamplePosRelative = quantizedSamplePosAbsolute - static_cast<int>(clockTimeSeconds * m_sampleRate.get());
		int finalSamplePos = static_cast<int>((1.0 - quantizationpercent.get()) * metadata.samplePosition + quantizationpercent.get() * quantizedSamplePosRelative);
		
		// add volume variation and 
		float var = getVariedNoteVelocity(msg.getVelocity());
		msg.setVelocity((currentNotePlay * var) / 127);
		juce::MidiMessage noteOff = juce::MidiMessage::noteOff(msg.getChannel(), msg.getNoteNumber());

		if (activeNotes.count(msg.getNoteNumber()) > 0)
		{
		    int earlyOffSample = juce::jmax(0, finalSamplePos - 2);
		    juce::MidiMessage earlyNoteOff = juce::MidiMessage::noteOff(msg.getChannel(), msg.getNoteNumber());
		    midiMessages.addEvent(earlyNoteOff, earlyOffSample);
		    activeNotes.erase(msg.getNoteNumber());
		}

		if (finalSamplePos < (m_samplesPerBlock.get() + 1))
		{
		    midiMessages.addEvent(msg, finalSamplePos + 1); // NoteOn
		    activeNotes.insert(msg.getNoteNumber());
		
		    midiMessages.addEvent(noteOff, finalSamplePos); // Immediate off (if needed)
		}
		else
		{
			finalSamplePos = finalSamplePos - m_samplesPerBlock.get();
			pendingEvents.push_back({ msg, finalSamplePos });
		}

		int noteDurationSamples = static_cast<int>(NOTE_MIDI_DURATION * m_sampleRate.get());
		int noteOffPos = finalSamplePos + noteDurationSamples;

		if (noteOffPos < m_samplesPerBlock.get())
		{
			midiMessages.addEvent(noteOff, noteOffPos);
		}
		else
		{
			int futureSample = noteOffPos - m_samplesPerBlock.get();
			juce::MidiMessage noteOff = juce::MidiMessage::noteOff(msg.getChannel(), msg.getNoteNumber());
			pendingEvents.push_back({ noteOff, futureSample });
		}
	}

	updateBallsSnapshot();
	updateAbstractedEdgeColors();
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

juce::StringArray getScaleOptions()
{
	return { "Chromatic", "Octatonic", "Dominant Diminished", "Diminished", "Major", "Minor",
			 "Melodic Minor", "Harmonic Minor", "Gypsy", "Symmetrical", "Enigmatic", "Arabian",
			 "Hungarian", "Whole Tone", "Augmented", "Blues Major", "Blues Minor",
			 "Pentatonic", "Minor Pentatonic" };
}

juce::StringArray getRootNoteTypes()
{
	return { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };
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

juce::StringArray getDelayRateTypes()
{
	return { "4", "2", "1", "1/2", "1/4", "1/8" };
}

juce::StringArray getDelayNoteMovementTypes()
{
	return { "Same note", "Up", "Down", "Up and Down", "Down and Up", "Random" };
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
		std::string xVelocityInverterId = "xVelocityInverter" + std::to_string(i);
		std::string yVelocityInverterId = "yVelocityInverter" + std::to_string(i);
		std::string BallActivationId = "BallActivation" + std::to_string(i);
		std::string delayAmountId = "delayAmount" + std::to_string(i);
		std::string delayFeedbackId = "delayFeedback" + std::to_string(i);
		std::string delayRateId = "delayRate" + std::to_string(i);
		std::string delayNoteMovementId = "delayNoteMovement" + std::to_string(i);

		params.add(std::make_unique<juce::AudioParameterFloat>(ballXId, "Ball X", BALL_X_SLIDER_MIN, BALL_X_SLIDER_MAX, BALL_X_DEFAULT_1));
		params.add(std::make_unique<juce::AudioParameterFloat>(ballYId, "Ball Y", BALL_Y_SLIDER_MIN, BALL_Y_SLIDER_MAX, BALL_Y_DEFAULT));
		params.add(std::make_unique<juce::AudioParameterFloat>(ballRadiusId, "Radius", BALL_RADIUS_MIN, BALL_RADIUS_MAX, BALL_RADIUS_DEFAULT));
		params.add(std::make_unique<juce::AudioParameterFloat>(ballVelocityId, "Velocity", BALL_VELOCITY_MIN, BALL_VELOCITY_MAX, BALL_VELOCITY_DEFAULT));
		params.add(std::make_unique<juce::AudioParameterFloat>(ballAngleId, "Angle", BALL_ANGLE_MIN, BALL_ANGLE_MAX, BALL_ANGLE_DEFAULT_1));
		params.add(std::make_unique<juce::AudioParameterInt>(ballXVelocityId, "XVelocity", BALL_X_VELOCITY_MIN, BALL_X_VELOCITY_MAX, BALL_X_VELOCITY_DEFAULT));
		params.add(std::make_unique<juce::AudioParameterInt>(ballYVelocityId, "YVelocity", BALL_Y_VELOCITY_MIN, BALL_Y_VELOCITY_MAX, BALL_Y_VELOCITY_DEFAULT));
		params.add(std::make_unique<juce::AudioParameterBool>(xVelocityInverterId, "x velocity inverter", false));
		params.add(std::make_unique<juce::AudioParameterBool>(yVelocityInverterId, "x velocity inverter", false));
		params.add(std::make_unique<juce::AudioParameterBool>(BallActivationId, "BallActivation", ((i == 0) ? true : false)));
		params.add(std::make_unique<juce::AudioParameterInt>(delayAmountId, "Delay Amount", DELAY_AMOUNT_MIN, DELAY_AMOUNT_MAX, DELAY_AMOUNT_DEFAULT));
		params.add(std::make_unique<juce::AudioParameterFloat>(delayFeedbackId, "Delay Feedback", DELAY_FEEDBACK_MIN, DELAY_FEEDBACK_MAX, DELAY_FEEDBACK_DEFAULT));
		params.add(std::make_unique<juce::AudioParameterChoice>(delayRateId, "Delay Rate", getDelayRateTypes(), DELAY_RATE_DEFAULT));
		params.add(std::make_unique<juce::AudioParameterChoice>(delayNoteMovementId, "Delay Note Movement", getDelayNoteMovementTypes(), DELAY_NOTE_MOVEMENT_DEFAULT));
	}

	std::string edgePhaseId = "edgePhase";
	params.add(std::make_unique<juce::AudioParameterFloat>(edgePhaseId, "Edge Phase", EDGE_PHASE_MIN, EDGE_PHASE_MAX, EDGE_PHASE_DEFAULT));

	std::string edgeDenomenatorId = "edgeDenomenator";
	params.add(std::make_unique<juce::AudioParameterInt>(edgeDenomenatorId, "Edge Denomenator", EDGE_DENOMINATOR_MIN, EDGE_DENOMINATOR_MAX, EDGE_DENOMINATOR_DEFAULT));

	std::string edgeRangeId = "edgeRange";
	params.add(std::make_unique<juce::AudioParameterInt>(edgeRangeId, "Edge Range", EDGE_RANGE_MIN, EDGE_RANGE_MAX, EDGE_RANGE_DEFAULT));

	std::string scaleKindId = "scaleChoice";
	params.add(std::make_unique<juce::AudioParameterChoice>(scaleKindId, "Scale", getScaleOptions(), SCALE_DEFAULT));
	
	std::string rootNoteId = "rootNote";
	params.add(std::make_unique<juce::AudioParameterChoice>(rootNoteId, "Root Note", getRootNoteTypes(), ROOT_NOTE_DEFAULT));

	std::string edgeTypeID = "edgeType";
	params.add(std::make_unique<juce::AudioParameterChoice>(edgeTypeID, "Edge Type", getEdgeTypes(), EDGE_TYPE_DEFAULT));

	std::string ballsPositioningTypeId = "ballsPositioningType";
	params.add(std::make_unique<juce::AudioParameterChoice>(ballsPositioningTypeId, "BallsPositioning", getBallsPositioningTypes(), BALLS_POSITIONING_DEFAULT));

	std::string snapToGridId = "snapToGrid";
	params.add(std::make_unique<juce::AudioParameterBool>(snapToGridId, "Snap To Grid", false));

	std::string collisionId = "collision";
	params.add(std::make_unique<juce::AudioParameterBool>(collisionId, "Collision", false));

	std::string quantizationId = "quantization";
	params.add(std::make_unique<juce::AudioParameterFloat>(quantizationId, "Quantization", QUANTIZATION_MIN, QUANTIZATION_MAX, QUANTIZATION_DEFAULT));

	std::string quantizationDivisionID = "quantizationDivision";
	params.add(std::make_unique<juce::AudioParameterChoice>(quantizationDivisionID, "Quantization Division", getQuantizationDivisionTypes(), QUANTIZATION_DIV_DEFAULT));

	std::string volumeVariationId = "volumeVariation";
	params.add(std::make_unique<juce::AudioParameterFloat>(volumeVariationId, "volumeVariation", VOLUME_VARIATION_MIN, VOLUME_VARIATION_MAX, VOLUME_VARIATION_DEFAULT));
	
	std::string sizePercentageID = "sizePercentage";
	params.add(std::make_unique<juce::AudioParameterChoice>(sizePercentageID, "size Percentage", getsizePercentageTypes(), SIZE_PERCENTAGE_DEFAULT));
	
	std::string probabilityID = "probability";
	params.add(std::make_unique<juce::AudioParameterFloat>(probabilityID, "Probability", PROBABILITY_MIN, PROBABILITY_MAX, PROBABILITY_DEFAULT));

	std::string transposeID = "transpose";
	params.add(std::make_unique<juce::AudioParameterInt>(transposeID, "Transpose", -24, 24, 0));
	
	std::string toggleStateID = "toggleState";
 	params.add(std::make_unique<juce::AudioParameterBool>(toggleStateID,  "Toggle State", false));
	
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
	int currentEdgeType = pit.getPitEdgeType();
	if (currentEdgeType == 4) //random
	{
		if ((parameterID == "edgePhase") || (parameterID == "edgeDenomenator") || 
		(parameterID == "edgeRange") || (parameterID == "scaleChoice") || 
		(parameterID == "rootNote")  || (parameterID == "edgeType") || (parameterID == "transpose"))
		{
			wasGUIUpdated = true;
		}
		else 
		{
			wasGUIUpdated = false;
		}
	}
	else
	{
		wasGUIUpdated = true;
	}
	
	if (parameterID == "toggleState")
	{
		this->pit.toggleBallMovement();
	}
	else if (parameterID == "BallActivation0")
	{
		this->pit.getBalls()[0]->setActive(newValue > 0.5f);
	}
	  else if (parameterID == "BallActivation1")
  	{
  	 this->pit.getBalls()[1]->setActive(newValue > 0.5f);
  	}
  	else if (parameterID == "BallActivation2")
  	{
  	 this->pit.getBalls()[2]->setActive(newValue > 0.5f);
  	}
}

//--------------------------------------------------------------------------------------------
//--------------------------------------------------------------------------------------------
//getters and setters

bool BallPitAudioProcessor::getToggleState()
{
 	return toggleState.get();
}
void BallPitAudioProcessor::setToggleState(bool newState)
{
 	toggleState.set(newState);
}
bool BallPitAudioProcessor::getIsPlaying()
{
 	return isPlaying.get();
}
void BallPitAudioProcessor::setIsPlaying(bool newState)
{
 	isPlaying.set(newState);
}
double BallPitAudioProcessor::getSampleRate() const
{
 	return m_sampleRate.get();
}
void BallPitAudioProcessor::setSampleRate(double newSampleRate)
{
 	m_sampleRate.set(newSampleRate);
}
int BallPitAudioProcessor::getSamplesPerBlock() const
{
 	return m_samplesPerBlock.get();
}
void BallPitAudioProcessor::setSamplesPerBlock(int newSamplesPerBlock)
{
 	m_samplesPerBlock.set(newSamplesPerBlock);
}
float BallPitAudioProcessor::getProbability() const
{
 	return m_probability.get();
}
void BallPitAudioProcessor::setProbability(float newProbability)
{
 	m_probability.set(newProbability);
}
double BallPitAudioProcessor::getBPM() const
{
	return m_bpm.get();
}
void BallPitAudioProcessor::setBPM(double newBPM)
{
 	m_bpm.set(newBPM);
}
juce::AudioPlayHead::TimeSignature BallPitAudioProcessor::getTimeSignature() const
{
 	return m_timeSignature.get();
}
void BallPitAudioProcessor::setTimeSignature(juce::AudioPlayHead::TimeSignature newTimeSignature)
{
 	m_timeSignature = newTimeSignature;
}
float BallPitAudioProcessor::getQuantizationPercent() const
{
	return quantizationpercent.get();
}
void BallPitAudioProcessor::setQuantizationPercent(float newQuantizationPercent)
{
 	quantizationpercent.set(newQuantizationPercent);
}
float BallPitAudioProcessor::getQuantizationDivision() const
{
	return quantizationDivision.get();
}
void BallPitAudioProcessor::setQuantizationDivision(float newQuantizationDivision)
{
 	quantizationDivision.set(newQuantizationDivision);
}
int BallPitAudioProcessor::getSamplesPerStep() const
{
	return samplesPerStep.get();
}
void BallPitAudioProcessor::setSamplesPerStep(int newSamplesPerStep)
{
	samplesPerStep.set(newSamplesPerStep);
}
int BallPitAudioProcessor::getSampleCounter() const
{
	return sampleCounter.get();
}
void BallPitAudioProcessor::setSampleCounter(int newSampleCounter)
{
 	sampleCounter.set(newSampleCounter);
}

bool BallPitAudioProcessor::getWasGUIUpdated() const
{
	return this->wasGUIUpdated.get();
}
void BallPitAudioProcessor::setWasGUIUpdated(bool newStatus)
{
 	this->wasGUIUpdated.set(newStatus);
}
// double BallPitAudioProcessor::getClockTimeSeconds() const
// {
//  	return clockTimeSeconds.get();
// }
// void BallPitAudioProcessor::setClockTimeSeconds(double newClockTimeSeconds)
// {
//  	this->clockTimeSeconds.set(newClockTimeSeconds);
// }
void BallPitAudioProcessor::setWasGUIUploaded(bool newStatus)
{
	wasGUIUploaded.set(newStatus);
}
bool BallPitAudioProcessor::getWasGUIUploaded()
{
	return wasGUIUploaded.get();
}
void BallPitAudioProcessor::setAreBallsMoving(bool newState)
{
	areBallsMoving.set(newState);
}
bool BallPitAudioProcessor::getAreBallsMoving()
{
	return areBallsMoving.get();
}
//-----------------------------------------------------------------

const std::vector<BallGUIEssentials> BallPitAudioProcessor::getBallsSnapshot() const
{
    const juce::ScopedLock lock(ballsSnapshotPointerLock);
    return ballsSnapshot;
}

void BallPitAudioProcessor::updateBallsSnapshot()
{
    const juce::ScopedLock lock(ballsSnapshotPointerLock);
	ballsSnapshot.clear();
	ballsSnapshot[0] = pit.getBalls()[0]->getBallGUINesseceities();
	ballsSnapshot[1] = pit.getBalls()[1]->getBallGUINesseceities();
	ballsSnapshot[2] = pit.getBalls()[2]->getBallGUINesseceities();
}

const std::vector<int> BallPitAudioProcessor::getAbstractedEdgeColors() const
{
	const juce::ScopedLock lock(edgeColorsPointerLock);

	return abstractedEdgeColors;
}

void BallPitAudioProcessor::updateAbstractedEdgeColors()
{
    const juce::ScopedLock lock(edgeColorsPointerLock);
	auto currentAbstractedEdgeColors = this->pit.getAbstractedEdgeColors();

	for (int i = 0; i < 1568; i++)
	{
		abstractedEdgeColors[i] = currentAbstractedEdgeColors[i];
	}
}