/*
  ==============================================================================

    PresetManagerBackend.h
    Created: 25 Dec 2024 5:45:38pm
	Author:  Akash Murthy (TheAudioProgrammer channel)

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
using namespace juce;

namespace Service
{
	class PresetManager : juce::ValueTree::Listener
	{
	public:
		static const File defaultDirectory;
		static const String extension;
		static const String presetNameProperty;

		PresetManager(AudioProcessorValueTreeState&);

		void savePreset(const String& presetName);
		void deletePreset(const String& presetName);
		void loadPreset(const String& presetName);
		int loadNextPreset();
		int loadPreviousPreset();
		StringArray getAllPresets() const;
		String getCurrentPreset() const;
	private:
		void valueTreeRedirected(ValueTree& treeWhichHasBeenChanged) override;

		AudioProcessorValueTreeState& valueTreeState;
		Value currentPreset;
	};
}