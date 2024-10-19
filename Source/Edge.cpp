/*
  ==============================================================================

    Edge.cpp
    Created: 18 Oct 2024 3:31:30pm
    Author:  dogom

  ==============================================================================
*/

#include "Edge.h"

Edge::Edge() : scale(Scale::ScaleKinds::MAJOR, 0)
{
	// Initialize the pit edges
	phase = 0;
	numerator = 1;
	denomenator = 1;
	range = 2;

	// Initialize the pit edges musical scale
	rootNote = 60; // middle C4
	scaleNotes[0] = 0;
	scaleNotes[1] = 2;
	scaleNotes[2] = 4;
	scaleNotes[3] = 5;
	scaleNotes[4] = 7;
	scaleNotes[5] = 9;
	scaleNotes[6] = 11;
	scaleNotes[7] = 12;
}

void Edge::setScale(Scale::ScaleKinds scaleKind, int rootNote, uint8_t mode)
{
	this->scale.SetScale(scaleKind);
	this->scale.SetMode(mode);
}

void Edge::getMIDI()
{
	scale.getMIDINotesFromScale(this->scaleNotes, this->scaleNotes[0]);
}

int Edge::hitPositionToScalenote(float x, float y)
{
	int index = (y-15) + (392 * (x-12));
	return abstractedEdge[index];
}

void Edge::updateAbstractedEdge()
{
	for (int i = 0; i < this->denomenator; i++) // number of different colors
	{
		for (int j = 0; j < this->range; j++) // number of different notes
		{
			int index = (i * 392) + j;
			abstractedEdge[index] = this->rootNote + this->scaleNotes[j];
		}
	}
}