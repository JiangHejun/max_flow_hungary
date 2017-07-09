#ifndef __MAP_H__
#define __MAP_H__ __ZIE_IUK__

#include "rand.h"
#include <cmath>
#include "stdafx.h"
#include <stdio.h>

/* Number of intersections. */
#define HOR_INT 10//���������ˮƽ
#define VER_INT 10//�����������ֱ

/* Length of segments in metres. */
#define HOR_LEN 1000//ÿ�����񳤶�,ˮƽ
#define VER_LEN 1000//ÿ�����񳤶�,��ֱ

//����
#define NORTH 0//��
#define EAST 1//��
#define SOUTH 2//��
#define WEST 3//��

/* Number of roadside units.*/
#define NUM_U (HOR_INT * VER_INT)//·�ߵ�Ԫ������ÿ��·�ڲ���һ��

void rand_position(struct coord *c);//���������λ�ã�route[0]
int rand_dir(struct coord c, int coming_dir);//�������������
int find_unit(int x, int y);//Ѱ�ҿ�ͨ�ŵ�·�ߵ�Ԫ
int get_u2v(double x, double y, int uid);//
int move(struct coord cur, int dir, struct coord *next, double speed);//����λ��


#endif