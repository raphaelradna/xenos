/*
  ==============================================================================

    PluginEditor.cpp

    Xenos: Xenharmonic Stochastic Synthesizer
    Raphael Radna
    This code is licensed under the GPLv3

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
XenosAudioProcessorEditor::XenosAudioProcessorEditor(
    XenosAudioProcessor& p, juce::AudioProcessorValueTreeState& vts)
    : AudioProcessorEditor(&p)
    , audioProcessor(p)
    , valueTreeState(vts)
    , customButton("load...")
    , keyboardComponent(p.keyboardState,
                        juce::MidiKeyboardComponent::horizontalKeyboard)
{
    setSize(700, 560);

    red = juce::Colours::red;
    green = juce::Colours::lawngreen;
    blue = juce::Colours::cornflowerblue;

    formatHeaderLabel(pitchLabel, "PITCH");

    initParamSlider(pitchWidth, "pitchWidth", "WIDTH", vertical, red);
    initParamSlider(pitchBarrier, "pitchBarrier", "BARRIER", vertical, red);
    initParamSlider(pitchStep, "pitchStep", "STEP", vertical, red);

    initParamMenu(pitchDistribution, "pitchDistribution", "DISTRIBUTION");
    initParamMenu(pitchWalk, "pitchWalk", "WALK");
    initParamSlider(pitchAlpha, "pitchAlpha", "α", horizontal, red);
    initParamSlider(pitchBeta, "pitchBeta", "β", horizontal, red);

    formatHeaderLabel(amplitudeLabel, "AMPLITUDE");

    initParamSlider(ampGain, "ampGain", "GAIN", vertical, green);
    initParamSlider(ampBarrier, "ampBarrier", "BARRIER", vertical, green);
    initParamSlider(ampStep, "ampStep", "STEP", vertical, green);

    initParamMenu(ampDistribution, "ampDistribution", "DISTRIBUTION");
    initParamMenu(ampWalk, "ampWalk", "WALK");
    initParamSlider(ampAlpha, "ampAlpha", "α", horizontal, green);
    initParamSlider(ampBeta, "ampBeta", "β", horizontal, green);

    formatHeaderLabel(envelopeLabel, "GLOBAL");

    initParamSlider(attack, "attack", "A", vertical, blue);
    initParamSlider(decay, "decay", "D", vertical, blue);
    initParamSlider(sustain, "sustain", "S", vertical, blue);
    initParamSlider(release, "release", "R", vertical, blue);

    initParamSlider(segments, "segments", "SEGMENTS", horizontal, blue);
    initParamMenu(scale, "scale", "SCALE",
                  29.16675f); // ideally refactor to use relative pixel value
    scale.setItemEnabled(customScaleMenuIndex, false);
    initParamSlider(root, "root", "ROOT", horizontal, blue);

    if (!audioProcessor.customScaleData.isEmpty()) {
        scale.changeItemText(customScaleMenuIndex,
                             audioProcessor.customScaleName);
        scale.setItemEnabled(customScaleMenuIndex, true);
        if (dynamic_cast<juce::AudioParameterChoice*>(
                valueTreeState.getParameter("scale"))
                ->getIndex()
            == customScaleParamIndex) {
            scale.setSelectedId(customScaleMenuIndex);
        }
    }

    customButton.setLookAndFeel(&xenosLookAndFeel);
    addAndMakeVisible(customButton);
    customButton.addListener(this);

    addAndMakeVisible(keyboardComponent);
}

XenosAudioProcessorEditor::~XenosAudioProcessorEditor() {}

//==============================================================================
void XenosAudioProcessorEditor::paint(juce::Graphics& g)
{
    g.fillAll(
        getLookAndFeel().findColour(juce::ResizableWindow::backgroundColourId));
    juce::Image background = juce::ImageCache::getFromMemory(
        BinaryData::background_png, BinaryData::background_pngSize);
    g.drawImageAt(background, 0, 0);

    double w = getWidth();
    double h = getHeight();
    auto header = h / 16;
    auto margin = w / 64;
    auto panel1W = w / 3 - margin * 2;

    g.setColour(juce::Colours::white);
    auto line1X = 2 * margin + panel1W;
    auto line2X = line1X * 2;
    auto lineY = header / 2;
    g.drawLine(line1X, lineY, line1X, 25 * h / 32, 2.0f);
    g.drawLine(line2X, lineY, line2X, 25 * h / 32, 2.0f);

    g.setFont(9.0f);
    juce::String copyr = juce::String::fromUTF8(u8"\u00A9");
    g.drawText(
        "Copyright " + copyr + " 2022 Raphael Radna. All rights reserved.",
        margin, h - margin, w - margin * 2, margin, juce::Justification::right);
}

void XenosAudioProcessorEditor::resized()
{
    double w = getWidth();
    double h = getHeight();

    auto header = h / 16;
    auto margin = w / 64;
    auto panel1W = w / 3 - margin * 2;
    auto panel1H = 8 * h / 16;
    pitchLabel.setBounds(margin, 0, panel1W, header);

    auto sliderStep = panel1W / 3 + ((panel1W / 3) - (panel1W / 4)) / 2;
    auto vSliderY = header + margin;
    auto vSliderH = panel1H - margin * 2;
    pitchWidth.setBounds(margin, vSliderY, panel1W / 4, vSliderH);
    pitchBarrier.setBounds(margin + sliderStep, vSliderY, panel1W / 4,
                           vSliderH);
    pitchStep.setBounds(margin + 2 * sliderStep, vSliderY, panel1W / 4,
                        vSliderH);

    auto panel1X2 = w / 3 + margin;
    amplitudeLabel.setBounds(panel1X2, 0, panel1W, header);

    ampGain.setBounds(panel1X2, vSliderY, panel1W / 4, vSliderH);
    ampBarrier.setBounds(panel1X2 + sliderStep, vSliderY, panel1W / 4,
                         vSliderH);
    ampStep.setBounds(panel1X2 + 2 * sliderStep, vSliderY, panel1W / 4,
                      vSliderH);

    auto panel1X3 = 2 * w / 3 + margin;
    envelopeLabel.setBounds(panel1X3, 0, panel1W, header);

    attack.setBounds(panel1X3, vSliderY, panel1W / 4, vSliderH);
    decay.setBounds(panel1X3 + panel1W / 4, vSliderY, panel1W / 4, vSliderH);
    sustain.setBounds(panel1X3 + 2 * panel1W / 4, vSliderY, panel1W / 4,
                      vSliderH);
    release.setBounds(panel1X3 + 3 * panel1W / 4, vSliderY, panel1W / 4,
                      vSliderH);

    auto panel2Y = 9 * h / 16 + margin * 1.5;
    auto panel2H = 4 * h / 16 - margin;
    auto menuH = panel2H / 8;
    auto menuW = panel1W / 2 - margin / 2;
    auto hSliderYOffset = panel2H / 3;
    auto hSliderW = panel1W + margin / 2;
    pitchDistribution.setBounds(margin, panel2Y, menuW, menuH);
    pitchWalk.setBounds(margin + panel1W / 2 + margin / 2, panel2Y, menuW,
                        menuH);
    pitchAlpha.setBounds(margin, panel2Y + hSliderYOffset, hSliderW, menuH);
    pitchBeta.setBounds(margin, panel2Y + hSliderYOffset * 2, hSliderW, menuH);

    ampDistribution.setBounds(panel1X2, panel2Y, menuW, menuH);
    ampWalk.setBounds(panel1X2 + panel1W / 2 + margin / 2, panel2Y, menuW,
                      menuH);
    ampAlpha.setBounds(panel1X2, panel2Y + hSliderYOffset, hSliderW, menuH);
    ampBeta.setBounds(panel1X2, panel2Y + hSliderYOffset * 2, hSliderW, menuH);

    scale.setBounds(panel1X3, panel2Y, panel1W * 0.75, menuH);
    customButton.setBounds(panel1X3 + panel1W * 0.75, panel2Y, panel1W * 0.25,
                           menuH);
    root.setBounds(panel1X3, panel2Y + hSliderYOffset, hSliderW, menuH);
    segments.setBounds(panel1X3, panel2Y + hSliderYOffset * 2, hSliderW, menuH);

    auto keyboardY = 13 * h / 16;
    keyboardComponent.setBounds(margin, keyboardY, w - margin * 2,
                                h - keyboardY - margin);
}

//==============================================================================

void XenosAudioProcessorEditor::formatHeaderLabel(juce::Label& label,
                                                  std::string t)
{
    label.setText(t, juce::dontSendNotification);
    label.setFont(20.0f);
    label.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(label);
}

void XenosAudioProcessorEditor::initParamSlider(ParamSlider& slider,
                                                std::string p, std::string d,
                                                sliderStyle s, juce::Colour c)
{
    slider.param = p;
    slider.disp = d;

    slider.attachment.reset(
        new SliderAttachment(valueTreeState, slider.param, slider));
    slider.listener = new ParamListener(audioProcessor);
    valueTreeState.addParameterListener(slider.param, slider.listener);
    slider.setTextValueSuffix(
        " " + valueTreeState.getParameter(slider.param)->getLabel());

    slider.label.setText(slider.disp, juce::dontSendNotification);
    switch (s) {
    case vertical:
        slider.setSliderStyle(juce::Slider::SliderStyle::LinearVertical);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 52.0f, 16.0f);

        slider.label.attachToComponent(dynamic_cast<juce::Component*>(&slider),
                                       false);
        slider.label.setJustificationType(juce::Justification::centred);
        slider.label.setMinimumHorizontalScale(1.0f);
        slider.label.setSize(slider.getWidth() * 1.5, slider.label.getHeight());
        slider.label.setCentrePosition(slider.getX() + slider.getWidth() / 2,
                                       slider.label.getY()
                                           + slider.label.getHeight() * 0.75f);
        break;
    case horizontal:
        slider.setSliderStyle(juce::Slider::SliderStyle::LinearHorizontal);
        slider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 52.0f, 16.0f);

        slider.label.attachToComponent(dynamic_cast<juce::Component*>(&slider),
                                       false);
        slider.label.setJustificationType(juce::Justification::centred);
        break;
    case rotary:
        slider.setSliderStyle(juce::Slider::SliderStyle::Rotary);
        slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 52.0f, 16.0f);

        slider.label.attachToComponent(dynamic_cast<juce::Component*>(&slider),
                                       false);
        slider.label.setJustificationType(juce::Justification::centred);
        break;
    }

    slider.setLookAndFeel(&xenosLookAndFeel);
    slider.setColour(juce::Slider::thumbColourId, c);
    slider.setColour(juce::Slider::trackColourId, c.darker(2.0f));
    slider.setColour(juce::CaretComponent::caretColourId, c);

    addAndMakeVisible(slider);
    addAndMakeVisible(slider.label);
}

void XenosAudioProcessorEditor::initParamMenu(ParamMenu& menu, std::string p,
                                              std::string d, float labelOffsetX)
{
    menu.param = p;
    menu.disp = d;

    auto paramObject = dynamic_cast<juce::AudioParameterChoice*>(
        valueTreeState.getParameter(menu.param));
    menu.addItemList(paramObject->choices, 1);
    addAndMakeVisible(menu);
    menu.attachment.reset(
        new ComboBoxAttachment(valueTreeState, menu.param, menu));
    menu.listener = new ParamListener(audioProcessor);
    valueTreeState.addParameterListener(menu.param, menu.listener);

    //    menu.label.setText(valueTreeState.getParameter(menu.param)->getName(99),
    //    juce::dontSendNotification);
    menu.label.setText(menu.disp, juce::dontSendNotification);
    menu.label.setJustificationType(juce::Justification::centred);
    menu.label.attachToComponent(dynamic_cast<juce::ComboBox*>(&menu), false);
    auto labelPos = menu.label.getPosition();
    labelPos.addXY(labelOffsetX, 0.0f);
    menu.label.setTopLeftPosition(labelPos);

    menu.setLookAndFeel(&xenosLookAndFeel);
    menu.setColour(juce::ComboBox::backgroundColourId,
                   getLookAndFeel().findColour(juce::Slider::trackColourId));

    addAndMakeVisible(menu.label);
}

void XenosAudioProcessorEditor::buttonClicked(juce::Button* button)
{
    if (button == &customButton) loadCustomScale();
}

void XenosAudioProcessorEditor::loadCustomScale()
{
    juce::FileChooser chooser("Select a Scala tuning file (.scl) to load.",
                              juce::File{}, "*.scl");
    if (chooser.browseForFileToOpen()) {
        juce::File f = chooser.getResult();
        juce::StringArray scaleData;
        f.readLines(scaleData);
        juce::String scaleName = f.getFileNameWithoutExtension();

        if (audioProcessor.xenosAudioSource.loadScl(scaleData, true)) {
            scale.changeItemText(customScaleMenuIndex, scaleName);
            scale.setItemEnabled(customScaleMenuIndex, true);
            scale.setSelectedId(customScaleMenuIndex);

            audioProcessor.customScaleName = scaleName;
            audioProcessor.customScaleData = scaleData;

            audioProcessor.updateHostDisplay();
        }
    }
}
