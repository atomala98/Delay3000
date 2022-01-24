/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Delay3000AudioProcessorEditor::Delay3000AudioProcessorEditor(Delay3000AudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Make sure that before the constructor has finished, you've set the
    // editor's size to whatever you need it to be.
    setSize(400, 300);


    setSlider(&feedbackSlider, 0.4, 0.0, 0.8);
    setLabel(&feedbackSlider, &feedbackLabel, "Feedback");

    setSlider(&delaySlider, 0.5);
    setLabel(&delaySlider, &delayLabel, "Delay amount");

    setSlider(&delayTimeSlider, 500, 20, 2000, 1);
    setLabel(&delayTimeSlider, &delayTimeLabel, "Delay Time");

    setSlider(&drySlider);
    setLabel(&drySlider, &dryLabel, "Dry");

    setSlider(&wetSlider);
    setLabel(&wetSlider, &wetLabel, "Wet");

    setSlider(&outputGainSlider);
    setLabel(&outputGainSlider, &outputGainLabel, "Output Gain");
}

void Delay3000AudioProcessorEditor::setSlider(juce::Slider* slider, float base, float start, float stop, float step) {
    addAndMakeVisible(slider);
    slider->setRange(start, stop, step);
    slider->setValue(base);
    slider->addListener(this);
}

void Delay3000AudioProcessorEditor::setLabel(juce::Slider* slider, juce::Label* label, juce::String labelValue) {
    addAndMakeVisible(label);
    juce::Font labelFont(juce::Typeface::createSystemTypefaceFor(BinaryData::MontserratExtraBoldItalic_ttf,
        BinaryData::MontserratExtraBoldItalic_ttfSize));
    label->setText(labelValue, juce::dontSendNotification);
    label->setFont(labelFont);
    label->attachToComponent(slider, true);
}

Delay3000AudioProcessorEditor::~Delay3000AudioProcessorEditor()
{
}

//==============================================================================
void Delay3000AudioProcessorEditor::paint(juce::Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll(juce::Colours::white);

    juce::Image background = juce::ImageCache::getFromMemory(BinaryData::bcg2_png, BinaryData::bcg2_pngSize);
    g.drawImageAt(background, 0, 0);

    juce::Font mainFont(juce::Typeface::createSystemTypefaceFor(BinaryData::MontserratExtraBoldItalic_ttf,
        BinaryData::MontserratExtraBoldItalic_ttfSize));
    mainFont.setHeight(45.0f);
    g.setFont(mainFont);
    g.setColour(juce::Colours::white);

    g.drawFittedText("Delay3000", 0, 20, getWidth(), 40, juce::Justification::centred, 1);

}

void Delay3000AudioProcessorEditor::resized()
{
    // This is generally where you'll want to lay out the positions of any
    // subcomponents in your editor..
    int firstY = 90;
    int ySplit = 30;
    int sliderLeft = 120;
    feedbackSlider.setBounds(sliderLeft, firstY, getWidth() - sliderLeft - 10, 20);
    delaySlider.setBounds(sliderLeft, firstY + ySplit, getWidth() - sliderLeft - 10, 20);
    delayTimeSlider.setBounds(sliderLeft, firstY + 2 * ySplit, getWidth() - sliderLeft - 10, 20);
    drySlider.setBounds(sliderLeft, firstY + 3 * ySplit, getWidth() - sliderLeft - 10, 20);
    wetSlider.setBounds(sliderLeft, firstY + 4 * ySplit, getWidth() - sliderLeft - 10, 20);
    outputGainSlider.setBounds(sliderLeft, firstY + 5 * ySplit, getWidth() - sliderLeft - 10, 20);
}

void Delay3000AudioProcessorEditor::sliderValueChanged(juce::Slider* slider)
{
    audioProcessor.feedbackAmount = feedbackSlider.getValue();
    audioProcessor.delayAmount = delaySlider.getValue();
    audioProcessor.delayTime = delayTimeSlider.getValue();
    audioProcessor.dry = drySlider.getValue();
    audioProcessor.wet = wetSlider.getValue();
    audioProcessor.outputGain = outputGainSlider.getValue();
}
