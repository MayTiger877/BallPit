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


static int getNoteFromBallIndex(int noteIndex, int ballIndex, int *scaleNotes, int noteMovement = 0)
{
	switch (noteMovement)
	{
		case 1: // no movement
			return scaleNotes[noteIndex];
		case 2: // up
			return scaleNotes[(noteIndex + ballIndex) % 8];
		case 3: // down
			return scaleNotes[(noteIndex - ballIndex) % 8];
		case 4: // up and down
		{
			int adjustedIndex = (ballIndex % 2 == 0) ? (ballIndex / 2) : (((((-1) * ballIndex) % 8) / 2 + 1));
			return scaleNotes[(noteIndex + adjustedIndex) % 8];
		}
		case 5: // down and up
		{
			int adjustedIndex = (ballIndex % 2 == 0) ? (((((-1) * ballIndex) % 8) / 2 + 1)) : (ballIndex / 2);
			return scaleNotes[(noteIndex + adjustedIndex) % 8];
		}
		case 6: // Random
			return scaleNotes[noteIndex] + (rand() % 12) - 6; // Randomly between -6 and +5 semitones
		default: // no movement
			return scaleNotes[noteIndex];
	}

	return scaleNotes[noteIndex]; // Default case, no movement
}

class EdgeEventListener 
{
public:
	virtual void onEdgeHit(int noteIndex, int *scaleNotes, int noteVelocity, double sampleRate, int delayInstances = 0, double delayRate = 0.0, float delayFeedback = 0.0f, int delayNoteMovement = 0) = 0;
	virtual void onBallsColide(int *scaleNotes, double sampleRate) = 0;
};

class BallEdgeEventListener : public EdgeEventListener 
{
public:
	BallEdgeEventListener(juce::MidiBuffer& midiBuffer) : midiBuffer(midiBuffer) {}

	void onEdgeHit(int noteIndex, int scaleNotes[8], int noteVelocity, double sampleRate, int delayInstances, double delayRate, float delayFeedback, int delayNoteMovement) override
	{
		juce::MidiMessage noteOn = juce::MidiMessage::noteOn(1, scaleNotes[noteIndex], (juce::uint8)noteVelocity);
		midiBuffer.addEvent(noteOn, 0);

		if (delayInstances > 0 && delayRate > 0.0)
		{
			for (int i = 1; i <= delayInstances; ++i)
			{
				double delayTime = i * delayRate * sampleRate;
				float delaySizeRatio = pow(0.8f, i);
				noteOn = juce::MidiMessage::noteOn(1, getNoteFromBallIndex(noteIndex, i, scaleNotes, delayNoteMovement),
												  (juce::uint8)(delayFeedback * noteVelocity * delaySizeRatio));
				midiBuffer.addEvent(noteOn, delayTime);
			}
		}
	}

	void onBallsColide(int scaleNotes[8], double sampleRate) override
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

	void onEdgeHit(int noteIndex, int scaleNotes[8], int noteVelocity, double sampleRate, int delayInstances, double delayRate, float delayFeedback, int delayNoteMovement) override
	{
		return;
	}

	void onBallsColide(int scaleNotes[8], double sampleRate) override
	{
		int noteVelocity = 100;

		juce::MidiMessage noteOn1 = juce::MidiMessage::noteOn(1, scaleNotes[0], (juce::uint8)noteVelocity);
		juce::MidiMessage noteOn2 = juce::MidiMessage::noteOn(1, scaleNotes[1], (juce::uint8)noteVelocity);
		juce::MidiMessage noteOn3 = juce::MidiMessage::noteOn(1, scaleNotes[2], (juce::uint8)noteVelocity);

		midiBuffer.addEvent(noteOn1, 0);
		midiBuffer.addEvent(noteOn2, 0);
		midiBuffer.addEvent(noteOn3, 0);
	}

private:
	juce::MidiBuffer& midiBuffer;
};
