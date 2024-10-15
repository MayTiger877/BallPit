#include "Pit.h"
#include <iostream>

//----------------------------------------------------------------------------

Pit::Pit()
{
	// Initialize the pit edges
	this->phase = 0;
	this->numerator = 1;
	this->denomenator = 1;

	// Initialize the pit edges musical scale
	this->fundamental = 60; // middle C4
	this->scale[0] = 0;
	this->scale[1] = 2;
	this->scale[2] = 4;
	this->scale[3] = 5;
	this->scale[4] = 7;
	this->scale[5] = 9;
	this->scale[6] = 11;
	this->scale[7] = 12;
	this->range = 8;
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

void Pit::update()
{
	// edges hit
	for (auto& ball : balls)
	{
		ball->update();
		ball->edgeBounce();
	}

	// balls collisions
	for (size_t i = 0; i < balls.size(); ++i) 
	{
		for (size_t j = i + 1; j < balls.size(); ++j) 
		{
			if (balls[i]->checkCollision(*balls[j])) 
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

void Pit::setBallParams(int index, float x, float y, float radius, float velocity, float angle)
{
	balls[index]->setPosition(x, y);
	balls[index]->setRadius(radius);
	balls[index]->setVelocity(velocity);
	balls[index]->setAngle(angle);
}