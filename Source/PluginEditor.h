/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>
#include "PluginProcessor.h"

//==============================================================================
/**
*/
class Delay3000AudioProcessorEditor : public juce::AudioProcessorEditor, private juce::Slider::Listener
{
public:
    Delay3000AudioProcessorEditor(Delay3000AudioProcessor&);
    ~Delay3000AudioProcessorEditor() override;
    //==============================================================================
    void paint(juce::Graphics&) override;
    void resized() override;
    juce::Slider feedbackSlider;
    juce::Label feedbackLabel;
    juce::Slider delaySlider;
    juce::Label delayLabel;
    juce::Slider delayTimeSlider;
    juce::Label delayTimeLabel;
    juce::Slider drySlider;
    juce::Label dryLabel;
    juce::Slider wetSlider;
    juce::Label wetLabel;
    juce::Slider outputGainSlider;
    juce::Label outputGainLabel;

private:
    void sliderValueChanged(juce::Slider* slider) override;
    void setLabel(juce::Slider* slider, juce::Label* label, juce::String labelValue);
    void setSlider(juce::Slider* slider, float base = 1, float start = 0, float stop = 1, float step = 0.01);
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    Delay3000AudioProcessor& audioProcessor;
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Delay3000AudioProcessorEditor)
};
