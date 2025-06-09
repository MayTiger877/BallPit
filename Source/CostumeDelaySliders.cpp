/*
  ==============================================================================

    CostumeDelaySliders.cpp
    Created: 8 Jun 2025 6:57:14pm
    Author:  MayTigerStation

  ==============================================================================
*/

#include "CostumeDelaySliders.h"

MyCostumeDelayAmountLAF::MyCostumeDelayAmountLAF()
{
}

void MyCostumeDelayAmountLAF::drawLinearSlider(Graphics& g, int x, int y, int width, int height,
                                               float sliderPos, float minSliderPos, float maxSliderPos,
                                               Slider::SliderStyle style, Slider& slider)
{
    auto trackWidth = jmin(6.0f, slider.isHorizontal() ? (float)height * 0.25f : (float)width * 0.25f);

    Point<float> startPoint(slider.isHorizontal() ? (float)x : (float)x + (float)width * 0.5f,
        slider.isHorizontal() ? (float)y + (float)height * 0.5f : (float)(height + y));

    Point<float> endPoint(slider.isHorizontal() ? (float)(width + x) : startPoint.x,
        slider.isHorizontal() ? startPoint.y : (float)y);

    Path backgroundTrack;
    backgroundTrack.startNewSubPath(startPoint);
    backgroundTrack.lineTo(endPoint);
    g.setColour(slider.findColour(Slider::backgroundColourId));
    g.strokePath(backgroundTrack, { trackWidth, PathStrokeType::curved, PathStrokeType::rounded });
    g.setColour(slider.findColour(Slider::backgroundColourId).brighter());
    g.strokePath(backgroundTrack, { (0.2f * trackWidth), PathStrokeType::curved, PathStrokeType::rounded });

    Path scaleGraduation;
    float lengthOfScale = std::abs(startPoint.getX() - endPoint.getX());
    Point<float> scaleGraduationPlacement((startPoint.getX() - 1.0f), (startPoint.getY() - 3.0f));
    for (int i = 0; i < 4; i++)
    {
        scaleGraduationPlacement.setXY((startPoint.getX() - 1.0f + (i * (lengthOfScale / 3))), (startPoint.getY() - 2.0f));
        scaleGraduation.startNewSubPath(scaleGraduationPlacement);
        scaleGraduation.lineTo(scaleGraduationPlacement.getX(), scaleGraduationPlacement.getY() + 3.0f);
        g.setColour(juce::Colours::darkcyan);
        g.strokePath(scaleGraduation, { 2.0f, PathStrokeType::curved, PathStrokeType::rounded });
        g.setColour(BUTTON_TEXT_COLOUR.darker(0.1f));
        g.drawText(std::to_string(i), scaleGraduationPlacement.getX() - 3.2f, scaleGraduationPlacement.getY() + 1.0f, 25.0, 25.0, Justification::bottomLeft);
    }

    auto kx = slider.isHorizontal() ? sliderPos : ((float)x + (float)width * 0.5f);
    auto ky = slider.isHorizontal() ? ((float)y + (float)height * 0.5f) : sliderPos;
    Point<float> maxPoint = { kx, ky };
    auto thumbWidth = getSliderThumbRadius(slider);

    g.setColour(slider.findColour(Slider::thumbColourId).darker(0.3f));
    g.fillEllipse(Rectangle<float>(static_cast<float> (thumbWidth), static_cast<float> (thumbWidth)).withCentre(maxPoint));
}


//------------------------------------------------------------------

MyCostumeDelayFeedbackLAF::MyCostumeDelayFeedbackLAF()
{
}

void MyCostumeDelayFeedbackLAF::drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
    const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{    
    g.setColour(BALL_1_COLOUR.withAlpha(juce::jmap(sliderPos, 0.0f, 1.0f, 0.4f, 1.0f)));
    g.fillEllipse((width / 8), height - 33.0f, 30.0f, 30.0f);

    g.setColour(BALL_1_COLOUR.withAlpha(juce::jmap(sliderPos, 0.0f, 1.0f, 0.0f, 0.7f)));
    g.fillEllipse((width / 8) + 40.0f, height - 23.0f, 20.0f, 20.0f);
    
    g.setColour(BALL_1_COLOUR.withAlpha(juce::jmap(sliderPos, 0.0f, 1.0f, 0.0f, 0.5f)));
    g.fillEllipse((width / 8) + 70.0f, height - 15.0f, 12.0f, 12.0f);

    g.setColour(BUTTON_TEXT_COLOUR);
	float feedbackPercentage = sliderPos * 100.0f;
	std::string feedbackText = std::to_string(static_cast<int>(feedbackPercentage)) + "%";
    g.drawText(feedbackText, width - 50, 15, 50, 20, Justification::left);

    g.setColour(BUTTON_BG_COLOUR.brighter(0.6f));
    g.drawLine(0.0f,height - 2.f, width - 2.0f, height - 2.f, 4.0f);
    g.setColour(BUTTON_BG_COLOUR);
    for (int i = 0; i < 7; i++)
    {
        g.drawLine(i * (width - 2.0f) / 6,height - 4.f,
            i * (width - 2.0f) / 6 + 3.0f, height, 2.0f);
    }
}