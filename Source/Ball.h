#pragma once
#include <JuceHeader.h>
#include "EdgeEventListener.h"

class Ball
{
public:
    Ball(float x, float y, float radius, float speedX, float speedY);
    
    void setBallEdgeEventListener(BallEdgeEventListener* l);

    void update();

    void setRadius(float radius);
    float getRadius() const;
    
	void setAngledSpeed(float speed, float angle);
	void setSpeed(float speedX, float speedY);

	void setPosition(float x, float y);
	float getX() const { return x; }
	float getY() const { return y; }
    
    void draw(juce::Graphics& g) const;

private:
    // pit coordinates- stupid simple
    const int minX = 12;
    const int minY = 15;
    const int maxX = 402;
    const int maxY = 405;
    
    float x, y;
    float radius;
    float speedX, speedY;

    BallEdgeEventListener* listener;

    void bounce();
};