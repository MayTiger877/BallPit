/*
  ==============================================================================

    MyCostumeDial.cpp
    Created: 15 May 2025 11:23:04am
    Author:  MayTigerStation

  ==============================================================================
*/

#include "MyCostumeDial.h"

MyCostumeDial::MyCostumeDial()
{
}
using namespace juce;


void MyCostumeDial::drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
                                     const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{
    auto bounds = Rectangle<int>(x, y, width, height).toFloat().reduced(10);
    float radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    float angle =  rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    float rx = bounds.getCentreX() - radius;
    float ry = bounds.getCentreY() - radius;


    g.setColour(MyrotarySliderFillColourId);
    g.fillEllipse(bounds);
    g.setColour(juce::Colours::black);
    g.fillEllipse(bounds.expanded(1.0f));
    g.setColour(MyrotarySliderFillColourId.darker(0.2f));
    g.fillEllipse(bounds.reduced(1.0f));
    g.setColour(MyrotarySliderFillColourId.darker(0.4f));
    g.fillEllipse(bounds.reduced(2.0f));
    g.setColour(MyrotarySliderFillColourId.darker(0.7f));
    g.fillEllipse(bounds.reduced(4.0f));

    juce::Path line;
    g.setColour(MyrotarySliderFillColourId.brighter(0.7f));
    line.addRectangle(0, -radius + 6, 2.0f, radius * 0.8);
    g.fillPath(line, juce::AffineTransform::rotation(angle).translated(bounds.getCentreX(), bounds.getCentreY()));

    if (sliderPos == 0.5f)
        DBG("sliderPos is " << std::to_string(sliderPos));
}
