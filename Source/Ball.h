#pragma once
#include <JuceHeader.h>
#include "PluginProcessor.h"

class Ball
{
public:
    Ball(float x, float y, float radius, float speedX, float speedY, BallPitAudioProcessor& processor);

    void update();
    void draw(juce::Graphics& g) const;

    void setDiameter(float radius);
    float getDiameter() const;

private:
    const int minX = 12;
    const int minY = 15;
    const int maxX = 402;
    const int maxY = 405;

    float x, y;           // Position of the ball
    float radius;       // radius of the ball
    float speedX, speedY; // Speed in X and Y directions

    BallPitAudioProcessor& audioProcessor;

    void bounce();
};
