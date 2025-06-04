cmake_minimum_required(VERSION 3.16)

project(BallPit VERSION 1.0.0)

# First, we'll add the CMake folder, incase we'll need to find_package later:
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

# Compile commands, useful for some IDEs like VS-Code
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Minimum MacOS target, set globally
if (${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
    set(CMAKE_OSX_DEPLOYMENT_TARGET 11.0 CACHE STRING "Minimum OS X deployment version" FORCE)

    #code signing to run on an iOS device:
    #    set(CMAKE_XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY "iPhone Developer" CACHE STRING "" FORCE)
    #    set(CMAKE_XCODE_ATTRIBUTE_DEVELOPMENT_TEAM "XXXXXXXXXX" CACHE STRING "" FORCE)
else ()
    set(CMAKE_OSX_DEPLOYMENT_TARGET "10.11" CACHE STRING "Minimum OS X deployment version" FORCE)
endif ()

option(UniversalBinary "Build universal binary for mac" OFF)

if (UniversalBinary)
    set(CMAKE_OSX_ARCHITECTURES "x86_64;arm64" CACHE INTERNAL "")
endif ()

# Static linking in Windows
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# Set this to the path where JUCE was cloned by GitHub Actions or locally
set(JUCE_DIR "${CMAKE_SOURCE_DIR}/JUCE")

# Add JUCE to the project directly (this is the standard way for JUCE projects)
add_subdirectory(${JUCE_DIR} JUCE)

# Enable JUCE modules
set(CMAKE_CXX_STANDARD 20)

juce_add_plugin(BallPit
    COMPANY_NAME "YourCompany"
    IS_SYNTH TRUE
    IS_MIDI_EFFECT TRUE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD TRUE
    PLUGIN_MANUFACTURER_CODE Juce
    PLUGIN_CODE Bpit
    FORMATS VST3
    PRODUCT_NAME "BallPit"
)

juce_generate_juce_header(BallPit)

target_sources(BallPit PRIVATE
    Source/Ball.cpp
    Source/Ball.h
    Source/Configs.h
    Source/CostumeComboBox.cpp
    Source/CostumeComboBox.h
    Source/CostumeDial.cpp
    Source/CostumeDial.h
    Source/CostumeLogger.h
    Source/CostumeToggleButton.cpp
    Source/CostumeToggleButton.h
    Source/Edge.cpp
    Source/Edge.h
    Source/EdgeEventListener.h
    Source/Pit.cpp
    Source/Pit.h
    Source/PluginEditor.cpp
    Source/PluginEditor.h
    Source/PluginProcessor.cpp
    Source/PluginProcessor.h
    Source/PresetManagerBackend.cpp
    Source/PresetManagerBackend.h
    Source/PresetManagerGUI.h
    Source/Scales.cpp
    Source/Scales.h
)

# Add resources
juce_add_binary_data(ResourcesData SOURCES
    Resources/Ball_tab_0.svg
    Resources/Ball_tab_1.svg
    Resources/Ball_tab_2.svg
    Resources/ByKnobs.svg
    Resources/ByTempo.svg
)

target_link_libraries(BallPit PRIVATE
    juce::juce_audio_utils
    juce::juce_dsp
    juce::juce_gui_extra
    juce::juce_audio_plugin_client
    ResourcesData
)

# Fix VST3 parameter automation conflict
target_compile_definitions(BallPit PRIVATE
    JUCE_VST3_CAN_REPLACE_VST2=0
)