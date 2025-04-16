#include "Ball.h"
#include <iostream>

Ball::Ball(int ballIndex, float x, float y, float radius, float velocity, float angle)
	: ballIndex(ballIndex), x(x), y(y), radius(radius), velocity(0.1 * velocity), angle(angle)
{
	setAngledSpeed();
	setInterceptsMouseClicks(true, false);
	setWantsKeyboardFocus(true);
}

void Ball::setBallEdgeEventListener(BallEdgeEventListener* l)
{
	this->edgeListener = l;
}

void Ball::setBallCollideEventListener(BallCollideEventListener* l)
{
	this->collideListener = l;
}

void Ball::update()
{
	if (this->active == true)
	{
		x += speedX;
		y += speedY;
	}
	else
	{
		speedX = speedY = 0;
		x = 440 + ballIndex * 50;
		y = 380;
	}

}

void Ball::updateBallAbstractedEdge(const int* abstractedEdge)
{
	for (int i = 0; i < 1568; i++)
	{
		abstractedEdgeDuplicate[i] = abstractedEdge[i];
	}
}

void Ball::updateScaleNotes(int* newScaleNotes)
{
	this->scaleNotes[0] = newScaleNotes[0];
	this->scaleNotes[1] = newScaleNotes[1];
	this->scaleNotes[2] = newScaleNotes[2];
}

void Ball::draw(juce::Graphics& g) const
{
	juce::Colour ballColor;
	switch (ballIndex)
	{
		case 0:
			ballColor = juce::Colours::blue;
			break;
		case 1:
			ballColor = juce::Colours::crimson;
			break;
		case 2:
			ballColor = juce::Colours::orange;
			break;
	default:
		break;
	}

	float mouseOnMagnifier = (isMouseOverBall == true) ?6.0f : 0.0f;

	//black outline ball
	g.setColour(juce::Colours::black);
	g.fillEllipse(x - radius - 1.0 - mouseOnMagnifier,
				  y - radius - 1.0 - mouseOnMagnifier,
				  radius * 2.0f + 2.0 + mouseOnMagnifier * 2,
				  radius * 2.0f + 2.0 + mouseOnMagnifier * 2);
	// colored ball
	g.setColour(ballColor);
	g.fillEllipse(x - radius - mouseOnMagnifier,
				  y - radius - mouseOnMagnifier,
				  radius * 2.0f + mouseOnMagnifier * 2,
				  radius * 2.0f + mouseOnMagnifier * 2);

	if (isMoving == false) // draw direction arrow
	{
		float radiusRatio = pow(radius, 1.2);
		float angleInRadians = (angle - 90) * (PI / 180.0f); // (angle-90) is the actual knob direction
		float startX = x + (5.0f + mouseOnMagnifier + radius) * cos(angleInRadians);
		float endX = x + (20.0f + mouseOnMagnifier + radiusRatio) * cos(angleInRadians);
		float startY = y + (5.0f + mouseOnMagnifier + radius) * sin(angleInRadians);
		float endY = y + (20.0f + mouseOnMagnifier + radiusRatio) * sin(angleInRadians);
		float arrowThickness = (2.5f + (radius / 6.0f));
		float arrowHeadWidth = (6.0f + (radius / 3.0f));
		float arrowHeadLength = (8.0f + (radius / 8.0f));
		juce::Line<float> line(startX, startY, endX, endY);
		startX = x + (3.0f + mouseOnMagnifier + radius) * cos(angleInRadians);
		endX = x + (24.0f + mouseOnMagnifier + radiusRatio) * cos(angleInRadians);
		startY = y + (3.0f + mouseOnMagnifier + radius) * sin(angleInRadians);
		endY = y + (24.0f + mouseOnMagnifier + radiusRatio) * sin(angleInRadians);
		juce::Line<float> thickLine(startX, startY, endX, endY);

		g.setColour(juce::Colours::black);
		g.drawArrow(thickLine, arrowThickness + 3.0f, arrowHeadWidth + 3.0f, arrowHeadLength + 5.0f);
		g.setColour(ballColor);
		g.drawArrow(line, arrowThickness, arrowHeadWidth, arrowHeadLength);
	}
}

void Ball::setRadius(float radius)
{
	this->radius = radius;
}

float Ball::getRadius() const
{
	return radius;
}

void Ball::setAngledSpeed()
{
	float offset = juce::MathConstants<float>::pi * (this->angle - 90.0f) / 180.0f;
	this->speedX = velocity * cos(offset);
	this->speedY = velocity * sin(offset);
	// actually angle of the knob is (angle - 90)
	if (angle == 90.0f || angle == 270.0f) // 0 or 180
	{
		this->speedY = NO_SPEED;
	}
	if (angle == 0.0f || angle == 180.0f) // 270 or 90
	{
		this->speedX = NO_SPEED;
	}
}

void Ball::setAngle(float angle)
{
	this->angle = angle;
	setAngledSpeed();
}

void Ball::setVelocity(float velocity)
{
	this->velocity = velocity;
	setAngledSpeed();
}

void Ball::setPosition(float newX, float newY)
{
	if (newX < minX + radius)
	{
		newX = minX + radius;
	}
	else if (newX > maxX - radius)
	{
		newX = maxX - radius;
	}
	
	if (newY < minY + radius)
	{
		newY = minY + radius;
	}
	else if (newY > maxY - radius)
	{
		newY = maxY - radius;
	}
		
	this->x = newX;
	this->y = newY;
}

int Ball::getEdgeHitIndex(HitPossition currentHitPosition)
{
	switch (currentHitPosition)
	{
		case LEFT:
			return (y - minY);
		case RIGHT:
			return (1568 - 392 - (y - minY));
		case UP:
			return (1568 - (x - minX));
		case DOWN:
			return (392 + (x - minX));
		default:
			break;
	}

	return 0;
}

void Ball::edgeBounce()
{
	if (this->active == false)
	{
		return;
	}

	HitPossition currentHitPos = LEFT;
	
	if ((x - radius <= minX || x + radius >= maxX) && (speedX != 0))// TODO - make this if better to cover radius when positioning balls
	{
		if (x - radius <= minX)
		{
			float distPassed = minX - (x - radius);
			x = minX + distPassed + radius;
			currentHitPos = LEFT;
		}

		if (x + radius >= maxX)
		{
			float distPassed = (x + radius) - maxX;
			x = maxX - distPassed - radius;
			currentHitPos = RIGHT;
		}
		
		speedX = -speedX;
		if (this->edgeListener)
		{
			int edgeIndex = getEdgeHitIndex(currentHitPos);

			if ((this->isMoving) && (speedX != NO_SPEED))
			{
				int noteVelocity = (int)(60 + (60 * (this->radius / 25)));
				this->edgeListener->onEdgeHit(abstractedEdgeDuplicate[edgeIndex], noteVelocity, sampleRate);
			}
		}
	}

	if ((y - radius <= minY || y + radius >= maxY) && (speedY != 0))
	{
		if (y - radius <= minY)
		{
			float distPassed = minY - (y - radius);
			y = minY + distPassed + radius;
			currentHitPos = UP;
		}
		if (y + radius >= maxY)
		{
			float distPassed = (y + radius) - maxY;
			y = maxY - distPassed - radius;
			currentHitPos = DOWN;
		}
		
		speedY = -speedY;
		if (this->edgeListener)
		{
			int edgeIndex = getEdgeHitIndex(currentHitPos);

			if ((this->isMoving) && (speedY != NO_SPEED))
			{
				int noteVelocity = (int)(60 + (60 * (this->radius / 25)));
				this->edgeListener->onEdgeHit(abstractedEdgeDuplicate[edgeIndex], noteVelocity, sampleRate);
			}
		}
	}
}

bool Ball::checkCollision(const Ball& other) const 
{
	float dx = x - other.x;
	float dy = y - other.y;
	float distance = std::sqrt(dx * dx + dy * dy);
	return distance <= (radius + other.radius);
}

void Ball::resolveCollision(Ball& other) 
{
	// Calculate distance between balls
	float dx = x - other.x;
	float dy = y - other.y;
	float distance = std::sqrt(dx * dx + dy * dy);

	// Normalize the direction vector
	float nx = dx / distance;
	float ny = dy / distance;

	// Correct overlap by moving balls apart
	float overlap = (radius + other.radius) - distance;
	x += nx * (overlap / 2.0f);
	y += ny * (overlap / 2.0f);
	other.x -= nx * (overlap / 2.0f);
	other.y -= ny * (overlap / 2.0f);

	// Calculate relative velocity in the normal direction
	float dvx = speedX - other.speedX;
	float dvy = speedY - other.speedY;
	float dotProduct = dvx * nx + dvy * ny;

	// If the balls are moving away from each other, no need to resolve the collision
	if ((this->isMoving) && (dotProduct > 0.0f))
	{
		return;
	}

	// Reflect velocities without changing the speed
	float speedA = std::sqrt(speedX * speedX + speedY * speedY);  // Speed of ball A
	float speedB = std::sqrt(other.speedX * other.speedX + other.speedY * other.speedY);  // Speed of ball B

	// Exchange velocities along the normal direction
	speedX -= 2.0f * dotProduct * nx;
	speedY -= 2.0f * dotProduct * ny;
	other.speedX += 2.0f * dotProduct * nx;
	other.speedY += 2.0f * dotProduct * ny;

	// Normalize the new velocities to maintain the original speed
	float newSpeedA = std::sqrt(speedX * speedX + speedY * speedY);
	speedX = (speedX / newSpeedA) * speedA;
	speedY = (speedY / newSpeedA) * speedA;

	float newSpeedB = std::sqrt(other.speedX * other.speedX + other.speedY * other.speedY);
	other.speedX = (other.speedX / newSpeedB) * speedB;
	other.speedY = (other.speedY / newSpeedB) * speedB;

	if (this->isMoving)
	{
		if (this->collideListener) this->collideListener->onBallsColide(scaleNotes, sampleRate);
	}
}

float Ball::isMouseInsineBall(juce::Point<float> mousePosition) const
{
	juce::Point<float> ballPosition(this->x, this->y);
	float distance = mousePosition.getDistanceFrom(ballPosition);
	return (distance <= radius) ? distance : MOUSE_NOT_IN_BALL;
}