//
//  TeensyJuce.h
//  TeensyJuce
//
//  Created by Scott Pitkethly on 23/09/2017.
//
//

#ifndef TeensyJuce_h
#define TeensyJuce_h

// TODO derive from Teensy AudioStream
class TEENSY_AUDIO_STREAM_WRAPPER
#ifdef TARGET_TEENSY
: public AudioStream
#endif
{
#ifdef TARGET_TEENSY
    audio_block_t*        m_input_queue_array[1];
#endif
    
public:
    
    TEENSY_AUDIO_STREAM_WRAPPER()
#ifdef TARGET_TEENSY
        :   AudioStream( 1, m_input_queue_array ),
            m_input_queue_array()
#endif
    {
        
    }
    
    virtual ~TEENSY_AUDIO_STREAM_WRAPPER()      {;}
    
    virtual	 void update();
};



#endif /* TeensyJuce_h */
