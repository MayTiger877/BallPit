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
	int index = 0;
	if (x <= 12)
	{
		index = (y - 15);
	}
	else if (x >= 404)
	{
		index = 1568 - (392 * 2) + (y - 15);
	}
	else if (y <= 15)
	{
		index = 1568 - 392 - (x - 12);
	}
	else if (y >= 407)
	{
		index = 392 + (x - 12);
	}
	
	return abstractedEdge[index];
}

void Edge::updateAbstractedEdge()
{
	int numOfEntriesForEachNote = (1568 / this->denomenator);
	int reminder = (1568 % this->denomenator);
	int colorIndex = 0;

	for (int i = 0; i < this->denomenator; i++)
	{
		for (int j = 0; j < numOfEntriesForEachNote; j++)
		{
			abstractedEdge[(i * this->denomenator) + j] = this->scaleNotes[colorIndex];
		}
		colorIndex++;
	}
	if (reminder != 0)
	{
		for (int i = 0; i < reminder; i++)
		{
			abstractedEdge[1567 - i] = this->scaleNotes[colorIndex]; // Maybe 1568....
		}
	}
}