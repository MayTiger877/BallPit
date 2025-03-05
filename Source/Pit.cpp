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

void Pit::update()
{
	// edges hit
	for (auto& ball : balls)
	{
		ball->update();
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

void Pit::setBallParams(int index, float x, float y, float radius, float velocity, float angle)
{
	balls[index]->setPosition(x, y);
	balls[index]->setRadius(radius);
	balls[index]->setVelocity(velocity);
	balls[index]->setAngle(angle);
	
	// TODO - add a func that sets the x,y of the other balls to not overlap
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

void Pit::setEdgeParams(int phase, int denomenator, int range)
{
	this->edge.setPhase(phase);
	this->edge.setDenomenator(denomenator);
	this->edge.setRange(range);
	this->edge.updateAbstractedEdge();
}

void Pit::setBallsEdgeNotes()
{
	for (auto& ball : balls)
	{
		ball->updateAbstractedEdge(this->edge.getAbstractedEdge());
		int scaleNotes[3] = { this->edge.getScaleNotes(0), this->edge.getScaleNotes(1), this->edge.getScaleNotes(2) };
		ball->updateScaleNotes(scaleNotes);
	}
}

void Pit::checkBallsPositionBeforeStart()
{
	for (auto& ball : balls) // TODO - delete this kaka
	{
		DBG("tttttt");
	}
}

void Pit::setEdgeScale(Scale::ScaleKinds scaleKind, int rootNote, uint8_t mode)
{
	this->edge.setScale(scaleKind, rootNote, mode);
	this->edge.getMIDI();
}

static int isRectNeedsToBeDivided(int currentIndex, int rectSize)
{
	jassert((rectSize >0) && (currentIndex > (-1)));

	int endOfRect = currentIndex + rectSize;

	if ((currentIndex < 392) && (endOfRect > 392))
	{
		return 392 - currentIndex;
	}
	else if ((currentIndex < 784) && (endOfRect > 784))
	{
		return 784 - currentIndex;
	}
	else if ((currentIndex < 1176) && (endOfRect > 1176))
	{
		return 1176 - currentIndex;
	}
	else if ((currentIndex < 1568) && (endOfRect > 1568))
	{
		return 1568 - currentIndex;
	}

	return -1; // no need
}

void Pit::drawPitEdge(juce::Graphics& g, juce::Colour* edgeColors) const
{
	int numOfSplits = this->edge.getDenomenator();
	int numOfColors = this->edge.getRange();
	int split = 1568 / numOfSplits;
	int remainder = 1568 % numOfSplits;
	int index, colorIndex = 0;
	for (int i = 0; i < numOfSplits; i++)
	{
		index = ((i * split) + this->edge.getPhase()) % 1568;
		g.setColour(edgeColors[colorIndex]);
		int divideIndex = isRectNeedsToBeDivided(index, split);

		if (divideIndex == -1)
		{
			if (index <= 392)
			{
				g.fillRect(8, 10 + (index % 392), 4, split);
			}
			else if ((index > 392) && (index <= 784))
			{
				g.fillRect(10 + (index % 392), 8, split, 4);
			}
			else if ((index > 784) && (index <= 1176))
			{
				g.fillRect(404, 404 - (index % 392), 4, split);
			}
			else if ((index > 1176) && (index <= 1568))
			{
				g.fillRect(404 - (index % 392), 11, split, 4);
			}
		}
		else
		{
			int rectReminder = split - divideIndex;
			if (index <= 392)
			{
				g.fillRect(8, 10 + index, 4, divideIndex);
				g.fillRect(10, 404, rectReminder, 4);
			}
			else if ((index > 392) && (index <= 784))
			{
				g.fillRect(10 + index, 404, divideIndex, 4);
				g.fillRect(404, (12 + 1176 - rectReminder), 4, rectReminder);
			}
			else if ((index > 784) && (index <= 1176))
			{
				g.fillRect(204, (12 + 1176 - divideIndex), 4, divideIndex);
				g.fillRect(10 + 1568 - rectReminder, 11, rectReminder, 4);
			}
			else if ((index > 1176) && (index <= 1568))
			{
				g.fillRect(10 + 1568 - divideIndex, 11, divideIndex, 4);
				g.fillRect(8, 10, 4, rectReminder);
			}
		}

		index += split;
		colorIndex = (colorIndex + 1) % numOfColors;
	}

	for (int i = 0; i < remainder; i++)
	{
		g.fillRect(12 + i, 12, 2, 4);
	}
}