/*
  ==============================================================================

    DialLAF.h
    Created: 19 Oct 2023 6:11:55pm
    Author:  Brendan Morrison

  ==============================================================================
*/

#pragma once
#include <JuceHeader.h>

class CustomDial : public juce::LookAndFeel_V4
{
public:
    CustomDial();
    
    void drawRotarySlider (juce::Graphics& g, int x, int y, int width, int height, float sliderPos,
                                float rotaryStartAngle, float rotaryEndAngle, juce::Slider& slider) override;
};
