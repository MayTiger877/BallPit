#include "Ball.h"
#include <iostream>

#define PI 3.14159265
#define NO_SPEED 0

Ball::Ball(int ballndex, float x, float y, float radius, float velocity, float angle)
	: ballndex(ballndex), x(x), y(y), radius(radius), velocity(0.1 * velocity), angle(angle)
{
	this->isMoving = false;
	setAngledSpeed();
}

void Ball::setBallEdgeEventListener(BallEdgeEventListener* l)
{
	this->edgeListener = l;
}

void Ball::setBallCollideEventListener(BallCollideEventListener* l)
{
	this->collideListener = l;
}

void Ball::update()
{
	if (this->active == true)
	{
		x += speedX;
		y += speedY;
	}
	else
	{
		speedX = speedY = 0;
		x = 440 + ballndex * 50;
		y = 380;
	}

}

void Ball::updateAbstractedEdge(int* abstractedEdge)
{
	for (int i = 0; i < 1568; i++)
	{
		abstractedEdgeDuplicate[i] = abstractedEdge[i];
	}
}

void Ball::updateScaleNotes(int* newScaleNotes)
{
	this->scaleNotes[0] = newScaleNotes[0];
	this->scaleNotes[1] = newScaleNotes[1];
	this->scaleNotes[2] = newScaleNotes[2];
}

void Ball::draw(juce::Graphics& g) const
{
	g.setColour(juce::Colours::honeydew);
	g.fillEllipse(x - radius, y - radius, (radius * 2), (radius * 2));
}

void Ball::setRadius(float radius)
{
	this->radius = radius;
}

float Ball::getRadius() const
{
	return radius;
}

void Ball::setAngledSpeed()
{
	float offset = juce::MathConstants<float>::pi * (this->angle - 90) / 180;
	this->speedX = velocity * cos(offset);
	this->speedY = velocity * sin(offset);
	// actually angle of the knob is (angle - 90)
	if (angle == 90 || angle == 270) // 0 or 180
	{
		this->speedY = NO_SPEED;
	}
	if (angle == 0 || angle == 180) // 270 or 90
	{
		this->speedX = NO_SPEED;
	}
}

void Ball::setAngle(float angle)
{
	this->angle = angle;
	setAngledSpeed();
}

void Ball::setVelocity(float velocity)
{
	this->velocity = velocity;
	setAngledSpeed();
}

void Ball::setPosition(float newX, float newY)
{
	if (newX < minX + radius)
	{
		newX = minX + radius;
	}
	else if (newX > maxX - radius)
	{
		newX = maxX - radius;
	}
	
	if (newY < minY + radius)
	{
		newY = minY + radius;
	}
	else if (newY > maxY - radius)
	{
		newY = maxY - radius;
	}
		
	this->x = newX;
	this->y = newY;
}

void Ball::edgeBounce()
{
	if (this->active == false)
	{
		return;
	}
	
	if (x - radius <= minX || x + radius >= maxX) // TODO - make this if better to cover radius when positioning balls
	{
		if (speedX == 0)
		{
			return;
		}

		if (x - radius <= minX)
		{
			float distPassed = minX - (x - radius);
			x = minX + distPassed;
		}

		if (x + radius >= maxX)
		{
			float distPassed = (x + radius) - maxX;
			x = maxX - distPassed;
		}
		
		speedX = -speedX;
		if (this->edgeListener)
		{
			int edgeIndex = 0;
			if (x - radius <= minX)
			{
				edgeIndex = (y - minY);
			}
			else if (x + radius >= maxX)
			{
				edgeIndex = 1568 - 392 - (y - minY);
			}

			if ((this->isMoving) && (speedX != NO_SPEED))
			{
				int noteVelocity = (int)(60 + (60 * (this->radius / 25)));
				this->edgeListener->onEdgeHit(abstractedEdgeDuplicate[edgeIndex], noteVelocity, sampleRate);
			}
		}
	}

	if (y - radius <= minY || y + radius >= maxY)
	{
		if (speedY == 0)
		{
			return;
		}

		if (y - radius <= minY)
		{
			float distPassed = minY - (y - radius);
			y = minY + distPassed;
		}
		if (y + radius >= maxY)
		{
			float distPassed = (y + radius) - maxY;
			y = maxY - distPassed;
		}
		
		speedY = -speedY;
		if (this->edgeListener)
		{
			int edgeIndex = 0;
			if (y - radius <= minY)
			{
				edgeIndex = 1568 - (x - minX);
			}
			else if (y + radius >= maxY)
			{
				edgeIndex = 392 + (x - minX);
			}

			if ((this->isMoving) && (speedY != NO_SPEED))
			{
				int noteVelocity = (int)(60 + (60 * (this->radius / 25)));
				this->edgeListener->onEdgeHit(abstractedEdgeDuplicate[edgeIndex], noteVelocity, sampleRate);
			}
		}
	}
}

bool Ball::checkCollision(const Ball& other) const 
{
	float dx = x - other.x;
	float dy = y - other.y;
	float distance = std::sqrt(dx * dx + dy * dy);
	return distance <= (radius + other.radius);
}

void Ball::resolveCollision(Ball& other) 
{
	// Calculate distance between balls
	float dx = x - other.x;
	float dy = y - other.y;
	float distance = std::sqrt(dx * dx + dy * dy);

	// Normalize the direction vector
	float nx = dx / distance;
	float ny = dy / distance;

	// Correct overlap by moving balls apart
	float overlap = (radius + other.radius) - distance;
	x += nx * (overlap / 2.0f);
	y += ny * (overlap / 2.0f);
	other.x -= nx * (overlap / 2.0f);
	other.y -= ny * (overlap / 2.0f);

	// Calculate relative velocity in the normal direction
	float dvx = speedX - other.speedX;
	float dvy = speedY - other.speedY;
	float dotProduct = dvx * nx + dvy * ny;

	// If the balls are moving away from each other, no need to resolve the collision
	if ((this->isMoving) && (dotProduct > 0.0f))
	{
		return;
	}

	// Reflect velocities without changing the speed
	float speedA = std::sqrt(speedX * speedX + speedY * speedY);  // Speed of ball A
	float speedB = std::sqrt(other.speedX * other.speedX + other.speedY * other.speedY);  // Speed of ball B

	// Exchange velocities along the normal direction
	speedX -= 2.0f * dotProduct * nx;
	speedY -= 2.0f * dotProduct * ny;
	other.speedX += 2.0f * dotProduct * nx;
	other.speedY += 2.0f * dotProduct * ny;

	// Normalize the new velocities to maintain the original speed
	float newSpeedA = std::sqrt(speedX * speedX + speedY * speedY);
	speedX = (speedX / newSpeedA) * speedA;
	speedY = (speedY / newSpeedA) * speedA;

	float newSpeedB = std::sqrt(other.speedX * other.speedX + other.speedY * other.speedY);
	other.speedX = (other.speedX / newSpeedB) * speedB;
	other.speedY = (other.speedY / newSpeedB) * speedB;

	if (this->isMoving)
	{
		if (this->collideListener) this->collideListener->onBallsColide(scaleNotes, sampleRate);
	}
}