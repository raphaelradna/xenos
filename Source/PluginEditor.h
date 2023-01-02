/*
  ==============================================================================

    PluginEditor.h

    Xenos: Xenharmonic Stochastic Synthesizer
    Raphael Radna
    This code is licensed under the GPLv3

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "ParamMenu.h"
#include "ParamSlider.h"
#include "PluginProcessor.h"

//==============================================================================

enum sliderStyle { vertical, horizontal, rotary };

class XenosLookAndFeel : public juce::LookAndFeel_V4 {
public:
    XenosLookAndFeel()
    {
        backgroundColor = findColour(juce::Slider::trackColourId);
        outlineColor = findColour(juce::Slider::textBoxOutlineColourId);
        sliderColor = findColour(juce::Slider::backgroundColourId);

        setColour(juce::Slider::backgroundColourId, backgroundColor);
        setColour(juce::Slider::thumbColourId, juce::Colours::red);
        setColour(juce::Slider::textBoxBackgroundColourId, backgroundColor);
        setColour(juce::Slider::textBoxHighlightColourId,
                  outlineColor.darker(1.0f));
    }

    void drawButtonBackground(juce::Graphics& g, juce::Button& b,
                              const juce::Colour& backgroundColour,
                              bool shouldDrawButtonAsHighlighted,
                              bool shouldDrawButtonAsDown) override
    {
        juce::Colour buttonColor = (!shouldDrawButtonAsHighlighted)
                                       ? backgroundColor
                                       : outlineColor.darker(1.0f);
        g.setColour(buttonColor);
        auto w = b.getWidth();
        auto h = b.getHeight();
        g.fillRect(0, 0, w, h);
        g.setColour(outlineColor);
        g.drawRect(0, 0, w, h, 1);
    }

    void drawComboBox(juce::Graphics& g, int width, int height,
                      bool isButtonDown, int buttonX, int buttonY, int buttonW,
                      int buttonH, juce::ComboBox& c) override
    {
        g.setColour(backgroundColor);
        g.fillRect(0, 0, width, height);
        g.setColour(outlineColor);
        g.drawRect(0, 0, width, height, 1);

        int p1X = buttonX + buttonW / 3;
        int p1Y = buttonY + buttonH / 3;
        int p3X = buttonX + 2 * buttonW / 3;
        int p2X = (p1X + p3X) / 2.f;
        int p2Y = buttonY + buttonH - buttonH / 3;
        int shift = 5;

        juce::Path p;
        p.addTriangle(p1X + shift, p1Y, p2X + shift, p2Y, p3X + shift, p1Y);
        g.fillPath(p);
    }

    void positionComboBoxText(juce::ComboBox& c,
                              juce::Label& labelToPosition) override
    {
        labelToPosition.setSize(c.getWidth() - 25, c.getHeight());
    }

    juce::Font getLabelFont(juce::Label&) override { return juce::Font(12.0f); }

    juce::Font getComboBoxFont(juce::ComboBox&) override
    {
        return juce::Font(12.0f);
    }

    void drawPopupMenuBackground(juce::Graphics& g, int width,
                                 int height) override
    {
        g.setColour(backgroundColor);
        g.fillRect(0, 0, width, height);
        g.setColour(outlineColor);
        g.drawRect(0, 0, width, height);
    }

    juce::Font getPopupMenuFont() override { return juce::Font(24.0f); }
private:
    juce::Colour backgroundColor, outlineColor, sliderColor;
};

class XenosAudioProcessorEditor
    : public juce::AudioProcessorEditor
    , private juce::Button::Listener {
public:
    typedef juce::AudioProcessorValueTreeState::SliderAttachment
        SliderAttachment;
    typedef juce::AudioProcessorValueTreeState::ComboBoxAttachment
        ComboBoxAttachment;

    XenosAudioProcessorEditor(XenosAudioProcessor&,
                              juce::AudioProcessorValueTreeState&);
    ~XenosAudioProcessorEditor() override;

    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;

    //==============================================================================
    void formatHeaderLabel(juce::Label& label, std::string t);
    void initParamSlider(ParamSlider& slider, std::string p, std::string d,
                         sliderStyle s = vertical,
                         juce::Colour c = juce::Colours::red);
    void initParamMenu(ParamMenu& menu, std::string p, std::string d,
                       float labelOffsetX = 0.0f);
    void buttonClicked(juce::Button* button) override;
    void loadCustomScale();
private:
    XenosAudioProcessor& audioProcessor;
    juce::AudioProcessorValueTreeState& valueTreeState;
    XenosLookAndFeel xenosLookAndFeel;

    ParamSlider pitchWidth;
    ParamSlider pitchBarrier;
    ParamSlider pitchStep;
    ParamMenu pitchDistribution;
    ParamMenu pitchWalk;
    ParamSlider pitchAlpha;
    ParamSlider pitchBeta;

    ParamSlider ampGain;
    ParamSlider ampBarrier;
    ParamSlider ampStep;
    ParamMenu ampDistribution;
    ParamMenu ampWalk;
    ParamSlider ampAlpha;
    ParamSlider ampBeta;

    ParamSlider attack;
    ParamSlider decay;
    ParamSlider sustain;
    ParamSlider release;
    ParamSlider segments;

    ParamMenu scale;
    juce::TextButton customButton;
    ParamSlider root;

    juce::MidiKeyboardComponent keyboardComponent;

    juce::Label pitchLabel, amplitudeLabel, envelopeLabel;

    juce::Colour red, green, blue;

    const int customScaleParamIndex = SCALE_PRESETS + 1;
    const int customScaleMenuIndex = SCALE_PRESETS + 2;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(XenosAudioProcessorEditor)
};
