/*
  ==============================================================================

    PluginProcessor.h

    Xenos: Xenharmonic Stochastic Synthesizer
    Raphael Radna
    This code is licensed under the GPLv3

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "Xenos.h"

#define SCALE_PRESETS (14)

//==============================================================================
/**
 */
class XenosAudioProcessor : public juce::AudioProcessor {
public:
    //==============================================================================
    XenosAudioProcessor();
    ~XenosAudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    //==============================================================================
    juce::MidiKeyboardState keyboardState;

    juce::String customScaleName;
    juce::StringArray customScaleData;

    XenosSynthAudioSource xenosAudioSource;
private:
    //==============================================================================
    juce::AudioProcessorValueTreeState params;

    std::atomic<float>* segmentsParam = nullptr;

    std::atomic<float>* pitchWidthParam = nullptr;
    std::atomic<float>* pitchBarrierParam = nullptr;
    std::atomic<float>* pitchStepParam = nullptr;
    std::atomic<float>* ampGainParam = nullptr;
    std::atomic<float>* ampBarrierParam = nullptr;
    std::atomic<float>* ampStepParam = nullptr;

    std::atomic<float>* pitchDistributionParam = nullptr;
    std::atomic<float>* pitchWalkParam = nullptr;
    std::atomic<float>* pitchAlphaParam = nullptr;
    std::atomic<float>* pitchBetaParam = nullptr;

    std::atomic<float>* ampDistributionParam = nullptr;
    std::atomic<float>* ampWalkParam = nullptr;
    std::atomic<float>* ampAlphaParam = nullptr;
    std::atomic<float>* ampBetaParam = nullptr;

    std::atomic<float>* attackParam = nullptr;
    std::atomic<float>* decayParam = nullptr;
    std::atomic<float>* sustainParam = nullptr;
    std::atomic<float>* releaseParam = nullptr;

    std::atomic<float>* scaleParam = nullptr;
    std::atomic<float>* rootParam = nullptr;

    const int customScaleParamIndex = SCALE_PRESETS + 1;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(XenosAudioProcessor)
};
