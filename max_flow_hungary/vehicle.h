#ifndef __VEHICLE_H__
#define __VEHICLE_H__ __ZIE_IUK__

#include "map.h"
//#include <cstring>

struct vehicle {//车辆数组
	struct coord route[C * T];//车辆路径
	int requests[MAX_REQ];//车辆请求数组
	int passing_units[MAX_PASSING_UNIT];//传输路边单元数组
	//int passing_vehicle[1];
	struct v2v_pack v2v;//数据包
	double speed; /* m/s. 车辆速度*/
	int nit; /* Number of requests. 请求数据的数量*/
	int nsr; /* Number of satisfied requests.已满足数据的数量 */
	int recieving; /* The item that is recieving, -1 if none. 车辆正在传输数据，-1表示无*/
	int timeout; /* Timeout for transfering one item. 传输超时*/
	int npu; /* Number of passing units. 可传输路边单元的数量*/
};

void clean_vehicle(struct vehicle *v);
void init_vehicle(struct vehicle *v);
void get_passing_units(struct vehicle *v, int l, int r);
void make_requests(struct vehicle *v);
int find_vehicle(struct vehicle *v, int t, double x, double y);
void V2V(struct vehicle *v);

#endif