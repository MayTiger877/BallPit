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
    auto outline = MyrotarySliderOutlineColourId;
    auto fill = MyrotarySliderFillColourId;

    auto bounds = Rectangle<int>(x, y, width, height).toFloat().reduced(10);

    auto radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
    auto toAngle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
    auto realAngle = juce::jmap<float>(sliderPos, 0.0, 1.0, 45.0, 315.0) - 270;
    auto lineW = jmin(8.0f, radius * 0.5f);
    auto arcRadius = radius - lineW * 0.5f;
    float angleInRadians = (realAngle) * (PI / 180.0f); // (angle-90) is the actual knob direction

    float endX = bounds.getCentreX() + (0.9 * radius) * cos(angleInRadians);
    float endY = bounds.getCentreY() + (0.9 * radius) * sin(angleInRadians);

    g.setColour(fill);
    g.fillEllipse(bounds);
    g.setColour(juce::Colours::white);
    g.drawLine(bounds.getCentreX(), bounds.getCentreY(), endX, endY);

}
