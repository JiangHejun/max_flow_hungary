#include "unit.h"

void init_unit(struct unit *u)
{
	u->nitem = 0;
	u->broadcasting = -1;
	u->nreciever = 0;
	u->nv2v = 0;
}