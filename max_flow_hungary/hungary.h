#ifndef __HUNGARY_H__
#define __HUNGARY_H__ __ZIE_IUK__

#include <stdio.h>
#include <stdlib.h>
#include "vehicle.h"
#include "unit.h"

void hungary_assign(struct vehicle *cars, struct unit *units);
void vehicle_thread(struct vehicle *v,int total_time,int ncycle);

#endif