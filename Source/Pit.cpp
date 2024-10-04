#include "Pit.h"
#include <iostream>

bool Pit::checkHitPoint(const int hitPointX, const int hitPointY)
{
	if (hitPointX < this->minX ||
		hitPointX > this->maxX ||
		hitPointY < this->minY ||
		hitPointY > this->maxY )
	{
		return false;
	}

	return true;
}

void createMIDI() { return; } // TODO

//----------------------------------------------------------------------------

Pit::Pit(int phase, int numerator, int denomenator)
	: phase(phase), numerator(numerator), denomenator(denomenator)
{}

void Pit::playMIDI(const int hitPointX, const int hitPointY)
{
	if (!checkHitPoint(hitPointX, hitPointY))
	{
		return;
	}
	
	if (hitPointX <= this->minX) { createMIDI(); }
	else if (hitPointX >= this->maxX) { createMIDI(); }

	if (hitPointY <= this->minY) { createMIDI(); }
	else if (hitPointY >= this->maxY) { createMIDI(); }
}