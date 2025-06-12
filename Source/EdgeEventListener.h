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
	virtual void onEdgeHit(int note, int noteVelocity, double sampleRate, int delayInstances = 0, double delayRate = 0.0, float delayFeedback = 0.0f, int delayNoteMovement = 0) = 0;
	virtual void onBallsColide(int notes[3], double sampleRate) = 0;
};


static int getNoteFromBallIndex(int ballIndex, int baseNote = 60, int noteMovement = 0)
{
	switch (noteMovement)
	{
		case 1: // no movement
			return baseNote;
		case 2: // up
			return baseNote + ballIndex;
		case 3: // down
			return baseNote - ballIndex;
		case 4: // up and down
			return (ballIndex % 2 == 0) ? (baseNote + (ballIndex / 2)) : (baseNote - (ballIndex / 2 + 1));
		case 5: // down and up
			return (ballIndex % 2 == 0) ? (baseNote - (ballIndex / 2)) : (baseNote + (ballIndex / 2 + 1));
		case 6: // Random
			return baseNote + (rand() % 12) - 6; // Randomly between -6 and +5 semitones
		default: // no movement
			return baseNote;
	}

	return baseNote; // Default case, no movement
}

class BallEdgeEventListener : public EdgeEventListener 
{
public:
	BallEdgeEventListener(juce::MidiBuffer& midiBuffer) : midiBuffer(midiBuffer) {}

	void onEdgeHit(int note, int noteVelocity, double sampleRate, int delayInstances, double delayRate, float delayFeedback, int delayNoteMovement) override
	{
		juce::MidiMessage noteOn = juce::MidiMessage::noteOn(1, note, (juce::uint8)noteVelocity);
		midiBuffer.addEvent(noteOn, 0);

		if (delayInstances > 0 && delayRate > 0.0)
		{
			for (int i = 1; i <= delayInstances; ++i)
			{
				double delayTime = i * delayRate * sampleRate;
				float delaySizeRatio = pow(0.8f, i);
				noteOn = juce::MidiMessage::noteOn(1, getNoteFromBallIndex(i, note, delayNoteMovement),
												  (juce::uint8)(delayFeedback * noteVelocity * delaySizeRatio));
				midiBuffer.addEvent(noteOn, delayTime);
			}
		}
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

	void onEdgeHit(int note, int noteVelocity, double sampleRate, int delayInstances, double delayRate, float delayFeedback, int delayNoteMovement) override
	{
		return;
	}

	void onBallsColide(int notes[3], double sampleRate) override
	{
		int noteVelocity = 100; // noteVelocity for the note

		juce::MidiMessage noteOn1 = juce::MidiMessage::noteOn(1, notes[0], (juce::uint8)noteVelocity);
		juce::MidiMessage noteOn2 = juce::MidiMessage::noteOn(1, notes[1], (juce::uint8)noteVelocity);
		juce::MidiMessage noteOn3 = juce::MidiMessage::noteOn(1, notes[2], (juce::uint8)noteVelocity);

		midiBuffer.addEvent(noteOn1, 0);
		midiBuffer.addEvent(noteOn2, 0);
		midiBuffer.addEvent(noteOn3, 0);
	}

private:
	juce::MidiBuffer& midiBuffer;
};
