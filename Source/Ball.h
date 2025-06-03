#pragma once
#include <JuceHeader.h>
#include "EdgeEventListener.h"
#include "Configs.h"

#define NO_SPEED 0.0f
#define MOUSE_NOT_IN_BALL -1.0
#define MOUSE_NOT_IN_TAB -1

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

    void update(double timePassed);
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

    void setBallSpeedType(int newSpeedType) { this->ballSpeedType = newSpeedType; }

    void updateBallAbstractedEdge(const int* abstractedEdge);
    void updateScaleNotes(int* scaleNotes);

	int getBallIndex() const { return ballIndex; }

	bool isBallMoving() { return isMoving; }
	void setBallMoving(bool isMoving) { this->isMoving = isMoving; }

    void draw(juce::Graphics& g) const;

	float isMouseInsideBall(juce::Point<float> mousePosition) const;
    void setIsMouseOverBall(bool newIsMouseOverBall) { this->isMouseOverBall = newIsMouseOverBall; }

    void setsizePercentage(float newsizePercentage) { this->sizePercentage = newsizePercentage; }

private:
    // pit coordinates- stupid simple
    const float minX = PIT_MIN_X + PIT_INNER_DIFF;
    const float minY = PIT_MIN_Y + PIT_INNER_DIFF;
    const float maxX = PIT_MAX_X - PIT_INNER_DIFF;
    const float maxY = PIT_MAX_Y + PIT_INNER_DIFF;
    
    float x, y;
    float radius;
    double velocity, angle;
    double speedX, speedY;
    bool active;
    int ballIndex;
    int ballSpeedType = 0;

    juce::Path ballArrow;

    double sampleRate;
    float sizePercentage = 1.0f;
    bool isMoving = false;
    bool isMouseOverBall = false;
    
    int abstractedEdgeDuplicate[1568];
    int scaleNotes[3];

    BallEdgeEventListener* edgeListener;
	BallCollideEventListener* collideListener;

    int getEdgeHitIndex(HitPossition currentPosition);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Ball)
};
