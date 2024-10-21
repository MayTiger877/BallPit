#include "Ball.h"
#include <iostream>

Ball::Ball(int index, float x, float y, float radius, float velocity, float angle)
	: index(index), x(x), y(y), radius(radius), velocity(0.4 * velocity), angle(angle)
{
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
		x = 440 + index * 50;
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
	this->speedX = velocity * cos(angle);
	this->speedY = velocity * sin(angle);
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

void Ball::setPosition(float x, float y)
{
	if (x < minX + radius)
	{
		x = minX + radius;
	}
	else if (x > maxX - radius)
	{
		x = maxX - radius;
	}
	
	if (y < minY + radius)
	{
		y = minY + radius;
	}
	else if (y > maxY - radius)
	{
		y = maxY - radius;
	}
		
	this->x = x;
	this->y = y;
}

void Ball::edgeBounce()
{
	if (this->active == false)
	{
		return;
	}
	
	if (x - radius <= minX || x + radius >= maxX)
	{
		speedX = -speedX;
		if (this->edgeListener)
		{
			// TODO- figure out how to get the not from the edge....
			int index = 0;
			if (x - radius <= 12)
			{
				index = (y - 15);
			}
			else if (x + radius >= 404)
			{
				index = 1568 - (392 * 2) + (y - 15);
			}
			else if (y - radius <= 15)
			{
				index = 1568 - 392 - (x - 12);
			}
			else if (y + radius >= 407)
			{
				index = 392 + (x - 12);
			}

			this->edgeListener->onEdgeHit(abstractedEdgeDuplicate[index], sampleRate);
		}
	}

	if (y - radius <= minY || y + radius >= maxY)
	{
		speedY = -speedY;
		if (this->edgeListener)
		{
			// TODO- figure out how to get the not from the edge....
			int index = 0;
			if (x - radius <= 12)
			{
				index = (y - 15);
			}
			else if (x + radius >= 404)
			{
				index = 1568 - (392 * 2) + (y - 15);
			}
			else if (y - radius <= 15)
			{
				index = 1568 - 392 - (x - 12);
			}
			else if (y + radius >= 407)
			{
				index = 392 + (x - 12);
			}

			this->edgeListener->onEdgeHit(abstractedEdgeDuplicate[index], sampleRate);
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
	if (dotProduct > 0.0f)
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

	if (this->collideListener) this->collideListener->onEdgeHit(abstractedEdgeDuplicate[0], sampleRate);
}