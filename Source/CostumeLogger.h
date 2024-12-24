/*
  ==============================================================================

    CostumeLogger.h
    Created: 24 Dec 2024 4:39:48pm
    Author:  MayTigerStation

  ==============================================================================
*/

#pragma once

#include <juce_gui_extra/juce_gui_extra.h>

class CostumeLogger : public juce::Logger
{
public:
    void logMessage(const juce::String& message)
    {
        const juce::ScopedLock lock(writeLock);
        messages.add(message);

        // Optional: Keep only the last 100 messages to avoid excessive memory usage
        if (messages.size() > 100)
            messages.remove(0);
    }

    juce::StringArray getMessages() const
    {
        const juce::ScopedLock lock(writeLock);
        return messages;
    }

private:
    juce::CriticalSection writeLock;
    juce::StringArray messages;
};

