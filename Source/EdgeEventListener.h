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
		int midiNote = 70;
		int velocity = 100; // Velocity for the note
		juce::MidiMessage noteOn = juce::MidiMessage::noteOn(1, midiNote, (juce::uint8)velocity);
		juce::MidiMessage noteOff = juce::MidiMessage::noteOff(1, midiNote);
		int noteDurationSamples = static_cast<int>(0.200 * sampleRate); // 200ms

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
		int midiNote1 = 60;  // C4
		int midiNote2 = 64;  // E4
		int midiNote3 = 67;  // G4
		int velocity = 100;  // Velocity for the notes
		int noteDurationSamples = static_cast<int>(0.200 * sampleRate); // 200ms;

		// Create MIDI messages
		juce::MidiMessage noteOn1 = juce::MidiMessage::noteOn(1, midiNote1, (juce::uint8)velocity);
		juce::MidiMessage noteOff1 = juce::MidiMessage::noteOff(1, midiNote1);
		juce::MidiMessage noteOn2 = juce::MidiMessage::noteOn(1, midiNote2, (juce::uint8)velocity);
		juce::MidiMessage noteOff2 = juce::MidiMessage::noteOff(1, midiNote2);
		juce::MidiMessage noteOn3 = juce::MidiMessage::noteOn(1, midiNote3, (juce::uint8)velocity);
		juce::MidiMessage noteOff3 = juce::MidiMessage::noteOff(1, midiNote3);

		//midiBuffer.addEvent(noteOn1, 0);
		midiBuffer.addEvent(noteOn2, 0);
		midiBuffer.addEvent(noteOn3, 0);
		//midiBuffer.addEvent(noteOff1, noteDurationSamples);
		midiBuffer.addEvent(noteOff2, noteDurationSamples + 1);
		midiBuffer.addEvent(noteOff3, noteDurationSamples + 2);
	}

private:
	juce::MidiBuffer& midiBuffer;
};
