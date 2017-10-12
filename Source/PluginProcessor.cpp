/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#include <array>

#include "JuceHeader.h"

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

float TeensyJuceAudioProcessor::MAX_FEEDBACK = 0.85f;

TeensyJuceAudioProcessor::TeensyJuceAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
     : AudioProcessor (BusesProperties()
                     #if ! JucePlugin_IsMidiEffect
                      #if ! JucePlugin_IsSynth
                       .withInput  ("Input",  AudioChannelSet::mono(), true)
                      #endif
                       .withOutput ("Output", AudioChannelSet::mono(), true)
                     #endif
                       ),
#endif
    m_mix( nullptr ),
    m_loop_size( nullptr ),
    m_jitter( nullptr ),
    m_feedback( nullptr ),
    m_low_head(nullptr),
    m_normal_head(nullptr),
    m_high_head(nullptr)
{
    // create the wrapped effect
    m_effect = make_unique< GLITCH_DELAY_EFFECT >();
    
    addParameter( m_mix = new AudioParameterFloat(          "mix",          // parameterID
                                                            "Mix",          // parameter name
                                                            0.0f,           // minimum value
                                                            1.0f,           // maximum value
                                                            0.5f ) );       // default value
    
    addParameter( m_loop_size = new AudioParameterFloat(    "loop_size",    // parameterID
                                                            "Loop Size",    // parameter name
                                                            0.0f,           // minimum value
                                                            1.0f,           // maximum value
                                                            0.5f ) );       // default value
    
    addParameter( m_jitter = new AudioParameterFloat(       "jitter",       // parameterID
                                                            "Jitter",       // parameter name
                                                            0.0f,           // minimum value
                                                            1.0f,           // maximum value
                                                            0.5f ) );       // default value
    
    addParameter( m_feedback = new AudioParameterFloat(     "feedback",     // parameterID
                                                            "Feedback",     // parameter name
                                                            0.0f,           // minimum value
                                                            MAX_FEEDBACK,   // maximum value
                                                            0.5f ) );       // default value
    
    addParameter( m_low_head = new AudioParameterFloat(     "low_head",     // parameterID
                                                            "Low Head",     // parameter name
                                                            0.0f,           // minimum value
                                                            0.33f,          // maximum value
                                                            0.2f ) );       // default value
    
    addParameter( m_normal_head = new AudioParameterFloat(  "normal_head",  // parameterID
                                                            "Normal Head",  // parameter name
                                                            0.0f,           // minimum value
                                                            0.33f,          // maximum value
                                                            0.33f ) );      // default value
    
    addParameter( m_high_head = new AudioParameterFloat(    "high_head",    // parameterID
                                                            "High Head",    // parameter name
                                                            0.0f,           // minimum value
                                                            0.33f,          // maximum value
                                                            0.2f ) );       // default value
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
        m_prev_buffer.clear( 0, 0, m_prev_buffer.getNumSamples() );
    }
    else
    {
        // blend feedback into input
        mix_into( buffer, m_prev_buffer, 0, 1.0f - (*m_feedback), *m_feedback );
    }
    
    m_effect->pre_process_audio( buffer, m_effect->num_input_channels(), m_effect->num_output_channels() );
    
    m_effect->set_speed( *m_jitter );
    m_effect->set_loop_size( *m_loop_size );
    m_effect->set_loop_moving(false);
    m_effect->update();
    
    AudioSampleBuffer output( m_effect->num_output_channels(), buffer.getNumSamples() );
    m_effect->post_process_audio( output );
    
    // mix down effect output to 1 channel
    std::array<float, 3> mix_weights = { *m_low_head, *m_normal_head, *m_high_head };
    mix_down( output, mix_weights );
    
    // mix output with original input
    mix_into( output, buffer, 0, *m_mix, 1.0f - *(m_mix) );
    
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
    return new TeensyJuceAudioProcessorEditor (*this, *m_effect);
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
