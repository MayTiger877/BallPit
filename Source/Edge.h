/*
  ==============================================================================

    Edge.h
    Created: 18 Oct 2024 3:31:30pm
    Author:  dogom

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Scales.h"



class Edge
{
public:
	Edge();

	void setPhase(int phase) { this->phase = phase; }
	int getPhase() const { return phase; }

	void setNumerator(int numerator) { this->numerator = numerator; }
	int getNumerator() const { return numerator; }
	
	void setDenomenator(int denomenator) { this->denomenator = denomenator; }
	int getDenomenator() const { return denomenator; }

	void setRootNote(int rootNote) { this->rootNote = rootNote; }
	int getRootNote() const { return rootNote; }
	
	void setRange(int range) { this->range = range; }
	int getRange() const { return range; }

	void setScaleNote(int index, int scaleNote) { this->scaleNotes[index] = scaleNote; }
	int getScaleNotes(int index) const { return scaleNotes[index]; }
	
	void setScale(Scale::ScaleKinds scaleKind, int rootNote, uint8_t mode);
	Scale getScale() const { return scale; }
	
	void getMIDI();

	int hitPositionToScalenote(float x, float y);

private:
	
	// pit edges diviation
	int phase;
	int numerator;
	int denomenator;

	// pit edges musical scale
	int rootNote;
	int scaleNotes[8];
	int range;

	Scale scale;

	int abstractedEdge[1568];
	void updateAbstractedEdge();
};