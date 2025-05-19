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

class MyCostumeComboBox : public juce::LookAndFeel_V4
{
public:
    MyCostumeComboBox();


    void drawComboBox(juce::Graphics&, int width, int height, bool isButtonDown,
                      int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox&) override;

    void drawPopupMenuItem(juce::Graphics&, const juce::Rectangle<int>& area,
                           bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu,
                           const juce::String& text, const juce::String& shortcutKeyText,
                           const juce::Drawable* icon, const juce::Colour* textColour) override;


private:
    juce::Colour MybackgroundColourId = juce::Colour::fromRGB(10, 16, 20);  /**< A colour to use to fill the slider's background. */
    juce::Colour MythumbColourId = juce::Colour::fromRGB(117, 137, 167);  /**< The colour to draw the thumb with. It's up to the look and feel class how this is used. */
    juce::Colour MytrackColourId = juce::Colour::fromRGB(250, 250, 250);  /**< The colour to draw the groove that the thumb moves along. */
    juce::Colour MyrotarySliderFillColourId = juce::Colour::fromRGB(25, 78, 113);  /**< For rotary sliders, this colour fills the outer curve. */
    juce::Colour MyrotarySliderOutlineColourId = juce::Colour::fromRGB(18, 38, 67);  /**< For rotary sliders, this colour is used to draw the outer curve's outline. */
};