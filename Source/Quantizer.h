/*
  ==============================================================================

    Quantizer.h

    Xenos: Xenharmonic Stochastic Synthesizer
    Raphael Radna
    This code is licensed under the GPLv3

  ==============================================================================
*/

#pragma once

#include "Scale.h"

class Quantizer {
public:
    void loadScl(juce::StringArray s, bool load);
    void calcSteps();
    double calcStart();
    double operator()(double per);

    double getFactor();
    void setActive(bool a);
    void setFactor(double sP);
    void setRange(double hi, double lo);
    void setRoot(const double r);
    void setScale(const unsigned char n);
private:
    double root = 0.0;
    double pitchRange[2] = {48.5, 47.5};
    double factor = 1.0;
    std::vector<double> steps;
    bool active = false;
    std::array<Scale, 15> scale = {
        Scale({1., 1.122462, 1.259921, 1.498307, 1.681793}, 2), // pentatonic
        Scale({1., 1.125, 1.265625, 1.5, 1.6875},
              2), // pentatonic (pythagorean)
        Scale({1., 1.189207, 1.33484, 1.414214, 1.498307, 1.781797},
              2), // blues
        Scale({1., 1.166667, 1.333333, 1.4, 1.5, 1.75}, 2), // blues (7-limit)
        Scale({1., 1.122462, 1.259921, 1.414214, 1.587401, 1.781797},
              2), // whole-tone
        Scale({1., 1.122462, 1.259921, 1.33484, 1.498307, 1.681793, 1.887749},
              2), // major
        Scale({1., 1.125, 1.25, 1.333333, 1.5, 1.666667, 1.875},
              2), // major (5-limit)
        Scale({1., 1.122462, 1.189207, 1.33484, 1.498307, 1.587401, 1.781797},
              2), // minor
        Scale({1., 1.125, 1.2, 1.333333, 1.5, 1.6, 1.777778},
              2), // minor (5-limit)
        Scale({1., 1.122462, 1.189207, 1.33484, 1.414214, 1.587401, 1.681793,
               1.887749},
              2), // octatonic
        Scale({1., 1.125, 1.25, 1.375, 1.5, 1.625, 1.75, 1.875}, 2), // overtone
        Scale({1., 1.059463, 1.122462, 1.189207, 1.259921, 1.33484, 1.414214,
               1.498307, 1.587401, 1.681793, 1.781797, 1.887749},
              2), // chromatic
        Scale({1., 1.088182, 1.18414, 1.288561, 1.402189, 1.525837, 1.660388,
               1.806806, 1.966134, 2.139512, 2.328178, 2.533484, 2.756892},
              3), // bohlen–pierce
        Scale({1.,       1.029302, 1.059463, 1.090508, 1.122462, 1.155353,
               1.189207, 1.224054, 1.259921, 1.29684,  1.33484,  1.373954,
               1.414214, 1.455653, 1.498307, 1.542211, 1.587401, 1.633915,
               1.681793, 1.731073, 1.781797, 1.834008, 1.887749, 1.943064},
              2), // quarter-tone
        Scale() // custom scale placeholder
    };
    Scale* scalePtr = &scale[0];
    const unsigned long nPresets = scale.size();
};
