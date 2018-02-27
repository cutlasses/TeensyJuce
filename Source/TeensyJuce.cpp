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
		if( s > 0.0f )
		{
			return s * std::numeric_limits< int16_t >::max();
		}
		else
		{
			return -s * std::numeric_limits< int16_t >::min();
		}
	}
    
    // convert in16 sample to float [-1..1]
    float sample_convert_int16_to_float( int16_t s )
    {
        float sf = s / static_cast<float>( std::numeric_limits< int16_t >::max() );
        ASSERT_MSG( sf >= -1.0f && sf <= 1.0f, "Sample out of range" );
        
        return sf;
    }
}

////////////////////////////////////////////////////////////////////////

#ifdef TARGET_JUCE

TEENSY_AUDIO_STREAM_WRAPPER::TEENSY_AUDIO_STREAM_WRAPPER() :
    m_num_input_channels(0),
    m_num_output_channels(0),
    m_channel_buffers()
{
    
}

TEENSY_AUDIO_STREAM_WRAPPER::~TEENSY_AUDIO_STREAM_WRAPPER()
{
}

bool TEENSY_AUDIO_STREAM_WRAPPER::process_audio_in( int channel )
{
    const SAMPLE_BUFFER& sample_buffer = m_channel_buffers[ channel ];
    
    if( !sample_buffer.empty() )
    {
        process_audio_in_impl( channel, sample_buffer.data(), static_cast<int>(sample_buffer.size()) );
        
        return true;
    }
    
    return false;
}

bool TEENSY_AUDIO_STREAM_WRAPPER::process_audio_out( int channel )
{
    SAMPLE_BUFFER& sample_buffer = m_channel_buffers[ channel ];
    
    if( !sample_buffer.empty() )
    {
        process_audio_out_impl( channel, sample_buffer.data(), static_cast<int>(sample_buffer.size()) );
        
        return true;
    }
    
    return false;
    
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
        
        // is this an input/output channel
        if( c < num_input_channels )
        {
            for( int s = 0; s < num_samples; ++s )
            {
                const float sample = audio_in.getSample( 0, s );
                samples.push_back( sample_convert_float_to_int16( sample ) );
            }
        }
        // this is an output only channel
        else
        {
            samples.resize( num_samples, 0.0f );
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

#endif // TARGET_JUCE
