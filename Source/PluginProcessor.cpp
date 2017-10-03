/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include <array>

#include "PluginProcessor.h"
#include "PluginEditor.h"

#include "GlitchDelayEffect.h"

namespace
{
    // mixdown N channels of buffer into channel 0, using the weights array
    template< size_t N >
    void mix_down( AudioSampleBuffer& buffer, std::array<float, N > weights )
    {
        jassert( buffer.getNumChannels() == N );
        
        for( int s = 0; s < buffer.getNumSamples(); ++s )
        {
            float mixed = 0.0f;
            for( int c = 0; c < buffer.getNumChannels(); ++c )
            {
                mixed += buffer.getSample( c, s ) * weights[ c ];
            }
            
            buffer.setSample( 0, s, mixed );
        }
    }
    
    // mix buffer b2 into b1 using weights w1, w2
    void mix_into( AudioSampleBuffer& b1, AudioSampleBuffer& b2, int channel, float w1, float w2 )
    {
        jassert( b1.getNumChannels() > channel && b2.getNumChannels() > channel );
        
        for( int s = 0; s < b1.getNumSamples(); ++s )
        {
            float mixed = ( b1.getSample( channel, s ) * w1 ) + ( b2.getSample( channel, s ) * w2 );
            
            b1.setSample( channel, s, mixed );
        }
    }
}

//==============================================================================
TeensyJuceAudioProcessor::TeensyJuceAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::mono(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::mono(), true)
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
    
    if( m_prev_buffer.getNumChannels() != 1 || m_prev_buffer.getNumSamples() != buffer.getNumSamples() )
    {
        // set size of prev_buffer if not correct dimensions
        m_prev_buffer.setSize( 1, buffer.getNumSamples() );
    }
    else
    {
        // blend feedback into input
        mix_into( buffer, m_prev_buffer, 0, 0.5f, 0.5f );
    }
    
    m_effect->pre_process_audio( buffer, m_effect->num_input_channels(), m_effect->num_output_channels() );
    
    m_effect->update();
    
    AudioSampleBuffer output( m_effect->num_output_channels(), buffer.getNumSamples() );
    m_effect->post_process_audio( output );
    
    // mix down effect output to 1 channel
    std::array<float, 3> mix_weights = { 0.25f, 0.5f, 0.25f };
    mix_down( output, mix_weights );
    
    // mix output with original input
    
    // copy our mixed output to channel 0 of buffer
    buffer.copyFrom( 0, 0, output, 0, 0, output.getNumSamples() );
    
    // setup feedback for next block
    m_prev_buffer.copyFrom( 0, 0, output, 0, 0, output.getNumSamples() );
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
