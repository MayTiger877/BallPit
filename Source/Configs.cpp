/*
  ==============================================================================

    Configs.cpp
    Created: 15 Jun 2025 3:51:37pm
    Author:  MayTigerStation

  ==============================================================================
*/

#include "Configs.h"

int chosenTabIndex = 0;

void setChosenTabIndex(int index)
{
    if (index >= 0 && index <= 2)
    {
        chosenTabIndex = index;
    }
}

int getChosenTabIndex()
{
    return chosenTabIndex;
}