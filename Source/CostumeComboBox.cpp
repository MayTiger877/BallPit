/*
  ==============================================================================

    CostumeComboBox.cpp
    Created: 15 May 2025 11:23:31am
    Author:  MayTigerStation

  ==============================================================================
*/

#include "CostumeComboBox.h"

using namespace juce;

MyCostumeComboBox::MyCostumeComboBox()
{
}

void MyCostumeComboBox::drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
                                     int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& box)
{

    auto cornerSize = box.findParentComponentOfClass<ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
    Rectangle<int> boxBounds(0, 0, width, height);

    g.setColour(MyrotarySliderFillColourId.darker(0.2f));
    g.fillRoundedRectangle(boxBounds.toFloat(), cornerSize);

    g.setColour(MyrotarySliderFillColourId);
    g.drawRoundedRectangle(boxBounds.toFloat().reduced(0.5f, 0.5f), cornerSize, 1.0f);
}
// ------------------------------------------------------------------------------------------------------------------------------------------------

void MyCostumeComboBox::positionComboBoxText(ComboBox& box, Label& label) // to get eid of the stupid arrow padding!!! 
{
    label.setBounds(1, 1, box.getWidth() - 1, box.getHeight() - 2);

    label.setFont(getComboBoxFont(box));
}

// ------------------------------------------------------------------------------------------------------------------------------------------------

void MyCostumeComboBox::drawPopupMenuItem(Graphics& g, const Rectangle<int>& area,
                                          bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu,
                                          const String& text, const String& shortcutKeyText,
                                          const Drawable* icon, const Colour* textColourToUse)
{
    if (isSeparator)
    {
        auto r = area.reduced(5, 0);
        r.removeFromTop(roundToInt(((float)r.getHeight() * 0.5f) - 0.5f));

        g.setColour(juce::Colours::white.withAlpha(0.3f));
        g.fillRect(r.removeFromTop(1));
    }
    else
    {
        auto textColour = (textColourToUse == nullptr ? (MyrotarySliderFillColourId.brighter(0.8f)) : *textColourToUse);

        auto r = area.reduced(1);
        auto font = getPopupMenuFont();

        if (isHighlighted && isActive)
        {
            g.setColour(MyrotarySliderFillColourId.brighter(0.9f));
            g.fillRect(r);
            font.setBold(true);

            g.setColour(MyrotarySliderFillColourId);
        }
        else
        {
            g.setColour(textColour.withMultipliedAlpha(isActive ? 1.0f : 0.5f));
            font.setBold(false);
        }

        r.reduce(jmin(5, area.getWidth() / 20), 0);

        auto maxFontHeight = (float)r.getHeight() / 1.3f;

        if (font.getHeight() > maxFontHeight)
            font.setHeight(maxFontHeight);

        g.setFont(font);

        if (hasSubMenu)
        {
            auto arrowH = 0.6f * getPopupMenuFont().getAscent();

            auto x = static_cast<float> (r.removeFromRight((int)arrowH).getX());
            auto halfH = static_cast<float> (r.getCentreY());

            Path path;
            path.startNewSubPath(x, halfH - arrowH * 0.5f);
            path.lineTo(x + arrowH * 0.6f, halfH);
            path.lineTo(x, halfH + arrowH * 0.5f);

            g.strokePath(path, PathStrokeType(2.0f));
        }

        r.removeFromRight(3);
        g.drawFittedText(text, r, Justification::centred, 1);

        if (shortcutKeyText.isNotEmpty())
        {
            auto f2 = font;
            f2.setHeight(f2.getHeight() * 0.75f);
            f2.setHorizontalScale(0.95f);
            g.setFont(f2);
            g.setColour(BUTTON_BG_COLOUR);
            g.drawText(shortcutKeyText, r, Justification::centred, true);
        }
    }
}
