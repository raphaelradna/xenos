/*
  ==============================================================================

    Utility.h

    Xenos: Xenharmonic Stochastic Synthesizer
    Raphael Radna
    This code is licensed under the GPLv3

  ==============================================================================
*/

#pragma once

double mtof(double n, double b = 440); // midi to frequency
double ftom(double f, double b = 440); // frequency to midi
double mtos(double m, double sr = 44100); // midi to samples
double stom(double s, double sr = 44100); // samples to midi
double rtoc(double t1, double t2 = 1); // ratio to cents
double ctor(double c); // cents to ratio
double scale(double v, double inL, double inH, double outL, double outH,
             double c = 1);
