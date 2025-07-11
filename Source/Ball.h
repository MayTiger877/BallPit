#pragma once
#include <JuceHeader.h>
#include "EdgeEventListener.h"
#include "Configs.h"

typedef enum {
    UP,
    DOWN,
    LEFT,
    RIGHT
}HitPosition;

typedef struct DelaySettings
{
	int delayAmount = 0;
	float delayFeedback = 0.0f;
	float delayRate = 0.0f;
	int delayNoteMovement = 0;
};


class Ball : public juce::Component
{
public:
    Ball(int ballndex, float x, float y, float radius, float velocity, float angle);
    
    BallGUIEssentials getBallGUINesseceities();

    void setBallEdgeEventListener(BallEdgeEventListener* l);
	void setBallCollideEventListener(BallCollideEventListener* l);

    void setSampleRate(double newSampleRate) { this->sampleRate = newSampleRate; }

    void update(double timePassed, double clockTimeSeconds);
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
    void setActive(bool active);

	DelaySettings getDelaySettings() const { return delaySettings; }
    void setDelaySettings(const DelaySettings& newDelaySettings);

    void setBallSpeedType(int newSpeedType) { this->ballSpeedType = newSpeedType; }

    void updateBallAbstractedEdge(const int* abstractedEdge);
    void updateScaleNotes(int* scaleNotes);

	int getBallIndex() const { return ballIndex; }

	bool isBallMoving() { return isMoving; }
	void setBallMoving(bool isMoving) { this->isMoving = isMoving; }

	void setBallTranspose(int newTranspose) { this->transpose = newTranspose; }

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
	int transpose = 0;

	DelaySettings delaySettings;
	juce::Point<float> delayPoints[3];
	std::queue<juce::Point<float>> ballPathPoints[3];
	void insertDelayPoints(double clockTimeSeconds);

    juce::Path ballArrow;

    double sampleRate;
    bool isMoving = false;
    
	int abstractedEdgeDuplicate[1568] = { 60 };
    int scaleNotes[8];

    BallEdgeEventListener* edgeListener;
	BallCollideEventListener* collideListener;

    int getEdgeHitIndex(HitPosition currentPosition);

	JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Ball)
};
