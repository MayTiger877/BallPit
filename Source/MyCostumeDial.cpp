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

void MyCostumeDial::drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
    const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{
    if (slider.getSliderStyle() == Slider::SliderStyle::MayT_PhaseKnob)
    {
        /*
        float radius = jmin(knobBounds.getWidth(), knobBounds.getHeight()) / 2.0f;
        float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        float rx = knobBounds.getCentreX() - radius;
        float ry = knobBounds.getCentreY() - radius;
        g.setColour(MyrotarySliderFillColourId);
        g.fillEllipse(knobBounds);
        g.setColour(juce::Colours::black);
        g.fillEllipse(knobBounds.expanded(1.0f));
        g.setColour(MyrotarySliderFillColourId.darker(0.2f));
        g.fillEllipse(knobBounds.reduced(1.0f));
        g.setColour(MyrotarySliderFillColourId.darker(0.4f));
        g.fillEllipse(knobBounds.reduced(2.0f));
        g.setColour(MyrotarySliderFillColourId.darker(0.7f));
        g.fillEllipse(knobBounds.reduced(4.0f));

        juce::Path line;
        line.addRectangle(0, -radius + 6, 2.0f, radius * 0.8);
        g.fillPath(line, juce::AffineTransform::rotation(angle).translated(knobBounds.getCentreX(), knobBounds.getCentreY()));*/

        //auto knobBounds = Rectangle<int>(x, y + (height / 4.5), width / 3.5, width / 3.5).toFloat().reduced(10);
        juce::Path sinus;
        float sinusX = x + 10;
        float sinusY = y + (height * 0.4) + 5;
        sinus.startNewSubPath(sinusX, sinusY);
        float nextX, nextY, phase;
        
        g.setColour(MyrotarySliderFillColourId.darker());
        g.drawLine(sinusX, sinusY,
                   sinusX + (width - 15),
                   sinusY + std::sin(juce::MathConstants<float>::twoPi) * (height * 0.7 / 2.0f));

        for (int i = 1; i <= 100; ++i)
        {
            nextX = sinusX + ((width - 15)/ (float)100 * i);
            phase = (float)i / (float)100 * juce::MathConstants<float>::twoPi;
            nextY = sinusY + std::sin(phase) * (height * 0.7 / 2.0f);
            sinus.lineTo(nextX, nextY);
        }
        g.setColour(MyrotarySliderFillColourId);
        g.strokePath(sinus, { 2, PathStrokeType::curved, PathStrokeType::rounded });
        g.setColour(juce::Colours::green);
        phase = sliderPos * juce::MathConstants<float>::twoPi;
        g.fillEllipse(sinusX + ((width - 15) * sliderPos) - 3.5,
                      sinusY + std::sin(phase) * (height * 0.7 / 2.0f) - 3.5, 7, 7);

        juce::String displayPhase = std::to_string(static_cast<int>(std::round(sliderPos*360)));
        g.setColour(BUTTON_TEXT_COLOUR);
        g.drawText(displayPhase, x + width - 55, y + height - 28, 50, 25, juce::Justification::left);
    }
    //else if (slider.getSliderStyle() == Slider::SliderStyle::MayT_DirectionKnob)
    //{
    //    // TODO...
    //}
    else
    {
        auto bounds = Rectangle<int>(x, y, width, height).toFloat().reduced(10);
        float radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
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
    }
}


void MyCostumeDial::drawLinearSlider(Graphics& g, int x, int y, int width, int height,
                                     float sliderPos, float minSliderPos, float maxSliderPos,
                                     const Slider::SliderStyle style, Slider& slider)
{
    if (slider.isBar())
    {
        g.setColour(slider.findColour(Slider::trackColourId));
        g.fillRect(slider.isHorizontal() ? Rectangle<float>(static_cast<float> (x), (float)y + 0.5f, sliderPos - (float)x, (float)height - 1.0f)
            : Rectangle<float>((float)x + 0.5f, sliderPos, (float)width - 1.0f, (float)y + ((float)height - sliderPos)));

        drawLinearSliderOutline(g, x, y, width, height, style, slider);
    }
    else
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
        g.strokePath(backgroundTrack, {(0.2f * trackWidth), PathStrokeType::curved, PathStrokeType::rounded });

        Path scaleGraduation;
        float lengthOfScale = std::abs(startPoint.getX() - endPoint.getX());
        Point<float> scaleGraduationPlacement((startPoint.getX() - 1.0f), (startPoint.getY() - 3.0f));
        for (int i = 0; i < 8; i++)
        {
            scaleGraduationPlacement.setXY((startPoint.getX() - 1.0f + (i * (lengthOfScale / 7))), (startPoint.getY() - 2.0f));
            scaleGraduation.startNewSubPath(scaleGraduationPlacement);
            scaleGraduation.lineTo(scaleGraduationPlacement.getX(), scaleGraduationPlacement.getY() + 3.0f);
            g.setColour(juce::Colours::darkcyan);
            g.strokePath(scaleGraduation, { 1.0f, PathStrokeType::curved, PathStrokeType::rounded });
            g.setColour(BUTTON_TEXT_COLOUR.darker(0.1f));
            g.drawText(std::to_string(i + 1), scaleGraduationPlacement.getX() - 3.2f, scaleGraduationPlacement.getY() + 1.0f, 25.0, 25.0, Justification::bottomLeft);
        }

        auto kx = slider.isHorizontal() ? sliderPos : ((float)x + (float)width * 0.5f);
        auto ky = slider.isHorizontal() ? ((float)y + (float)height * 0.5f) : sliderPos;
        Point<float> maxPoint = { kx, ky };
        auto thumbWidth = getSliderThumbRadius(slider);

        g.setColour(slider.findColour(Slider::thumbColourId).darker(0.3f));
        g.fillEllipse(Rectangle<float>(static_cast<float> (thumbWidth), static_cast<float> (thumbWidth)).withCentre(maxPoint));
    }
}