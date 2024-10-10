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
    virtual void onEdgeHit(float x, float y) = 0;
};


class BallEdgeEventListener : public EdgeEventListener 
{
public:
    BallEdgeEventListener(juce::MidiBuffer& midiBuffer) : midiBuffer(midiBuffer) {}

    void onEdgeHit(float x, float y) override 
    {
		int midiNote = 60; // C4
        int velocity = 100; // Velocity for the note
        juce::MidiMessage noteOn = juce::MidiMessage::noteOn(1, midiNote, (juce::uint8)velocity);
        juce::MidiMessage noteOff = juce::MidiMessage::noteOff(1, midiNote);

        midiBuffer.addEvent(noteOn, 0);
        midiBuffer.addEvent(noteOff, 400);
    }

private:
    juce::MidiBuffer& midiBuffer;
};

class BallCollideEventListener : public EdgeEventListener
{
public:
	BallCollideEventListener(juce::MidiBuffer& midiBuffer, double sampleRate) : 
        midiBuffer(midiBuffer), sampleRate(sampleRate) {}

    void onEdgeHit(float x, float y) override
    {   
        int midiNote1 = 60;  // C4
        int midiNote2 = 64;  // E4
        int midiNote3 = 67;  // G4
        int velocity = 100;  // Velocity for the notes

        // Convert milliseconds to samples
        int noteDurationSamples = static_cast<int>(0.600 * sampleRate); // 600ms
        int noteOffTime = noteDurationSamples;

        // Create MIDI messages
        juce::MidiMessage noteOn1 = juce::MidiMessage::noteOn(1, midiNote1, (juce::uint8)velocity);
        juce::MidiMessage noteOff1 = juce::MidiMessage::noteOff(1, midiNote1);
        juce::MidiMessage noteOn2 = juce::MidiMessage::noteOn(1, midiNote2, (juce::uint8)velocity);
        juce::MidiMessage noteOff2 = juce::MidiMessage::noteOff(1, midiNote2);
        juce::MidiMessage noteOn3 = juce::MidiMessage::noteOn(1, midiNote3, (juce::uint8)velocity);
        juce::MidiMessage noteOff3 = juce::MidiMessage::noteOff(1, midiNote3);

        // Add MIDI events to the buffer
        midiBuffer.addEvent(noteOn1, 0);
        midiBuffer.addEvent(noteOff1, noteOffTime);
        //midiBuffer.addEvent(noteOn2, 0);
        //midiBuffer.addEvent(noteOff2, noteOffTime);
        //midiBuffer.addEvent(noteOn3, 0);
        //midiBuffer.addEvent(noteOff3, noteOffTime);

    }

private:
    juce::MidiBuffer& midiBuffer;
    double sampleRate;
};
