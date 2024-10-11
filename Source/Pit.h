#pragma once
#include <JuceHeader.h>
#include <vector>
#include <memory>

#include "Ball.h"

class Pit
{

public:
	Pit();

	void addBall(std::unique_ptr<Ball> ball);
	void setSampleRate(double updatedSampleRate);
	void update();
	
	const std::vector<std::unique_ptr<Ball>>& getBalls() const;

private:
	// pit coordinates
	const int minX = 12;
	const int minY = 15;
	const int maxX = 402;
	const int maxY = 405;
	
	// pit edges diviation
	int phase;
	int numerator;
	int denomenator;

	// pit edges musical scale
	int fundamental;
	int scale[8];
	int range;
	
	// balls
	std::vector<std::unique_ptr<Ball>> balls;
};
