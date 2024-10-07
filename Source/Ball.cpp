#include "Ball.h"
#include <iostream>

Ball::Ball(float x, float y, float radius, float speedX, float speedY)
	: x(x), y(y), radius(radius), speedX(speedX), speedY(speedY)
{}

void Ball::setEdgeEventListener(EdgeEventListener* l) 
{
	this->listener = l;
}

void Ball::update()
{
	x += speedX;
	y += speedY;
	bounce();
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

void Ball::setAngledSpeed(float speed, float angle)
{
	this->speedX = speed * cos(angle);
	this->speedY = speed * sin(angle);
}

void Ball::setSpeed(float speedX, float speedY)
{
	this->speedX = speedX;
	this->speedY = speedY;
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

void Ball::bounce()
{
	if (x - radius <= minX || x + radius >= maxX)
	{
		speedX = -speedX;
		if (this->listener) this->listener->onEdgeHit(x, y);
	}

	if (y - radius <= minY || y + radius >= maxY)
	{
		speedY = -speedY;
		if (this->listener) this->listener->onEdgeHit(x, y);
	}
}

