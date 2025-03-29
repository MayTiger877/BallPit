#pragma once
#include <JuceHeader.h>
#include "EdgeEventListener.h"

#define PI 3.14159265
#define NO_SPEED 0
#define MOUSE_NOT_IN_BALL -1.0

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
}HitPossition;


class Ball : public juce::Component
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

	int getBallIndex() const { return ballIndex; }

	bool isBallMoving() { return isMoving; }
	void setBallMoving(bool isMoving) { this->isMoving = isMoving; }

    void draw(juce::Graphics& g) const;

	float isMouseInsineBall(juce::Point<float> mousePosition) const;
    void setIsMouseOverBall(bool newIsMouseOverBall) { this->isMouseOverBall = newIsMouseOverBall; }

private:
    // pit coordinates- stupid simple
    const float minX = 12;
    const float minY = 15;
    const float maxX = 402;
    const float maxY = 405;
    
    float x, y;
    float radius;
    float velocity, angle;
    float speedX, speedY;
    bool active;
    int ballIndex;

    juce::Path ballArrow;

    double sampleRate;
    bool isMoving = false;
    bool isMouseOverBall = false;
    
    int abstractedEdgeDuplicate[1568];
    int scaleNotes[3];

    BallEdgeEventListener* edgeListener;
	BallCollideEventListener* collideListener;

    int getEdgeHitIndex(HitPossition currentPosition);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Ball)
};
