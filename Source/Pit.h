#pragma once
#include <JuceHeader.h>

class Pit
{

public:
	Pit(int phase, int numerator, int denomenator);

	const int getMinX() { return minX; }
	const int getMinY() { return minY; }
	const int getMaxX() { return maxX; }
	const int getMaxY() { return maxY; }
	void findAndPlayNote(const int hitPointX, const int hitPointY);

private:
	const int minX = 12;
	const int minY = 15;
	const int maxX = 402;
	const int maxY = 405;
	int phase;
	int numerator;
	int denomenator;

	bool checkHitPoint(const int hitPointX, const int hitPointY);
};
