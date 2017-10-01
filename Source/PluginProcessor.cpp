/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "GlitchDelayEffect.h"


//==============================================================================
TeensyJuceAudioProcessor::TeensyJuceAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::stereo(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::stereo(), true)
                     #endif
                       )
#endif
{
    // create the wrapped effect
    m_effect = make_unique< GLITCH_DELAY_EFFECT >();
}

TeensyJuceAudioProcessor::~TeensyJuceAudioProcessor()
{
}

//==============================================================================
const String TeensyJuceAudioProcessor::getName() const
{
    return JucePlugin_Name;
}

bool TeensyJuceAudioProcessor::acceptsMidi() const
{
   #if JucePlugin_WantsMidiInput
    return true;
   #else
    return false;
   #endif
}

bool TeensyJuceAudioProcessor::producesMidi() const
{
   #if JucePlugin_ProducesMidiOutput
    return true;
   #else
    return false;
   #endif
}

bool TeensyJuceAudioProcessor::isMidiEffect() const
{
   #if JucePlugin_IsMidiEffect
    return true;
   #else
    return false;
   #endif
}

double TeensyJuceAudioProcessor::getTailLengthSeconds() const
{
    return 0.0;
}

int TeensyJuceAudioProcessor::getNumPrograms()
{
    return 1;   // NB: some hosts don't cope very well if you tell them there are 0 programs,
                // so this should be at least 1, even if you're not really implementing programs.
}

int TeensyJuceAudioProcessor::getCurrentProgram()
{
    return 0;
}

void TeensyJuceAudioProcessor::setCurrentProgram (int index)
{
}

const String TeensyJuceAudioProcessor::getProgramName (int index)
{
    return {};
}

void TeensyJuceAudioProcessor::changeProgramName (int index, const String& newName)
{
}

//==============================================================================
void TeensyJuceAudioProcessor::prepareToPlay (double sampleRate, int samplesPerBlock)
{
    // Use this method as the place to do any pre-playback
    // initialisation that you need..
}

void TeensyJuceAudioProcessor::releaseResources()
{
    // When playback stops, you can use this as an opportunity to free up any
    // spare memory, etc.
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool TeensyJuceAudioProcessor::isBusesLayoutSupported (const BusesLayout& layouts) const
{
  #if JucePlugin_IsMidiEffect
    ignoreUnused (layouts);
    return true;
  #else
    // This is the place where you check if the layout is supported.
    // In this template code we only support mono or stereo.
    if (layouts.getMainOutputChannelSet() != AudioChannelSet::mono()
     && layouts.getMainOutputChannelSet() != AudioChannelSet::stereo())
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

void TeensyJuceAudioProcessor::processBlock (AudioSampleBuffer& buffer, MidiBuffer& midiMessages)
{
    ScopedNoDenormals no_denormals;
    
    m_effect->pre_process_audio( buffer, getTotalNumInputChannels(), getTotalNumOutputChannels() );
    
    //m_effect->update();
    
    m_effect->post_process_audio( buffer );
}

//==============================================================================
bool TeensyJuceAudioProcessor::hasEditor() const
{
    return true; // (change this to false if you choose to not supply an editor)
}

AudioProcessorEditor* TeensyJuceAudioProcessor::createEditor()
{
    return new TeensyJuceAudioProcessorEditor (*this);
}

//==============================================================================
void TeensyJuceAudioProcessor::getStateInformation (MemoryBlock& destData)
{
    // You should use this method to store your parameters in the memory block.
    // You could do that either as raw data, or use the XML or ValueTree classes
    // as intermediaries to make it easy to save and load complex data.
}

void TeensyJuceAudioProcessor::setStateInformation (const void* data, int sizeInBytes)
{
    // You should use this method to restore your parameters from this memory block,
    // whose contents will have been created by the getStateInformation() call.
}

//==============================================================================
// This creates new instances of the plugin..
AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new TeensyJuceAudioProcessor();
}
