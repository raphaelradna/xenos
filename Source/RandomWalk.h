/*
  ==============================================================================

    RandomWalk.h

    Xenos: Xenharmonic Stochastic Synthesizer
    Raphael Radna
    This code is licensed under the GPLv3

  ==============================================================================
*/

#pragma once

#include <vector>

class RandomWalk {
public:
    void initialize(int n);
    void reset(int i, double v);
    void setParams(double* pR, int nP);
    void setParams(double v);
    void calcSecBarriers(double* pR, int nP);
    void setSecBarriers(double v);
    void calcPriBarriers();
    void calcPriStepSize();
    void step(int n, double r);
    double reflect(double val, double min, double max);
    double realLookup(std::vector<double>& a, double x, int nP);
    double operator()(unsigned i, double f);
    double operator()(double idx, int nP);

    double getSumPeriod();
    void setBarrierRatio(double bR);
    void setStepRatio(double sR);
    void setWalk(bool w);
private:
    double secBarrier[2];
    double secStepSize, priBarrier, priStepSize,
        sumPeriod = 0.0, barrierRatio = 0.1, stepRatio = 0.01;
    int distribution;
    bool walk = true;
    std::vector<double> pri, sec;
};
