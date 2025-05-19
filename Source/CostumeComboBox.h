/*
  ==============================================================================

    CostumeComboBox.h
    Created: 15 May 2025 11:23:31am
    Author:  MayTigerStation

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Configs.h"
using namespace juce;

class MyCostumeComboBox : public LookAndFeel_V4
{
public:
    MyCostumeComboBox();


    void drawComboBox(Graphics&, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH, ComboBox&) override;

    void positionComboBoxText(ComboBox&, Label&) override;
    
    void drawPopupMenuItem(Graphics&, const Rectangle<int>& area,
                           bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu,
                           const String& text, const String& shortcutKeyText,
                           const Drawable* icon, const Colour* textColour) override;


private:
    Colour MybackgroundColourId = Colour::fromRGB(10, 16, 20);  /**< A colour to use to fill the slider's background. */
    Colour MythumbColourId = Colour::fromRGB(117, 137, 167);  /**< The colour to draw the thumb with. It's up to the look and feel class how this is used. */
    Colour MytrackColourId = Colour::fromRGB(250, 250, 250);  /**< The colour to draw the groove that the thumb moves along. */
    Colour MyrotarySliderFillColourId = Colour::fromRGB(25, 78, 113);  /**< For rotary sliders, this colour fills the outer curve. */
    Colour MyrotarySliderOutlineColourId = Colour::fromRGB(18, 38, 67);  /**< For rotary sliders, this colour is used to draw the outer curve's outline. */
};