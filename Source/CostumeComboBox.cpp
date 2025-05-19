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

    // arrow- do i want it??
    /*Rectangle<int> arrowZone(width - 30, 0, 20, height);
    Path path;
    path.startNewSubPath((float)arrowZone.getX() + 3.0f, (float)arrowZone.getCentreY() - 2.0f);
    path.lineTo((float)arrowZone.getCentreX(), (float)arrowZone.getCentreY() + 3.0f);
    path.lineTo((float)arrowZone.getRight() - 3.0f, (float)arrowZone.getCentreY() - 2.0f);
    g.setColour(box.(ComboBox::arrowColourId).withAlpha((box.isEnabled() ? 0.9f : 0.2f)));
    g.strokePath(path, PathStrokeType(2.0f));*/
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

        // Tick is ugly
        /*auto iconArea = r.removeFromLeft(roundToInt(maxFontHeight)).toFloat();
        if (icon != nullptr)
        {
            icon->drawWithin(g, iconArea, RectanglePlacement::centred | RectanglePlacement::onlyReduceInSize, 1.0f);
            r.removeFromLeft(roundToInt(maxFontHeight * 0.5f));
        }
        else if (isTicked)
        {
            auto tick = getTickShape(1.0f);
            g.fillPath(tick, tick.getTransformToScaleToFit(iconArea.reduced(iconArea.getWidth() / 5, 0).toFloat(), true));
        }*/

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

            g.drawText(shortcutKeyText, r, Justification::centredRight, true);
        }
    }
}
