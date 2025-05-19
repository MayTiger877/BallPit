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

#define VISUAL_FRAMES_PER_SECOND 50.0

#define SECONDS_IN_MINUTE 60.0

#define PI 3.14159265

//-----------------------------------------------------------------------
// defaults values for the BallPit multiverse


#define DEFAULT_BPM 120.0

#define DEFAULT_TIME_SIGNATURE_NUMERATOR 4

#define DEFAULT_TIME_SIGNATURE_DENOMINATOR 4

#define DEFAULT_BALL_RADIUS 10.0

#define DEFAULT_BALL_VELOCITY 5.0

#define NOTE_MIDI_DURATION 0.250 //ms

#define BUTTON_BG_COLOUR juce::Colour::fromRGB(25, 78, 113).darker(0.1f)

//-----------------------------------------------------------------------
// Sizes and positions map

#define APP_WINDOW_WIDTH (836)
#define APP_WINDOW_HIGHT (654)

#define PIT_WIDTH 396.0
#define PIT_SIZE (PIT_WIDTH,PIT_WIDTH)
#define PIT_MIN_X 220.0
#define PIT_MIN_Y 60.0
#define PIT_MAX_X 616.0
#define PIT_MAX_Y 456.0

#define PIT_INNER_DIFF 3.0

#define PIT_EDGE_THICKNESS 2
#define PIT_CORNER_THICKNESS 12

#define PIT_CORNER_TL juce::Rectangle<float>(PIT_MIN_X - (PIT_CORNER_THICKNESS / 4), PIT_MIN_Y - (PIT_CORNER_THICKNESS / 4), PIT_CORNER_THICKNESS, PIT_CORNER_THICKNESS)
#define PIT_CORNER_TR juce::Rectangle<float>(PIT_MAX_X - (3 * PIT_CORNER_THICKNESS / 4), PIT_MIN_Y - (PIT_CORNER_THICKNESS / 4), PIT_CORNER_THICKNESS, PIT_CORNER_THICKNESS)
#define PIT_CORNER_BL juce::Rectangle<float>(PIT_MIN_X - (PIT_CORNER_THICKNESS / 4), PIT_MAX_Y - (3 * PIT_CORNER_THICKNESS / 4), PIT_CORNER_THICKNESS, PIT_CORNER_THICKNESS)
#define PIT_CORNER_BR juce::Rectangle<float>(PIT_MAX_X - (3 * PIT_CORNER_THICKNESS / 4), PIT_MAX_Y - (3 * PIT_CORNER_THICKNESS / 4), PIT_CORNER_THICKNESS, PIT_CORNER_THICKNESS)

#define BALLS_TABS_BOUNDS juce::Rectangle<int>(417, 491, 413, 157)
#define BALLS_TAB_0_BOUNDS juce::Rectangle<int>(BALLS_TABS_BOUNDS.getX() + 3, BALLS_TABS_BOUNDS.getY() + 5, 68, 43)
#define BALLS_TAB_1_BOUNDS juce::Rectangle<int>(BALLS_TABS_BOUNDS.getX() + 3, BALLS_TABS_BOUNDS.getY() + 55, 68, 43)
#define BALLS_TAB_2_BOUNDS juce::Rectangle<int>(BALLS_TABS_BOUNDS.getX() + 3, BALLS_TABS_BOUNDS.getY() + 105, 68, 43)

#define BALL_X_KNOB_BOUNDS juce::Rectangle<int>(635, 522, 55, 55)
#define BALL_Y_KNOB_BOUNDS juce::Rectangle<int>(725, 522, 55, 55)
#define ADD_REMOVE_BUTTON_BOUNDS juce::Rectangle<int>(505, 512, 80, 25)
#define BALL_SPEED_KNOB_BOUNDS juce::Rectangle<int>(520, 595, 55, 55)
#define BALL_DIRECTION_KNOB_BOUNDS juce::Rectangle<int>(635, 595, 55, 55)
#define BALL_SIZE_KNOB_BOUNDS juce::Rectangle<int>(725, 595, 55, 55)
#define BALL_X_VELOCITY_KNOB_BOUNDS juce::Rectangle<int>(490, 595, 55, 55)
#define BALL_Y_VELOCITY_KNOB_BOUNDS juce::Rectangle<int>(600, 595, 55, 55)

#define EDGE_PHASE_SLIDER_BOUNDS juce::Rectangle<int>(40, 130, 100, 25)
#define EDGE_DENOMENATOR_SLIDER_BOUNDS juce::Rectangle<int>(40, 165 , 150, 50)
#define EDGE_TYPE_COMBOBOX_BOUNDS juce::Rectangle<int>(40, 240, 100, 20)

#define SCALE_COMBOBOX_BOUNDS juce::Rectangle<int>(40, 340, 100, 20)
#define RANGE_SLIDER_BOUNDS     juce::Rectangle<int>(40, 375, 150, 50)
#define ROOT_NOTE_COMBOBOX_BOUNDS     juce::Rectangle<int>(40, 450, 100, 20)

#define EDGE_LAYOUT_DICE_BOUNDS juce::Rectangle<int>(182, 68, 20, 20)
#define EDGE_RANDOM_DICE_MIN_X EDGE_LAYOUT_DICE_BOUNDS.getX()
#define EDGE_RANDOM_DICE_MIN_Y EDGE_LAYOUT_DICE_BOUNDS.getY()
#define EDGE_RANDOM_DICE_MAX_X (EDGE_LAYOUT_DICE_BOUNDS.getX() + EDGE_LAYOUT_DICE_BOUNDS.getWidth())
#define EDGE_RANDOM_DICE_MAX_Y (EDGE_LAYOUT_DICE_BOUNDS.getY() + EDGE_LAYOUT_DICE_BOUNDS.getHeight())

#define SCALE_PARAMS_DICE_BOUNDS juce::Rectangle<int>(182, 278, 20 ,20)
#define SCALE_RANDOM_DICE_MIN_X SCALE_PARAMS_DICE_BOUNDS.getX()
#define SCALE_RANDOM_DICE_MIN_Y SCALE_PARAMS_DICE_BOUNDS.getY()
#define SCALE_RANDOM_DICE_MAX_X (SCALE_PARAMS_DICE_BOUNDS.getX() + SCALE_PARAMS_DICE_BOUNDS.getWidth())
#define SCALE_RANDOM_DICE_MAX_Y (SCALE_PARAMS_DICE_BOUNDS.getY() + SCALE_PARAMS_DICE_BOUNDS.getHeight())

#define START_STOP_BUTTON_BOUNDS juce::Rectangle<int>(43, 13, 100, 25)
#define PRESET_MANAGER_MENU_BUTTON_BOUNDS juce::Rectangle<int>(675, 13, 100, 25)

#define QUANTIZATION_DIVISION_COMBOBOX_BOUNDS juce::Rectangle<int>(15, 535, 60, 20)
#define QUANTIZATION_KNOB_BOUNDS juce::Rectangle<int>(90, 522, 55, 55)

#define VOLUME_VARIATION_KNOB_BOUNDS juce::Rectangle<int>(170, 510, 55, 55)
#define SNAP_TO_GRID_BUTTON_BOUNDS juce::Rectangle<int>(290, 525, 110, 35)
#define COLLISION_BUTTON_BOUNDS juce::Rectangle<int>(40, 600, 90, 25)
#define BALLS_POSITIONING_TYPE_COMBOBOX_BOUNDS juce::Rectangle<int>(190, 600, 90, 20)


//-----------------------------------------------------------------------
// Ball Position Values

// X Slider
#define BALL_X_SLIDER_MIN PIT_MIN_X   // Already defined elsewhere
#define BALL_X_SLIDER_MAX PIT_MAX_X   // Already defined elsewhere
#define BALL_X_SLIDER_STEP 10.0f
#define BALL_X_DEFAULT_1 (PIT_MIN_X + 50.0f)
#define BALL_X_DEFAULT_2 (PIT_MIN_X + 50.0f + 130.0f)
#define BALL_X_DEFAULT_3 (PIT_MIN_X + 50.0f + 260.0f)

// Y Slider
#define BALL_Y_SLIDER_MIN PIT_MIN_Y   // Already defined elsewhere
#define BALL_Y_SLIDER_MAX PIT_MAX_Y   // Already defined elsewhere
#define BALL_Y_SLIDER_STEP 10.0f
#define BALL_Y_DEFAULT 200.0f

//-----------------------------------------------------------------------
// Ball Direction Values

#define BALL_ANGLE_MIN 0.0f
#define BALL_ANGLE_MAX 360.0f
#define BALL_ANGLE_STEP 1.0f
#define BALL_ANGLE_DEFAULT_1 0.0f
#define BALL_ANGLE_DEFAULT_2 60.0f
#define BALL_ANGLE_DEFAULT_3 120.0f
#define BALL_ANGLE_DOUBLE_CLICK_VALUE 0.0f

//-----------------------------------------------------------------------
// Ball Speed Values

#define BALL_VELOCITY_MIN 0.0f
#define BALL_VELOCITY_MAX 2000.0f
#define BALL_VELOCITY_STEP 20.0f
#define BALL_VELOCITY_DEFAULT 400.0f
#define BALL_VELOCITY_DOUBLE_CLICK_VALUE 400.0f

//-----------------------------------------------------------------------
// Ball Size Values

#define BALL_RADIUS_MIN 10.0f
#define BALL_RADIUS_MAX 25.0f
#define BALL_RADIUS_STEP 0.5f
#define BALL_RADIUS_DEFAULT 10.0f
#define BALL_RADIUS_DOUBLE_CLICK_VALUE 10.0f

//-----------------------------------------------------------------------
// Velocity Components

#define BALL_X_VELOCITY_MIN 0.0f
#define BALL_X_VELOCITY_MAX 10.0f
#define BALL_X_VELOCITY_STEP 1.0f
#define BALL_X_VELOCITY_DEFAULT 1.0f
#define BALL_X_VELOCITY_DOUBLE_CLICK_VALUE 1.0f

#define BALL_Y_VELOCITY_MIN 0.0f
#define BALL_Y_VELOCITY_MAX 10.0f
#define BALL_Y_VELOCITY_STEP 1.0f
#define BALL_Y_VELOCITY_DEFAULT 1.0f
#define BALL_Y_VELOCITY_DOUBLE_CLICK_VALUE 1.0f

//-----------------------------------------------------------------------
// Edge Phase Values

#define EDGE_PHASE_MIN 0
#define EDGE_PHASE_MAX 360
#define EDGE_PHASE_STEP 5
#define EDGE_PHASE_DEFAULT 0

//-----------------------------------------------------------------------
// Edge Denominator Values

#define EDGE_DENOMINATOR_MIN 1
#define EDGE_DENOMINATOR_MAX 8
#define EDGE_DENOMINATOR_STEP 1
#define EDGE_DENOMINATOR_DEFAULT 1

//-----------------------------------------------------------------------
// Range Values

#define EDGE_RANGE_MIN 1
#define EDGE_RANGE_MAX 8
#define EDGE_RANGE_STEP 1
#define EDGE_RANGE_DEFAULT 2

//-----------------------------------------------------------------------
// Quantization Values

#define QUANTIZATION_MIN 0.0
#define QUANTIZATION_MAX 1.0
#define QUANTIZATION_STEP 0.01
#define QUANTIZATION_DEFAULT 0.0
#define QUANTIZATION_DOUBLE_CLICK_VALUE 0.5f

//-----------------------------------------------------------------------
// Scale IDs

#define SCALE_CHROMATIC 1
#define SCALE_OCTATONIC 2
#define SCALE_DOMINANT_DIMINISHED 3
#define SCALE_DIMINISHED 4
#define SCALE_MAJOR 5
#define SCALE_MINOR 6
#define SCALE_MELODIC_MINOR 7
#define SCALE_HARMONIC_MINOR 8
#define SCALE_GYPSY 9
#define SCALE_SYMMETRICAL 10
#define SCALE_ENIGMATIC 11
#define SCALE_ARABIAN 12
#define SCALE_HUNGARIAN 13
#define SCALE_WHOLE_TONE 14
#define SCALE_AUGMENTED 15
#define SCALE_BLUES_MAJOR 16
#define SCALE_BLUES_MINOR 17
#define SCALE_PENTATONIC 18
#define SCALE_MINOR_PENTATONIC 19
#define SCALE_DEFAULT SCALE_MAJOR

//-----------------------------------------------------------------------
// Root Note IDs

#define ROOT_NOTE_C 1
#define ROOT_NOTE_CSHARP 2
#define ROOT_NOTE_D 3
#define ROOT_NOTE_DSHARP 4
#define ROOT_NOTE_E 5
#define ROOT_NOTE_F 6
#define ROOT_NOTE_FSHARP 7
#define ROOT_NOTE_G 8
#define ROOT_NOTE_GSHARP 9
#define ROOT_NOTE_A 10
#define ROOT_NOTE_ASHARP 11
#define ROOT_NOTE_B 12
#define ROOT_NOTE_DEFAULT ROOT_NOTE_C

//-----------------------------------------------------------------------
// Edge Type IDs

#define EDGE_TYPE_CYCLIC_UP 1
#define EDGE_TYPE_CYCLIC_DOWN 2
#define EDGE_TYPE_PING_PONG 3
#define EDGE_TYPE_RANDOM 4
#define EDGE_TYPE_DEFAULT EDGE_TYPE_CYCLIC_UP

//-----------------------------------------------------------------------
// Ball Positioning Type IDs

#define BALLS_POSITIONING_CHAOS 1
#define BALLS_POSITIONING_BY_TEMPO 2
#define BALLS_POSITIONING_DEFAULT BALLS_POSITIONING_CHAOS

//-----------------------------------------------------------------------
// Quantization Division IDs

#define QUANTIZATION_DIV_1_32 32
#define QUANTIZATION_DIV_1_16 16
#define QUANTIZATION_DIV_1_8 8
#define QUANTIZATION_DIV_1_4 4
#define QUANTIZATION_DIV_DEFAULT QUANTIZATION_DIV_1_32