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
	void checkBallsPositionBeforeStart();
	void setBallsEdgeNotes();
	
	void toggleBallMovement() {
		isMoving = !isMoving;
		for (auto& ball : balls)
		{
			ball->setBallMoving(isMoving);
		}
	}

	bool areBallsMoving() const { return isMoving; }

	void setCollision(bool newcollisionState) { isCollisionOn = newcollisionState; }
	
	void setEdgeParams(int phase, int denomenator, int range, int edgeType);
	void setEdgeTypeToRandom();
	void setEdgeScale(Scale::ScaleKinds scaleKind, int rootNote, uint8_t mode);
	void drawPitEdge(juce::Graphics& g, juce::Colour* edgeColors);

private:
	// pit coordinates
	// size is 390x390
	const float minX = 12;
	const float minY = 15;
	const float maxX = 402;
	const float maxY = 405;
	
	Edge edge;
	
	// balls
	std::vector<std::unique_ptr<Ball>> balls;

	bool isMoving;
	bool isCollisionOn;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Pit)
};
