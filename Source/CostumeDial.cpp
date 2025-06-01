/*
  ==============================================================================

    MyCostumeDial.cpp
    Created: 15 May 2025 11:23:04am
    Author:  MayTigerStation

  ==============================================================================
*/

#include "CostumeDial.h"

MyCostumeDial::MyCostumeDial()
{
}

void MyCostumeDial::drawRotarySlider(Graphics& g, int x, int y, int width, int height, float sliderPos,
    const float rotaryStartAngle, const float rotaryEndAngle, Slider& slider)
{
    if (slider.getSliderStyle() == Slider::SliderStyle::MayT_PhaseKnob)
    {
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
        g.setFont(20.0f);
        g.setColour(BUTTON_TEXT_COLOUR);
        g.drawText(displayPhase, x + width - 55, y + height - 28, 50, 25, juce::Justification::left);
    }
    else if (slider.getSliderStyle() == Slider::SliderStyle::MayT_DirectionKnob)
    {
        auto bounds = Rectangle<int>(x, y, width, height).toFloat().reduced(10);
        float radius = jmin(bounds.getWidth(), bounds.getHeight()) / 2.0f;
        float angle = rotaryStartAngle + sliderPos * (rotaryEndAngle - rotaryStartAngle);
        float rx = bounds.getCentreX() - radius;
        float ry = bounds.getCentreY() - radius;

        g.setColour(MyrotarySliderFillColourId.darker(0.7f));
        g.fillEllipse(bounds.reduced(1.0f));
        g.setColour(juce::Colours::black);
        g.drawEllipse(bounds.expanded(0.1f), 1.0f);

        juce::Path line;
        g.setColour(juce::Colours::red.brighter(0.7f));
        line.addRectangle(0, -radius + 6, 2.0f, radius * 0.2);
        g.fillPath(line, juce::AffineTransform::rotation(angle).translated(bounds.getCentreX(), bounds.getCentreY()));
    }
    else if (slider.getSliderStyle() == Slider::SliderStyle::MayT_TempoKnob)
    {
        juce::String chosenSpeedText;
        int chosenSpeed = 10 * sliderPos;
        switch (chosenSpeed)
        {
        case 0: { chosenSpeedText = "---"; break; }
        case 1: {chosenSpeedText = "4"; break;}
        case 2: {chosenSpeedText = "2"; break;}
        case 3: {chosenSpeedText = "5/4"; break;}
        case 4: {chosenSpeedText = "1"; break; }
        case 5: {chosenSpeedText = "3/4"; break;}
        case 6: {chosenSpeedText = "1/2"; break;}
        case 7: {chosenSpeedText = "1/3"; break;}
        case 8: {chosenSpeedText = "1/4"; break;}
        case 9: {chosenSpeedText = "1/6"; break;}
        case 10: { chosenSpeedText = "1/8"; break; }
        default: chosenSpeedText = "4";
        }

        auto bounds = Rectangle<int>(x, y, width-30, height).toFloat().reduced(10);
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

        g.setFont(18.0f);
        g.setColour(BUTTON_TEXT_COLOUR);
        g.drawText(chosenSpeedText, bounds.getWidth() + 20, 0, width, height, juce::Justification::centredLeft);
    }
    else if (slider.getSliderStyle() == Slider::SliderStyle::MayT_VelocityKnob)
    {
        g.setColour(BUTTON_BG_COLOUR);
        g.fillRect(0, 0, width, height);

        g.setColour(BUTTON_BG_COLOUR.darker(0.5f));
        g.fillRect(0, 0, (int)(width * sliderPos), height);
        
        g.setColour(BUTTON_BG_COLOUR.brighter(0.7f));
        for (int i = 1; i < 10; i++)
        {
            float lineLength = 3.0f;
            if ((i % 2) == 0) 
            {
                lineLength = 5.0f;
            }
            g.drawLine(i * (width / 10.0f), height - lineLength, i * (width / 10.0f), height, 1.0f);
        }

        long diagonal = sqrt(pow(width, 2) * pow(height, 2));
        g.setColour(MAIN_BG_COLOUR);
        g.fillEllipse((-2.3 * width), (-1.4 * height), 3.3 * width, 2.5 * height);

        juce::String displayVelocity = std::to_string(static_cast<int>(std::round(sliderPos * BALL_VELOCITY_MAX)));
        g.setFont(20.0f);
        g.setColour(BUTTON_TEXT_COLOUR);
        g.drawText(displayVelocity, 0, 0, 50, 25, juce::Justification::left);
    }
    else if (slider.getSliderStyle() == Slider::SliderStyle::MayT_VariationKnob)
    {
        float lineBegin = x + 5;
        float lineY = y + 15;

        g.setColour(BUTTON_BG_COLOUR.darker(0.2f));
        g.drawLine(lineBegin, lineY,  x + width - 5, lineY, 8.0f);
        for (int i = 0; i < 6; i++)
        {
            g.setColour(BUTTON_BG_COLOUR.darker(0.2f));
            g.drawLine(lineBegin + i * (width - 10) / 5, y + 4, lineBegin + i * (width - 10) / 5, y + 26, 3.0f);
            g.setColour(BUTTON_BG_COLOUR.darker(0.7f));
            g.drawLine(lineBegin + i * (width - 10) / 5, y + 4, lineBegin + i * (width - 10) / 5, y + 26, 1.0f);
        }
        g.setColour(BUTTON_BG_COLOUR.darker(0.4f));
        g.drawLine(lineBegin, lineY, x + width - 5, lineY, 4.0f);
        g.setColour(BUTTON_BG_COLOUR.darker(0.7f));
        g.drawLine(lineBegin, lineY, x + width - 5, lineY, 2.0f);

        g.setColour(BUTTON_BG_COLOUR.brighter(0.2f));
        float barLength = sliderPos * (x + width - 5 - lineBegin);
        float lineMiddle = (lineBegin + x + width - 5) / 2;
        g.drawLine(lineMiddle - barLength / 2 , lineY, lineMiddle + barLength / 2, lineY, 6.0f);

        g.drawText("-", x + 2, y + height - 35, 20, 25, juce::Justification::left);
        g.drawText("+", x + width - 10, y + height - 35, 20, 25, juce::Justification::left);
    }
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