/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include <JuceHeader.h>

//==============================================================================
/**
*/
class Delay3000AudioProcessor : public juce::AudioProcessor
{
public:
    //==============================================================================
    Delay3000AudioProcessor();
    ~Delay3000AudioProcessor() override;

    //==============================================================================
    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    //==============================================================================
    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    //==============================================================================
    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;
    void Delay3000AudioProcessor::fillDelayBuffer(int channel, const int bufferLength, const int delayBufferLength, const float* bufferData, const float* delayBufferData);
    void Delay3000AudioProcessor::getFromDelayBuffer(juce::AudioBuffer<float>& buffer, int channel, const int bufferLength, const int delayBufferLength,
        const float* bufferData, const float* delayBufferData);
    void Delay3000AudioProcessor::feedbackDelay(int channel, const int bufferLength, const int delayBufferLength, float* dryBuffer);
    float feedbackAmount = 0.5;
    float delayAmount = 0.5;
    int delayTime = 500;
    float dry = 1;
    float wet = 1;
    float outputGain = 1;
private:
    juce::AudioSampleBuffer delayBuffer;
    juce::AudioSampleBuffer outputBuffer;
    int writePosition{ 0 };
    int sampleRate{ 44100 };
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(Delay3000AudioProcessor)
};
