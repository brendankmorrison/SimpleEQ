/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"
#include "./LookAndFeel/DialLAF.h"

//==============================================================================
/**
*/
class SimpleEQAudioProcessorEditor  : public juce::AudioProcessorEditor, public juce::ComboBox::Listener, public juce::Timer
{
public:
    SimpleEQAudioProcessorEditor (SimpleEQAudioProcessor&);
    ~SimpleEQAudioProcessorEditor() override;

    //==============================================================================
    void paint (juce::Graphics&) override;
    void resized() override;
    void comboBoxChanged(juce::ComboBox* comboBoxThatHasChanged) override;
    void timerCallback() override;
    void paintHistogram();//juce::Graphics&);
    void drawFrame(juce::Graphics&);

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    SimpleEQAudioProcessor& audioProcessor;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (SimpleEQAudioProcessorEditor)
    
    CustomDial dialLAF;
    int rectArray[1025] = { 0 }; //for audio histogram
    
    juce::Slider lowCutFreq;
    juce::Label lowCutFreqLabel;
    juce::Label lowCutFreqVal;
    void initLCFDial();
    
    juce::Slider highCutFreq;
    juce::Label highCutFreqLabel;
    void initHCFDial();
    
    juce::Slider peakFreq;
    juce::Label peakFreqLabel;
    void initPeakFreqDial();
    
    juce::Slider peakGain;
    juce::Label peakGainLabel;
    void initPeakGainDial();
    
    juce::Slider peakQuality;
    juce::Label peakQualityLabel;
    void initPeakQualityDial();
    
    //Menus
    juce::ComboBox lowCutSlopeMenu;
    void init_lowCutSlopeMenu();
    CustomDial lowCutSlopeMenuLAF;
    
    juce::ComboBox highCutSlopeMenu;
    void init_highCutSlopeMenu();
    CustomDial highCutSlopeMenuLAF;
    
    //FFT Fundamental Freq Box
    void init_freqBox(juce::Graphics& g);
    
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> lowCutFreqAttatchment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> highCutFreqAttatchment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> peakFreqAttatchment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> peakGainAttatchment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> peakQualityAttatchment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> lowCutSlopeAttachment;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> highCutSlopeAttachment;
    
};
