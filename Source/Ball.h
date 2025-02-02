#pragma once
#include <JuceHeader.h>
#include "EdgeEventListener.h"

class Ball
{
public:
    Ball(int ballndex, float x, float y, float radius, float velocity, float angle);

    void setBallEdgeEventListener(BallEdgeEventListener* l);
	void setBallCollideEventListener(BallCollideEventListener* l);

    void setSampleRate(double newSampleRate) { this->sampleRate = newSampleRate; }

    void update();
    void edgeBounce();
    bool checkCollision(const Ball& other) const;
    void resolveCollision(Ball& other);

    void setRadius(float radius);
    float getRadius() const;
    
	void setAngledSpeed();
	void setAngle(float angle);
	void setVelocity(float velocity);

	void setPosition(float x, float y);
	float getX() const { return x; }
	float getY() const { return y; }

	bool isActive() const { return active; }
	void setActive(bool active) { this->active = active; }
    
    void updateAbstractedEdge(int* abstractedEdge);
    void updateScaleNotes(int* scaleNotes);

	bool isBallMoving() { return isMoving; }
	void setBallMoving(bool isMoving) { this->isMoving = isMoving; }

    void draw(juce::Graphics& g) const;

private:
    // pit coordinates- stupid simple
    const int minX = 12;
    const int minY = 15;
    const int maxX = 402;
    const int maxY = 405;
    
    float x, y;
    float radius;
    float velocity, angle;
    float speedX, speedY;
    bool active;
    int ballndex;

    double sampleRate;
    bool isMoving;
    
    int abstractedEdgeDuplicate[1568];
    int scaleNotes[3];

    BallEdgeEventListener* edgeListener;
	BallCollideEventListener* collideListener;

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Ball)
};
