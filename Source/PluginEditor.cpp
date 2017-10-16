/*
  ==============================================================================

    This file was auto-generated!

    It contains the basic framework code for a JUCE plugin editor.

  ==============================================================================
*/

#include "GlitchDelayEffect.h"
#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================

GLITCH_DELAY_VIEW::GLITCH_DELAY_VIEW( int num_heads ) :
    m_tl_x(0),
    m_tl_y(0),
    m_width(0),
    m_height(0)
{
    m_heads.resize( num_heads, DELAY_HEAD_PROXY() );
    m_heads.back().m_write_head = true;
}

void GLITCH_DELAY_VIEW::set_dimensions( int x, int y, int width, int height )
{
    m_tl_x      = x;
    m_tl_y      = y;
    m_width     = width;
    m_height    = height;
}

void GLITCH_DELAY_VIEW::update( const GLITCH_DELAY_EFFECT& effect )
{
    for( int h = 0; h < m_heads.size(); ++h )
    {
        DELAY_HEAD_PROXY& head  = m_heads[h];
        
        effect.head_ratio_details( h, head.m_start, head.m_end, head.m_current_position );
    }
}

void GLITCH_DELAY_VIEW::paint( Graphics& g )
{
    g.setColour( Colours::aquamarine );
    g.drawRect( m_tl_x, m_tl_y, m_width, m_height );
   
    g.setColour( Colours::white );
    
    for( const DELAY_HEAD_PROXY& head : m_heads )
    {
        Point<int> tl, br;
        if( head.m_start == head.m_end )
        {
            // write head (or head without loop) - fills the entire area in height
            if( head.m_write_head )
            {
                g.setColour( Colours::goldenrod );
            }
            tl = Point<int>( m_tl_x + roundToInt( head.m_current_position * m_width ), m_tl_y );
            br = Point<int>( tl.getX() + 5, tl.getY() + m_height );
        }
        else
        {
            // draw the current position
            const Point<int>cp_tl( m_tl_x + roundToInt( head.m_current_position * m_width ), m_tl_y );
            const Point<int>cp_br ( cp_tl.getX() + 1, cp_tl.getY() + m_height );
            const Rectangle<int> cp_rect( cp_tl, cp_br );
            g.fillRect( cp_rect );
            
            g.setColour( Colours::lightgrey );
            const float height_ratio    = 0.65f;
            const int loop_height       = roundToInt( m_height * height_ratio );
            const int height_offset     = ( m_height - loop_height ) / 2;
            tl = Point<int>( m_tl_x + roundToInt( head.m_start * m_width ), m_tl_y + height_offset );
            br = Point<int>( m_tl_x + roundToInt( head.m_end * m_width ), tl.getY() + loop_height );
        }
        
        if( tl.x < br.x )
        {
            const Rectangle<int> head_rect( tl, br );
            
            g.fillRect( head_rect );
        }
        else
        {
            // region crosses the end of the buffer - draw 2 rects
            const Point<int> new_br( m_tl_x + m_width, br.y );
            const Rectangle<int> head_rect1( tl, new_br );
            
            g.fillRect( head_rect1 );
            
            const Point<int> new_tl( m_tl_x, tl.y );
            const Rectangle<int> head_rect2( new_tl, br );
            
            g.fillRect( head_rect2 );
        }
    }
}

///////////////////////////////////////////////////////////////////////////

const int TeensyJuceAudioProcessorEditor::DIAL_ROW_COUNT        = 4;
const int TeensyJuceAudioProcessorEditor::DIAL_SEPARATION       = 30;
const int TeensyJuceAudioProcessorEditor::DIAL_SIZE             = 95;
const int TeensyJuceAudioProcessorEditor::LABEL_HEIGHT          = 10;
const int TeensyJuceAudioProcessorEditor::GLITCH_DELAY_HEIGHT   = 60;
const int TeensyJuceAudioProcessorEditor::BORDER                = 40;

TeensyJuceAudioProcessorEditor::TeensyJuceAudioProcessorEditor (TeensyJuceAudioProcessor& p, const GLITCH_DELAY_EFFECT& effect) :
    AudioProcessorEditor(&p),
    Slider::Listener(),
    Timer(),
    m_processor(p),
    m_effect(effect),
    m_param_sliders(),
    m_param_labels(),
    m_num_dial_rows(0),
    m_glitch_view(nullptr)
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
    
    m_glitch_view           = make_unique<GLITCH_DELAY_VIEW>( effect.num_heads() );
    
    m_num_dial_rows         = m_param_sliders.size() / DIAL_ROW_COUNT;
    if( m_param_sliders.size() % DIAL_ROW_COUNT != 0 )
    {
        ++m_num_dial_rows;
    }
    
    const float width       = ( BORDER * 2.0f ) + DIAL_SIZE * DIAL_ROW_COUNT;
    const float height      = ( BORDER * 2.0f ) + ( DIAL_SIZE * m_num_dial_rows ) + (DIAL_SEPARATION * m_num_dial_rows) + GLITCH_DELAY_HEIGHT;
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
    
    m_glitch_view->paint(g);
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
    
    Rectangle<int> glitch_rect = reduced.removeFromTop( GLITCH_DELAY_HEIGHT );
    Point<int> glitch_top_left = glitch_rect.getTopLeft();
    
    m_glitch_view->set_dimensions( glitch_top_left.getX(), glitch_top_left.getY(), glitch_rect.getWidth(), glitch_rect.getHeight() );
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
                m_param_sliders[i]->setValue(*param);
            }
        }
    }
    
    m_glitch_view->update( m_effect );
    
    repaint();
}
