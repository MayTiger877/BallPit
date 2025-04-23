/*
  ==============================================================================

    Configs.h
    Created: 25 Dec 2024 2:34:15pm
    Author:  MayTigerStation

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

//-----------------------------------------------------------------------
// defines and constants

#define VISUAL_FRAMES_PER_SECOND 60.0

#define SECONDS_IN_MINUTE 60.0


//-----------------------------------------------------------------------
// defaults values for the BallPit multiverse


#define DEFAULT_BPM 120.0

#define DEFAULT_TIME_SIGNATURE_NUMERATOR 4

#define DEFAULT_TIME_SIGNATURE_DENOMINATOR 4

#define DEFAULT_BALL_RADIUS 10.0

#define DEFAULT_BALL_VELOCITY 5.0


//-----------------------------------------------------------------------
// Sizes and positions map

#define APP_WINDOW_WIDTH (836)
#define APP_WINDOW_HIGHT (654)

#define PIT_SIZE (396,396)
#define PIT_MIN_X (220)
#define PIT_MIN_Y (60)
#define PIT_MAX_X (616)
#define PIT_MAX_Y (456)

#define PIT_EDGE_THICKNESS 2
#define PIT_CORNER_THICKNESS 12

#define PIT_CORNER_TL juce::Rectangle<float>(PIT_MIN_X - (PIT_CORNER_THICKNESS / 4), PIT_MIN_Y - (PIT_CORNER_THICKNESS / 4), PIT_CORNER_THICKNESS, PIT_CORNER_THICKNESS)
#define PIT_CORNER_TR juce::Rectangle<float>(PIT_MAX_X - (3 * PIT_CORNER_THICKNESS / 4), PIT_MIN_Y - (PIT_CORNER_THICKNESS / 4), PIT_CORNER_THICKNESS, PIT_CORNER_THICKNESS)
#define PIT_CORNER_BL juce::Rectangle<float>(PIT_MIN_X - (PIT_CORNER_THICKNESS / 4), PIT_MAX_Y - (3 * PIT_CORNER_THICKNESS / 4), PIT_CORNER_THICKNESS, PIT_CORNER_THICKNESS)
#define PIT_CORNER_BR juce::Rectangle<float>(PIT_MAX_X - (3 * PIT_CORNER_THICKNESS / 4), PIT_MAX_Y - (3 * PIT_CORNER_THICKNESS / 4), PIT_CORNER_THICKNESS, PIT_CORNER_THICKNESS)

#define BALLS_TABS_BOUNDS juce::Rectangle<int>(120, 490, 50, 150)

#define BALL_X_KNOB_BOUNDS juce::Rectangle<int>(265, 525, 65, 65)
#define BALL_Y_KNOB_BOUNDS juce::Rectangle<int>(370, 525, 65, 65)
#define BALL_DIRECTION_KNOB_BOUNDS juce::Rectangle<int>(500, 525, 65, 65)
#define BALL_SPEED_KNOB_BOUNDS juce::Rectangle<int>(630, 525, 65, 65)
#define BALL_SIZE_KNOB_BOUNDS juce::Rectangle<int>(750, 525, 65, 65)
#define BALL_X_VELOCITY_KNOB_BOUNDS juce::Rectangle<int>(550, 525, 65, 65)
#define BALL_Y_VELOCITY_KNOB_BOUNDS juce::Rectangle<int>(650, 525, 65, 65)

#define EDGE_PHASE_COMBOBOX_BOUNDS juce::Rectangle<int>(50, 155, 100, 25)
#define EDGE_DENOMENATOR_COMBOBOX_BOUNDS juce::Rectangle<int>(50, 235 , 100, 25)
#define EDGE_TYPE_COMBOBOX_BOUNDS juce::Rectangle<int>(50, 315, 100, 25)

#define ROOT_NOTE_COMBOBOX_BOUNDS juce::Rectangle<int>(670, 155, 100, 25)
#define SCALE_COMBOBOX_BOUNDS juce::Rectangle<int>(670, 235, 100, 25)
#define RANGE_COMBOBOX_BOUNDS juce::Rectangle<int>(670, 315, 100, 25)

#define EDGE_LAYOUT_DICE_BOUNDS juce::Rectangle<int>(81, 365, 40, 40)
#define EDGE_RANDON_DICE_MIN_X EDGE_LAYOUT_DICE_BOUNDS.getX()
#define EDGE_RANDON_DICE_MIN_Y EDGE_LAYOUT_DICE_BOUNDS.getY()
#define EDGE_RANDON_DICE_MAX_X (EDGE_LAYOUT_DICE_BOUNDS.getX() + EDGE_LAYOUT_DICE_BOUNDS.getWidth())
#define EDGE_RANDON_DICE_MAX_Y (EDGE_LAYOUT_DICE_BOUNDS.getY() + EDGE_LAYOUT_DICE_BOUNDS.getHeight())

#define SCALE_PARAMS_DICE_BOUNDS juce::Rectangle<int>(698, 365, 40 ,40)

#define START_STOP_BUTTON_BOUNDS juce::Rectangle<int>(43, 13, 100, 25)
#define PRESET_MANAGER_MENU_BUTTON juce::Rectangle<int>(675, 13, 100, 25)
#define ADD_REMOVE_BUTTON_BOUNDS juce::Rectangle<int>(190, 600, 100, 25)
#define BALLS_POSITIONING_TYPE_BUTTON juce::Rectangle<int>(15, 530, 90, 25)
#define SNAP_TO_GRID_BUTTON juce::Rectangle<int>(15, 570, 90, 25)
#define COLLISION_BUTTON juce::Rectangle<int>(15, 610, 90, 25)
