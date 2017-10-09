# TeensyJuce

TeensyJuce is intended as a way to prototype Teensy effects in a more fully featured dev environment, to make debugging possible. It uses the JUCE API, so can be compiled on Mac, PC and Linux. It will allow you to compile your Teensy effects as VSTs or Audio Units.

Simply derive your effect from `TEENSY_AUDIO_STREAM_WRAPPER` rather than `AudioStream`. This parent class handles the internal audio_block_t queue data, so you don't need to define that. Then, rather than making any Teensy audio library calls in your code, call

```
bool                            process_audio_in( int channel )
bool                            process_audio_out( int channel )
```

This will then call do the Teensy or Juce based audio handling, and call the functions below that you can override with your audio code.

```
// add audio processing code in these 2 functions
virtual void                    process_audio_in_impl( int channel, const int16_t* sample_data, int num_samples ) = 0;
virtual void                    process_audio_out_impl( int channel, int16_t* sample_data, int num_samples ) = 0;
```

Then simply add one of the following defines to switch between Teensy and Juce

```
//#define TARGET_TEENSY
#define TARGET_JUCE
```

I've ported my Glitch Delay Effect to this system, so have a look at the source code for an example of how to use it.
