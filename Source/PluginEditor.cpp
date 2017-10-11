/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

const int TeensyJuceAudioProcessorEditor::DIAL_ROW_COUNT        = 4;
const int TeensyJuceAudioProcessorEditor::DIAL_SEPARATION       = 30;
const int TeensyJuceAudioProcessorEditor::DIAL_SIZE             = 95;
const int TeensyJuceAudioProcessorEditor::LABEL_HEIGHT          = 10;
const int TeensyJuceAudioProcessorEditor::BORDER                = 40;

TeensyJuceAudioProcessorEditor::TeensyJuceAudioProcessorEditor (TeensyJuceAudioProcessor& p) :
    AudioProcessorEditor(&p),
    Slider::Listener(),
    Timer(),
    m_processor(p),
    m_param_sliders(),
    m_param_labels(),
    m_num_dial_rows(0)
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
            label->setFont( Font(LABEL_HEIGHT) );
            addAndMakeVisible( label );
        }
    }
    
    m_num_dial_rows         = m_param_sliders.size() / DIAL_ROW_COUNT;
    if( m_param_sliders.size() % DIAL_ROW_COUNT != 0 )
    {
        ++m_num_dial_rows;
    }
    
    const float width       = ( BORDER * 2.0f ) + DIAL_SIZE * DIAL_ROW_COUNT;
    const float height      = ( BORDER * 2.0f ) + ( DIAL_SIZE * m_num_dial_rows ) + (DIAL_SEPARATION * (m_num_dial_rows - 1));
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
    Rectangle<int> reduced = getLocalBounds().reduced( BORDER );
    
    int dial = 0;
    
    for( int row = 0; row < m_num_dial_rows; ++row )
    {
        Rectangle<int> row_rect = reduced.removeFromTop( DIAL_SIZE );
        
        const int row_size      = min_val( DIAL_ROW_COUNT, m_param_sliders.size() - dial );
        const int remainder     = ( DIAL_ROW_COUNT - row_size ) * DIAL_SIZE;
        row_rect.reduce( remainder / 2, 0 );
        
        for( int col = 0; col < row_size; ++col )
        {
            Rectangle<int> dial_bounds          = row_rect.removeFromLeft( DIAL_SIZE );
            const Rectangle<int> label_bounds   = dial_bounds.removeFromBottom( LABEL_HEIGHT );
            
            m_param_labels[dial]->setBounds( label_bounds );
            m_param_sliders[dial]->setBounds( dial_bounds );
            ++dial;
        }
        
        // leave space between each row
        reduced.removeFromTop( DIAL_SEPARATION );
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
