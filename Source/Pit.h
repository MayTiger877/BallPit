#pragma once
#include <JuceHeader.h>
#include <vector>
#include <memory>

#include "Ball.h"
#include "Edge.h"

class Pit
{

public:
	Pit();

	void addBall(std::unique_ptr<Ball> ball);
	void setSampleRate(double updatedSampleRate);
	void update();
	
	const std::vector<std::unique_ptr<Ball>>& getBalls() const;
	void setBallParams(int index, float x, float y, float radius, float velocity, float angle);
	void setBallsEdgeNotes();
	void toggleBallMovement() { isMoving = !isMoving; }
	bool isBallsMoving() const { return isMoving; }
	
	void setEdgeParams(int phase, int denomenator, int range);
	void drawPitEdge(juce::Graphics& g, juce::Colour* edgeColors) const;

private:
	// pit coordinates
	// size is 390x390
	const int minX = 12;
	const int minY = 15;
	const int maxX = 402;
	const int maxY = 405;
	
	Edge edge;
	
	// balls
	std::vector<std::unique_ptr<Ball>> balls;

	bool isMoving;
};
