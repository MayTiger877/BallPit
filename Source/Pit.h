#pragma once
#include <JuceHeader.h>
#include <vector>
#include <memory>
#include "Configs.h"
#include "Ball.h"
#include "Edge.h"

class Pit
{

public:
	Pit();

	void addBall(std::unique_ptr<Ball> ball);
	void setSampleRate(double updatedSampleRate);
	void update(double timePassed);
	
	const std::vector<std::unique_ptr<Ball>>& getBalls() const;
	void setBallParams(int index, float x, float y, float radius, float velocity, float angle, int ballSpeedType);
	void setBallsEdgeNotes();
	
	void toggleBallMovement() 
	{
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
	void setScaleTypeToRandom(int scaleChoice, int rootNote);
	void setEdgeScale(Scale::ScaleKinds scaleKind, int rootNote, uint8_t mode = 0);
	void drawPitEdge(juce::Graphics& g, juce::Colour* edgeColors);

private:
	
	Edge edge;
	
	// balls
	std::vector<std::unique_ptr<Ball>> balls;

	bool isMoving;
	bool isCollisionOn;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Pit)
};
