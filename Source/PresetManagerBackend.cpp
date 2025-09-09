/*
  ==============================================================================

    PresetManagerBackend.cpp
    Created: 25 Dec 2024 5:45:38pm
    Author:  Akash Murthy (TheAudioProgrammer channel)

  ==============================================================================
*/

#include "PresetManagerBackend.h"

namespace Service
{
	const File PresetManager::defaultDirectory
	{
		File::getSpecialLocation(File::SpecialLocationType::commonDocumentsDirectory).getChildFile(ProjectInfo::companyName).getChildFile(ProjectInfo::projectName)
	};
	const String PresetManager::extension{ "preset" };
	const String PresetManager::presetNameProperty{ "presetName" };

	PresetManager::PresetManager(AudioProcessorValueTreeState& apvts) : valueTreeState(apvts)
	{
		// Create a default Preset Directory, if it doesn't exist
		if (!defaultDirectory.exists())
		{
			const auto result = defaultDirectory.createDirectory();
			if (result.failed())
			{
				DBG("Could not create preset directory: " + result.getErrorMessage());
				jassertfalse;
			}
		}

		initialiseFactoryPresets();

		valueTreeState.state.addListener(this);
		currentPreset.referTo(valueTreeState.state.getPropertyAsValue(presetNameProperty, nullptr));
	}

	void PresetManager::savePreset(const String& presetName)
	{
		if (presetName.isEmpty())
			return;

		currentPreset.setValue(presetName);
		const auto xml = valueTreeState.copyState().createXml();

		if (xml != nullptr)
		{
			// First, remove size percentage as before
			for (auto* param = xml->getFirstChildElement(); param != nullptr;)
			{
				auto* nextParam = param->getNextElement();
				if (param->hasTagName("PARAM") && param->getStringAttribute("id") == "sizePercentage")
				{
					xml->removeChildElement(param, true);
				}
				param = nextParam;
			}

			// Now add missing values to parameters that don't have them
			addMissingParameterValues(xml.get());

			const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
			if (!xml->writeTo(presetFile))
			{
				DBG("Could not create preset file: " + presetFile.getFullPathName());
				jassertfalse;
			}
		}
	}

	void PresetManager::addMissingParameterValues(XmlElement* xml)
	{
		// Helper lambda to add value attribute if it's missing
		auto addValueIfMissing = [xml](const String& paramId, const var& defaultValue) {
			for (auto* param = xml->getFirstChildElement(); param != nullptr; param = param->getNextElement())
			{
				if (param->hasTagName("PARAM") && param->getStringAttribute("id") == paramId)
				{
					if (!param->hasAttribute("value"))
					{
						param->setAttribute("value", defaultValue.toString());
					}
					return;
				}
			}
			};

		// Add missing values for ball parameters
		for (int i = 0; i < 3; i++)
		{
			addValueIfMissing("ballX" + String(i), BALL_X_DEFAULT_1);
			addValueIfMissing("ballY" + String(i), BALL_Y_DEFAULT);
			addValueIfMissing("ballRadius" + String(i), BALL_RADIUS_DEFAULT);
			addValueIfMissing("ballVelocity" + String(i), BALL_VELOCITY_DEFAULT);
			addValueIfMissing("ballAngle" + String(i), BALL_ANGLE_DEFAULT_1);
			addValueIfMissing("ballXVelocity" + String(i), BALL_X_VELOCITY_DEFAULT);
			addValueIfMissing("ballYVelocity" + String(i), BALL_Y_VELOCITY_DEFAULT);
			addValueIfMissing("xVelocityInverter" + String(i), false);
			addValueIfMissing("yVelocityInverter" + String(i), false);
			addValueIfMissing("BallActivation" + String(i), (i == 0) ? true : false);
			addValueIfMissing("delayAmount" + String(i), DELAY_AMOUNT_DEFAULT);
			addValueIfMissing("delayFeedback" + String(i), DELAY_FEEDBACK_DEFAULT);
			addValueIfMissing("delayRate" + String(i), DELAY_RATE_DEFAULT);
			addValueIfMissing("delayNoteMovement" + String(i), DELAY_NOTE_MOVEMENT_DEFAULT);
		}

		// Add missing values for global parameters
		addValueIfMissing("edgePhase", EDGE_PHASE_DEFAULT);
		addValueIfMissing("edgeDenomenator", EDGE_DENOMINATOR_DEFAULT);
		addValueIfMissing("edgeRange", EDGE_RANGE_DEFAULT);
		addValueIfMissing("scaleChoice", SCALE_DEFAULT);
		addValueIfMissing("rootNote", ROOT_NOTE_DEFAULT);
		addValueIfMissing("edgeType", EDGE_TYPE_DEFAULT);
		addValueIfMissing("ballsPositioningType", BALLS_POSITIONING_DEFAULT);
		addValueIfMissing("snapToGrid", false);
		addValueIfMissing("collision", false);
		addValueIfMissing("quantization", QUANTIZATION_DEFAULT);
		addValueIfMissing("quantizationDivision", QUANTIZATION_DIV_DEFAULT);
		addValueIfMissing("volumeVariation", VOLUME_VARIATION_DEFAULT);
		addValueIfMissing("probability", PROBABILITY_DEFAULT);
		addValueIfMissing("transpose", 0);
		addValueIfMissing("toggleState", false);
	}

	void PresetManager::deletePreset(const String& presetName)
	{
		if (presetName.isEmpty())
			return;

		const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
		if (!presetFile.existsAsFile())
		{
			DBG("Preset file " + presetFile.getFullPathName() + " does not exist");
			jassertfalse;
			return;
		}
		if (!presetFile.deleteFile())
		{
			DBG("Preset file " + presetFile.getFullPathName() + " could not be deleted");
			jassertfalse;
			return;
		}
		currentPreset.setValue("");
	}

	void PresetManager::loadPreset(const String& presetName)
	{
		if (presetName.isEmpty())
			return;

		// Initialize shared variables once
		const float currentSizePercentage = valueTreeState.getRawParameterValue("sizePercentage")->load();
		ValueTree valueTreeToLoad;
		bool presetLoaded = false;
		String loadedPresetName;

		// Try loading from file first
		const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
		if (presetFile.existsAsFile())
		{
			XmlDocument xmlDocument{ presetFile };
			if (auto docElement = xmlDocument.getDocumentElement())
			{
				valueTreeToLoad = ValueTree::fromXml(*docElement);
				loadedPresetName = presetName;
				presetLoaded = true;
			}
		}

		// If file loading failed, try factory presets
		if (!presetLoaded)
		{
			for (const auto& preset : factoryPresets)
			{
				if (preset.name == presetName)
				{
					if (auto xml = XmlDocument::parse(preset.xml))
					{
						valueTreeToLoad = ValueTree::fromXml(*xml);
						loadedPresetName = preset.name;
						presetLoaded = true;
						break; // Exit loop once found
					}
				}
			}
		}

		// Apply the loaded preset if successful
		if (presetLoaded && valueTreeToLoad.isValid())
		{
			valueTreeState.replaceState(valueTreeToLoad);

			// Restore the size percentage
			if (auto* sizeParam = valueTreeState.getParameter("sizePercentage"))
			{
				sizeParam->setValueNotifyingHost(currentSizePercentage);
			}

			currentPreset.setValue(loadedPresetName);
		}
		else
		{
			// Only debug/assert if neither source worked
			DBG("Preset '" + presetName + "' not found in files or factory presets");
			jassertfalse;
		}
	}

	int PresetManager::loadNextPreset()
	{
		const auto allPresets = getAllPresets();
		if (allPresets.isEmpty())
			return -1;
		const auto currentIndex = allPresets.indexOf(currentPreset.toString());
		const auto nextIndex = currentIndex + 1 > (allPresets.size() - 1) ? 0 : currentIndex + 1;
		loadPreset(allPresets.getReference(nextIndex));
		return nextIndex;
	}

	int PresetManager::loadPreviousPreset()
	{
		const auto allPresets = getAllPresets();
		if (allPresets.isEmpty())
			return -1;
		const auto currentIndex = allPresets.indexOf(currentPreset.toString());
		const auto previousIndex = currentIndex - 1 < 0 ? allPresets.size() - 1 : currentIndex - 1;
		loadPreset(allPresets.getReference(previousIndex));
		return previousIndex;
	}

	StringArray Service::PresetManager::getAllPresets() const
	{
		StringArray allPresets;

		// Factory
		for (auto& p : factoryPresets)
			allPresets.add(p.name);

		// User
		if (defaultDirectory.exists())
		{
			for (auto& f : defaultDirectory.findChildFiles(File::findFiles, false, "*." + extension))
				allPresets.add(f.getFileNameWithoutExtension());
		}

		return allPresets;
	}


	String PresetManager::getCurrentPreset() const
	{
		return currentPreset.toString();
	}

	void PresetManager::valueTreeRedirected(ValueTree& treeWhichHasBeenChanged)
	{
		currentPreset.referTo(treeWhichHasBeenChanged.getPropertyAsValue(presetNameProperty, nullptr));
	}

	void Service::PresetManager::initialiseFactoryPresets()
	{
		factoryPresets.push_back({ "Init", String::fromUTF8(BinaryData::Init_preset, BinaryData::Init_presetSize) });
		factoryPresets.push_back({ "Dance Of Fire", String::fromUTF8(BinaryData::DanceOfFire_preset, BinaryData::DanceOfFire_presetSize )});
		factoryPresets.push_back({ "Drip Drop", String::fromUTF8(BinaryData::DripDrop_preset, BinaryData::DripDrop_presetSize) });
		factoryPresets.push_back({ "Harpeggio", String::fromUTF8(BinaryData::Harpeggio_preset, BinaryData::Harpeggio_presetSize )});
		factoryPresets.push_back({ "Juggling", String::fromUTF8(BinaryData::Juggling_preset, BinaryData::Juggling_presetSize) });
		factoryPresets.push_back({ "Master Card", String::fromUTF8(BinaryData::MasterCard_preset, BinaryData::MasterCard_presetSize) });
		factoryPresets.push_back({ "Polyrythm", String::fromUTF8(BinaryData::Polyrythm_preset, BinaryData::Polyrythm_presetSize) });
		factoryPresets.push_back({ "Oval Galaxy", String::fromUTF8(BinaryData::OvalGalaxy_preset, BinaryData::OvalGalaxy_presetSize) });
		factoryPresets.push_back({ "Shuffle Hussle", String::fromUTF8(BinaryData::ShuffleHussle_preset, BinaryData::ShuffleHussle_presetSize) });
		factoryPresets.push_back({ "Stress Timer", String::fromUTF8(BinaryData::StressTimer_preset, BinaryData::StressTimer_presetSize) });
		factoryPresets.push_back({ "Total Chaos", String::fromUTF8(BinaryData::TotalChaos_preset, BinaryData::TotalChaos_presetSize) });
		factoryPresets.push_back({ "Wiggling Snake", String::fromUTF8(BinaryData::WigglingSnake_preset, BinaryData::WigglingSnake_presetSize)});
	}

	StringArray Service::PresetManager::getFactoryPresets() const
	{
		StringArray names;
		for (auto& p : factoryPresets)
			names.add(p.name);
		return names;
	}

	void Service::PresetManager::loadFactoryPreset(const String& presetName)
	{
		for (auto& p : factoryPresets)
		{
			if (p.name == presetName)
			{
				if (auto xml = XmlDocument::parse(p.xml))
					valueTreeState.replaceState(ValueTree::fromXml(*xml));

				currentPreset.setValue(p.name);
				break;
			}
		}
	}
}