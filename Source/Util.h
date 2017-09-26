#pragma once

#include "CompileSwitches.h"

#ifdef DEBUG_OUTPUT

#ifdef TARGET_TEENSY

extern bool serial_port_initialised;

bool _assert_fail( const char* assert, const char* msg )
{
  if( serial_port_initialised )
  {
    Serial.print(assert);
    Serial.print(" ");
    Serial.print(msg);
    Serial.print("\n");
  }
  
  return true;
}

#define ASSERT_MSG(x, msg) ((void)((x) || (_assert_fail(#x,msg))))
#define DEBUG_TEXT(x) if(serial_port_initialised) Serial.print(x);

#else // !TARGET_TEENSY

#include <cassert>
#include <stdio>

#define ASSERT_MSG(x, msg)      assert( x )

#define DEBUG_TEXT(x) // TODO write a templated printf

#endif // !TARGET_TEENSY

#else // !DEBUG_OUPUT

#define ASSERT_MSG(x, msg)
#define DEBUG_TEXT(x)

#endif // !DEBUG_OUTPUT


#ifdef TARGET_JUCE
#include <stdint.h>
#endif

/////////////////////////////////////////////////////

template <typename T>
T clamp( const T& value, const T& min, const T& max )
{
  if( value < min )
  {
    return min;
  }
  if( value > max )
  {
    return max;
  }
  return value;
}

template <typename T>
T max_val( const T& v1, const T& v2 )
{
  if( v1 > v2 )
  {
    return v1;
  }
  else
  {
    return v2;
  }
}

template <typename T>
T min_val( const T& v1, const T& v2 )
{
  if( v1 < v2 )
  {
    return v1;
  }
  else
  {
    return v2;
  }
}

/////////////////////////////////////////////////////

template <typename T>
T lerp( const T& v1, const T& v2, float t )
{
  return v1 + ( (v2 - v1) * t );
}

/////////////////////////////////////////////////////

int trunc_to_int( float v )
{
  return static_cast<int>( v );
}

/////////////////////////////////////////////////////

template < typename TYPE, int CAPACITY >
class RUNNING_AVERAGE
{
  TYPE                    m_values[ CAPACITY ];
  int                     m_current;
  int                     m_size;

public:

  RUNNING_AVERAGE() :
    m_values(),
    m_current(0),
    m_size(0)
  {
  }

  void add( TYPE value )
  {
    m_values[ m_current ] = value;
    m_current             = ( m_current + 1 ) % CAPACITY;
    ++m_size;
    if( m_size > CAPACITY )
    {
      m_size              = CAPACITY;
    }
  }

  void reset()
  {
    m_size                = 0;
    m_current             = 0;
  }
  
  TYPE average() const
  {
    if( m_size == 0 )
    {
      return 0;  
    }
    
    TYPE avg = 0;
    for( int x = 0; x < m_size; ++x )
    {
      avg += m_values[ x ];
    }

    return avg / m_size;
  }

  int size() const
  {
    return m_size;
  }
};

#ifdef TARGET_JUCE
/*
#include "maths/juce_Random.h"
int random( int max )
{
    static Random r;
    static_cast<int>( r.nextFloat() * max );
}
 */
#include <stdlib.h>
int random( int max )
{
    return static_cast<int>( ( static_cast<float>( rand() ) / RAND_MAX ) * max );
}

#endif
