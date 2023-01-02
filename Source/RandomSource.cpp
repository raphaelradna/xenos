/*
  ==============================================================================

    RandomSource.cpp

    Xenos: Xenharmonic Stochastic Synthesizer
    Raphael Radna
    This code is licensed under the GPLv3

  ==============================================================================
*/

#define _USE_MATH_DEFINES
#include <cmath>
#include "RandomSource.h"

double RandomSource::uniform(double a) { return uniformDist(generator) * a; }

double RandomSource::normal(double a, double b)
{
    normalDist.param(std::normal_distribution<double>::param_type(a, b));
    return normalDist(generator);
}

double RandomSource::poisson(double a)
{
    if (a < 0.0) a *= -1;
    poissonDist.param(std::poisson_distribution<int>::param_type(a));
    return poissonDist(generator);
}

double RandomSource::cauchy(double z, double a)
{
    return (a * tan((z - 0.5) * M_PI));
}

double RandomSource::logist(double z, double a, double b)
{
    if (a > -0.001 && a < 0.001)
        a = 0.001 * ((a > 0.0) * 2 - 1); // prevent divide by 0
    return (-(log((1 - z) / z) + b) / a);
}

double RandomSource::hyperbcos(double z, double a)
{
    return (a * log(tan(z * M_PI / 2.0)));
}

double RandomSource::arcsine(double z, double a)
{
    return (a * (0.5 - 0.5 * sin((0.5 - z) * M_PI)));
}

double RandomSource::exponential(double z, double a)
{
    if (a > -0.001 && a < 0.001)
        a = 0.001 * ((a > 0.0) * 2 - 1); // prevent divide by 0
    return (-(log(1 - z)) / a);
}

double RandomSource::triangle(double z, double a)
{
    return (a * (1 - sqrt(1 - z)));
}

double RandomSource::sinus(double z, double a, double b)
{
    return (a * sin(z * (2 * M_PI) * b));
}

double RandomSource::operator()()
{
    double rand = uniformDist(generator);
    int sign = (rand < 0.5) * 2 - 1;
    double v;
    switch (mode) {
    case 0:
        v = uniform(alpha);
        v *= sign;
        break;
    case 1:
        v = normal(alpha, beta);
        v *= sign;
        break;
    case 2:
        v = poisson(alpha);
        v *= sign;
        break;
    case 3:
        v = cauchy(rand, alpha);
        break;
    case 4:
        v = logist(rand, alpha, beta);
        v *= sign;
        break;
    case 5:
        v = hyperbcos(rand, alpha);
        break;
    case 6:
        v = arcsine(rand, alpha);
        v *= sign;
        break;
    case 7:
        v = exponential(rand, alpha);
        v *= sign;
        break;
    case 8:
        v = triangle(rand, alpha);
        v *= sign;
        break;
    case 9:
        v = sinus(rand, alpha, beta);
    }
    return v;
}

void RandomSource::setMode(int m) { mode = m; }

void RandomSource::setAlpha(double a) { alpha = a; }

void RandomSource::setBeta(double b) { beta = b; }
