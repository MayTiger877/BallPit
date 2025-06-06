/*
  ==============================================================================

    CostumeToggleButton.cpp
    Created: 21 May 2025 4:06:24pm
    Author:  MayTigerStation

  ==============================================================================
*/

#include "CostumeToggleButton.h"

MyCostumeSnapToGridLAF::MyCostumeSnapToGridLAF()
{
}

void MyCostumeSnapToGridLAF::drawToggleButton(Graphics& g, ToggleButton& button,
    bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    float x = 0.0f;
    float y = 0.0f;
    float width = button.getWidth() - 5.0;
    float height = button.getHeight() - 5.0;

    //drew grid
    g.setColour(BUTTON_BG_COLOUR.brighter(0.6f));
    g.drawLine(x + width / 2, y, x + width / 2, y + height, 2.0f);
    g.drawLine(x + width / 4, y, x + width / 4, y + height, 1.0f);
    g.drawLine(x + width * 3 / 4, y, x + width * 3 / 4, y + height, 1.0f);
    g.drawLine(x, y + height / 2, x + width, y + height / 2, 2.0f);
    g.drawLine(x, y + height / 4, x + width, y + height / 4, 1.0f);
    g.drawLine(x, y + height * 3 / 4, x + width, y + height * 3 / 4, 1.0f);

    // draw balls
    if (button.getToggleState() == false)
    {
        g.setColour(BALL_1_COLOUR.withAlpha(0.4f));
        g.fillEllipse(x + width / 2 + 12.0f, y + height / 2 + 9.0f, 12.0f, 12.0f);
        g.setColour(BALL_2_COLOUR.withAlpha(0.4f));
        g.fillEllipse(x + width / 2, y + height / 2 - 17.0f, 12.0f, 12.0f);
        g.setColour(BALL_3_COLOUR.withAlpha(0.4f));
        g.fillEllipse(x + width / 2 - 21.0f, y + height / 2 - 2.0f, 12.0f, 12.0f);
    }
    else
    {
        g.setColour(juce::Colours::black);
        g.fillEllipse(x + width / 2 - 6.0f, y + height / 2 - 6.0f, 12.0f, 12.0f);
    }
}

// ------------------------------------------------------------------------------------------

MyCostumeCollisionLAF::MyCostumeCollisionLAF()
{
}

void MyCostumeCollisionLAF::drawToggleButton(Graphics& g, ToggleButton& button,
    bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    float x = 0.0f;
    float y = 0.0f;
    float width = button.getWidth() - 5.0;
    float height = button.getHeight() - 10.0;

    g.setColour(BUTTON_BG_COLOUR.brighter(0.6f));
    g.drawLine(x, y + height - 2.f, x + width - 2.0f, y + height - 2.f, 4.0f);
    g.setColour(BUTTON_BG_COLOUR);
    for (int i = 0; i < 7; i++)
    {
        g.drawLine(x + i * (width - 2.0f) / 6, y + height - 4.f,
                   x + i * (width - 2.0f) / 6 + 3.0f, y + height, 2.0f);
    }

    if (button.getToggleState() == false)
    {
        g.setColour(BALL_1_COLOUR.withAlpha(0.4f));
        g.fillEllipse(x + width / 2 - 3.0, y + height - 30.0f, 25.0f, 25.0f);
        g.setColour(BALL_3_COLOUR.withAlpha(0.4f));
        g.fillEllipse(x + width / 2 - 20.0f, y + height - 30.0f, 25.0f, 25.0f);
        g.setColour(juce::Colours::white);
        g.drawLine(x + width / 2.5 - 37.0f, y + height - 23.0f,
                   x + width / 2.5 - 17.0f , y + height - 23.0f, 1.0f);
        g.drawLine(x + width / 2.5 - 30.0f, y + height - 13.0f,
                   x + width / 2.5 - 12.0f, y + height - 13.0f, 1.0f);
        g.drawLine(x + width / 1.5 + 40.0f, y + height - 15.0f,
                   x + width / 1.5 + 20.0f, y + height - 15.0f, 1.0f);
        g.drawLine(x + width / 1.5 + 30.0f, y + height - 26.0f,
                   x + width / 1.5 + 12.0f, y + height - 26.0f, 1.0f);
    }
    else
    {
        g.setColour(BALL_1_COLOUR.withAlpha(0.7f));
        g.fillEllipse(x + width / 2, y + height - 30.0f, 25.0f, 25.0f);
        g.setColour(BALL_3_COLOUR.withAlpha(0.7f));
        g.fillEllipse(x + width / 2 - 25.0f, y + height - 30.0f, 25.0f, 25.0f);
        g.setColour(juce::Colours::white);
        g.drawLine(x + width / 2 - 8.0,  y + height - 26.0f,
                   x + width / 2,        y + height - 20.0f, 1.0f);
        g.drawLine(x + width / 2 - 11.0, y + height - 16.0f,
                   x + width / 2 - 3.0,  y + height - 15.0f, 1.0f);
        g.drawLine(x + width / 2 - 6.0,  y + height - 3.0f,
                   x + width / 2 - 1.0,  y + height - 9.0f, 1.0f);
        g.drawLine(x + width / 2 + 1.0,  y + height - 18.0f,
                   x + width / 2 + 5.0,  y + height - 21.0f, 1.0f);
        g.drawLine(x + width / 2      ,  y + height - 13.0f,
                   x + width / 2 + 6.0,  y + height - 10.0f, 1.0f);
    }
}

// ------------------------------------------------------------------------------------------

MyCostumeVelocityInverterLAF::MyCostumeVelocityInverterLAF()
{
}

void MyCostumeVelocityInverterLAF::drawToggleButton(Graphics& g, ToggleButton& button,
    bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    float width = button.getWidth();
    float height = button.getHeight();
    g.setColour(BUTTON_BG_COLOUR);
    g.fillAll();
    g.setColour(juce::Colours::black);
    if (button.getToggleState() == false)
    {
        juce::Line<float> arrowLine(width / 5, height / 2, width * 4 / 5, height / 2);
        g.drawArrow(arrowLine, 2.0f, 6.0f, 5.0f);
    }
    else 
    {
        juce::Line<float> arrowLine(width * 4 / 5, height / 2, width / 5, height / 2);
        g.drawArrow(arrowLine, 2.0f, 6.0f, 5.0f);
    }
}