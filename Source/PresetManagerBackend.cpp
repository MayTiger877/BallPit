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
			// remove size percentage and link nextParam to keep xml validityyy
			for (auto* param = xml->getFirstChildElement(); param != nullptr;)
			{
				auto* nextParam = param->getNextElement();
				if (param->hasTagName("PARAM") && param->getStringAttribute("id") == "sizePercentage")
				{
					xml->removeChildElement(param, true);
				}

				param = nextParam;
			}

			const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
			if (!xml->writeTo(presetFile))
			{
				DBG("Could not create preset file: " + presetFile.getFullPathName());
				jassertfalse;
			}
		}
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

		const auto presetFile = defaultDirectory.getChildFile(presetName + "." + extension);
		if (!presetFile.existsAsFile())
		{
			// try factory presets
			for (auto& p : factoryPresets)
			{
				if (p.name == presetName)
				{
					if (auto xml = XmlDocument::parse(p.xml))
						valueTreeState.replaceState(ValueTree::fromXml(*xml));
					currentPreset.setValue(p.name);
					return;
				}
			}

			DBG("Preset file " + presetFile.getFullPathName() + " does not exist");
			jassertfalse;
			return;
		}
		// presetFile (XML) -> (ValueTree)
		XmlDocument xmlDocument{ presetFile };

		const auto valueTreeToLoad = ValueTree::fromXml(*xmlDocument.getDocumentElement());
		float currentSizePercentage = valueTreeState.getRawParameterValue("sizePercentage")->load();
		valueTreeState.replaceState(valueTreeToLoad);
		if (auto* sizeParam = valueTreeState.getParameter("sizePercentage"))
		{
			sizeParam->setValueNotifyingHost(currentSizePercentage);
		}
		currentPreset.setValue(presetName);

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
		factoryPresets.push_back({ "Default", String::fromUTF8(BinaryData::Init_preset, BinaryData::Init_presetSize) });
		factoryPresets.push_back({ "Dance Of Fire", String::fromUTF8(BinaryData::Dance_Of_Fire_preset, BinaryData::Dance_Of_Fire_presetSize )});
		factoryPresets.push_back({ "Drip Drop", String::fromUTF8(BinaryData::Drip_Drop_preset, BinaryData::Drip_Drop_presetSize) });
		factoryPresets.push_back({ "Harpeggio", String::fromUTF8(BinaryData::Harpeggio_preset, BinaryData::Harpeggio_presetSize )});
		factoryPresets.push_back({ "Juggling", String::fromUTF8(BinaryData::Juggling_preset, BinaryData::Juggling_presetSize) });
		factoryPresets.push_back({ "Master Card", String::fromUTF8(BinaryData::Master_Card_preset, BinaryData::Master_Card_presetSize) });
		factoryPresets.push_back({ "Polyrythm 1", String::fromUTF8(BinaryData::Polyrythm_1_preset, BinaryData::Polyrythm_1_presetSize) });
		factoryPresets.push_back({ "Polyrythm 2", String::fromUTF8(BinaryData::Polyrythm_2_preset, BinaryData::Polyrythm_2_presetSize) });
		factoryPresets.push_back({ "Shuffle Hussle", String::fromUTF8(BinaryData::Shuffle_Hussle_preset, BinaryData::Shuffle_Hussle_presetSize) });
		factoryPresets.push_back({ "Stress Timer", String::fromUTF8(BinaryData::Stress_Timer_preset, BinaryData::Stress_Timer_presetSize) });
		factoryPresets.push_back({ "Total Chaos", String::fromUTF8(BinaryData::Total_Chaos_preset, BinaryData::Total_Chaos_presetSize) });
		factoryPresets.push_back({ "Wiggling Snake", String::fromUTF8(BinaryData::Wiggling_Snake_preset, BinaryData::Wiggling_Snake_presetSize)});
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