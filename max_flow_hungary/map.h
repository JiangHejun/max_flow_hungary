#ifndef __MAP_H__
#define __MAP_H__ __ZIE_IUK__

#include "rand.h"
#include <cmath>
#include "stdafx.h"
#include <stdio.h>

/* Number of intersections. */
#define HOR_INT 10//网格格数，水平
#define VER_INT 10//网格格数，垂直

/* Length of segments in metres. */
#define HOR_LEN 1000//每个网格长度,水平
#define VER_LEN 1000//每个网格长度,垂直

//方向
#define NORTH 0//北
#define EAST 1//东
#define SOUTH 2//南
#define WEST 3//西

/* Number of roadside units.*/
#define NUM_U (HOR_INT * VER_INT)//路边单元数量，每个路口布置一个

void rand_position(struct coord *c);//随机化车辆位置，route[0]
int rand_dir(struct coord c, int coming_dir);//随机化车辆方向
int find_unit(int x, int y);//寻找可通信的路边单元
int get_u2v(double x, double y, int uid);//
int move(struct coord cur, int dir, struct coord *next, double speed);//车辆位移


#endif