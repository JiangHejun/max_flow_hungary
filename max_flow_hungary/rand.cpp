#include "rand.h"

void set_seed()//������ĳ�ʼ����
{
	srand(time(NULL));
}

int rand_int()//���������
{
	return rand();
}

double rand_01()//����0��1֮��������
{
	return ((double) rand()) / ((double) RAND_MAX);
}

double rand_range(double min, double max)//����min��max��Χ�ڵ������
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