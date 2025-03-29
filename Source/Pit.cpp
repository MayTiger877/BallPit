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

void Pit::setEdgeParams(int phase, int denomenator, int range, int edgeType)
{
	this->edge.setPhase(phase);
	this->edge.setDenomenator(denomenator);
	this->edge.setRange(range);
	this->edge.setEdgeType(edgeType);
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
		g.fillRect(7, 13 + (index % 392), 5, rectSizeToDraw);
	}
	else if ((index >= 392) && (index < 784))
	{
		g.fillRect(11 + (index % 392), 404, rectSizeToDraw, 5);
	}
	else if ((index >= 784) && (index < 1176))
	{
		g.fillRect(403, 405 - (index % 392) - rectSizeToDraw, 5, rectSizeToDraw);
	}
	else if ((index >= 1176) && (index < 1568))
	{
		g.fillRect(404 - rectSizeToDraw - (index % 392), 10, rectSizeToDraw, 5);
	}
}

static void drawRectDividers(juce::Graphics& g, int numOfSplits) // KISS  W(-_-)W
{
	g.setColour(juce::Colours::black);
	int dividerSize = 4; // must be even
	jassert((numOfSplits > 0) && (dividerSize%2 == 0));

	if ((numOfSplits == 1) ||
		(numOfSplits == 2) ||
		(numOfSplits == 4))
	{
		return;
	}
	else if (numOfSplits == 3)
	{
		g.fillRect(11 + (523 % 392) - (dividerSize / 2), 404, dividerSize, 5);
		g.fillRect(403, 405 - (1045 % 392) - dividerSize + (dividerSize / 2), 5, dividerSize);
	}
	else if (numOfSplits == 5)
	{
		g.fillRect(7, 13 + (314 % 392) - (dividerSize / 2), 5, dividerSize);
		g.fillRect(11 + (627 % 392) - (dividerSize / 2), 404, dividerSize, 5);
		g.fillRect(403, 405 - (941 % 392) - dividerSize + (dividerSize / 2), 5, dividerSize);
		g.fillRect(404 - dividerSize - (1254 % 392) + (dividerSize / 2), 10, dividerSize, 5);
	}
	else if (numOfSplits == 6)
	{
		g.fillRect(7, 13 + (261 % 392) - (dividerSize / 2), 5, dividerSize);
		g.fillRect(11 + (523 % 392) - (dividerSize / 2), 404, dividerSize, 5);
		// can pass the 3rd
		g.fillRect(403, 405 - (1045 % 392) - dividerSize + (dividerSize / 2), 5, dividerSize);
		g.fillRect(404 - dividerSize - (1307 % 392) + (dividerSize / 2), 10, dividerSize, 5);
	}
	else if (numOfSplits == 7)
	{
		g.fillRect(7, 13 + (224 % 392) - (dividerSize / 2), 5, dividerSize);
		g.fillRect(11 + (448 % 392) - (dividerSize / 2), 404, dividerSize, 5);
		g.fillRect(11 + (672 % 392) - (dividerSize / 2), 404, dividerSize, 5);
		g.fillRect(403, 405 - (896 % 392) - dividerSize + (dividerSize / 2), 5, dividerSize);
		g.fillRect(403, 405 - (1120 % 392) - dividerSize + (dividerSize / 2), 5, dividerSize);
		g.fillRect(404 - dividerSize - (1344 % 392) + (dividerSize / 2), 10, dividerSize, 5);
	}
	else if (numOfSplits == 8)
	{
		g.fillRect(7, 13 + (196 % 392) - (dividerSize / 2), 5, dividerSize);
		g.fillRect(11 + (588 % 392) - (dividerSize / 2), 404, dividerSize, 5);
		g.fillRect(403, 405 - (980 % 392) - dividerSize + (dividerSize / 2), 5, dividerSize);
		g.fillRect(404 - dividerSize - (1372 % 392) + (dividerSize / 2), 10, dividerSize, 5);
	}
}

void Pit::drawPitEdge(juce::Graphics& g, juce::Colour* edgeColors)
{
	int numOfSplits = this->edge.getDenomenator();
	int numOfColors = this->edge.getRange();
	int noteRectSize = 1568 / numOfSplits;
	int reminder = 1568 % numOfSplits;
	int colorIndex = 0;
	int index = this->edge.getPhase() % 1568;
	int currentRectSize = -1;
	g.setColour(edgeColors[colorIndex]);

	for (int i = 0; i < numOfSplits;)
	{
		do
		{
			currentRectSize = getCurrentRectSize(index, noteRectSize);
			
			if (currentRectSize == -1)
			{
				drawSingleRect(g, index, noteRectSize);
				index += noteRectSize % 1568;
				i++;
			}
			else
			{
				drawSingleRect(g, index, currentRectSize);
				index += currentRectSize % 1568;
				noteRectSize -= currentRectSize;
			}
		} while (currentRectSize > 0);

		//colorIndex = (colorIndex + 1) % numOfColors;
		colorIndex = this->edge.promoteColorIndexByEdgeType(colorIndex, numOfColors);
		g.setColour(edgeColors[colorIndex]);
		noteRectSize = 1568 / numOfSplits;
	}

	if (this->edge.getRange() > 1)
	{
		drawRectDividers(g, numOfSplits);
	}

	if (reminder != 0) 
	{
		colorIndex = this->edge.promoteColorIndexByEdgeType(colorIndex, numOfColors, true);
		g.setColour(edgeColors[colorIndex]);
		g.fillRect(10, 12, reminder, 4);
	}
}