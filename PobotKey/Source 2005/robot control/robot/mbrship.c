/*
 *  Fuzzy Logic Membership Functions
 *
 *  ./software/ch9/membership.c
 *
 *  mtj@cogitollc.com
 *
 */
#include <math.h>
#include "fuzzy.h"

#define STOP      0.0
#define VERY_SLOW 0.1
#define SLOW      0.3
#define NORMAL    0.6
#define FAST      1.0

#define FULL_LEFT 	0.9
#define STRONG_LEFT 	0.7
#define LEFT        	0.3
#define CENTER			0.0
#define RIGHT			0.3
#define STRONG_RIGHT 0.7
#define FULL_RIGHT	0.9


/*
 *  normalize()
 *
 *  Normalize the fuzzy logic value.
 *
 */

int normalize( fuzzyType in )
{
  if (in >= 0.5) return 1;
  else return 0;
}


/*
 *  spikeProfile()
 *
 *  Return the fuzzified value given the spike profile.
 *
 */

fuzzyType spikeProfile( float value, float lo, float high )
{
  float peak;

  value += (-lo);

  if      ((lo < 0) && (high < 0)) {
    high = -(high - lo);
  } else if ((lo < 0) && (high > 0)) {
    high += -lo;
  } else if ((lo > 0) && (high > 0)) {
    high -= lo;
  }

  peak = (high / 2.0);
  lo = 0.0;

  if        (value < peak) {
    return( value / peak );
  } else if (value > peak) {
    return( (high-value) / peak );
  }

  return 1.0;
}


/*
 *  plateauProfile()
 *
 *  Return the fuzzified value given the plateau profile.
 *
 */

fuzzyType plateauProfile( float value, float lo, float lo_plat,
                          float hi_plat, float hi )
{
  float upslope;
  float downslope;

  value += (-lo);

  if (lo < 0.0) {
    lo_plat += -lo;  hi_plat += -lo;
    hi      += -lo;  lo       = 0;
  } else {
    lo_plat -= lo;  hi_plat -= lo;
    hi      -= lo;  lo       = 0;
  }

  upslope = (1.0 / (lo_plat - lo));
  downslope = (1.0 / (hi - hi_plat));

  if      (value < lo) return 0.0;
  else if (value > hi) return 0.0;
  else if ((value >= lo_plat) && (value <= hi_plat)) return 1.0;
  else if (value < lo_plat) return ((value-lo) * upslope);
  else if (value > hi_plat) return ((hi-value) * downslope);

  return 0.0;
}


/*
 * dist
 *
 *  Return the fuzzified value using the dist (distance 2 target)
 *  membership function.
 *
 */

fuzzyType m_dist_very_near( float dist )
{
  const float lo = 35.0;
  const float lo_plat = 35.0;
  const float hi_plat = 35.0;
  const float hi = 45.0;

  if (dist < lo) return 1.0;
  if (dist > hi) return 0.0;

  return plateauProfile( dist, lo, lo_plat, hi_plat, hi );
}


fuzzyType m_dist_near( float dist )
{
  const float lo = 35.0;

  const float hi = 70.0;

  if (dist < lo) return 0.0;
  if (dist > hi) return 0.0;

  return spikeProfile( dist, lo, hi );
}

fuzzyType m_dist_far( float dist )
{
  const float lo = 60.0;
  const float lo_plat = 70.0;
  const float hi_plat = 70.0;
  const float hi = 70.0;

  if (dist < lo) return 0.0;
  if (dist > hi) return 1.0;

  return plateauProfile( dist, lo, lo_plat, hi_plat, hi );
}


/*
 * delta_theta
 *
 *  Return the fuzzified value using the delta_theta (difference between the heading and the heading
 *  to reach the target) membership function.
 *
 */

fuzzyType m_dtheta_extreme_left( float dtheta )
{
  const float lo = M_PI/3;
  const float lo_plat = M_PI_2;
  const float hi_plat = M_PI_2 ;
  const float hi = M_PI_2;

  if (dtheta < lo) return 0.0;
  if (dtheta > hi) return 1.0;

  return plateauProfile( dtheta, lo, lo_plat, hi_plat, hi );
}

fuzzyType m_dtheta_far_left( float dtheta )
{
  const float lo = M_PI / 6;
  const float lo_plat = 3 * M_PI / 12 ;
  const float hi_plat = M_PI / 3 ;
  const float hi = 5*M_PI/12;

  if (dtheta < lo) return 0.0;
  if (dtheta > hi) return 0.0;

  return plateauProfile( dtheta, lo, lo_plat, hi_plat, hi );
}

fuzzyType m_dtheta_left( float dtheta )
{
  const float lo = 0;
  const float lo_plat = M_PI / 12 ;
  const float hi_plat = M_PI / 6 ;
  const float hi = 3*M_PI/12;

  if (dtheta < lo) return 0.0;
  if (dtheta > hi) return 0.0;

  return plateauProfile( dtheta, lo, lo_plat, hi_plat, hi );
}


fuzzyType m_dtheta_center( float dtheta )
{
  const float lo = -  M_PI / 12;
  const float hi = M_PI / 12;

  if (dtheta < lo) return 0.0;
  if (dtheta > hi) return 0.0;

  return spikeProfile( dtheta, lo, hi );
}

fuzzyType m_dtheta_right( float dtheta )
{
  const float lo = - 3*M_PI/12;
  const float lo_plat = - M_PI / 6 ;
  const float hi_plat = - M_PI / 12 ;
  const float hi = 0;

  if (dtheta < lo) return 0.0;
  if (dtheta > hi) return 0.0;

  return plateauProfile( dtheta, lo, lo_plat, hi_plat, hi );
}

fuzzyType m_dtheta_far_right( float dtheta )
{
  const float lo = - 5 * M_PI/12;
  const float lo_plat = -  M_PI / 3 ;
  const float hi_plat = - 3*  M_PI / 12 ;
  const float hi = - M_PI /6;

  if (dtheta < lo) return 0.0;
  if (dtheta > hi) return 0.0;

  return plateauProfile( dtheta, lo, lo_plat, hi_plat, hi );
}

fuzzyType m_dtheta_extreme_right( float dtheta )
{
  const float lo = - M_PI_2;
  const float lo_plat = - M_PI_2;
  const float hi_plat = - M_PI_2 ;
  const float hi = - M_PI / 3;

  if (dtheta < lo) return 1.0;
  if (dtheta > hi) return 0.0;

  return plateauProfile( dtheta, lo, lo_plat, hi_plat, hi );
}


/*
 * speed
 *
 *  Return the unfuzzified value using the speed ( speed of the robot )
 *  membership function.
 *
 */

fuzzyType m_speed_stop( float coeff )
{
  return STOP ;
}

fuzzyType m_speed_very_slow( float coeff )
{
  return coeff * VERY_SLOW ;
}

fuzzyType m_speed_slow( float coeff )
{
  return coeff * SLOW ;
}

fuzzyType m_speed_normal( float coeff )
{
  return coeff * NORMAL ;
}

fuzzyType m_speed_fast( float coeff )
{
  return coeff * FAST;
}

/*
 * bias
 *
 *  Return the unfuzzified value using the bias ( bias of the robot's wheels )
 *  membership function.
 *
 */

fuzzyType m_bias_full_left( float coeff )
{
  return coeff * FULL_LEFT ;
}

fuzzyType m_bias_strong_left( float coeff )
{
  return coeff * STRONG_LEFT ;
}

fuzzyType m_bias_left( float coeff )
{
  return coeff * LEFT ;
}

fuzzyType m_bias_center( float coeff )
{
  return coeff * CENTER ;
}

fuzzyType m_bias_right( float coeff )
{
  return - coeff * RIGHT ;
}

fuzzyType m_bias_strong_right( float coeff )
{
  return - coeff * STRONG_RIGHT ;
}

fuzzyType m_bias_full_right( float coeff )
{
  return - coeff * FULL_RIGHT ;
}
