#include "Ball.h"
#include <iostream>

Ball::Ball(float x, float y, float radius, float speedX, float speedY)
	: x(x), y(y), radius(radius), speedX(speedX), speedY(speedY) 
{}

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

void Ball::setDiameter(float radius) 
{
	this->radius = radius;
}

float Ball::getDiameter() const 
{
	return radius;
}

void Ball::bounce() 
{
	if (x - radius <= minX || x + radius >= maxX) 
	{
		speedX = -speedX;
	}

	if (y - radius <= minY || y + radius >= maxY) 
	{
		speedY = -speedY;
	}
}
