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
        int midiNote = 60; // C4 note
        int velocity = 100; // Velocity for the note
        juce::MidiMessage noteOn = juce::MidiMessage::noteOn(1, midiNote, (juce::uint8)velocity);
        juce::MidiMessage noteOff = juce::MidiMessage::noteOff(1, midiNote);

        // Send note-on
        midiBuffer.addEvent(noteOn, 0);

        // Add a delayed note-off event for 100 milliseconds later
        midiBuffer.addEvent(noteOff, 100);
    }

private:
    juce::MidiBuffer& midiBuffer;
};
