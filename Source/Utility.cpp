/*
  ==============================================================================

    Utility.cpp

    Xenos: Xenharmonic Stochastic Synthesizer
    Raphael Radna
    This code is licensed under the GPLv3

  ==============================================================================
*/

#include <cmath>
#include "Utility.h"

double mtof(double n, double b) { return b * std::pow(2, (n - 69) / 12); }

double ftom(double f, double b) { return 12 * std::log(f / b) / log(2) + 69; }

double mtos(double m, double sr)
{
    double freq = mtof(m);
    return sr / freq;
}

double stom(double s, double sr)
{
    double freq = sr / s;
    return ftom(freq);
}

double rtoc(double t1, double t2) { return 1200 * std::log(t1 / t2) / log(2); }

double ctor(double c) { return exp((c / 1200) * std::log(2)); }

double scale(double v, double inL, double inH, double outL, double outH,
             double c)
{
    if ((inL == inH) || (outL == outH)) {
        return outL + (outH - outL) / 2;
    } else {
        double vNorm = (v - inL) / (inH - inL);
        if (c != 1) vNorm = std::pow(vNorm, c);
        return vNorm * (outH - outL) + outL;
    }
}
