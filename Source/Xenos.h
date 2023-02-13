/*
  ==============================================================================

    Xenos.h

    Xenos: Xenharmonic Stochastic Synthesizer
    Raphael Radna
    This code is licensed under the GPLv3

  ==============================================================================
*/

#pragma once

#include <iostream>
#include <random>
#include "Quantizer.h"
#include "RandomWalk.h"
#include "RandomSource.h"
#include "Utility.h"

#define MAX_POINTS (128)
#define NUM_VOICES (128)

struct XenosCore {
    void initialize(double sr)
    {
        sampleRate = sr;
        pitchWalk.initialize(MAX_POINTS);
        ampWalk.initialize(MAX_POINTS);
        ampWalk.setParams(1.0);
        reset();
    }

    void reset()
    {
        for (unsigned i = 0; i < MAX_POINTS; ++i) {
            pitchWalk.reset(i, mtos(pitchCenter, sampleRate) / nPoints);
            ampWalk.reset(i, uniform(generator));
        }
        calcMetaParams();
    }

    void calcMetaParams()
    {
        calcPeriodRange(pitchCenter, pitchWidth);
        pitchWalk.setParams(periodRange, nPoints);
        quantizer.calcSteps();
    }

    void calcPeriodRange(double pC, double pW)
    {
        double min = pC - pW / 2;
        double max = pC + pW / 2;
        periodRange[0] = mtos(min, sampleRate);
        periodRange[1] = mtos(max, sampleRate);
        quantizer.setRange(max, min);
    }

    double operator()()
    {
        if (index < 0.0) index = 0.0;
        int intdex = floor(index);
        double segmentSamps = pitchWalk(intdex, quantizer.getFactor()) * bend;
        double increment = 1 / segmentSamps;

        if (intdex != _index) {
            step(_index);
            _index = intdex;
        }

        index += increment;

        if (index >= nPoints) {
            quantizer.setFactor(pitchWalk.getSumPeriod());
            index -= nPoints;
            if (nPoints_ > 0) setNPoints();
        }
        return ampWalk(index, nPoints);
    }

    void step(int n)
    {
        pitchWalk.step(n, pitchSource());
        ampWalk.step(n, ampSource());
    }

    void setNPoints()
    {
        nPoints = nPoints_;
        calcMetaParams();
        nPoints_ = 0;
    }

    void setPitchCenter(float pC)
    {
        pitchCenter = pC;
        calcMetaParams();
    }

    void setPitchWidth(float pW)
    {
        pitchWidth = pW;
        calcMetaParams();
    }

    void setBend(double b, double r = 2.0)
    {
        if (b > 8191) {
            bend = scale(b, 8192, 16383, 1.0, 1.0 / r);
        } else {
            bend = scale(b, 0, 8191, r, 1.0);
        }
    }

    double sampleRate = 44100.0;
    float pitchCenter = 48.0f; // midi pitch
    float pitchWidth = 1.0f; // semitones
    double periodRange[2];
    double bend = 1.0;
    int nPoints = 12;
    int nPoints_ = 0;
    double index = 0.0;
    int _index = 0;
    RandomWalk pitchWalk, ampWalk;
    RandomSource pitchSource, ampSource;
    Quantizer quantizer;
    std::default_random_engine generator;
    std::uniform_real_distribution<double> uniform{-1.0, 1.0};
};

//==============================================================================
struct XenosSound : public juce::SynthesiserSound {
    XenosSound() {}

    bool appliesToNote(int) override { return true; }
    bool appliesToChannel(int) override { return true; }
};

//==============================================================================
struct XenosVoice : public juce::SynthesiserVoice {
    XenosVoice() {}

    bool canPlaySound(juce::SynthesiserSound* sound) override
    {
        return dynamic_cast<XenosSound*>(sound) != nullptr;
    }

    void setCurrentPlaybackSampleRate(double newRate) override
    {
        if (newRate > 0.0) {
            xenos.initialize(newRate);
            adsr.setSampleRate(newRate);
            updateADSR();
        }
    }

    void updateADSR()
    {
        adsr.setParameters(juce::ADSR::Parameters(a, d, s, r));
    }

    void startNote(int note, float velocity, juce::SynthesiserSound*,
                   int currentPitchWheelPosition) override
    {
        xenos.pitchCenter = note;
        xenos.setBend(currentPitchWheelPosition);
        adsr.noteOn();
        xenos.reset();
    }

    void stopNote(float /*velocity*/, bool allowTailOff) override
    {
        if (adsr.isActive()) adsr.noteOff();
    }

    void pitchWheelMoved(int newPitchWheelValue) override
    {
        xenos.setBend(newPitchWheelValue);
    }

    void controllerMoved(int, int) override {}

    void renderNextBlock(juce::AudioSampleBuffer& outputBuffer, int startSample,
                         int numSamples) override
    {
        if (adsr.isActive()) {
            while (--numSamples >= 0) {
                auto currentSample
                    = xenos() * adsr.getNextSample() * polyGainFactor;
                for (auto i = outputBuffer.getNumChannels(); --i >= 0;)
                    outputBuffer.addSample(i, startSample, currentSample);
                ++startSample;
            }
        } else {
            clearCurrentNote();
        }
    }

    XenosCore xenos;
    juce::ADSR adsr;
    float a = 0.1f, d = 0.1f, s = 1.0f, r = 0.1f;
    const double polyGainFactor = 1 / sqrt(NUM_VOICES / 4);
};

//==============================================================================
class XenosSynth : public juce::Synthesiser {
public:
    void noteOn(const int midiChannel, const int midiNoteNumber,
                const float velocity)
    {
        const juce::ScopedLock sl(lock);

        for (auto* sound: sounds) {
            if (sound->appliesToNote(midiNoteNumber)
                && sound->appliesToChannel(midiChannel)) {
                juce::SynthesiserVoice* voice
                    = findFreeVoice(sound, midiChannel, midiNoteNumber,
                                    isNoteStealingEnabled());
                startVoice(voice, sound, midiChannel, midiNoteNumber, velocity);
            }
        }
    }
private:
};

//==============================================================================
class XenosSynthAudioSource : public juce::AudioSource {
public:
    XenosSynthAudioSource(juce::MidiKeyboardState& keyState)
        : keyboardState(keyState)
    {
        for (auto i = 0; i < NUM_VOICES; ++i)
            xenosSynth.addVoice(new XenosVoice());

        xenosSynth.addSound(new XenosSound());
    }

    void setUsingSineWaveSound() { xenosSynth.clearSounds(); }

    void prepareToPlay(int /*samplesPerBlockExpected*/,
                       double sampleRate) override
    {
        xenosSynth.setCurrentPlaybackSampleRate(sampleRate);
    }

    void releaseResources() override {}

    void
    getNextAudioBlock(const juce::AudioSourceChannelInfo& bufferToFill) override
    {
        bufferToFill.clearActiveBufferRegion();

        keyboardState.processNextMidiBuffer(midiBuffer,
                                            bufferToFill.startSample,
                                            bufferToFill.numSamples, true);

        xenosSynth.renderNextBlock(*bufferToFill.buffer, midiBuffer,
                                   bufferToFill.startSample,
                                   bufferToFill.numSamples);
    }

    void setParam(const juce::String& parameterID, float newValue)
    {
        for (int i = 0; i < xenosSynth.getNumVoices(); ++i) {
            auto voice = dynamic_cast<XenosVoice*>(xenosSynth.getVoice(i));
            XenosCore& xenos = voice->xenos;

            if (parameterID == "segments") { xenos.nPoints_ = newValue; }
            if (parameterID == "pitchWidth") {
                xenos.pitchWidth = newValue;
                if (voice->isVoiceActive()) xenos.calcMetaParams();
            }
            if (parameterID == "pitchBarrier") {
                xenos.pitchWalk.setBarrierRatio(newValue);
            }
            if (parameterID == "pitchStep") {
                xenos.pitchWalk.setStepRatio(newValue);
            }
            if (parameterID == "ampGain") {
                auto linear = juce::Decibels::decibelsToGain(newValue, -96.0f);
                xenos.ampWalk.setSecBarriers(linear);
                xenos.ampWalk.calcPriBarriers();
                xenos.ampWalk.calcPriStepSize();
            }
            if (parameterID == "ampBarrier") {
                xenos.ampWalk.setBarrierRatio(newValue);
            }
            if (parameterID == "ampStep") {
                xenos.ampWalk.setStepRatio(newValue);
            }

            if (parameterID == "pitchDistribution") {
                xenos.pitchSource.setMode(newValue);
            }
            if (parameterID == "pitchWalk") {
                xenos.pitchWalk.setWalk(newValue);
            }
            if (parameterID == "pitchAlpha") {
                xenos.pitchSource.setAlpha(newValue);
            }
            if (parameterID == "pitchBeta") {
                xenos.pitchSource.setBeta(newValue);
            }

            if (parameterID == "ampDistribution") {
                xenos.ampSource.setMode(newValue);
            }
            if (parameterID == "ampWalk") { xenos.ampWalk.setWalk(newValue); }
            if (parameterID == "ampAlpha") {
                xenos.ampSource.setAlpha(newValue);
            }
            if (parameterID == "ampBeta") { xenos.ampSource.setBeta(newValue); }

            if (parameterID == "attack") {
                voice->a = newValue;
                voice->updateADSR();
            }
            if (parameterID == "decay") {
                voice->d = newValue;
                voice->updateADSR();
            }
            if (parameterID == "sustain") {
                auto linear = juce::Decibels::decibelsToGain(newValue, -96.0f);
                voice->s = linear;
                voice->updateADSR();
            }
            if (parameterID == "release") {
                voice->r = newValue;
                voice->updateADSR();
            }

            if (parameterID == "scale") {
                if (newValue == 0) {
                    xenos.quantizer.setActive(0);
                } else {
                    xenos.quantizer.setScale(newValue - 1);
                    xenos.quantizer.setActive(1);
                }
            }
            if (parameterID == "root") { xenos.quantizer.setRoot(newValue); }
        }
    }

    void setMidiBuffer(juce::MidiBuffer& mB) { midiBuffer = mB; }

    bool loadScl(juce::StringArray& s, bool load)
    {
        bool result = false;
        if (!s.isEmpty()) {
            for (int i = 0; i < xenosSynth.getNumVoices(); ++i) {
                auto voice = dynamic_cast<XenosVoice*>(xenosSynth.getVoice(i));
                XenosCore& xenos = voice->xenos;
                xenos.quantizer.loadScl(s, load);
            }
            result = true;
        }
        return result;
    }
private:
    juce::MidiKeyboardState& keyboardState;
    juce::MidiBuffer midiBuffer;
    XenosSynth xenosSynth;
};
