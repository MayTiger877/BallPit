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

void Ball::insertDelayPoints(double clockTimeSeconds)
{
	if (clockTimeSeconds <= 0.0)
	{
		for (int i = 0; i < 3; i++)
		{
			this->delayPoints[i].x = x;
			this->delayPoints[i].y = y;
		}
	}
	else
	{
		if (clockTimeSeconds > (this->delaySettings.delayRate * 3)) // TODO- make the balls go in a similar difference delay
		{
			this->delayPoints[2] = this->ballPathPoints[2].front();
			this->ballPathPoints[2].pop(); 
			this->delayPoints[1] = this->ballPathPoints[1].front();
			this->ballPathPoints[1].pop();
			this->delayPoints[0] = this->ballPathPoints[0].front();
			this->ballPathPoints[0].pop();
		}
		else if (clockTimeSeconds > (this->delaySettings.delayRate * 2))
		{
			this->delayPoints[1] = this->ballPathPoints[1].front();
			this->ballPathPoints[1].pop();
			this->delayPoints[0] = this->ballPathPoints[0].front();
			this->ballPathPoints[0].pop();
		}
		else if (clockTimeSeconds > this->delaySettings.delayRate)
		{
			this->delayPoints[0] = this->ballPathPoints[0].front();
			this->ballPathPoints[0].pop();
		}
		else
		{
			this->delayPoints[0] = juce::Point<float>(x, y);
			this->delayPoints[1] = this->delayPoints[0];
			this->delayPoints[2] = this->delayPoints[0];
		}
	}
}

static void clearQueue(std::queue<juce::Point<float>>& q)
{
	std::queue<juce::Point<float>> empty;
	std::swap(q, empty);
}

void Ball::update(double timePassed, double clockTimeSeconds)
{
	if (this->active == true)
	{
		insertDelayPoints(clockTimeSeconds);
		x += speedX * timePassed;
		y += speedY * timePassed;
		ballPathPoints[0].push(juce::Point<float>(x, y));
		ballPathPoints[1].push(juce::Point<float>(x, y));
		ballPathPoints[2].push(juce::Point<float>(x, y));

	}
	else
	{
		speedX = speedY = 0;
		x = PIT_MIN_X + 20 + ballIndex * 50;
		y = PIT_MIN_Y + 200;
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
			ballColor = BALL_1_COLOUR;
			break;
		case 1:
			ballColor = BALL_2_COLOUR;
			break;
		case 2:
			ballColor = BALL_3_COLOUR;
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

	float angleInRadians = angle * (PI / 180.0f);
	float radiusRatio = pow(radius, 1.2);
	float arrowLenghtMultiplierByVelocity = 0.0f;
	if (this->ballSpeedType == 1) // chaos
	{
		angleInRadians = (angle - 90.0f) * (PI / 180.0f);
		arrowLenghtMultiplierByVelocity = juce::jmap<float>(velocity, 0.0, 2000.0, 0.0, 30.0);
	}
	else // byTempo
	{
		arrowLenghtMultiplierByVelocity = juce::jmap<float>(velocity, 0.0, 8300.0, 0.0, 30.0);
	}

	if (isMoving == false) // draw direction and length arrow and also delay balls
	{
		float startX = x + (5.0f + mouseOnMagnifier + radius) * cos(angleInRadians);
		float endX = x + (20.0f + mouseOnMagnifier + radiusRatio + arrowLenghtMultiplierByVelocity) * cos(angleInRadians);
		float startY = y + (5.0f + mouseOnMagnifier + radius) * sin(angleInRadians);
		float endY = y + (20.0f + mouseOnMagnifier + radiusRatio + arrowLenghtMultiplierByVelocity) * sin(angleInRadians);
		float arrowThickness = (2.5f + (radius / 6.0f));
		float arrowHeadWidth = (6.0f + (radius / 3.0f) + arrowLenghtMultiplierByVelocity / 3.0);
		float arrowHeadLength = (8.0f + (radius / 8.0f) + arrowLenghtMultiplierByVelocity / 3.0);
		juce::Line<float> line(startX, startY, endX, endY);
		startX = x + (3.0f + mouseOnMagnifier + radius) * cos(angleInRadians);
		endX = x + (24.0f + mouseOnMagnifier + radiusRatio + arrowLenghtMultiplierByVelocity) * cos(angleInRadians);
		startY = y + (3.0f + mouseOnMagnifier + radius) * sin(angleInRadians);
		endY = y + (24.0f + mouseOnMagnifier + radiusRatio + arrowLenghtMultiplierByVelocity) * sin(angleInRadians);
		juce::Line<float> thickLine(startX, startY, endX, endY);

		g.setColour(juce::Colours::black);
		g.drawArrow(thickLine, arrowThickness + 3.0f, arrowHeadWidth + 3.0f, arrowHeadLength + 5.0f);
		g.setColour(ballColor);
		g.drawArrow(line, arrowThickness, arrowHeadWidth, arrowHeadLength);

		// ----------- draw delay balls -----------
		if (this->delaySettings.delayAmount <= 0)
		{
			return;
		}

		for (int i = 1; i < (this->delaySettings.delayAmount + 1); i++)
		{
			float delaySizeRatio = pow(0.7f, i);
			float delayBallRadius = radius * delaySizeRatio;
			float delayBallX = x - (2.0f * radius * i + (this->delaySettings.delayRate * i)) * cos(angleInRadians);
			float delayBallY = y - (2.0f * radius * i + (this->delaySettings.delayRate * i)) * sin(angleInRadians);
			g.setColour(ballColor.withAlpha(0.1f + this->delaySettings.delayFeedback * delaySizeRatio));
			g.fillEllipse(delayBallX - delayBallRadius, delayBallY - delayBallRadius, 
						  delayBallRadius * 2.0f, delayBallRadius * 2.0f);
		}
	}
	else
	{
		for (int i = 1; i < (this->delaySettings.delayAmount + 1); i++)
		{
			float delaySizeRatio = pow(0.7f, i);
			float delayBallRadius = radius * delaySizeRatio;
			g.setColour(ballColor.withAlpha(0.1f + this->delaySettings.delayFeedback * delaySizeRatio));
			g.fillEllipse(this->delayPoints[i-1].getX() - delayBallRadius,
						  this->delayPoints[i-1].getY() - delayBallRadius,
						  delayBallRadius * 2.0f,
						  delayBallRadius * 2.0f);
		}
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
	if (ballSpeedType == 1) // ByKnobs
	{

		float offset = PI * (this->angle - 90.0f) / 180.0f;
		this->speedX = velocity * cos(offset);
		this->speedY = velocity * sin(offset);
		// offset is (angle - 90) because the + direction of Y axis is down..... god da#$^@#^
		if (angle == 90.0f || angle == 270.0f) // 0 or 180
		{
			this->speedY = NO_SPEED;
		}
		if (angle == 0.0f || angle == 180.0f) // 270 or 90
		{
			this->speedX = NO_SPEED;
		}
	}
	else // ByTempo
	{
		float angleInRadians = PI * this->angle / 180.0f;
		this->speedX = velocity * cos(angleInRadians);
		this->speedY = velocity * sin(angleInRadians);

		if (angle == 0.0f || angle == 180.0f) // 0 or 180
		{
			this->speedY = NO_SPEED;
		}
		if (angle == 90.0f || angle == 270.0f) // 270 or 90
		{
			this->speedX = NO_SPEED;
		}
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

void Ball::setActive(bool active)
{
	this->active = active;
	if (this->active == true)
	{
		insertDelayPoints(0.0); // reset path points when ball is activated
	}
}

void Ball::setDelaySettings(const DelaySettings& newDelaySettings)
{
	this->delaySettings = newDelaySettings;
	clearQueue(ballPathPoints[0]);
	clearQueue(ballPathPoints[1]);
	clearQueue(ballPathPoints[2]);
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
	float yBeforeCorrection = -1;
	
	if ((x - radius <= minX || x + radius >= maxX) && (speedX != 0))
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

		if ((y - radius <= minY || y + radius >= maxY) && (speedY != 0)) // corrct new y when x hit edge
		{
			yBeforeCorrection = y;
			if (y - radius <= minY)
			{
				float distPassed = minY - (y - radius);
				y = minY + distPassed + radius;
			}
			if (y + radius >= maxY)
			{
				float distPassed = (y + radius) - maxY;
				y = maxY - distPassed - radius;
			}
		}

		if (this->edgeListener)
		{
			int edgeIndex = getEdgeHitIndex(currentHitPos);
			jassert((edgeIndex >= 0) && (edgeIndex < 1568));
			if ((this->isMoving) && (speedX != NO_SPEED))
			{
				int noteVelocity = (int)(60 + (60 * (this->radius / 25)));
				this->edgeListener->onEdgeHit(abstractedEdgeDuplicate[edgeIndex], noteVelocity, sampleRate, this->delaySettings.delayAmount, this->delaySettings.delayRate);
			}
		}
	}

	if (yBeforeCorrection != -1)
		y = yBeforeCorrection;

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
			jassert((edgeIndex >= 0) && (edgeIndex < 1568));
			if ((this->isMoving) && (speedY != NO_SPEED))
			{
				int noteVelocity = (int)(60 + (60 * (this->radius / 25)));
				this->edgeListener->onEdgeHit(abstractedEdgeDuplicate[edgeIndex], noteVelocity, sampleRate, this->delaySettings.delayAmount, this->delaySettings.delayRate);
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

	if (std::isnan(other.x))
		other.x = PIT_MIN_X + 5 + (2 * other.radius) * 50;

	if (std::isnan(other.y))
		other.y = PIT_MIN_Y + 5 + (2 * other.radius) * 50;

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

float Ball::isMouseInsideBall(juce::Point<float> mousePosition) const
{
	juce::Point<float> ballPosition(this->x * sizePercentage, this->y * sizePercentage);
	float distance = mousePosition.getDistanceFrom(ballPosition);
	return (distance <= radius) ? distance : MOUSE_NOT_IN_BALL;
}