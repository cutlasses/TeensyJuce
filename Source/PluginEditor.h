/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#pragma once

#include "../JuceLibraryCode/JuceHeader.h"
#include "PluginProcessor.h"


//==============================================================================
/**
*/

// forward declarations
class GLITCH_DELAY_EFFECT;

class GLITCH_DELAY_VIEW
{
    struct DELAY_HEAD_PROXY
    {
        DELAY_HEAD_PROXY() :
            m_start( 0.0f ),
            m_size( 0.0f )
        {
        }
        
        float                           m_start;
        float                           m_size;
    };
    
    std::vector< DELAY_HEAD_PROXY >     m_heads;
    
    int                                 m_tl_x;         // top left x position
    int                                 m_tl_y;
    int                                 m_width;
    int                                 m_height;
    
public:
    
    GLITCH_DELAY_VIEW( int num_heads );
    
    void                                set_dimensions( int x, int y, int width, int height );
    
    void                                update( const GLITCH_DELAY_EFFECT& );
    void                                paint( Graphics& g );
};

///////////////////////////////////////////////////////////////////////////

class TeensyJuceAudioProcessorEditor  : public AudioProcessorEditor,
                                        public Slider::Listener,
                                        private Timer
{
public:
    TeensyJuceAudioProcessorEditor (TeensyJuceAudioProcessor&, const GLITCH_DELAY_EFFECT&);
    ~TeensyJuceAudioProcessorEditor();
    
    AudioParameterFloat*                            get_parameter_for_slider( Slider* slider );

    //==============================================================================
    void paint (Graphics&) override;
    void resized() override;
    
    //// Slider::Listener ////
    void sliderValueChanged (Slider* slider) override;
    void sliderDragStarted (Slider* slider) override;
    void sliderDragEnded (Slider* slider) override;
    /////////////////////////

private:
    // This reference is provided as a quick way for your editor to
    // access the processor object that created it.
    
    static const int                                DIAL_ROW_COUNT;
    static const int                                DIAL_SIZE;
    static const int                                DIAL_SEPARATION;
    static const int                                LABEL_HEIGHT;
    static const int                                GLITCH_DELAY_HEIGHT;
    static const int                                BORDER;
    
    TeensyJuceAudioProcessor&                       m_processor;
    const GLITCH_DELAY_EFFECT&                      m_effect;
    
    OwnedArray<Slider>                              m_param_sliders;
    OwnedArray<Label>                               m_param_labels;
    
    int                                             m_num_dial_rows;
    
    std::unique_ptr<GLITCH_DELAY_VIEW>              m_glitch_view;
    
    
    
    void                                            timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TeensyJuceAudioProcessorEditor)
};
