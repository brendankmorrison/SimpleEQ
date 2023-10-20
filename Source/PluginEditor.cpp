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
//==============================================================================
void SimpleEQAudioProcessorEditor::paint (juce::Graphics& g)
{
    g.fillAll(juce::Colour::fromFloatRGBA(245.0, 247.0, 248.0, 0.8));
    
}

void SimpleEQAudioProcessorEditor::resized()
{
    auto lcfLeftMargin = getWidth()*.03;
    auto lcfTopMargin = getWidth()*.075;
    auto lcfSize = getWidth()*.13;
    lowCutFreq.setBounds(lcfLeftMargin, lcfTopMargin, lcfSize, lcfSize);
    
    auto hcfLeftMargin = getWidth()*.97 - getWidth()*.13;
    auto hcfTopMargin = getWidth()*.075;
    auto hcfSize = getWidth()*.13;
    highCutFreq.setBounds(hcfLeftMargin, hcfTopMargin, hcfSize, hcfSize);
    
    auto peakFreqLeftMargin = getWidth()*.5 - getWidth()*.13/2;
    auto peakFreqTopMargin = getWidth()*.075;
    auto peakFreqSize = getWidth()*.13;
    peakFreq.setBounds(peakFreqLeftMargin, peakFreqTopMargin, peakFreqSize, peakFreqSize);
    
    auto peakGainLeftMargin = getWidth()*.33 - getWidth()*.1/2;
    auto peakGainTopMargin = getWidth()*.075 + getWidth()*.13 - getWidth()*.1;
    auto peakGainSize = getWidth()*.1;
    peakGain.setBounds(peakGainLeftMargin, peakGainTopMargin, peakGainSize, peakGainSize);
    
    auto peakQualityLeftMargin = getWidth()*.66 - getWidth()*.1/2;
    auto peakQualityTopMargin = getWidth()*.075 + getWidth()*.13 - getWidth()*.1;
    auto peakQualitySize = getWidth()*.1;
    peakQuality.setBounds(peakQualityLeftMargin, peakQualityTopMargin, peakQualitySize, peakQualitySize);
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
}
