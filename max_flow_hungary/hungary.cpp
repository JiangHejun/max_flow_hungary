#include "hungary.h"

int item[MAX_REQ * NUM_V];//存放数据的信息
int item_cid[MAX_REQ * NUM_V];//存放item的车辆编号,item_cid[k] = car
int used[UNIT_SIZE * NUM_U];//存放路边单元的占用信息，占用为1，为占用为1
int match[UNIT_SIZE * NUM_U];//存放路边单元单位匹配的数据信息
int nitem;

int get_item_num(struct vehicle *cars)//获取请求数据的总数：size
{
	int i, size = 0;
	for (i = 0; i < NUM_V; i++)
		size += cars[i].nit;
	return size;
}

int has_argue(int index, struct vehicle *cars)
{
	int i, j, k, cid = item_cid[index];//获取车辆编号
	for (i = 0; i < cars[cid].npu; i++) {//遍历可传输路边单元的数量
		k = cars[cid].passing_units[i] * UNIT_SIZE;//路边单元编号*存储大小，定位到used中
		for (j = 0; j < UNIT_SIZE; j++) {
			if (!used[k]) {//如果这个单元位置没有被占用，为0，遍历在这个UNIT_SIZE中的used数据，k++
				used[k] = 1;//占用
				if (match[k] == -1 || has_argue(match[k], cars)) {//如果该数据还没有匹配或者是再腾出一个位置来
					match[k] = index;//匹配数据存放该数据编号
					return 1;
				}
			}
			k++;
		}
	}
	return 0;
}

//匈牙利算法，最大二部图匹配算法
void hungary_assign(struct vehicle *cars, struct unit *units)
{
	int i, j, k, n;

	//is_hungary = 1;//采用hungary算法

	nitem = get_item_num(cars);//获取请求数据的总数，nitem

	k = 0;
	for (i = 0; i < NUM_V; i++) {//遍历每辆车
		for (j = 0; j < cars[i].nit; j++) {//遍历一辆车的请求数据数量
			item[k] = cars[i].requests[j];//获取该数据的信息
			item_cid[k++] = i;//item_cid[k] = i;k++;
		}
	}

	memset(match, -1, sizeof(match));//初始化-1

	for (i = 0; i < nitem; i++) {//遍历每个数据，寻找匹配
		memset(used, 0, sizeof(used));//初始化0
		has_argue(i, cars);
	}

	//更新信息
	for (i = 0; i < NUM_U; i++) {//遍历所有路边单元
		k = i * UNIT_SIZE;
		n = 0;
		for (j = 0; j < UNIT_SIZE; j++) {
			if (match[k] >= 0)//如果这个单元位置匹配有数据
				units[i].items[n++] = item[match[k]];//units.item里面存放匹配到的数据编号
			k++;
		}
		units[i].nitem = n;//总匹配到的数据为n
	}
}

//结果计算
int contains(int key, int data[], int num)
{
	int i;
	for (i = 0; i < num; i++) {
		if (key == data[i]) return 1;
	}
	return 0;
}

int walkout_dir(struct vehicle *v,int tltime)
{
	int t, u;
	t = tltime;
	do {
		u = v->route[t].uid;
		t++;
	} while (u >= 0 && t < C * T);
	if (t < C * T) return v->route[t].dir;
	return 0;
}

void download_item(struct vehicle *v, int item)
{
	int i;
	v->nsr++;
	for (i = 0; i < v->nit; i++) { /* Clear the downloaded item. */
		if (v->requests[i] == item)
			v->requests[i] = -1;
	}
	v->recieving = -1;
}

void copy_v2v(struct v2v_pack *vt, struct v2v_pack *vs)
{
	vt->dir = vs->dir;
	vt->item = vs->item;
	vt->pickup_unit = vs->pickup_unit;
	vt->target_v = vs->target_v;
}

extern int nv2vreq2[C];
extern int tu2v2[C];
extern int tv2v2[C];

extern int pass2[C];
extern int delay2[C];
extern int v2v_num2[C];
extern int lost_num2[C];

extern struct vehicle cars2[NUM_V];//车辆数组
extern struct unit units2[NUM_U];//路边单元数组


void vehicle_thread(struct vehicle *v,int total_time,int ncycle)
{
	int i, uid, wd, range_v2v = VEHICLE_RADIUS * VEHICLE_RADIUS, dx, dy;

	uid = v->route[total_time].uid;

	if (v->recieving >= 0) {
		if (v->timeout > 0) { /* If still transfering something. */
			--v->timeout;
			/* Check if going out of the connecting area. */
			if (uid < 0) {
				uid = v->route[total_time - 1].uid;
				if (V2V_ENABLED) {
					units2[uid].v2v[units2[uid].nv2v].item = v->recieving;
					units2[uid].v2v[units2[uid].nv2v].dir = v->route[total_time].dir;
					units2[uid].v2v[units2[uid].nv2v].target_v = v;
					units2[uid].v2v[units2[uid].nv2v].pickup_unit = uid;
					units2[uid].nv2v++;
				}
				units2[uid].nreciever--;
				if (units2[uid].nreciever == 0)
					units2[uid].broadcasting = -1;
				lost_num2[ncycle]++;
				v->recieving = -1;
			}
		}
		else {
			download_item(v, v->recieving);
			tu2v2[ncycle]++;
			delay2[ncycle] += total_time - ncycle * T;//算的是总时延
			if (uid < 0) uid = v->route[total_time - 1].uid;
			units2[uid].nreciever--;
			if (units2[uid].nreciever == 0)
				units2[uid].broadcasting = -1;
		}
		return; /* If not timeout, keep downloading. If timeout, we assume it will rest for a while. */
	}

	if (uid >= 0) {
		if (units2[uid].broadcasting < 0) {
			/* First try to look for downloads. */
			for (i = 0; i < v->nit; i++) {
				if (v->requests[i] == -1) continue;
				if (contains(v->requests[i], units2[uid].items, units2[uid].nitem)) {
					units2[uid].broadcasting = v->requests[i];
					units2[uid].nreciever = 1;
					v->timeout = T_TRAN;
					v->recieving = v->requests[i];
					return;
				}
			}

			/* Then try to take a v2v pack. */
			if (V2V_ENABLED) {
				if (v->v2v.item >= 0 && v->v2v.pickup_unit != uid)
					v->v2v.item = -1;
				if (v->v2v.item == -1) {
					wd = walkout_dir(v, total_time);
					for (i = 0; i < units2[uid].nv2v; i++) {
						if (wd == units2[uid].v2v[i].dir) {
							copy_v2v(&v->v2v, &units2[uid].v2v[i]);
							nv2vreq2[ncycle]++;
							tu2v2[ncycle]++;
							return;
						}
					}
				}
			}
			/* Drop through. */
		}
		else {
			if (contains(units2[uid].broadcasting, v->requests, v->nit)) {
				v->timeout = T_TRAN;
				v->recieving = units2[uid].broadcasting;
				units2[uid].nreciever++;
				return;
			}
		}
	}

	/* V2V. */
	if (V2V_ENABLED && v->v2v.item >= 0) {
		for (i = 0; i < NUM_V; i++) {
			if ((v == &cars2[i]) || (cars2[i].recieving >= 0)) continue;
			dx = cars2[i].route[total_time].x - v->route[total_time].x;
			dy = cars2[i].route[total_time].y - v->route[total_time].y;
			if (dx * dx + dy * dy < range_v2v) {
				if (v->v2v.target_v == &cars2[i]) {
					download_item(v->v2v.target_v, v->v2v.item);
					tv2v2[ncycle]++;
					v->v2v.item = -1;
					v2v_num2[ncycle]++;
					return;
				}

				if (cars2[i].recieving >= 0 || cars2[i].v2v.item >= 0 || cars2[i].route[total_time].dir != v->v2v.dir) continue;

				switch (v->v2v.dir) {
				case NORTH: if (cars2[i].route[total_time].y >= v->route[total_time].y) break;
				case EAST: if (cars2[i].route[total_time].x <= v->route[total_time].x) break;
				case SOUTH: if (cars2[i].route[total_time].y <= v->route[total_time].y) break;
				case WEST: if (cars2[i].route[total_time].y >= v->route[total_time].x) break;
				default:
					copy_v2v(&cars2[i].v2v, &v->v2v);
					v->v2v.item = -1;
					pass2[ncycle]++;
					tv2v2[ncycle]++;
					return;
				}
			}
		}
	}
}