/*
  ==============================================================================

    CostumeToggleButton.h
    Created: 21 May 2025 4:06:24pm
    Author:  MayTigerStation

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>
#include "Configs.h"
using namespace juce;

class CostumeToggleButoonLAF : public LookAndFeel_V4
{
public:
    CostumeToggleButoonLAF();

    void drawToggleButton(Graphics&, ToggleButton&,
        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
};
