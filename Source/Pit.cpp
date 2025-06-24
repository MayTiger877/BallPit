#include "Pit.h"
#include <iostream>

//----------------------------------------------------------------------------

Pit::Pit() : edge()
{	
	this->isMoving = false;
	this->isCollisionOn = true;
}

void Pit::addBall(std::unique_ptr<Ball> ball)
{
	balls.push_back(std::move(ball));
}

void Pit::setSampleRate(double updatedSampleRate)
{
	for (auto& ball : balls)
	{
		ball->setSampleRate(updatedSampleRate);
	}
}

void Pit::update(double timePassed, double clockTimeSeconds)
{
	// edges hit
	for (auto& ball : balls)
	{
		ball->update(timePassed, clockTimeSeconds);
		ball->edgeBounce();
	}

	// balls collisions
	for (size_t i = 0; i < balls.size(); ++i) 
	{
		for (size_t j = i + 1; j < balls.size(); ++j) 
		{
			if ((balls[i]->checkCollision(*balls[j])) && (this->isCollisionOn == true))
			{
				balls[i]->resolveCollision(*balls[j]);
			}
		}
	}
}

const std::vector<std::unique_ptr<Ball>>& Pit::getBalls() const
{
	return balls;
}

void Pit::setBallsTranspose(int newTranspose)
{
	for (int i = 0; i < 3; i++)
	{
		balls[i]->setBallTranspose(newTranspose);
		setBallsEdgeNotes();
	}
}

void Pit::setBallParams(int index, float x, float y, float radius, float velocity, float angle, int ballSpeedType, DelaySettings newDelaySettings)
{
	balls[index]->setBallSpeedType(ballSpeedType);
	balls[index]->setPosition(x, y);
	balls[index]->setRadius(radius);
	balls[index]->setVelocity(velocity);
	balls[index]->setAngle(angle);
	balls[index]->setDelaySettings(newDelaySettings);

	int otherIndex1 = (index + 1) % balls.size();
	int otherIndex2 = (index + 2) % balls.size();
	if (balls[index]->checkCollision(*balls[otherIndex1]))
	{
		balls[index]->resolveCollision(*balls[otherIndex1]);
	}
	if (balls[index]->checkCollision(*balls[otherIndex2]))
	{
		balls[index]->resolveCollision(*balls[otherIndex2]);
	}
	if (balls[otherIndex1]->checkCollision(*balls[otherIndex2]))
	{
		balls[otherIndex1]->resolveCollision(*balls[otherIndex2]);
	}
}

void Pit::setEdgeParams(int phase, int denomenator, int range, int edgeType)
{
	this->edge.setPhase(phase);
	this->edge.setDenomenator(denomenator);
	this->edge.setRange(range);
	this->edge.setEdgeType(edgeType);
	this->edge.updateAbstractedEdge();
}

void Pit::setEdgeTypeToRandom()
{
	this->edge.setEdgeType(4);
}

void Pit::setScaleTypeToRandom(int scaleChoice, int rootNote)
{
	setEdgeScale(static_cast<Scale::ScaleKinds>(scaleChoice), rootNote);
}

void Pit::setBallsEdgeNotes()
{
	for (auto& ball : balls)
	{
		ball->updateBallAbstractedEdge(this->edge.getAbstractedEdge());
		int scaleNotes[8] = { this->edge.getScaleNotes(0), this->edge.getScaleNotes(1), this->edge.getScaleNotes(2), this->edge.getScaleNotes(3),
							  this->edge.getScaleNotes(4), this->edge.getScaleNotes(5), this->edge.getScaleNotes(6), this->edge.getScaleNotes(7) };
		ball->updateScaleNotes(scaleNotes);
	}
}

void Pit::setEdgeScale(Scale::ScaleKinds scaleKind, int rootNote, uint8_t mode)
{
	this->edge.setScale(scaleKind, rootNote, mode);
	this->edge.getMIDI();
}
