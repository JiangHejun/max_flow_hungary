#include "vehicle.h"



void clean_vehicle(struct vehicle *v) //车辆信息初始化
{
	v->nsr = 0;
	v->nit = 0;
	v->recieving = -1;
	v->npu = 0;
	v->v2v.item = -1;
}

//初始化车辆路径
void init_vehicle(struct vehicle *v)
{
	int end_time = C * T, t, dir;
	v->speed = rand_range(MIN_SPEED, MAX_SPEED);
	clean_vehicle(v);
	rand_position(&v->route[0]);
	dir = rand_dir(v->route[0], -1);
	for(t = 1; t < end_time; t++) {
		//v->route[t-1].num_v = 0;
		v->route[t - 1].dir = dir;
		v->route[t - 1].uid = find_unit(v->route[t - 1].x, v->route[t - 1].y);
		v->route[t - 1].u2v_distance = get_u2v(v->route[t - 1].x, v->route[t - 1].y, v->route[t - 1].uid);
		dir = move(v->route[t - 1], dir, &v->route[t], v->speed);
	}
	v->route[end_time - 1].dir = dir;
	v->route[end_time - 1].uid = find_unit(v->route[end_time - 1].x, v->route[end_time - 1].y);
}

//查找可通信车辆
int find_vehicle(struct vehicle *v, int t, double x, double y){
	double distance = sqrt((x - v->route[t].x)*(x - v->route[t].x)+(y - v->route[t].y)*(y - v->route[t].y));
	if(distance < VEHICLE_RADIUS){//如果小于车辆通信半径
		return distance;//返回距离
	}
	else return 0;//不满足返回0
}  


/* Get the passing units in time range [l, r].获取可通信的路边单元 */
void get_passing_units(struct vehicle *v, int l, int r)
{
	int last_u = -1, u;
	memset(v->passing_units,-1,sizeof(v->passing_units));
	v->npu = 0;
	while(l <= r) {
		u = v->route[l].uid;
		if(u >= 0 && u != last_u) {//检查是否为同一个路边单元
			v->passing_units[v->npu++] = u;//存入相遇路边单元数组
			last_u = u;
		}
		++l;
	}
}


/* Make random number of random requests. */
void make_requests(struct vehicle *v)
{
	int i, cnt, item;
	memset(v->requests,-1,sizeof(v->requests));
	v->nit = rand_range_int(1, MAX_REQ + 1);
	cnt = 0;
	while (cnt < v->nit) {
		item = rand_range_int(0, NUM_REQ);
		for(i = 0; i < cnt && v->requests[i] != item; i++);
		if(i == cnt) v->requests[cnt++] = item;
	}
}

//车车通信
extern vehicle cars[NUM_V];
void V2V(struct vehicle *v) {
	for (int t = 0; t<T; t++) {//遍历所有时刻
		for (int j = 0; j<NUM_V; j++) {//遍历所有车辆
			int distance = find_vehicle(&cars[j], t, v->route[t].x, v->route[t].y);//返回通信车辆距离
			if (distance>0 && v->route[t].num_v < MAX_PASSING_VEHICLE) {//通信距离大于0，并且通信数组未满
				v->route[t].vid[v->route[t].num_v] = j;//录入相遇车辆编号
				v->route[t].v2v_distance[v->route[t].num_v] = distance;//录入可通信车辆距离
				v->route[t].num_v++;//当前相遇车辆定位加一		
			}
		}
	}
}