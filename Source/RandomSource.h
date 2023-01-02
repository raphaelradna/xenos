/*
  ==============================================================================

    RandomSource.h

    Xenos: Xenharmonic Stochastic Synthesizer
    Raphael Radna
    This code is licensed under the GPLv3

  ==============================================================================
*/

#pragma once

#include <random>

class RandomSource {
public:
    double uniform(double a = 1);
    double normal(double a = 1, double b = 1);
    double poisson(double a = 1);
    double cauchy(double z, double a = 1);
    double logist(double z, double a = 1, double b = 1);
    double hyperbcos(double z, double a = 1);
    double arcsine(double z, double a = 1);
    double exponential(double z, double a = 1);
    double triangle(double z, double a = 1);
    double sinus(double z, double a = 1, double b = 1);
    double operator()();

    void setMode(int m);
    void setAlpha(double a);
    void setBeta(double b);
private:
    std::random_device s;
    std::default_random_engine generator{s()};
    std::uniform_real_distribution<double> uniformDist{0.0, 1.0};
    std::normal_distribution<double> normalDist{5, 2};
    std::poisson_distribution<int> poissonDist{4.1};
    int mode = 0;
    double alpha = 1, beta = 1;
};
