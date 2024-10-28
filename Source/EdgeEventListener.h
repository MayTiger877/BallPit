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
	virtual void onEdgeHit(int note, double sampleRate) = 0;
};


class BallEdgeEventListener : public EdgeEventListener 
{
public:
	BallEdgeEventListener(juce::MidiBuffer& midiBuffer) : midiBuffer(midiBuffer) {}

	void onEdgeHit(int note, double sampleRate) override
	{
		int noteVelocity = 100; // noteVelocity for the note
		juce::MidiMessage noteOn = juce::MidiMessage::noteOn(1, note, (juce::uint8)noteVelocity);
		int noteDurationSamples = static_cast<int>(0.200 * sampleRate); // 200ms
		juce::MidiMessage noteOff = juce::MidiMessage::noteOff(1, note);

		midiBuffer.addEvent(noteOn, 0);
		midiBuffer.addEvent(noteOff, noteDurationSamples);
	}

private:
	juce::MidiBuffer& midiBuffer;
};

// -----------------------------------------------------------------------------------------

class BallCollideEventListener : public EdgeEventListener
{
public:
	BallCollideEventListener(juce::MidiBuffer& midiBuffer) : midiBuffer(midiBuffer) {}

	void onEdgeHit(int note, double sampleRate) override
	{
		int noteVelocity = 100; // noteVelocity for the note
		int noteDurationSamples = static_cast<int>(0.200 * sampleRate); // 200ms
		juce::MidiMessage noteOn1 = juce::MidiMessage::noteOn(1, note, (juce::uint8)noteVelocity);
		juce::MidiMessage noteOff1 = juce::MidiMessage::noteOff(1, note);

		juce::MidiMessage noteOn2 = juce::MidiMessage::noteOn(1, note + 3, (juce::uint8)noteVelocity);
		juce::MidiMessage noteOff2 = juce::MidiMessage::noteOff(1, note + 3);

		juce::MidiMessage noteOn3 = juce::MidiMessage::noteOn(1, note + 5, (juce::uint8)noteVelocity);
		juce::MidiMessage noteOff3 = juce::MidiMessage::noteOff(1, note + 5);

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
