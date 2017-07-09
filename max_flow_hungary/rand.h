#ifndef __RAND_H__
#define __RAND_H__ __ZIE_IUK__

#include <stdlib.h>
#include "settting.h"

void set_seed();
int rand_int();
double rand_01();
double rand_range(double min, double max);
int rand_range_int(int min, int max);

#endif