#include "Pit.h"
#include <iostream>

//----------------------------------------------------------------------------

Pit::Pit() : edge()
{	
	this->isMoving = false;
	this->isCollisionOn = true;
}

void Pit::addBall(std::unique_ptr<Ball> ball)
{
	balls.push_back(std::move(ball));
}

void Pit::setSampleRate(double updatedSampleRate)
{
	for (auto& ball : balls)
	{
		ball->setSampleRate(updatedSampleRate);
	}
}

void Pit::update(double timePassed, double clockTimeSeconds)
{
	// edges hit
	for (auto& ball : balls)
	{
		ball->update(timePassed, clockTimeSeconds);
		ball->edgeBounce();
	}

	// balls collisions
	for (size_t i = 0; i < balls.size(); ++i) 
	{
		for (size_t j = i + 1; j < balls.size(); ++j) 
		{
			if ((balls[i]->checkCollision(*balls[j])) && (this->isCollisionOn == true))
			{
				balls[i]->resolveCollision(*balls[j]);
			}
		}
	}
}

const std::vector<std::unique_ptr<Ball>>& Pit::getBalls() const
{
	return balls;
}

void Pit::setBallsSizePercentage(float newSizePercentage)
{
	for (int i = 0; i < 3; i++)
	{
		balls[i]->setsizePercentage(newSizePercentage);
	}
}

void Pit::setBallsTranspose(int newTranspose)
{
	for (int i = 0; i < 3; i++)
	{
		balls[i]->setBallTranspose(newTranspose);
	}
}

void Pit::setBallParams(int index, float x, float y, float radius, float velocity, float angle, int ballSpeedType, DelaySettings newDelaySettings)
{
	balls[index]->setBallSpeedType(ballSpeedType);
	balls[index]->setPosition(x, y);
	balls[index]->setRadius(radius);
	balls[index]->setVelocity(velocity);
	balls[index]->setAngle(angle);
	balls[index]->setDelaySettings(newDelaySettings);

	int otherIndex1 = (index + 1) % balls.size();
	int otherIndex2 = (index + 2) % balls.size();
	if (balls[index]->checkCollision(*balls[otherIndex1]))
	{
		balls[index]->resolveCollision(*balls[otherIndex1]);
	}
	if (balls[index]->checkCollision(*balls[otherIndex2]))
	{
		balls[index]->resolveCollision(*balls[otherIndex2]);
	}
	if (balls[otherIndex1]->checkCollision(*balls[otherIndex2]))
	{
		balls[otherIndex1]->resolveCollision(*balls[otherIndex2]);
	}
}

void Pit::setEdgeParams(int phase, int denomenator, int range, int edgeType)
{
	this->edge.setPhase(phase);
	this->edge.setDenomenator(denomenator);
	this->edge.setRange(range);
	this->edge.setEdgeType(edgeType);
	this->edge.updateAbstractedEdge();
}

void Pit::setEdgeTypeToRandom()
{
	this->edge.setEdgeType(4);
}

void Pit::setScaleTypeToRandom(int scaleChoice, int rootNote)
{
	setEdgeScale(static_cast<Scale::ScaleKinds>(scaleChoice), rootNote);
}

void Pit::setBallsEdgeNotes()
{
	for (auto& ball : balls)
	{
		ball->updateBallAbstractedEdge(this->edge.getAbstractedEdge());
		int scaleNotes[3] = { this->edge.getScaleNotes(0), this->edge.getScaleNotes(1), this->edge.getScaleNotes(2) };
		ball->updateScaleNotes(scaleNotes);
	}
}

void Pit::setEdgeScale(Scale::ScaleKinds scaleKind, int rootNote, uint8_t mode)
{
	this->edge.setScale(scaleKind, rootNote, mode);
	this->edge.getMIDI();
}

static int getCurrentRectSize(int currentIndex, int noteRectSize)
{
	jassert((noteRectSize > 0) && (currentIndex > (-1)));

	int endOfColor = currentIndex + noteRectSize;

	if ((currentIndex < 392) && (endOfColor > 392))
	{
		return 392 - currentIndex;
	}
	else if ((currentIndex < 784) && (endOfColor > 784))
	{
		return 784 - currentIndex;
	}
	else if ((currentIndex < 1176) && (endOfColor > 1176))
	{
		return 1176 - currentIndex;
	}
	else if ((currentIndex < 1568) && (endOfColor > 1568))
	{
		return 1568 - currentIndex;
	}

	return -1; // no need
}

static void drawSingleRect(juce::Graphics& g, int index, int rectSizeToDraw)
{
	if (index < 392)
	{
		g.fillRect(PIT_MIN_X + 1, PIT_MIN_Y + 1 + (index % 392), PIT_EDGE_THICKNESS, rectSizeToDraw);
	}
	else if ((index >= 392) && (index < 784))
	{
		g.fillRect(PIT_MIN_X + 2 + (index % 392), PIT_MAX_Y - 3, rectSizeToDraw, PIT_EDGE_THICKNESS);
	}
	else if ((index >= 784) && (index < 1176))
	{
		g.fillRect(PIT_MAX_X - 3, PIT_MAX_Y - 1 - (index % 392) - rectSizeToDraw, PIT_EDGE_THICKNESS, rectSizeToDraw);
	}
	else if ((index >= 1176) && (index < 1568))
	{
		g.fillRect(PIT_MAX_X - 1 - rectSizeToDraw - (index % 392), PIT_MIN_Y + 1, rectSizeToDraw, PIT_EDGE_THICKNESS);
	}
}

static void drawSingleRectDivider(juce::Graphics& g, int dividerSize, int currentIndex)
{
	if (currentIndex < 392)
	{
		currentIndex = currentIndex - (dividerSize / 2);
		g.fillRect(PIT_MIN_X, PIT_MIN_Y + 1 + (currentIndex % 392), PIT_EDGE_THICKNESS + 1, dividerSize);
	}
	else if ((currentIndex >= 392) && (currentIndex < 784))
	{
		g.fillRect(PIT_MIN_X + (currentIndex % 392), PIT_MAX_Y - 3, dividerSize, PIT_EDGE_THICKNESS + 1);
	}
	else if ((currentIndex >= 784) && (currentIndex < 1176))
	{
		g.fillRect(PIT_MAX_X - 3, PIT_MAX_Y - 1 - (currentIndex % 392) - dividerSize, PIT_EDGE_THICKNESS + 1, dividerSize);
	}
	else if ((currentIndex >= 1176) && (currentIndex < 1568))
	{
		g.fillRect(PIT_MAX_X - 1 - dividerSize - (currentIndex % 392), PIT_MIN_Y, dividerSize, PIT_EDGE_THICKNESS + 1);
	}
}

static void drawRectDividers(juce::Graphics& g, int numOfSplits, int currentPhase)
{
	if (numOfSplits == 1)
		return;

	g.setColour(juce::Colours::white);
	int currentSplitPos = 0;
	int dividerSize = 4; // must be even
	jassert((numOfSplits > 0) && (dividerSize % 2 == 0));

	for (int i = 0; i < numOfSplits; i++)
	{
		currentSplitPos = (static_cast<int>(i * 1568 / numOfSplits) + currentPhase) % 1568;
		drawSingleRectDivider(g, dividerSize, currentSplitPos);
	}
}

void Pit::drawPitEdge(juce::Graphics& g, juce::Colour* edgeColors)
{
	int numOfSplits = this->edge.getDenomenator();
	int numOfColors = this->edge.getRange();
	int currentPhase = juce::jmap<int>(this->edge.getPhase(), 0, 360, 0, 1567);
	if (currentPhase == 1567) { currentPhase = 0; } // weird but does the job.....
	const int* pointerToAbstractedEdgeColors = this->edge.getAbstractedEdgeColors();
	int noteRectSize = 1568 / numOfSplits;
	int reminder = 1568 % numOfSplits;
	int index = currentPhase;
	int currentRectSize = -1;
	g.setColour(edgeColors[pointerToAbstractedEdgeColors[index]]);

	for (int i = 0; i < numOfSplits;)
	{
		do
		{
			currentRectSize = getCurrentRectSize(index, noteRectSize);
			
			if (currentRectSize == -1)
			{
				drawSingleRect(g, index, noteRectSize);
				index = (index + noteRectSize) % 1568;
				if ((index > 1560) && (reminder != 0))
				{
					drawSingleRect(g, index, reminder);
					index = (index + reminder) % 1568;
					reminder = 0;
				}
				i++;
			}
			else
			{
				drawSingleRect(g, index, currentRectSize);
				index = (index + currentRectSize) % 1568;
				noteRectSize -= currentRectSize;
			}
		} while (currentRectSize > 0);

		noteRectSize = 1568 / numOfSplits;
		g.setColour(edgeColors[pointerToAbstractedEdgeColors[index]]);
	}

	if (this->edge.getRange() > 1)
	{
		drawRectDividers(g, numOfSplits, currentPhase);
	}

	if (reminder != 0) 
	{
		g.setColour(edgeColors[pointerToAbstractedEdgeColors[index]]);
		g.fillRect(PIT_MIN_X + 3, PIT_MIN_Y, reminder, 4);
	}
}