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
    g.setColour(juce::Colour::fromFloatRGBA(0.96, 0.8, 0.08, 0.85)); //yellow
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

void CustomDial::drawComboBox (juce::Graphics& g, int width, int height, bool, int, int, int, int, juce::ComboBox& box)
{
    auto cornerSize = box.findParentComponentOfClass<juce::ChoicePropertyComponent>() != nullptr ? 0.0f : 3.0f;
    juce::Rectangle<int> boxBounds (0, 0, width, height);

    g.setColour(juce::Colours::grey.withAlpha(0.5f));
    g.fillRoundedRectangle (boxBounds.toFloat(), cornerSize);

    //g.setColour (box.findColour (juce::ComboBox::outlineColourId));
    g.setColour(juce::Colours::grey.withAlpha(0.8f));
    g.drawRoundedRectangle (boxBounds.toFloat().reduced (0.5f, 0.5f), cornerSize, 1.0f);

    juce::Rectangle<int> arrowZone (width - 30, 0, 20, height);
    juce::Path path;
    path.startNewSubPath ((float) arrowZone.getX() + 3.0f, (float) arrowZone.getCentreY() - 2.0f);
    path.lineTo ((float) arrowZone.getCentreX(), (float) arrowZone.getCentreY() + 3.0f);
    path.lineTo ((float) arrowZone.getRight() - 3.0f, (float) arrowZone.getCentreY() - 2.0f);

    //g.setColour (box.findColour (juce::ComboBox::arrowColourId).withAlpha ((box.isEnabled() ? 0.9f : 0.2f)));
    g.setColour(juce::Colour::fromFloatRGBA(0.96, 0.8, 0.08, 0.85)); 
    g.strokePath (path, juce::PathStrokeType (2.0f));
}

void CustomDial::drawPopupMenuItem (juce::Graphics& g, const juce::Rectangle<int>& area,
                                        const bool isSeparator, const bool isActive,
                                        const bool isHighlighted, const bool isTicked,
                                        const bool hasSubMenu, const juce::String& text,
                                        const juce::String& shortcutKeyText,
                                        const juce::Drawable* icon, const juce::Colour* const textColourToUse)
{
    if (isSeparator)
    {
        auto r  = area.reduced (5, 0);
        r.removeFromTop (juce::roundToInt (((float) r.getHeight() * 0.5f) - 0.5f));

        //g.setColour (findColour (juce::PopupMenu::textColourId).withAlpha (0.3f));
        g.setColour(juce::Colours::grey.withAlpha(0.3f)); //0.3
        g.fillRect (r.removeFromTop (1));
    }
    else
    {
        auto textColour = (textColourToUse == nullptr ? findColour (juce::PopupMenu::textColourId)
                                                      : *textColourToUse);

        auto r  = area.reduced (1);

        if (isHighlighted && isActive)
        {
            //g.setColour (findColour (juce::PopupMenu::highlightedBackgroundColourId));
            g.setColour(juce::Colour::fromFloatRGBA(0.96, 0.8, 0.08, 0.5)); //Highlight color
            g.fillRect (r);

            g.setColour (findColour (juce::PopupMenu::highlightedTextColourId));
        }
        else
        {
            g.setColour (textColour.withMultipliedAlpha (isActive ? 1.0f : 0.5f));
        }

        r.reduce (juce::jmin (5, area.getWidth() / 20), 0);

        auto font = getPopupMenuFont();

        auto maxFontHeight = (float) r.getHeight() / 1.3f;

        if (font.getHeight() > maxFontHeight)
            font.setHeight (maxFontHeight);

        g.setFont (font);

        auto iconArea = r.removeFromLeft (juce::roundToInt (maxFontHeight)).toFloat();

        if (icon != nullptr)
        {
            icon->drawWithin (g, iconArea, juce::RectanglePlacement::centred | juce::RectanglePlacement::onlyReduceInSize, 1.0f);
            r.removeFromLeft (juce::roundToInt (maxFontHeight * 0.5f));
        }
        else if (isTicked)
        {
            auto tick = getTickShape (1.0f);
            g.fillPath (tick, tick.getTransformToScaleToFit (iconArea.reduced (iconArea.getWidth() / 5, 0).toFloat(), true));
        }

        if (hasSubMenu)
        {
            auto arrowH = 0.6f * getPopupMenuFont().getAscent();

            auto x = static_cast<float> (r.removeFromRight ((int) arrowH).getX());
            auto halfH = static_cast<float> (r.getCentreY());

            juce::Path path;
            path.startNewSubPath (x, halfH - arrowH * 0.5f);
            path.lineTo (x + arrowH * 0.6f, halfH);
            path.lineTo (x, halfH + arrowH * 0.5f);

            g.strokePath (path, juce::PathStrokeType (2.0f));
        }

        r.removeFromRight (3);
        g.drawFittedText (text, r, juce::Justification::centredLeft, 1);

        if (shortcutKeyText.isNotEmpty())
        {
            auto f2 = font;
            f2.setHeight (f2.getHeight() * 0.75f);
            f2.setHorizontalScale (0.95f);
            g.setFont (f2);

            g.drawText (shortcutKeyText, r, juce::Justification::centredRight, true);
        }
    }
}

