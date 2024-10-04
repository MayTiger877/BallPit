#include "Ball.h"
#include <iostream>

Ball::Ball(float x, float y, float radius, float speedX, float speedY, BallPitAudioProcessor& Processor)
	: x(x), y(y), radius(radius), speedX(speedX), speedY(speedY), audioProcessor(Processor)
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
		juce::MidiMessage noteOn(juce::MidiMessage::noteOn(1, 60, (juce::uint8)100));
		noteOn.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001);

		juce::MidiMessage noteOff(juce::MidiMessage::noteOff(1, 60));
		noteOff.setTimeStamp(juce::Time::getMillisecondCounterHiRes() * 0.001 + 0.3);

		// Add to the processor's buffer to output
		midiBuffer.addEvent(noteOn, (int)noteOn.getTimeStamp());
		midiBuffer.addEvent(noteOff, (int)noteOff.getTimeStamp());
	}

	if (y - radius <= minY || y + radius >= maxY)
	{
		speedY = -speedY;

	}
}
