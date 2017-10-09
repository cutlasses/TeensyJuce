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
    AudioProcessorEditor(&p),
    Slider::Listener(),
    Timer(),
    m_processor(p),
    m_param_sliders(),
    m_param_labels()
{
    const OwnedArray<AudioProcessorParameter>& params = p.getParameters();
    
    for( int i = 0; i < params.size(); ++i )
    {
        if( const AudioParameterFloat* param = dynamic_cast<AudioParameterFloat*>(params[i]) )
        {
            Slider* slider;
            
            m_param_sliders.add( slider = new Slider (param->name) );
            slider->setRange( param->range.start, param->range.end );
            slider->setSliderStyle( Slider::RotaryHorizontalDrag );
            slider->setValue( *param );
            slider->setTextBoxStyle( Slider::NoTextBox, false, 0, 0 );
            
            slider->addListener( this );
            addAndMakeVisible(slider );
            
            Label* label;
            m_param_labels.add( label = new Label (param->name, param->name) );
            label->setJustificationType( Justification::centred );
            addAndMakeVisible( label );
        }
    }
    
    const float width   = ( BORDER * 2.0f ) + DIAL_SIZE * m_param_sliders.size();
    const float height  = ( BORDER * 2.0f ) + DIAL_SIZE;
    setSize( width, height );
    
    // start the callback timer
    startTimer (100);
    
}

TeensyJuceAudioProcessorEditor::~TeensyJuceAudioProcessorEditor()
{
}

AudioParameterFloat* TeensyJuceAudioProcessorEditor::get_parameter_for_slider( Slider* slider )
{
    const OwnedArray<AudioProcessorParameter>& params = getAudioProcessor()->getParameters();
    return dynamic_cast<AudioParameterFloat*> (params[ m_param_sliders.indexOf(slider) ]);
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
    Rectangle<int> r = getLocalBounds().reduced( BORDER );
    
    for( int i = 0; i < m_param_sliders.size(); ++i )
    {
        Rectangle<int> dial_bounds          = r.removeFromLeft( DIAL_SIZE );
        const Rectangle<int> label_bounds   = dial_bounds.removeFromBottom( LABEL_HEIGHT );
        
        m_param_labels[i]->setBounds( label_bounds );
        m_param_sliders[i]->setBounds( dial_bounds );
    }
}

void TeensyJuceAudioProcessorEditor::sliderValueChanged (Slider* slider)
{
    if( AudioParameterFloat* param = get_parameter_for_slider(slider) )
    {
        *param = slider->getValue();
    }
}

void TeensyJuceAudioProcessorEditor::sliderDragStarted (Slider* slider)
{
    if( AudioParameterFloat* param = get_parameter_for_slider( slider ) )
    {
        param->beginChangeGesture();
    }
}

void TeensyJuceAudioProcessorEditor::sliderDragEnded (Slider* slider)
{
    if( AudioParameterFloat* param = get_parameter_for_slider( slider ) )
    {
        param->endChangeGesture();
    }
}

void TeensyJuceAudioProcessorEditor::timerCallback()
{
    const OwnedArray<AudioProcessorParameter>& params = getAudioProcessor()->getParameters();
    
    for( int i = 0; i < params.size(); ++i )
    {
        if( const AudioParameterFloat* param = dynamic_cast<AudioParameterFloat*> (params[i]) )
        {
            if( i < m_param_sliders.size() )
            {
                m_param_sliders[i]->setValue (*param);
            }
        }
    }
}
