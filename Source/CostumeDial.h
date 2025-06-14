/*
  ==============================================================================

    MyMyCostumeDial.h
    Created: 15 May 2025 11:23:04am
    Author:  MayTigerStation

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Configs.h"

using namespace juce;

class MyCostumeDial : public juce::LookAndFeel_V4
{
public:
	MyCostumeDial();

    void drawRotarySlider(juce::Graphics&, int x, int y, int width, int height,
                          float sliderPosProportional, float rotaryStartAngle,
                          float rotaryEndAngle, juce::Slider&) override;

    void drawLinearSlider(Graphics&, int x, int y, int width, int height,
                          float sliderPos, float minSliderPos, float maxSliderPos,
                          Slider::SliderStyle, Slider&) override;

private:
    juce::Colour MyrotarySliderFillColourId = BUTTON_BG_COLOUR;
    juce::Colour MyrotarySliderOutlineColourId = BUTTON_TEXT_COLOUR;
};

class CostumeHorizontalSlider : public juce::LookAndFeel_V4
{
public:
    CostumeHorizontalSlider();

    void drawLinearSlider(Graphics&, int x, int y, int width, int height,
        float sliderPos, float minSliderPos, float maxSliderPos,
        Slider::SliderStyle, Slider&) override;

private:
};
