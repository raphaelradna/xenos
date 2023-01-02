/*
  ==============================================================================

    PluginProcessor.cpp

    Xenos: Xenharmonic Stochastic Synthesizer
    Raphael Radna
    This code is licensed under the GPLv3

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
XenosAudioProcessor::XenosAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(
        BusesProperties()
#if !JucePlugin_IsMidiEffect
#if !JucePlugin_IsSynth
//                       .withInput  ("Input",  juce::AudioChannelSet::stereo(),
//                       true)
#endif
            .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
            )
    , xenosAudioSource(keyboardState)
    , params(
          *this, nullptr, juce::Identifier("Xenos"),
          {std::make_unique<juce::AudioParameterInt>(
               juce::ParameterID{"segments", 1}, // parameterID, version hint
               "segments", // parameter display name
               2, // minimum value
               128, // maximum value
               12), // default value
           std::make_unique<juce::AudioParameterFloat>(
               juce::ParameterID{"pitchWidth", 1}, "pitchWidth",
               juce::NormalisableRange<float>(0.0f, 96.0f, 0.0f, 1.0f), 1.0f,
               juce::AudioParameterFloatAttributes()
                   .withStringFromValueFunction(
                       [](auto x, auto) { return juce::String(x, 3); })
                   .withLabel("st")),
           std::make_unique<juce::AudioParameterFloat>(
               juce::ParameterID{"pitchBarrier", 1}, "pitchBarrier",
               juce::NormalisableRange<float>(0.0f, 1.0f, 0.0f, 0.5f), 0.1f,
               juce::AudioParameterFloatAttributes()
                   .withStringFromValueFunction(
                       [](auto x, auto) { return juce::String(x, 3); })),
           std::make_unique<juce::AudioParameterFloat>(
               juce::ParameterID{"pitchStep", 1}, "pitchStep",
               juce::NormalisableRange<float>(0.0f, 1.0f, 0.0f, 0.5f), 0.01f,
               juce::AudioParameterFloatAttributes()
                   .withStringFromValueFunction(
                       [](auto x, auto) { return juce::String(x, 3); })),
           std::make_unique<juce::AudioParameterFloat>(
               juce::ParameterID{"ampGain", 1}, "ampGain",
               juce::NormalisableRange<float>(-96.0f, 0.0f, 0.0f, 4.0f), 0.0f,
               juce::AudioParameterFloatAttributes()
                   .withStringFromValueFunction([](auto x, auto) {
                       return (x > -96.0f) ? juce::String(x, 3) : "-inf";
                   })
                   .withLabel("dB")),
           std::make_unique<juce::AudioParameterFloat>(
               juce::ParameterID{"ampBarrier", 1}, "ampBarrier",
               juce::NormalisableRange<float>(0.0f, 1.0f, 0.0f, 0.5f), 0.1f,
               juce::AudioParameterFloatAttributes()
                   .withStringFromValueFunction(
                       [](auto x, auto) { return juce::String(x, 3); })),
           std::make_unique<juce::AudioParameterFloat>(
               juce::ParameterID{"ampStep", 1}, "ampStep",
               juce::NormalisableRange<float>(0.0f, 1.0f, 0.0f, 0.5f), 0.01f,
               juce::AudioParameterFloatAttributes()
                   .withStringFromValueFunction(
                       [](auto x, auto) { return juce::String(x, 3); })),
           std::make_unique<juce::AudioParameterChoice>(
               juce::ParameterID{"pitchDistribution",
                                 1}, // parameterID, version hint
               "pitchDistribution", // parameter display name
               juce::StringArray{// choices
                                 "uniform", "gaussian", "poisson", "cauchy",
                                 "logistic", "hyperbolic cosine", "arcsine",
                                 "exponential", "triangular", "sinus"},
               0), // index of default value
           std::make_unique<juce::AudioParameterChoice>(
               juce::ParameterID{"pitchWalk", 1}, "pitchWalk",
               juce::StringArray{"primary", "secondary"}, 1),
           std::make_unique<juce::AudioParameterFloat>(
               juce::ParameterID{"pitchAlpha", 1}, "pitchAlpha",
               juce::NormalisableRange<float>(-100.0f, 100.0f, 0.0f, 0.3f,
                                              true),
               1.0f,
               juce::AudioParameterFloatAttributes()
                   .withStringFromValueFunction(
                       [](auto x, auto) { return juce::String(x, 3); })),
           std::make_unique<juce::AudioParameterFloat>(
               juce::ParameterID{"pitchBeta", 1}, "pitchBeta",
               juce::NormalisableRange<float>(-100.0f, 100.0f, 0.0f, 0.3f,
                                              true),
               1.0f,
               juce::AudioParameterFloatAttributes()
                   .withStringFromValueFunction(
                       [](auto x, auto) { return juce::String(x, 3); })),
           std::make_unique<juce::AudioParameterChoice>(
               juce::ParameterID{"ampDistribution", 1}, "ampDistribution",
               juce::StringArray{// choices
                                 "uniform", "gaussian", "poisson", "cauchy",
                                 "logistic", "hyperbolic cosine", "arcsine",
                                 "exponential", "triangular", "sinus"},
               0),
           std::make_unique<juce::AudioParameterChoice>(
               juce::ParameterID{"ampWalk", 1}, "ampWalk",
               juce::StringArray{"primary", "secondary"}, 1),
           std::make_unique<juce::AudioParameterFloat>(
               juce::ParameterID{"ampAlpha", 1}, "ampAlpha",
               juce::NormalisableRange<float>(-100.0f, 100.0f, 0.0f, 0.3f,
                                              true),
               1.0f,
               juce::AudioParameterFloatAttributes()
                   .withStringFromValueFunction(
                       [](auto x, auto) { return juce::String(x, 3); })),
           std::make_unique<juce::AudioParameterFloat>(
               juce::ParameterID{"ampBeta", 1}, "ampBeta",
               juce::NormalisableRange<float>(-100.0f, 100.0f, 0.0f, 0.3f,
                                              true),
               1.0f,
               juce::AudioParameterFloatAttributes()
                   .withStringFromValueFunction(
                       [](auto x, auto) { return juce::String(x, 3); })),
           std::make_unique<juce::AudioParameterFloat>(
               juce::ParameterID{"attack", 1}, // parameterID, version hint
               "attack", // parameter display name
               juce::NormalisableRange<float>(0.001f, 100.0f, 0.0f,
                                              0.3f), // range
               0.1f, // default value
               juce::AudioParameterFloatAttributes() // attributes
                   .withStringFromValueFunction(
                       [](auto x, auto) { return juce::String(x, 3); })
                   .withLabel("s")),
           std::make_unique<juce::AudioParameterFloat>(
               juce::ParameterID{"decay", 1}, "decay",
               juce::NormalisableRange<float>(0.001f, 100.0f, 0.0f, 0.3f), 0.1f,
               juce::AudioParameterFloatAttributes()
                   .withStringFromValueFunction(
                       [](auto x, auto) { return juce::String(x, 3); })
                   .withLabel("s")),
           std::make_unique<juce::AudioParameterFloat>(
               juce::ParameterID{"sustain", 1}, "sustain",
               juce::NormalisableRange<float>(-96.0f, 0.0f, 0.0f, 4.0f), 0.0f,
               juce::AudioParameterFloatAttributes()
                   .withStringFromValueFunction([](auto x, auto) {
                       return (x > -96.0f) ? juce::String(x, 3) : "-inf";
                   })
                   .withLabel("dB")),
           std::make_unique<juce::AudioParameterFloat>(
               juce::ParameterID{"release", 1}, "release",
               juce::NormalisableRange<float>(0.001f, 100.0f, 0.0f, 0.3f), 0.1f,
               juce::AudioParameterFloatAttributes()
                   .withStringFromValueFunction(
                       [](auto x, auto) { return juce::String(x, 3); })
                   .withLabel("s")),
           std::make_unique<juce::AudioParameterChoice>(
               juce::ParameterID{"scale", 1}, "scale",
               juce::StringArray{
                   "none", "pentatonic", "pentatonic (pythagorean)", "blues",
                   "blues (7-limit)", "whole-tone", "major", "major (5-limit)",
                   "minor", "minor (5-limit)", "octatonic", "overtone",
                   "chromatic", "bohlen-pierce", "quarter-tone", "custom"},
               0),
           std::make_unique<juce::AudioParameterFloat>(
               juce::ParameterID{"root", 1}, "root",
               juce::NormalisableRange<float>(0.0f, 12.0f, 0.0f, 1.0f), 0.0f,
               juce::AudioParameterFloatAttributes()
                   .withStringFromValueFunction(
                       [](auto x, auto) { return juce::String(x, 3); })
                   .withLabel("st"))})
#endif
{
    segmentsParam = params.getRawParameterValue("segments");
    pitchWidthParam = params.getRawParameterValue("pitchWidth");
    pitchBarrierParam = params.getRawParameterValue("pitchBarrier");
    pitchStepParam = params.getRawParameterValue("pitchStep");
    ampGainParam = params.getRawParameterValue("ampGain");
    ampBarrierParam = params.getRawParameterValue("ampBarrier");
    ampStepParam = params.getRawParameterValue("ampStep");

    pitchDistributionParam = params.getRawParameterValue("pitchDistribution");
    pitchWalkParam = params.getRawParameterValue("pitchWalk");
    pitchAlphaParam = params.getRawParameterValue("pitchAlpha");
    pitchBetaParam = params.getRawParameterValue("pitchBeta");

    ampDistributionParam = params.getRawParameterValue("ampDistribution");
    ampWalkParam = params.getRawParameterValue("ampWalk");
    ampAlphaParam = params.getRawParameterValue("ampAlpha");
    ampBetaParam = params.getRawParameterValue("ampBeta");

    attackParam = params.getRawParameterValue("attack");
    decayParam = params.getRawParameterValue("decay");
    sustainParam = params.getRawParameterValue("sustain");
    releaseParam = params.getRawParameterValue("release");

    scaleParam = params.getRawParameterValue("scale");
    rootParam = params.getRawParameterValue("root");
}

XenosAudioProcessor::~XenosAudioProcessor() {}

//==============================================================================
const juce::String XenosAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool XenosAudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool XenosAudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool XenosAudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double XenosAudioProcessor::getTailLengthSeconds() const { return 0.0; }

int XenosAudioProcessor::getNumPrograms()
{
    return 1; // NB: some hosts don't cope very well if you tell them there are
              // 0 programs, so this should be at least 1, even if you're not
              // really implementing programs.
}

int XenosAudioProcessor::getCurrentProgram() { return 0; }

void XenosAudioProcessor::setCurrentProgram(int index) {}

const juce::String XenosAudioProcessor::getProgramName(int index) { return {}; }

void XenosAudioProcessor::changeProgramName(int index,
                                            const juce::String& newName)
{}

//==============================================================================
void XenosAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..

    xenosAudioSource.prepareToPlay(samplesPerBlock, sampleRate);
}

void XenosAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.

    xenosAudioSource.releaseResources();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool XenosAudioProcessor::isBusesLayoutSupported(
    const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

        // This checks if the input layout matches the output layout
#if !JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void XenosAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer,
                                       juce::MidiBuffer& midiMessages)
{
    xenosAudioSource.setMidiBuffer(midiMessages);
    juce::AudioSourceChannelInfo channelInfo(buffer);
    xenosAudioSource.getNextAudioBlock(channelInfo);
}

//==============================================================================
bool XenosAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* XenosAudioProcessor::createEditor()
{
    return new XenosAudioProcessorEditor(*this, params);
}

//==============================================================================
void XenosAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.

    auto state = params.copyState();

    juce::XmlElement xmlParent("parent");

    std::unique_ptr<juce::XmlElement> xmlParams(state.createXml());
    xmlParent.addChildElement(xmlParams.release());

    juce::XmlElement* xmlScale = xmlParent.createNewChildElement(
        "scaleParams"); // add custom scale data to xml tree
    xmlScale->setAttribute(juce::Identifier(juce::String("CUSTOM_SCALE_DATA")),
                           customScaleData.joinIntoString("\n"));
    xmlScale->setAttribute(juce::Identifier(juce::String("CUSTOM_SCALE_NAME")),
                           customScaleName);

    copyXmlToBinary(xmlParent, destData);
}

void XenosAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory
    // block, whose contents will have been created by the getStateInformation()
    // call.

    std::unique_ptr<juce::XmlElement> xmlState(
        getXmlFromBinary(data, sizeInBytes));
    juce::XmlElement* xmlParams
        = xmlState->getChildByName(params.state.getType());
    juce::XmlElement* xmlScale = xmlState->getChildByName("scaleParams");

    if (xmlState.get() != nullptr) {
        if (xmlParams->hasTagName(params.state.getType())) {
            params.replaceState(juce::ValueTree::fromXml(*xmlParams));
            auto n = params.state.getNumChildren();
            for (int i = 0; i < n; ++i) { // recall stored parameters
                auto child = params.state.getChild(i);
                auto id = child["id"];
                auto value = child["value"];
                xenosAudioSource.setParam(id, value);
            }
        }
        if (xmlScale->hasTagName(juce::StringRef("scaleParams"))) {
            customScaleName = xmlScale->getStringAttribute(
                juce::String("CUSTOM_SCALE_NAME"), "custom");
            juce::String scaleData = xmlScale->getStringAttribute(
                juce::String("CUSTOM_SCALE_DATA"), "");
            customScaleData.clear();
            if (scaleData != "") customScaleData.addLines(scaleData);
            xenosAudioSource.loadScl(customScaleData,
                                     *scaleParam == customScaleParamIndex);
        }
    }
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new XenosAudioProcessor();
}
