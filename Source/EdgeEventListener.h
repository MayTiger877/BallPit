/*
  ==============================================================================

	EdgeEventListener.h
	Created: 7 Oct 2024 1:39:56pm
	Author:  dogom

  ==============================================================================
*/

#pragma once

class EdgeEventListener 
{
public:
	virtual void onEdgeHit(float x, float y, double sampleRate) = 0;
};


class BallEdgeEventListener : public EdgeEventListener 
{
public:
	BallEdgeEventListener(juce::MidiBuffer& midiBuffer) : midiBuffer(midiBuffer) {}

	void onEdgeHit(float x, float y, double sampleRate) override
	{
		DBG("EDGE HIT");
		int midiNote = 70;
		int noteVelocity = 100; // noteVelocity for the note
		juce::MidiMessage noteOn = juce::MidiMessage::noteOn(1, midiNote, (juce::uint8)noteVelocity);
		juce::MidiMessage noteOff = juce::MidiMessage::noteOff(1, midiNote);
		int noteDurationSamples = 1000;// static_cast<int>(0.300 * sampleRate); // 200ms

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

	void onEdgeHit(float x, float y, double sampleRate) override
	{
		DBG("BALL COLLIDE");
		//int noteDurationSamples = static_cast<int>(0.200 * sampleRate); // 200ms;

		// Define notes for a C major chord (C4, E4, G4)
		int midiNote1 = 60;  // C4
		int midiNote2 = 64;  // E4
		int midiNote3 = 67;  // G4
		int noteVelocity = 100;  // noteVelocity for the notes
		int samplePosition = 0;  // Starting at sample position 0
		int duration = 1000;//static_cast<int>(0.500 * sampleRate); // 200ms

		midiBuffer.addEvent(juce::MidiMessage::noteOn(1, midiNote1, (juce::uint8)noteVelocity), samplePosition);
		midiBuffer.addEvent(juce::MidiMessage::noteOff(1, midiNote1), samplePosition + duration);

		midiBuffer.addEvent(juce::MidiMessage::noteOn(1, midiNote2, (juce::uint8)noteVelocity), samplePosition + 1);
		midiBuffer.addEvent(juce::MidiMessage::noteOff(1, midiNote2), samplePosition + 1 + duration);

		midiBuffer.addEvent(juce::MidiMessage::noteOn(1, midiNote3, (juce::uint8)noteVelocity), samplePosition + 2);
		midiBuffer.addEvent(juce::MidiMessage::noteOff(1, midiNote3), samplePosition + 2 + duration);

	}

private:
	juce::MidiBuffer& midiBuffer;
};
