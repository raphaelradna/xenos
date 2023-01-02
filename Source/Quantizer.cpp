/*
  ==============================================================================

    Quantizer.cpp

    Xenos: Xenharmonic Stochastic Synthesizer
    Raphael Radna
    This code is licensed under the GPLv3

  ==============================================================================
*/

#include "Quantizer.h"
#include "Utility.h"

void Quantizer::loadScl(juce::StringArray s, bool load)
{
    const auto nPresets = scale.size() - 1;
    scale[nPresets] = Scale(s);
    if (load) setScale(nPresets);
}

void Quantizer::calcSteps()
{
    steps.clear();
    double start = calcStart();
    while (start <= pitchRange[0]) {
        for (int i = 0; i < scalePtr->size(); i++) {
            double q = mtos(start) / scalePtr->getInterval(i);
            steps.push_back(q);
        }
        start += rtoc(scalePtr->getRepeatRatio()) / 100.0;
    }
}

double Quantizer::calcStart()
{ // get the highest root transposition <= pitchRange[0]
    double s = root;
    double repeatPointMidi = rtoc(scalePtr->getRepeatRatio()) / 100.0;
    while (s < pitchRange[1] - repeatPointMidi) s += repeatPointMidi;
    return s;
}

double Quantizer::operator()(double per)
{
    if (active) {
        double min = 999999;
        int step = 0;
        for (int i = 0; i < steps.size(); i++) {
            double mag = abs(steps[i] - per);
            if (mag < min) {
                min = mag;
                step = i;
            }
        }
        return steps[step];
    } else {
        return per;
    }
}

double Quantizer::getFactor() { return factor; }

void Quantizer::setActive(bool a)
{
    if (active != a) active = a;
}

void Quantizer::setFactor(double sP) { factor = (*this)(sP) / sP; }

void Quantizer::setRange(double hi, double lo)
{
    if (hi == lo) hi += 0.000001; // quantizer misbehaves with a range of 0
    pitchRange[0] = hi;
    pitchRange[1] = lo;
}

void Quantizer::setRoot(const double r)
{
    root = (r > 11.9999999) ? 0.0 : r;
    calcSteps();
}

void Quantizer::setScale(const unsigned char n)
{
    scalePtr = &scale[n];
    calcSteps();
}
