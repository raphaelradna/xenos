/*
  ==============================================================================

    ParamSlider.h

    Xenos: Xenharmonic Stochastic Synthesizer
    Raphael Radna
    This code is licensed under the GPLv3

  ==============================================================================
*/

#pragma once

#include "ParamListener.h"

struct ParamSlider : juce::Slider {
    juce::Label label;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>
        attachment;
    ParamListener* listener;
    std::string param, disp;

    ParamSlider() {}
};
