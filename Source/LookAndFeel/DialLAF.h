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
    
    void drawComboBox (juce::Graphics& g, int width, int height, bool,
                       int, int, int, int, juce::ComboBox& box) override;
    
    void drawPopupMenuItem (juce::Graphics& g, const juce::Rectangle<int>& area,
                                            const bool isSeparator, const bool isActive,
                                            const bool isHighlighted, const bool isTicked,
                                            const bool hasSubMenu, const juce::String& text,
                                            const juce::String& shortcutKeyText,
                            const juce::Drawable* icon, const juce::Colour* const textColourToUse) override;
};
