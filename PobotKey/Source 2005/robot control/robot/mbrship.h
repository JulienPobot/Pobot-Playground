#include "fuzzy.h"

/*
 * dist
 *
 *  Return the fuzzified value using the dist (distance 2 target)
 *  membership function.
 *
 */

fuzzyType m_dist_very_near( float dist );


fuzzyType m_dist_near( float dist );

fuzzyType m_dist_far( float dist );


/*
 * delta_theta
 *
 *  Return the fuzzified value using the delta_theta (difference between the heading and the heading
 *  to reach the target) membership function.
 *
 */

fuzzyType m_dtheta_extreme_left( float dtheta );

fuzzyType m_dtheta_far_left( float dtheta );

fuzzyType m_dtheta_left( float dtheta );

fuzzyType m_dtheta_center( float dtheta );

fuzzyType m_dtheta_right( float dtheta );

fuzzyType m_dtheta_far_right( float dtheta );

fuzzyType m_dtheta_extreme_right( float dtheta );


/*
 * speed
 *
 *  Return the unfuzzified value using the speed ( speed of the robot )
 *  membership function.
 *
 */

fuzzyType m_speed_very_slow( float coeff );

fuzzyType m_speed_slow( float coeff );

fuzzyType m_speed_normal( float coeff );

fuzzyType m_speed_fast( float coeff );

/*
 * bias
 *
 *  Return the unfuzzified value using the bias ( bias of the robot's wheels )
 *  membership function.
 *
 */

fuzzyType m_bias_full_left( float coeff );

fuzzyType m_bias_strong_left( float coeff );

fuzzyType m_bias_left( float coeff );

fuzzyType m_bias_center( float coeff );

fuzzyType m_bias_right( float coeff );

fuzzyType m_bias_strong_right( float coeff );

fuzzyType m_bias_full_right( float coeff );
