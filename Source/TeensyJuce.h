//
//  TeensyJuce.h
//  TeensyJuce
//
//  Created by Scott Pitkethly on 23/09/2017.
//
//

#ifndef TeensyJuce_h
#define TeensyJuce_h

#include <stdint.h>
#include "CompileSwitches.h"

#ifdef TARGET_TEENSY

#include <Audio.h>

class TEENSY_AUDIO_STREAM_WRAPPER : public AudioStream
{
    audio_block_t*          m_input_queue_array[1];
    
protected:
    
    // these are the only functions that require bespoke Teensy code
    virtual void            process_audio_in( int channel, const int16_t* samples, int num_samples );
    virtual void            process_audio_out( int channel, int16_t samples, int num_samples );
    
public:
    
    TEENSY_AUDIO_STREAM_WRAPPER() :
        AudioStream( 1, m_input_queue_array ),
        m_input_queue_array()
    {
        
    }
    
    virtual ~TEENSY_AUDIO_STREAM_WRAPPER()      {;}
};

#endif // TARGET_TEENSY

#ifdef TARGET_JUCE

class AudioSampleBuffer;

class TEENSY_AUDIO_STREAM_WRAPPER
{
    // store the 16-bit in/out buffers
    
protected:
    
    // these are the only functions that require bespoke JUCE code
    virtual void            process_audio_in( int channel, const int16_t* samples, int num_samples );
    virtual void            process_audio_out( int channel, int16_t samples, int num_samples );
    
public:
    
    TEENSY_AUDIO_STREAM_WRAPPER()
    {
        
    }
    
    virtual ~TEENSY_AUDIO_STREAM_WRAPPER()      {;}
    
    
    void                    pre_process_audio( const AudioSampleBuffer& audio_in );
    void                    post_process_audio( AudioSampleBuffer& audio_out );
    
    virtual void            update() = 0;
};

#endif // TARGET_JUCE

#endif /* TeensyJuce_h */
