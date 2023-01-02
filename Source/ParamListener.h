/*
  ==============================================================================

    ParamListener.h

    Xenos: Xenharmonic Stochastic Synthesizer
    Raphael Radna
    This code is licensed under the GPLv3

  ==============================================================================
*/

#pragma once

class ParamListener : public juce::AudioProcessorValueTreeState::Listener {
public:
    ParamListener(XenosAudioProcessor& aP_) : aP(aP_) {}

    void parameterChanged(const juce::String& parameterID, float newValue)
    {
        aP.xenosAudioSource.setParam(parameterID, newValue);
    }
private:
    XenosAudioProcessor& aP;
};
