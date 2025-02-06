/*
  ==============================================================================

	EdgeEventListener.h
	Created: 7 Oct 2024 1:39:56pm
	Author:  dogom

  ==============================================================================
*/

#pragma once
#include <unordered_map>
#include <juce_audio_basics/juce_audio_basics.h>

class EdgeEventListener 
{
public:
	virtual void onEdgeHit(int note, int noteVelocity, double sampleRate) = 0;
	virtual void onBallsColide(int notes[3], double sampleRate) = 0;
};


class BallEdgeEventListener : public EdgeEventListener 
{
public:
	BallEdgeEventListener(juce::MidiBuffer& midiBuffer) : midiBuffer(midiBuffer) {}

	void onEdgeHit(int note, int noteVelocity, double sampleRate) override
	{
		juce::MidiMessage noteOn = juce::MidiMessage::noteOn(1, note, (juce::uint8)noteVelocity);
		int noteDurationSamples = static_cast<int>(0.200 * sampleRate); // 200ms
		juce::MidiMessage noteOff = juce::MidiMessage::noteOff(1, note);

		midiBuffer.addEvent(noteOn, 0);
		midiBuffer.addEvent(noteOff, noteDurationSamples);
	}

	void onBallsColide(int notes[3], double sampleRate) override
	{
		return;
	}

private:
	juce::MidiBuffer& midiBuffer;
};

// -----------------------------------------------------------------------------------------

class BallCollideEventListener : public EdgeEventListener
{
public:
	BallCollideEventListener(juce::MidiBuffer& midiBuffer) : midiBuffer(midiBuffer) {}

	void onEdgeHit(int note, int noteVelocity, double sampleRate) override
	{
		return;
	}

	void onBallsColide(int notes[3], double sampleRate) override
	{
		int noteVelocity = 100; // noteVelocity for the note
		int noteDurationSamples = static_cast<int>(0.250 * sampleRate); // 250ms

		DBG("The notes on colide are: " << notes[0] << ", " << notes[1] << ", " << notes[2]);

		juce::MidiMessage noteOn1 = juce::MidiMessage::noteOn(1, notes[0], (juce::uint8)noteVelocity);
		juce::MidiMessage noteOff1 = juce::MidiMessage::noteOff(1, notes[0]);

		juce::MidiMessage noteOn2 = juce::MidiMessage::noteOn(1, notes[1], (juce::uint8)noteVelocity);
		juce::MidiMessage noteOff2 = juce::MidiMessage::noteOff(1, notes[1]);

		juce::MidiMessage noteOn3 = juce::MidiMessage::noteOn(1, notes[2], (juce::uint8)noteVelocity);
		juce::MidiMessage noteOff3 = juce::MidiMessage::noteOff(1, notes[2]);

		midiBuffer.addEvent(noteOn1, 0);
		midiBuffer.addEvent(noteOff1, noteDurationSamples);

		midiBuffer.addEvent(noteOn2, 0);
		midiBuffer.addEvent(noteOff2, noteDurationSamples);

		midiBuffer.addEvent(noteOn3, 0);
		midiBuffer.addEvent(noteOff3, noteDurationSamples);
	}

private:
	juce::MidiBuffer& midiBuffer;
};
