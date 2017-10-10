/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin processor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"

#include "GlitchDelayEffect.h"


//==============================================================================
/**
*/
class TeensyJuceAudioProcessor  : public AudioProcessor
{
public:
    //==============================================================================
    TeensyJuceAudioProcessor();
    ~TeensyJuceAudioProcessor();

    //==============================================================================
    void prepareToPlay (double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

   #ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported (const BusesLayout& layouts) const override;
   #endif

    void processBlock (AudioSampleBuffer&, MidiBuffer&) override;

    //==============================================================================
    AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    //==============================================================================
    const String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect () const override;
    double getTailLengthSeconds() const override;

    //==============================================================================
    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram (int index) override;
    const String getProgramName (int index) override;
    void changeProgramName (int index, const String& newName) override;

    //==============================================================================
    void getStateInformation (MemoryBlock& destData) override;
    void setStateInformation (const void* data, int sizeInBytes) override;

private:

    static float                                                MAX_FEEDBACK;
    
    std::unique_ptr<GLITCH_DELAY_EFFECT>                        m_effect;
    AudioSampleBuffer                                           m_prev_buffer;
    
    AudioParameterFloat*                                        m_mix;
    AudioParameterFloat*                                        m_loop_size;
    AudioParameterFloat*                                        m_jitter;
    AudioParameterFloat*                                        m_feedback;
    AudioParameterFloat*                                        m_low_head;
    AudioParameterFloat*                                        m_normal_head;
    AudioParameterFloat*                                        m_high_head;
    
    
    //==============================================================================
    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TeensyJuceAudioProcessor)
};
