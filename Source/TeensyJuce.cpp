/*
  ==============================================================================

    TeensyJuce.cpp
    Created: 30 Sep 2017 3:05:37pm
    Author:  Scott Pitkethly

  ==============================================================================
*/

#include <limits>

#include "TeensyJuce.h"
#include "Util.h"

namespace
{
    // convert float sample [-1..1] to int16
    int16_t sample_convert_float_to_int16( float s )
    {
        // put in the range 0 .. 1
        s = ( s + 1.0f ) * 0.5f;
        ASSERT_MSG( s >= 0.0f && s <= 1.0f, "Sample out of range" );
        
        return s * std::numeric_limits< int16_t >::max();
    }
    
    // convert in16 sample to float [-1..1]
    float sample_convert_int16_to_float( int16_t s )
    {
        float sf = s / static_cast<float>( std::numeric_limits< int16_t >::max() );
        sf = ( sf * 2.0f ) - 1.0f;
        ASSERT_MSG( sf >= -1.0f && sf <= 1.0f, "Sample out of range" );
        
        return sf;
    }
}

TEENSY_AUDIO_STREAM_WRAPPER::TEENSY_AUDIO_STREAM_WRAPPER() :
    m_num_input_channels(0),
    m_num_output_channels(0),
    m_channel_buffers()
{
    
}

TEENSY_AUDIO_STREAM_WRAPPER::~TEENSY_AUDIO_STREAM_WRAPPER()
{
}


void TEENSY_AUDIO_STREAM_WRAPPER::pre_process_audio( const AudioSampleBuffer& audio_in, int num_input_channels, int num_output_channels )
{
    ASSERT_MSG( m_num_input_channels == 0 || num_input_channels == m_num_input_channels, "Num input channels has changed" );
    ASSERT_MSG( m_num_output_channels == 0 || num_output_channels == m_num_output_channels, "Num output channels has changed" );
    
    m_num_input_channels    = num_input_channels;
    m_num_output_channels   = num_output_channels;
    
    m_channel_buffers.clear();
    const int max_channels = max_val( num_input_channels, num_output_channels );
    
    m_channel_buffers.reserve( max_channels );
    
    const int num_samples   = audio_in.getNumSamples();
    
    for( int c = 0; c < max_channels; ++c )
    {
        SAMPLE_BUFFER samples;
        samples.reserve( num_samples );
        
        // is this an input channel
        if( c < num_input_channels )
        {
            //const float* sample_data = audio_in.getReadPointer( c );
            for( int s = 0; s < num_samples; ++s )
            {
                const float sample = audio_in.getSample( 0, s );
                samples.push_back( sample_convert_float_to_int16( sample ) );
            }
        }
        
        m_channel_buffers.emplace_back( samples );
    }
    
}

void TEENSY_AUDIO_STREAM_WRAPPER::post_process_audio( AudioSampleBuffer& audio_out )
{
    for( int c = 0; c < m_channel_buffers.size(); ++c )
    {
        const SAMPLE_BUFFER& sample_buffer = m_channel_buffers[ c ];
        for( int s = 0; s < sample_buffer.size(); ++s )
        {
            audio_out.setSample( c, s, sample_convert_int16_to_float( sample_buffer[s] ) );
        }
    }
}
