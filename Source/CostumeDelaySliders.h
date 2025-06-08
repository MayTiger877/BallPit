/*
  ==============================================================================

    CostumeDelaySliders.h
    Created: 8 Jun 2025 6:57:14pm
    Author:  MayTigerStation

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Configs.h"

using namespace juce;

class MyCostumeDelayAmountLAF : public LookAndFeel_V4
{
public:
    MyCostumeDelayAmountLAF();

    void drawLinearSlider(Graphics&, int x, int y, int width, int height,
                          float sliderPos, float minSliderPos, float maxSliderPos,
                          Slider::SliderStyle, Slider&) override;
};

// ------------------------------------------------------------------------------------------

class MyCostumeDelayFeedbackLAF : public LookAndFeel_V4
{
public:
    MyCostumeDelayFeedbackLAF();

    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height,
        float sliderPosProportional, float rotaryStartAngle,
        float rotaryEndAngle, juce::Slider&) override;
};
