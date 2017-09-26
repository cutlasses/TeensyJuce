/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"


//==============================================================================
TeensyJuceAudioProcessorEditor::TeensyJuceAudioProcessorEditor (TeensyJuceAudioProcessor& p) :
    AudioProcessorEditor (&p),
    processor (p),
    m_mix_slider("Mix Slider")
{
    setSize (200, 200);
    
    m_mix_slider.setSliderStyle( Slider::LinearBarVertical );
    m_mix_slider.setRange( 0.0f, 127.0f, 1.0f );
    m_mix_slider.setTextBoxStyle( Slider::NoTextBox, false, 90, 0 );
    //m_mix_slider.setPopupDisplayEnabled (true, this );
    //m_mix_slider.setTextValueSuffix( " Volume");
    m_mix_slider.setValue( 1.0f );
    
    addAndMakeVisible( m_mix_slider );
}

TeensyJuceAudioProcessorEditor::~TeensyJuceAudioProcessorEditor()
{
}

//==============================================================================
void TeensyJuceAudioProcessorEditor::paint (Graphics& g)
{
    // (Our component is opaque, so we must completely fill the background with a solid colour)
    g.fillAll (getLookAndFeel().findColour (ResizableWindow::backgroundColourId));

    g.setColour (Colours::white);
    g.setFont (15.0f);
}

void TeensyJuceAudioProcessorEditor::resized()
{
    // position the components
    m_mix_slider.setBounds( 40, 30, 20, getHeight()- 60 );
}
