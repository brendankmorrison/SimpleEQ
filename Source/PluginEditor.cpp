/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
SimpleEQAudioProcessorEditor::SimpleEQAudioProcessorEditor (SimpleEQAudioProcessor& p)
    : AudioProcessorEditor (&p), audioProcessor (p)
{
    setResizable(true, true);
    setResizeLimits(400, 200, 1000, 600);
    getConstrainer()->setFixedAspectRatio(2);
    initLCFDial();
    initHCFDial();
    initPeakFreqDial();
    initPeakGainDial();
    initPeakQualityDial();
    init_lowCutSlopeMenu();
    init_highCutSlopeMenu();
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize (800, 400);
}

SimpleEQAudioProcessorEditor::~SimpleEQAudioProcessorEditor()
{
    lowCutFreq.setLookAndFeel(nullptr);
    highCutFreq.setLookAndFeel(nullptr);
    peakFreq.setLookAndFeel(nullptr);
    peakGain.setLookAndFeel(nullptr);
    peakQuality.setLookAndFeel(nullptr);
    
    lowCutSlopeMenu.removeListener(this);
    lowCutSlopeMenu.setLookAndFeel(nullptr);
    highCutSlopeMenu.removeListener(this);
    highCutSlopeMenu.setLookAndFeel(nullptr);
}

//==============================================================================
void SimpleEQAudioProcessorEditor::initLCFDial() {
    lowCutFreqAttatchment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "LowCut Freq", lowCutFreq);
    addAndMakeVisible(lowCutFreq);
    lowCutFreq.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    lowCutFreq.setRange(20.f, 20000.f);
    lowCutFreq.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 25);
    lowCutFreq.setLookAndFeel(&dialLAF);
    addAndMakeVisible(lowCutFreqLabel);
    lowCutFreqLabel.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
    lowCutFreqLabel.setText("Low Cut Freq", juce::dontSendNotification);
    lowCutFreqLabel.attachToComponent(&lowCutFreq, false);
    lowCutFreqLabel.setJustificationType(juce::Justification::centred);
}

void SimpleEQAudioProcessorEditor::initHCFDial() {
    highCutFreqAttatchment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "HighCut Freq", highCutFreq);
    addAndMakeVisible(highCutFreq);
    highCutFreq.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    highCutFreq.setRange(20.f, 20000.f);
    highCutFreq.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 25);
    highCutFreq.setLookAndFeel(&dialLAF);
    addAndMakeVisible(highCutFreqLabel);
    highCutFreqLabel.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
    highCutFreqLabel.setText("High Cut Freq", juce::dontSendNotification);
    highCutFreqLabel.attachToComponent(&highCutFreq, false);
    highCutFreqLabel.setJustificationType(juce::Justification::centred);
}

void SimpleEQAudioProcessorEditor::initPeakFreqDial() {
    peakFreqAttatchment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "Peak Freq", peakFreq);
    addAndMakeVisible(peakFreq);
    peakFreq.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    peakFreq.setRange(20.f, 20000.f);
    peakFreq.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 25);
    peakFreq.setLookAndFeel(&dialLAF);
    addAndMakeVisible(peakFreqLabel);
    peakFreqLabel.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
    peakFreqLabel.setText("Peak Freq", juce::dontSendNotification);
    peakFreqLabel.attachToComponent(&peakFreq, false);
    peakFreqLabel.setJustificationType(juce::Justification::centred);
}

void SimpleEQAudioProcessorEditor::initPeakGainDial() {
    peakGainAttatchment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "Peak Gain", peakGain);
    addAndMakeVisible(peakGain);
    peakGain.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    peakGain.setRange(-24.f, 24.f);
    peakGain.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 25);
    peakGain.setLookAndFeel(&dialLAF);
    addAndMakeVisible(peakGainLabel);
    peakGainLabel.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
    peakGainLabel.setText("Peak Gain", juce::dontSendNotification);
    peakGainLabel.attachToComponent(&peakGain, false);
    peakGainLabel.setJustificationType(juce::Justification::centred);
}

void SimpleEQAudioProcessorEditor::initPeakQualityDial() {
    peakQualityAttatchment = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "Peak Quality", peakQuality);
    addAndMakeVisible(peakQuality);
    peakQuality.setSliderStyle(juce::Slider::SliderStyle::RotaryHorizontalVerticalDrag);
    peakQuality.setRange(.1f, 10.f);
    peakQuality.setTextBoxStyle(juce::Slider::TextBoxBelow, true, 60, 25);
    peakQuality.setLookAndFeel(&dialLAF);
    addAndMakeVisible(peakQualityLabel);
    peakQualityLabel.setColour(juce::Label::textColourId, juce::Colours::darkgrey);
    peakQualityLabel.setText("Peak Quality", juce::dontSendNotification);
    peakQualityLabel.attachToComponent(&peakQuality, false);
    peakQualityLabel.setJustificationType(juce::Justification::centred);
}

void SimpleEQAudioProcessorEditor::init_lowCutSlopeMenu()
{
    lowCutSlopeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "LowCut Slope", lowCutSlopeMenu);
    addAndMakeVisible(lowCutSlopeMenu);
    lowCutSlopeMenu.addItem("Slope 12", 1);
    lowCutSlopeMenu.addItem("Slope 24", 2);
    lowCutSlopeMenu.addItem("Slope 36", 3);
    lowCutSlopeMenu.addItem("Slope 48", 4);
    lowCutSlopeMenu.addListener(this);
    lowCutSlopeMenu.setLookAndFeel(&lowCutSlopeMenuLAF);
    
}

void SimpleEQAudioProcessorEditor::init_highCutSlopeMenu()
{
    highCutSlopeAttachment = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(audioProcessor.apvts, "HighCut Slope", highCutSlopeMenu);
    addAndMakeVisible(highCutSlopeMenu);
    highCutSlopeMenu.addItem("Slope 12", 1);
    highCutSlopeMenu.addItem("Slope 24", 2);
    highCutSlopeMenu.addItem("Slope 36", 3);
    highCutSlopeMenu.addItem("Slope 48", 4);
    highCutSlopeMenu.addListener(this);
    highCutSlopeMenu.setLookAndFeel(&highCutSlopeMenuLAF);
}

void SimpleEQAudioProcessorEditor::comboBoxChanged(juce::ComboBox *comboBoxThatHasChanged)
{
    if (comboBoxThatHasChanged == &lowCutSlopeMenu)
    {
        int selectedID = lowCutSlopeMenu.getSelectedId();
        //do something based on what was picked (ie assign parameter)
        std::cout << selectedID << std::endl;
    }
}
//==============================================================================
void SimpleEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colour::fromFloatRGBA(245.0, 247.0, 248.0, 0.8));
    
}

void SimpleEQAudioProcessorEditor::resized()
{
    auto lcfLeftMargin = getWidth()*.03;
    auto lcfTopMargin = getHeight()*.6;
    auto lcfSize = getWidth()*.13;
    lowCutFreq.setBounds(lcfLeftMargin, lcfTopMargin, lcfSize, lcfSize);
    
    auto hcfLeftMargin = getWidth()*.97 - getWidth()*.13;
    auto hcfTopMargin = getHeight()*.6;
    auto hcfSize = getWidth()*.13;
    highCutFreq.setBounds(hcfLeftMargin, hcfTopMargin, hcfSize, hcfSize);
    
    auto peakFreqLeftMargin = getWidth()*.5 - getWidth()*.13/2;
    auto peakFreqTopMargin = getHeight()*.6;
    auto peakFreqSize = getWidth()*.13;
    peakFreq.setBounds(peakFreqLeftMargin, peakFreqTopMargin, peakFreqSize, peakFreqSize);
    
    auto peakGainLeftMargin = getWidth()*.33 - getWidth()*.1/2;
    auto peakGainTopMargin = getHeight()*.6 + getWidth()*.13 - getWidth()*.1;
    auto peakGainSize = getWidth()*.1;
    peakGain.setBounds(peakGainLeftMargin, peakGainTopMargin, peakGainSize, peakGainSize);
    
    auto peakQualityLeftMargin = getWidth()*.66 - getWidth()*.1/2;
    auto peakQualityTopMargin = getHeight()*.6 + getWidth()*.13 - getWidth()*.1;
    auto peakQualitySize = getWidth()*.1;
    peakQuality.setBounds(peakQualityLeftMargin, peakQualityTopMargin, peakQualitySize, peakQualitySize);
    
    //=====ComboBox======
    //lowCutSlopeMenu
    auto lowCut_leftMargin = getWidth() * 0.038;
    auto lowCut_topMargin = getHeight() * 0.88;
    auto lowCut_sizeX = getWidth() * 0.115;
    auto lowCut_sizeY = getHeight() * 0.05;
    lowCutSlopeMenu.setBounds(lowCut_leftMargin, lowCut_topMargin, lowCut_sizeX, lowCut_sizeY);
    
    //highCutSlopeMenu
    auto highCut_leftMargin = getWidth() * 0.962 - getWidth() * 0.115;
    auto highCut_topMargin = getHeight() * 0.88;
    auto highCut_sizeX = getWidth() * 0.115;
    auto highCut_sizeY = getHeight() * 0.05;
    highCutSlopeMenu.setBounds(highCut_leftMargin, highCut_topMargin, highCut_sizeX, highCut_sizeY);
    
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
