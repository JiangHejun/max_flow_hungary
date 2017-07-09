#ifndef __SETTING_H__
#define __SETTING_H__ __ZIE_IUK__

#include <ctime>

/* System will run C cycles of periods that lasts for T unit time each. */
#define C 20//实验轮数
#define T 60//周期跳数s,
#define NUM_V 259//车辆数量
/* Number of data items. */
#define NUM_REQ 300//请求数据数量，hungary算法中，理解为一个包的大小。不影响
/* The maximum number of requests a vehicle can make during one period. */
#define MAX_REQ 3//车辆最大请求数量
/* Coverage radius of each vehicle. */
#define VEHICLE_RADIUS 150//车辆通信半径
/* Coverage radius of each unit. */
#define UNIT_RADIUS 250//路边单元通信半径
/* Storage size of each unit. */
#define UNIT_SIZE 30//存储单元大小
/* Maximum number of v2v packets a unit can hold. */
#define MAX_V2V 10//路边单元最大存储数据包数量
#define V2V_ENABLED 1//丢包使能
/* Speed constraints in m/s. */
#define MIN_SPEED 10//车辆最小速度
#define MAX_SPEED 20//车辆最大速度
/* Time of a transmission in unit time. */
#define T_TRAN 5//数据最大有效周期
#define MAX_PASSING_UNIT 5//最大可通信路边单元数量
#define MAX_PASSING_VEHICLE 30//最大可通信车辆数量

#define REAL_MAP 0 //1:启用真实数据，0:启用随机化数据
#define IS_HUNGARY 1//1:启用匈牙利算法，0：不启用

struct coord {//车辆信息
	double x;//车辆位置 
	double y;
	int rx, ry;//真实地图车辆坐标
	int dir;//方向
	int uid;//相遇的路边单元编号
	double u2v_distance;//相遇路边单元和车辆的距离
	int vid[MAX_PASSING_VEHICLE]; //相遇的车辆，存储的是之间编号
	int num_v;//目前定位能通信的车辆的编号
	double v2v_distance[MAX_PASSING_VEHICLE]; //车辆之间的距离，存储之间的距离
};

struct v2v_pack {//包信息
	struct vehicle *target_v;//目标车辆
	int dir;//方向
	int item;//数据内容
	int pickup_unit;
};

#endif