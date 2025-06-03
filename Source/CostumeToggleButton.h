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

class MyCostumeSnapToGridLAF : public LookAndFeel_V4
{
public:
    MyCostumeSnapToGridLAF();

    void drawToggleButton(Graphics&, ToggleButton&,
        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
};

// ------------------------------------------------------------------------------------------

class MyCostumeCollisionLAF : public LookAndFeel_V4
{
public:
    MyCostumeCollisionLAF();

    void drawToggleButton(Graphics&, ToggleButton&,
        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
};

class MyCostumeVelocityInverterLAF : public LookAndFeel_V4
{
public:
    MyCostumeVelocityInverterLAF();

    void drawToggleButton(Graphics&, ToggleButton&,
        bool shouldDrawButtonAsHighlighted, bool shouldDrawButtonAsDown) override;
};