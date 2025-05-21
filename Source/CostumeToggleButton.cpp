/*
  ==============================================================================

    CostumeToggleButton.cpp
    Created: 21 May 2025 4:06:24pm
    Author:  MayTigerStation

  ==============================================================================
*/

#include "CostumeToggleButton.h"

CostumeToggleButoonLAF::CostumeToggleButoonLAF()
{
}

void CostumeToggleButoonLAF::drawToggleButton(Graphics& g, ToggleButton& button,
    bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown)
{
    //drew grid
    float x = 0.0f;
    float y = 0.0f;
    float width = button.getWidth() - 5.0;
    float height = button.getHeight() - 5.0;

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