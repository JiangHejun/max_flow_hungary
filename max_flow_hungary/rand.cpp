#include "rand.h"

void set_seed()//随机数的初始化器
{
	srand(time(NULL));
}

int rand_int()//生成随机数
{
	return rand();
}

double rand_01()//产生0到1之间的随机数
{
	return ((double) rand()) / ((double) RAND_MAX);
}

double rand_range(double min, double max)//产生min到max范围内的随机数
{
	return rand_01() * (max - min) + min;
}

/* Random int in [min, max). */
int rand_range_int(int min, int max)
{
	int base = rand(), /* [0, RAND_MAX] */
		range = max - min,
		bucket = RAND_MAX / range,
		remainder = RAND_MAX % range;
	if(base + remainder < RAND_MAX) return min + base / bucket;
	return rand_range_int(min, max);
}