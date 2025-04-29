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
#define PIT_MIN_X 220.0
#define PIT_MIN_Y 60.0
#define PIT_MAX_X 616.0
#define PIT_MAX_Y 456.0

#define PIT_EDGE_THICKNESS 2
#define PIT_CORNER_THICKNESS 12

#define PIT_CORNER_TL juce::Rectangle<float>(PIT_MIN_X - (PIT_CORNER_THICKNESS / 4), PIT_MIN_Y - (PIT_CORNER_THICKNESS / 4), PIT_CORNER_THICKNESS, PIT_CORNER_THICKNESS)
#define PIT_CORNER_TR juce::Rectangle<float>(PIT_MAX_X - (3 * PIT_CORNER_THICKNESS / 4), PIT_MIN_Y - (PIT_CORNER_THICKNESS / 4), PIT_CORNER_THICKNESS, PIT_CORNER_THICKNESS)
#define PIT_CORNER_BL juce::Rectangle<float>(PIT_MIN_X - (PIT_CORNER_THICKNESS / 4), PIT_MAX_Y - (3 * PIT_CORNER_THICKNESS / 4), PIT_CORNER_THICKNESS, PIT_CORNER_THICKNESS)
#define PIT_CORNER_BR juce::Rectangle<float>(PIT_MAX_X - (3 * PIT_CORNER_THICKNESS / 4), PIT_MAX_Y - (3 * PIT_CORNER_THICKNESS / 4), PIT_CORNER_THICKNESS, PIT_CORNER_THICKNESS)

#define BALLS_TABS_BOUNDS juce::Rectangle<int>(415, 490, 50, 150)

#define BALL_X_KNOB_BOUNDS juce::Rectangle<int>(600, 522, 55, 55)
#define BALL_Y_KNOB_BOUNDS juce::Rectangle<int>(710, 522, 55, 55)
#define ADD_REMOVE_BUTTON_BOUNDS juce::Rectangle<int>(485, 522, 80, 25)
#define BALL_SPEED_KNOB_BOUNDS juce::Rectangle<int>(485, 595, 55, 55)
#define BALL_SIZE_KNOB_BOUNDS juce::Rectangle<int>(600, 595, 55, 55)
#define BALL_DIRECTION_KNOB_BOUNDS juce::Rectangle<int>(710, 595, 55, 55)
#define BALL_X_VELOCITY_KNOB_BOUNDS juce::Rectangle<int>(550, 595, 55, 55)
#define BALL_Y_VELOCITY_KNOB_BOUNDS juce::Rectangle<int>(650, 595, 55, 55)

#define EDGE_PHASE_COMBOBOX_BOUNDS juce::Rectangle<int>(40, 130, 100, 25)
#define EDGE_DENOMENATOR_COMBOBOX_BOUNDS juce::Rectangle<int>(40, 185 , 100, 25)
#define EDGE_TYPE_COMBOBOX_BOUNDS juce::Rectangle<int>(40, 240, 100, 25)

#define ROOT_NOTE_COMBOBOX_BOUNDS juce::Rectangle<int>(40, 340, 100, 25)
#define SCALE_COMBOBOX_BOUNDS juce::Rectangle<int>(40, 395, 100, 25)
#define RANGE_COMBOBOX_BOUNDS juce::Rectangle<int>(40, 450, 100, 25)

#define EDGE_LAYOUT_DICE_BOUNDS juce::Rectangle<int>(182, 68, 20, 20)
#define EDGE_RANDOM_DICE_MIN_X EDGE_LAYOUT_DICE_BOUNDS.getX()
#define EDGE_RANDOM_DICE_MIN_Y EDGE_LAYOUT_DICE_BOUNDS.getY()
#define EDGE_RANDOM_DICE_MAX_X (EDGE_LAYOUT_DICE_BOUNDS.getX() + EDGE_LAYOUT_DICE_BOUNDS.getWidth())
#define EDGE_RANDOM_DICE_MAX_Y (EDGE_LAYOUT_DICE_BOUNDS.getY() + EDGE_LAYOUT_DICE_BOUNDS.getHeight())

#define SCALE_PARAMS_DICE_BOUNDS juce::Rectangle<int>(698, 365, 20 ,20)
// TODO- add this dice

#define START_STOP_BUTTON_BOUNDS juce::Rectangle<int>(43, 13, 100, 25)
#define PRESET_MANAGER_MENU_BUTTON_BOUNDS juce::Rectangle<int>(675, 13, 100, 25)

#define QUANTIZATION_DIVISION_COMBOBOX_BOUNDS juce::Rectangle<int>(15, 535, 75, 25)
#define QUANTIZATION_KNOB_BOUNDS juce::Rectangle<int>(100, 522, 55, 55)

#define VOLUME_VARIATION_KNOB_BOUNDS juce::Rectangle<int>(170, 510, 55, 55)
#define SNAP_TO_GRID_BUTTON_BOUNDS juce::Rectangle<int>(300, 530, 90, 25)
#define COLLISION_BUTTON_BOUNDS juce::Rectangle<int>(40, 600, 90, 25)
#define BALLS_POSITIONING_TYPE_BUTTON_BOUNDS juce::Rectangle<int>(185, 600, 90, 25)

//-----------------------------------------------------------------------
// Sliders values

#define BALL_X_SLIDER_MIN PIT_MIN_X
#define BALL_X_SLIDER_MAX PIT_MAX_X

#define BALL_Y_SLIDER_MIN PIT_MIN_Y
#define BALL_Y_SLIDER_MAX PIT_MAX_Y
