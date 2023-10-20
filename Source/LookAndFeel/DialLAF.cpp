/*
  ==============================================================================

    DialLAF.cpp
    Created: 19 Oct 2023 6:11:55pm
    Author:  Brendan Morrison

  ==============================================================================
*/

#include "DialLAF.h"

CustomDial::CustomDial()
{
    
}

void CustomDial::drawRotarySlider(juce::Graphics &g, int x, int y, int width, int height, float sliderPos, float rotaryStartAngle, float rotaryEndAngle, juce::Slider &slider)
{
    float diameter = fmin(width, height);
    float radius = diameter * 0.5;
    float centerX = x + width * 0.5;
    float centerY = y + width * 0.5;
    float rx = centerX - radius;
    float ry = centerY - radius;
    float angle = rotaryStartAngle + (sliderPos * (rotaryEndAngle - rotaryStartAngle));
    
    juce::Rectangle<float> dialArea (rx, ry, diameter, diameter);
    //g.setColour(juce::Colour::fromFloatRGBA(0.96, 0.8, 0.08, 0.85).darker(0.8625));
    g.setColour(juce::Colour::fromFloatRGBA(0.96, 0.8, 0.08, 0.85));
    g.fillEllipse(dialArea);
    
    //Test Gradient Params
//    float rx_l = rx;
//    float rx_y = ry;
//    float diameter_l = diameter;
//    float alpha = 0.825;
//    for (int i = 0; i < 20; ++i)
//    {
//        diameter_l -= 3.0;
//        alpha -= 0.0375;
//        rx_l += 1.5; //1.5 is correct offset, maybe offset is half the diameter
//        rx_y += 1.5;
//        juce::Rectangle<float> temp (rx_l, rx_y, diameter_l, diameter_l);
//        g.setColour(juce::Colour::fromFloatRGBA(0.96, 0.8, 0.08, 0.85).darker(alpha));
//        g.fillEllipse(temp);
//    }
    
    g.setColour(juce::Colours::grey.withAlpha(0.25f)); //light gray outline
    g.drawEllipse(rx, ry, diameter, diameter, 10.0f);
    
    g.setColour(juce::Colour::fromFloatRGBA(0.28, 0.37, 0.34, 0.9)); //green outline
    g.drawEllipse(rx, ry, diameter, diameter, 1.75f);
    
    g.setColour(juce::Colours::darkgrey.withAlpha(0.25f)); //dark gray outline
    g.drawEllipse(rx-2.5, ry-2.5, diameter+5, diameter+5, 1.75f);
    
    juce::Path dialTick;
    g.setColour(juce::Colour::fromFloatRGBA(0.28, 0.37, 0.34, 0.9)); //tick color
    dialTick.addRectangle(0, -radius + 6, 3.0f, radius * 0.6);
    g.fillPath(dialTick, juce::AffineTransform::rotation(angle).translated(centerX, centerY));
    
    
}
