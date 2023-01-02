/*
  ==============================================================================

    ParamMenu.h

    Xenos: Xenharmonic Stochastic Synthesizer
    Raphael Radna
    This code is licensed under the GPLv3

  ==============================================================================
*/

#pragma once

#include "ParamListener.h"

struct ParamMenu : juce::ComboBox {
    juce::Label label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>
        attachment;
    ParamListener* listener;
    std::string param, disp;

    ParamMenu() {}
};
