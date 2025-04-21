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
#define PIT_MIN_X (219)
#define PIT_MIN_Y (122)
#define PIT_MAX_X (615)
#define PIT_MAX_Y (518)

#define PIT_CORNER_TL juce::Rectangle<float>(PIT_MIN_X, PIT_MIN_Y, 9, 9)
#define PIT_CORNER_TR juce::Rectangle<float>(PIT_MAX_X, PIT_MIN_Y, 9, 9)
#define PIT_CORNER_BL juce::Rectangle<float>(PIT_MIN_X, PIT_MAX_Y, 9, 9)
#define PIT_CORNER_BR juce::Rectangle<float>(PIT_MAX_X, PIT_MAX_Y, 9, 9)

#define BALLS_TABS_BOUNDS juce::Rectangle<int>(120, 525, 50, 100)

#define BALL_X_KNOB_BOUNDS juce::Rectangle<int>(235, 570, 65, 65)
#define BALL_Y_KNOB_BOUNDS juce::Rectangle<int>(340, 570, 65, 65)
#define BALL_DIRECTION_KNOB_BOUNDS juce::Rectangle<int>(470, 570, 65, 65)
#define BALL_SPEED_KNOB_BOUNDS juce::Rectangle<int>(600, 570, 65, 65)
#define BALL_SIZE_KNOB_BOUNDS juce::Rectangle<int>(720, 570, 65, 65)
#define BALL_X_VELOCITY_KNOB_BOUNDS juce::Rectangle<int>(550, 570, 65, 65)
#define BALL_Y_VELOCITY_KNOB_BOUNDS juce::Rectangle<int>(650, 570, 65, 65)

#define EDGE_PHASE_COMBOBOX_BOUNDS juce::Rectangle<int>(110, 205, 75, 25)
#define EDGE_DENOMENATOR_COMBOBOX_BOUNDS juce::Rectangle<int>(146, 275 , 40, 25)
#define EDGE_TYPE_COMBOBOX_BOUNDS juce::Rectangle<int>(90, 355, 75, 25)

#define ROOT_NOTE_COMBOBOX_BOUNDS juce::Rectangle<int>(720, 205, 75, 25)
#define SCALE_COMBOBOX_BOUNDS juce::Rectangle<int>(720, 275, 75, 25)
#define RANGE_COMBOBOX_BOUNDS juce::Rectangle<int>(760, 355, 75, 25)

#define EDGE_LAYOUT_DICE_BOUNDS juce::Rectangle<int>(105, 425, 40, 40)
#define SCALE_PARAMS_DICE_BOUNDS juce::Rectangle<int>(695, 425, 40 ,40)

#define START_STOP_BUTTON_BOUNDS juce::Rectangle<int>(50, 80, 90, 25)
#define ADD_REMOVE_BUTTON_BOUNDS juce::Rectangle<int>(120, 580, 50, 25)
#define BALLS_POSITIONING_TYPE_BUTTON juce::Rectangle<int>(15, 530, 90, 25)
#define SNAP_TO_GRID_BUTTON juce::Rectangle<int>(15, 570, 90, 25)
#define COLLISION_BUTTON juce::Rectangle<int>(15, 610, 90, 25)
#define PRESET_MANAGER_MENU_BUTTON juce::Rectangle<int>(625, 80, 90, 25)

#define EDGE_RANDON_DICE_MIN_X 105
#define EDGE_RANDON_DICE_MIN_Y 427
#define EDGE_RANDON_DICE_MAX_X 145
#define EDGE_RANDON_DICE_MAX_Y 467