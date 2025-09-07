/*
  ==============================================================================

    PresetManagerGUI.h
    Created: 25 Dec 2024 5:45:20pm
    Author:  Akash Murthy (TheAudioProgrammer channel)

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PresetManagerBackend.h"

using namespace juce;

class PresetPanel : public juce::Component,
                    public juce::Button::Listener,
                    public juce::ComboBox::Listener
{
public:
    PresetPanel(Service::PresetManager& pm) : presetManager(pm)
    {
        setSize(200, 100);
        setAlpha(0.9);
        setVisible(false);
        setInterceptsMouseClicks(true, true);

		pluginBounds = getLocalBounds();

        configureButton(saveButton, "Save");
        configureButton(deleteButton, "Delete");
        configureButton(previousPresetButton, "<");
        configureButton(nextPresetButton, ">");

        presetList.setTextWhenNothingSelected("No Preset Selected");
        presetList.setMouseCursor(juce::MouseCursor::PointingHandCursor);
        addAndMakeVisible(presetList);
        presetList.addListener(this);

        loadPresetList();

		addChildComponent(&saveButton);
		addChildComponent(&deleteButton);
		addChildComponent(&previousPresetButton);
		addChildComponent(&nextPresetButton);
		addChildComponent(&presetList);
    }

    ~PresetPanel()
    {
        saveButton.removeListener(this);
        deleteButton.removeListener(this);
        previousPresetButton.removeListener(this);
        nextPresetButton.removeListener(this);
        presetList.removeListener(this);
    }

    void paint(juce::Graphics& g) override
    {
		juce::Colour darrrrkGreen = Colour(3,32,13);
        g.fillAll(darrrrkGreen); // Fill color of the rectangle
        g.setColour(Colour(0x00000000));
        g.drawRect(getLocalBounds(), 2); // Border
        g.setColour(Colour(0x00000000));
    }

	void setPluginBounds(juce::Rectangle<int> newPluginBounds)
	{
        Point<int> newPresetPanelTopLeft = Point(newPluginBounds.getTopLeft().getX() + 10, newPluginBounds.getTopLeft().getY() + 50);
        Point<int> newPresetPanelBottomRight = Point(newPluginBounds.getBottomRight().getX() - 10, newPluginBounds.getBottomRight().getY() - 120);
		this->setBounds(Rectangle(newPresetPanelTopLeft, newPresetPanelBottomRight));
	}

    void resized() override
    {
		juce::Point<int> presetPanelTopLeftPoint = getLocalBounds().getTopLeft();
        saveButton.setBounds(presetPanelTopLeftPoint.getX() + 120, presetPanelTopLeftPoint.getY() + 10, 80, 50); // x+100 because width of the panel is 80 so we leave 150-80=70px space
		previousPresetButton.setBounds(saveButton.getBounds().getTopRight().getX() + 10, saveButton.getBounds().getY(), 50, 50);
		presetList.setBounds(previousPresetButton.getBounds().getTopRight().getX() + 10, previousPresetButton.getBounds().getY(), 270, 50);
		nextPresetButton.setBounds(presetList.getBounds().getTopRight().getX() + 10, presetList.getBounds().getY(), 50, 50);
		deleteButton.setBounds(nextPresetButton.getBounds().getTopRight().getX() + 10, nextPresetButton.getBounds().getY(), 80, 50);
    }


private:

    void buttonClicked(juce::Button* button) override
    {
        if (button == &saveButton)
        {
            fileChooser = std::make_unique<juce::FileChooser>(
                "Please enter the name of the preset to save",
                Service::PresetManager::defaultDirectory,
                "*." + Service::PresetManager::extension
            );
            fileChooser->launchAsync(juce::FileBrowserComponent::saveMode, [&](const juce::FileChooser& chooser)
                {
                    const auto resultFile = chooser.getResult();
                    presetManager.savePreset(resultFile.getFileNameWithoutExtension());
                    loadPresetList();
                });
        }
        if (button == &previousPresetButton)
        {
            const auto index = presetManager.loadPreviousPreset();
            presetList.setSelectedItemIndex(index, juce::dontSendNotification);
        }
        if (button == &nextPresetButton)
        {
            const auto index = presetManager.loadNextPreset();
            presetList.setSelectedItemIndex(index, juce::dontSendNotification);
        }
        if (button == &deleteButton)
        {
            presetManager.deletePreset(presetManager.getCurrentPreset());
            loadPresetList();
        }
    }

    void comboBoxChanged(ComboBox* comboBoxThatHasChanged) override
    {
       	if (comboBoxThatHasChanged == &presetList)
       	{
       		presetManager.loadPreset(presetList.getItemText(presetList.getSelectedItemIndex()));
       	}
    }

    void configureButton(juce::Button& button, const juce::String& buttonText)
    {
        button.setButtonText(buttonText);
        button.setMouseCursor(juce::MouseCursor::PointingHandCursor);
        addAndMakeVisible(button);
        button.addListener(this);
    }

    void loadPresetList()
    {
        presetList.clear(juce::dontSendNotification);
        const auto allPresets = presetManager.getAllPresets();
        const auto currentPreset = presetManager.getCurrentPreset();
        presetList.addItemList(allPresets, 1);
        presetList.setSelectedItemIndex(allPresets.indexOf(currentPreset), juce::dontSendNotification);
    }

    Service::PresetManager& presetManager;
	bool isPresetWindowOpen = false;
    juce::TextButton saveButton, deleteButton, previousPresetButton, nextPresetButton;
    juce::ComboBox presetList;
    std::unique_ptr<juce::FileChooser> fileChooser;
	Rectangle<int> pluginBounds;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PresetPanel)
};
