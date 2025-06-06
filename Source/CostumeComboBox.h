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
    Colour MyrotarySliderFillColourId = BUTTON_BG_COLOUR;
    Colour MyrotarySliderOutlineColourId = BUTTON_TEXT_COLOUR;
};