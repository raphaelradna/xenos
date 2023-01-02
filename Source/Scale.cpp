/*
  ==============================================================================

    Scale.cpp

    Xenos: Xenharmonic Stochastic Synthesizer
    Raphael Radna
    This code is licensed under the GPLv3

  ==============================================================================
*/

#include <regex>
#include "Scale.h"
#include "Utility.h"

Scale::Scale()
{
    intervals.push_back(1);
    repeatRatio = 2;
}

Scale::Scale(std::vector<double> _intervals, double _repeatPoint)
{
    for (int i = 0; i < _intervals.size(); i++)
        intervals.push_back(_intervals[i]);
    repeatRatio = _repeatPoint;
}

Scale::Scale(juce::StringArray scalaLines)
{
    for (int i = 0; i < scalaLines.size(); i++) {
        std::smatch match;
        std::string str = scalaLines[i].toStdString();
        if (std::regex_search(
                str, match,
                std::regex("^\\s*\\d+((\\.\\d*)|(\\/\\d+))?(\\s*cents)?")))
            intervals.push_back(parseSclLine(str));
    }
    intervals.erase(intervals.begin()); // remove cardinality

    if (intervals[0] == 1) {
        repeatRatio = 2;
    } else {
        repeatRatio = intervals.back();
        intervals.pop_back();
        intervals.insert(intervals.begin(), 1);
    }
}

double Scale::parseSclLine(std::string str)
{
    std::smatch match;
    double interval = 1;
    if (std::regex_search(str, match,
                          std::regex("^\\s*(\\d+)\\/(\\d+)"))) { // ratio
        interval = stod(match[1]) / stod(match[2]);
    } else if (std::regex_search(
                   str, match,
                   std::regex("^\\s*(\\d+\\.\\d*)\\s*cents"))) { // cents
        interval = ctor(stod(match[1]));
    } else if (std::regex_search(str, match,
                                 std::regex("^\\s*\\d+\\.\\d*"))) { // decimal
        interval = ctor(stod(match[0]));
    } else if (std::regex_search(str, match,
                                 std::regex("^\\s*(\\d+)"))) { // integer
        interval = stoi(match[1]);
    }
    return interval;
}

unsigned long Scale::size() { return intervals.size(); }

double Scale::getInterval(int i) { return intervals[i]; }

double Scale::getRepeatRatio() { return repeatRatio; }
