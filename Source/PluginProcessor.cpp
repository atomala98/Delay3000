/*
  ==============================================================================

    This file contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
Delay3000AudioProcessor::Delay3000AudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
#if ! JucePlugin_IsMidiEffect
#if ! JucePlugin_IsSynth
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
#endif
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)
#endif
    )
#endif
{
}

Delay3000AudioProcessor::~Delay3000AudioProcessor()
{
}

//==============================================================================
const juce::String Delay3000AudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool Delay3000AudioProcessor::acceptsMidi() const
{
#if JucePlugin_WantsMidiInput
    return true;
#else
    return false;
#endif
}

bool Delay3000AudioProcessor::producesMidi() const
{
#if JucePlugin_ProducesMidiOutput
    return true;
#else
    return false;
#endif
}

bool Delay3000AudioProcessor::isMidiEffect() const
{
#if JucePlugin_IsMidiEffect
    return true;
#else
    return false;
#endif
}

double Delay3000AudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int Delay3000AudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int Delay3000AudioProcessor::getCurrentProgram()
{
    return 0;
}

void Delay3000AudioProcessor::setCurrentProgram(int index)
{
}

const juce::String Delay3000AudioProcessor::getProgramName(int index)
{
    return {};
}

void Delay3000AudioProcessor::changeProgramName(int index, const juce::String& newName)
{
}

//==============================================================================
void Delay3000AudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    const int numChannels = getTotalNumInputChannels();
    const int delayBufferSize = 2 * (sampleRate + samplesPerBlock);

    delayBuffer.setSize(numChannels, delayBufferSize);
    outputBuffer.setSize(numChannels, delayBufferSize);
}

void Delay3000AudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool Delay3000AudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
#if JucePlugin_IsMidiEffect
    juce::ignoreUnused(layouts);
    return true;
#else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    // Some plugin hosts, such as certain GarageBand versions, will only
    // load plugins that support stereo bus layouts.
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::mono()
        && layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;

    // This checks if the input layout matches the output layout
#if ! JucePlugin_IsSynth
    if (layouts.getMainOutputChannelSet() != layouts.getMainInputChannelSet())
        return false;
#endif

    return true;
#endif
}
#endif

void Delay3000AudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    auto totalNumInputChannels = getTotalNumInputChannels();
    auto totalNumOutputChannels = getTotalNumOutputChannels();

    // In case we have more outputs than inputs, this code clears any output
    // channels that didn't contain input data, (because these aren't
    // guaranteed to be empty - they may contain garbage).
    // This is here to avoid people getting screaming feedback
    // when they first compile a plugin, but obviously you don't need to keep
    // this code if your algorithm always overwrites all the output channels.
    for (auto i = totalNumInputChannels; i < totalNumOutputChannels; ++i)
        buffer.clear(i, 0, buffer.getNumSamples());

    // This is the place where you'd normally do the guts of your plugin's
    // audio processing...
    // Make sure to reset the state if your inner loop is processing
    // the samples and the outer loop is handling the channels.
    // Alternatively, you can process the samples with the channels
    // interleaved by keeping the same state.

    const int bufferLength = buffer.getNumSamples();
    const int delayBufferLength = delayBuffer.getNumSamples();


    for (int channel = 0; channel < totalNumInputChannels; ++channel)
    {
        const float* bufferData = buffer.getReadPointer(channel);
        const float* delayBufferData = delayBuffer.getReadPointer(channel);
        float* dryBuffer = buffer.getWritePointer(channel);
        fillDelayBuffer(channel, bufferLength, delayBufferLength, bufferData, delayBufferData);
        getFromDelayBuffer(buffer, channel, bufferLength, delayBufferLength, bufferData, delayBufferData);
        feedbackDelay(channel, bufferLength, delayBufferLength, dryBuffer);
        outputBuffer.addFromWithRamp(channel, 0, bufferData, bufferLength, dry, dry);
        buffer.copyFromWithRamp(channel, 0, outputBuffer.getReadPointer(channel), bufferLength, outputGain, outputGain);
    }

    writePosition += bufferLength;
    writePosition %= delayBufferLength;
}

void Delay3000AudioProcessor::fillDelayBuffer(int channel, const int bufferLength, const int delayBufferLength, const float* bufferData, const float* delayBufferData) {
    if (delayBufferLength > bufferLength + writePosition)
    {
        delayBuffer.copyFromWithRamp(channel, writePosition, bufferData, bufferLength, delayAmount, delayAmount);
    }
    else
    {
        const int bufferRemaining = delayBufferLength - writePosition;
        delayBuffer.copyFromWithRamp(channel, writePosition, bufferData, bufferRemaining, delayAmount, delayAmount);
        delayBuffer.copyFromWithRamp(channel, 0, bufferData + bufferRemaining, bufferLength - bufferRemaining, delayAmount, delayAmount);
    }
}


void Delay3000AudioProcessor::getFromDelayBuffer(juce::AudioBuffer<float>& buffer, int channel, const int bufferLength, const int delayBufferLength,
    const float* bufferData, const float* delayBufferData) {
    const int readPosition = (int)(delayBufferLength + writePosition - (sampleRate * delayTime / 1000)) % delayBufferLength;

    if (delayBufferLength > bufferLength + readPosition) {
        outputBuffer.copyFromWithRamp(channel, 0, delayBufferData + readPosition, bufferLength, wet, wet);
    }
    else {
        const int bufferRemaining = delayBufferLength - readPosition;
        outputBuffer.copyFromWithRamp(channel, 0, delayBufferData + readPosition, bufferRemaining, wet, wet);
        outputBuffer.copyFromWithRamp(channel, bufferRemaining, delayBufferData, bufferLength - bufferRemaining, wet, wet);
    }
}

void Delay3000AudioProcessor::feedbackDelay(int channel, const int bufferLength, const int delayBufferLength, float* dryBuffer) {
    if (delayBufferLength > bufferLength + writePosition) {
        delayBuffer.addFromWithRamp(channel, writePosition, dryBuffer, bufferLength, feedbackAmount, feedbackAmount);
    }
    else {
        const int bufferRemaining = delayBufferLength - writePosition;
        delayBuffer.addFromWithRamp(channel, bufferRemaining, dryBuffer, bufferRemaining, feedbackAmount, feedbackAmount);
        delayBuffer.addFromWithRamp(channel, 0, dryBuffer, bufferLength - bufferRemaining, feedbackAmount, feedbackAmount);
    }
}

//==============================================================================
bool Delay3000AudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

juce::AudioProcessorEditor* Delay3000AudioProcessor::createEditor()
{
    return new Delay3000AudioProcessorEditor(*this);
}

//==============================================================================
void Delay3000AudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void Delay3000AudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new Delay3000AudioProcessor();
}
