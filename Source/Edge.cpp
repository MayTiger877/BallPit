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
	denomenator = 8;
	range = 2;
	edgeType = 0; //Cycle up

	// Initialize the pit edges musical scale
	rootNote = 60; // middle C4
	scaleNotes[0] = 60;
	scaleNotes[1] = 62;
	scaleNotes[2] = 64;
	scaleNotes[3] = 65;
	scaleNotes[4] = 67;
	scaleNotes[5] = 69;
	scaleNotes[6] = 71;
	scaleNotes[7] = 75;

	updateAbstractedEdge();
}

void Edge::setScale(Scale::ScaleKinds scaleKind, int rootNote, uint8_t mode)
{
	this->scale.SetScale(scaleKind);
	this->scale.SetMode(mode);
	this->rootNote = rootNote;
}

void Edge::setEdgeType(int newEdgeType)
{
	this->edgeType = newEdgeType;
	if (edgeType == 2)
	{
		randomEdgeTypeSelected = true;
	}
	else
	{
		randomEdgeTypeSelected = false;
	}
}

void Edge::getMIDI()
{
	scale.getMIDINotesFromScale(this->scaleNotes, this->rootNote);
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

int Edge::promoteColorIndexByEdgeType(int currentColorIndex, int numOfColors, bool reverse)
{
	int reverseFactor = (reverse == true) ? -1 : 1;
	switch (edgeType)
	{
		case 0: // Cycle up
		{
			return (currentColorIndex + (1 * reverseFactor) + numOfColors) % numOfColors; 
		}
		case 1: // Cycle down
		{
			return (currentColorIndex - (1 * reverseFactor) + numOfColors) % numOfColors;
		}
		case 2 : // Random
		{
			return rand() % numOfColors;
		}
		case 3: // Ping-Pong effect
		{
			float middle = numOfColors / 2.0f;
			if (currentColorIndex < middle)
			{
				return ((numOfColors - currentColorIndex) - (1 * reverseFactor) + numOfColors) % numOfColors;
			}
			else if (currentColorIndex > middle)
			{
				return ((-1 - currentColorIndex) + (1 * reverseFactor) + numOfColors) % numOfColors;
			}
			else
			{
				return (reverseFactor == false ? 0 : (numOfColors - 1));
			}
		}
		default:
			break;
	}
}

void Edge::updateAbstractedEdge()
{
	int numOfSplits = this->denomenator;
	int numOfColors = this->range;
	int split = 1568 / numOfSplits;
	int remainder = 1568 % numOfSplits;
	int index = 0;
	int colorIndex = (edgeType == 2) ? (rand() % numOfColors) : 0; // if random, so also first edge part is random, daa....

	for (int i = 0; i < numOfSplits; i++)
	{
		for (int j = 0; j < split; j++)
		{
			index = (j + (i * split) + this->phase) % 1568;
			abstractedEdge[index] = this->scaleNotes[colorIndex];
			abstractedEdgeColors[index] = colorIndex;
		}
		if (index < 1568)
		{
			colorIndex = promoteColorIndexByEdgeType(colorIndex, numOfColors);
		}
	}
	
	for (int i = 0; i < remainder; i++)
	{
		abstractedEdge[1567 - i] = this->scaleNotes[colorIndex];
		abstractedEdgeColors[1567 - i] = colorIndex;
	}
}