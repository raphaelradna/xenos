/*
  ==============================================================================

    RandomWalk.cpp

    Xenos: Xenharmonic Stochastic Synthesizer
    Raphael Radna
    This code is licensed under the GPLv3

  ==============================================================================
*/

#include <cmath>
#include "RandomWalk.h"
#include "Utility.h"

void RandomWalk::initialize(int n)
{
    pri.resize(n);
    sec.resize(n);
}

void RandomWalk::reset(int i, double v)
{
    pri[i] = 0.0;
    sec[i] = v;
}

void RandomWalk::setParams(double* pR, int nP)
{
    calcSecBarriers(pR, nP);
    calcPriBarriers();
    calcPriStepSize();
}

void RandomWalk::setParams(double v)
{
    setSecBarriers(v);
    calcPriBarriers();
    calcPriStepSize();
}

void RandomWalk::calcSecBarriers(double* pR, int nP)
{
    secBarrier[0] = pR[1] / nP; // lo samps/hi freq
    secBarrier[1] = pR[0] / nP; // hi samps/lo freq
}

void RandomWalk::setSecBarriers(double v)
{
    secBarrier[0] = v * -1;
    secBarrier[1] = v;
}

void RandomWalk::calcPriBarriers()
{
    double secWalkSize = secBarrier[1] - secBarrier[0];
    secStepSize = secWalkSize * stepRatio;
    priBarrier = secWalkSize / 2 * barrierRatio;
}

void RandomWalk::calcPriStepSize()
{
    double priWalkSize = priBarrier * 2;
    priStepSize = priWalkSize * stepRatio;
}

void RandomWalk::step(int n, double r)
{
    if (walk) {
        // scale primary walk step
        double rnd = r * priStepSize;

        // do primary walk
        pri[n] += rnd;
        pri[n] = reflect(pri[n], priBarrier * -1, priBarrier);

        // do secondary walk
        sec[n] += pri[n];
        sec[n] = reflect(sec[n], secBarrier[0], secBarrier[1]);
    } else {
        double rnd = r * secStepSize;

        // do secondary walk, primary style
        sec[n] += rnd;
        sec[n] = reflect(sec[n], secBarrier[0], secBarrier[1]);
    }
    sumPeriod += sec[n];
}

double RandomWalk::reflect(double val, double min, double max)
{
    if (min == max) {
        val = min;
    } else {
        double space = max - min;
        double diff, ref;
        bool dir;
        if (val > max) {
            diff = val - max;
            dir = static_cast<int>(diff / space) % 2;
            ref = fmod(diff, space);

            val = (dir) ? min + ref : max - ref;
        }
        if (val < min) {
            diff = min - val;
            dir = static_cast<int>(diff / space) % 2;
            ref = fmod(diff, space);

            val = (dir) ? max - ref : min + ref;
        }
    }
    return val;
}

double RandomWalk::realLookup(std::vector<double>& a, double x, int nP)
{
    int x1 = std::floor(x);
    int x2 = x1 + 1;
    if (x2 >= nP) x2 = 0;
    double w = x - x1;
    return (1 - w) * a[x1] + w * a[x2];
}

double RandomWalk::operator()(unsigned i, double f) { return sec[i] * f; }

double RandomWalk::operator()(double idx, int nP)
{
    return realLookup(sec, idx, nP);
}

double RandomWalk::getSumPeriod()
{
    double temp = sumPeriod;
    sumPeriod = 0;
    return temp;
}

void RandomWalk::setBarrierRatio(double bR)
{
    barrierRatio = bR;
    calcPriBarriers();
    calcPriStepSize();
}

void RandomWalk::setStepRatio(double sR)
{
    stepRatio = sR;
    calcPriBarriers();
    calcPriStepSize();
}

void RandomWalk::setWalk(bool w) { walk = w; }
