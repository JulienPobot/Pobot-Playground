#ifndef UTILS_H
#define UTILS_H

#include "WProgram.h"

/**
 *  signed maxmimum : both signs are tested
 */
#define S_MAX(to_saturate, value_max)    \
{                                     \
   if (to_saturate > value_max)          \
     to_saturate = value_max;            \
   else if (to_saturate < -value_max)    \
     to_saturate = -value_max;           \
}


#endif
