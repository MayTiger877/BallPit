cmake_minimum_required(VERSION 3.16)

project(BallPit VERSION 1.0.0)

# Enable JUCE modules
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# First, we'll add the CMake folder, incase we'll need to find_package later:
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/CMake")

# Compile commands, useful for some IDEs like VS-Code
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# Minimum MacOS target, set globally
if (${CMAKE_SYSTEM_NAME} STREQUAL "iOS")
    set(CMAKE_OSX_DEPLOYMENT_TARGET 11.0 CACHE STRING "Minimum OS X deployment version" FORCE)
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
# set(JUCE_DIR "${CMAKE_SOURCE_DIR}/../JUCE") # my pc
# set(JUCE_DIR "${CMAKE_SOURCE_DIR}/JUCE") # CI pipeline
if(EXISTS "${CMAKE_SOURCE_DIR}/../JUCE")
    set(JUCE_DIR "${CMAKE_SOURCE_DIR}/../JUCE")
    message(STATUS "Using JUCE from parent directory")
elseif(EXISTS "${CMAKE_SOURCE_DIR}/JUCE")
    set(JUCE_DIR "${CMAKE_SOURCE_DIR}/JUCE")
    message(STATUS "Using JUCE from current directory")
else()
    message(FATAL_ERROR "JUCE directory not found in either location")
endif()

# Add JUCE to the project directly (this is the standard way for JUCE projects)
add_subdirectory(${JUCE_DIR} JUCE)

juce_add_plugin(BallPit
    COMPANY_NAME "MayTigerPlugins"
    IS_SYNTH TRUE
    IS_MIDI_EFFECT TRUE
    NEEDS_MIDI_INPUT FALSE
    NEEDS_MIDI_OUTPUT TRUE
    EDITOR_WANTS_KEYBOARD_FOCUS TRUE
    COPY_PLUGIN_AFTER_BUILD FALSE
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
    Source/Configs.cpp
    Source/CostumeComboBox.cpp
    Source/CostumeComboBox.h
    Source/CostumeDelaySliders.cpp
    Source/CostumeDelaySliders.h
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

juce_add_binary_data(ResourcesData SOURCES
    Resources/Ball_tab_0.svg
    Resources/Ball_tab_1.svg
    Resources/Ball_tab_2.svg
    Resources/ByKnobs.svg
    Resources/ByTempo.svg
)

# Get all preset files automatically
file(GLOB PRESET_FILES "Presets/*.preset")
juce_add_binary_data(PresetsData SOURCES ${PRESET_FILES})

target_link_libraries(BallPit PRIVATE
    juce::juce_audio_basics
    juce::juce_audio_devices
    juce::juce_audio_formats
    juce::juce_audio_plugin_client
    juce::juce_audio_processors
    juce::juce_core
    juce::juce_data_structures
    juce::juce_events
    juce::juce_graphics
    juce::juce_gui_basics
    juce::juce_gui_extra
    ResourcesData
)

# Fix VST3 parameter automation conflict
target_compile_definitions(BallPit PRIVATE
    JUCE_VST3_CAN_REPLACE_VST2=0
)

# Safe clean target that doesn't remove the directory it's running from
add_custom_target(clean-all
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/BallPit_artefacts
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/CMakeFiles
    COMMAND ${CMAKE_COMMAND} -E remove_directory ${CMAKE_BINARY_DIR}/_deps
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/CMakeCache.txt
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/cmake_install.cmake
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/*.vcxproj
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/*.vcxproj.filters
    COMMAND ${CMAKE_COMMAND} -E remove ${CMAKE_BINARY_DIR}/*.sln
    COMMENT "Cleaning all build artifacts and CMake generated files"
)

# cmake -B build -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Release
# cmake --build build --config Release