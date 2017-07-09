#ifndef __UNIT_H__
#define __UNIT_H__ __ZIE_IUK__

#include "settting.h"
#include "stdafx.h"

struct unit {//路边单元信息
	int items[UNIT_SIZE];//每个存储数据内容的大小
	struct v2v_pack v2v[MAX_V2V];//需要传输数据包数组
	int nitem;//目前传输数据内容组的定位
	int broadcasting; /* The index of the very item that is being broadcasted by one unit.广播状态，表示是否有数据需要广播 */
	int nreciever; /* Number of vehicles now recieving from this unit.目前路边单元接受到车辆请求的车辆数 */
	int nv2v;//目前传输包数组的定位
	int rx;//定义路边单元坐标，用于real_map中
	int ry;
};

void init_unit(struct unit *u);//初始化路边单元

#endif