/*
  ==============================================================================

    Scale.h

    Xenos: Xenharmonic Stochastic Synthesizer
    Raphael Radna
    This code is licensed under the GPLv3

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

class Scale {
public:
    Scale();
    Scale(std::vector<double> _intervals, double _repeatPoint);
    Scale(juce::StringArray scalaLines);

    double parseSclLine(std::string str);
    unsigned long size();
    double getInterval(int i);
    double getRepeatRatio();
private:
    std::vector<double> intervals;
    double repeatRatio;
};
