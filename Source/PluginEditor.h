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
class TeensyJuceAudioProcessorEditor  : public AudioProcessorEditor,
                                        public Slider::Listener,
                                        private Timer
{
public:
    TeensyJuceAudioProcessorEditor (TeensyJuceAudioProcessor&);
    ~TeensyJuceAudioProcessorEditor();
    
    AudioParameterFloat*        get_parameter_for_slider( Slider* slider );

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
    
    static const int            DIAL_ROW_COUNT;
    static const int            DIAL_SIZE;
    static const int            LABEL_HEIGHT;
    static const int            BORDER;
    
    TeensyJuceAudioProcessor&   m_processor;
    
    OwnedArray<Slider>          m_param_sliders;
    OwnedArray<Label>           m_param_labels;
    
    int                         m_num_dial_rows;
    
    void                        timerCallback() override;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR (TeensyJuceAudioProcessorEditor)
};
