#include "hungary.h"

int item[MAX_REQ * NUM_V];//������ݵ���Ϣ
int item_cid[MAX_REQ * NUM_V];//���item�ĳ������,item_cid[k] = car
int used[UNIT_SIZE * NUM_U];//���·�ߵ�Ԫ��ռ����Ϣ��ռ��Ϊ1��Ϊռ��Ϊ1
int match[UNIT_SIZE * NUM_U];//���·�ߵ�Ԫ��λƥ���������Ϣ
int nitem;

int get_item_num(struct vehicle *cars)//��ȡ�������ݵ�������size
{
	int i, size = 0;
	for (i = 0; i < NUM_V; i++)
		size += cars[i].nit;
	return size;
}

int has_argue(int index, struct vehicle *cars)
{
	int i, j, k, cid = item_cid[index];//��ȡ�������
	for (i = 0; i < cars[cid].npu; i++) {//�����ɴ���·�ߵ�Ԫ������
		k = cars[cid].passing_units[i] * UNIT_SIZE;//·�ߵ�Ԫ���*�洢��С����λ��used��
		for (j = 0; j < UNIT_SIZE; j++) {
			if (!used[k]) {//��������Ԫλ��û�б�ռ�ã�Ϊ0�����������UNIT_SIZE�е�used���ݣ�k++
				used[k] = 1;//ռ��
				if (match[k] == -1 || has_argue(match[k], cars)) {//��������ݻ�û��ƥ����������ڳ�һ��λ����
					match[k] = index;//ƥ�����ݴ�Ÿ����ݱ��
					return 1;
				}
			}
			k++;
		}
	}
	return 0;
}

//�������㷨��������ͼƥ���㷨
void hungary_assign(struct vehicle *cars, struct unit *units)
{
	int i, j, k, n;

	//is_hungary = 1;//����hungary�㷨

	nitem = get_item_num(cars);//��ȡ�������ݵ�������nitem

	k = 0;
	for (i = 0; i < NUM_V; i++) {//����ÿ����
		for (j = 0; j < cars[i].nit; j++) {//����һ������������������
			item[k] = cars[i].requests[j];//��ȡ�����ݵ���Ϣ
			item_cid[k++] = i;//item_cid[k] = i;k++;
		}
	}

	memset(match, -1, sizeof(match));//��ʼ��-1

	for (i = 0; i < nitem; i++) {//����ÿ�����ݣ�Ѱ��ƥ��
		memset(used, 0, sizeof(used));//��ʼ��0
		has_argue(i, cars);
	}

	//������Ϣ
	for (i = 0; i < NUM_U; i++) {//��������·�ߵ�Ԫ
		k = i * UNIT_SIZE;
		n = 0;
		for (j = 0; j < UNIT_SIZE; j++) {
			if (match[k] >= 0)//��������Ԫλ��ƥ��������
				units[i].items[n++] = item[match[k]];//units.item������ƥ�䵽�����ݱ��
			k++;
		}
		units[i].nitem = n;//��ƥ�䵽������Ϊn
	}
}

//�������
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

extern struct vehicle cars2[NUM_V];//��������
extern struct unit units2[NUM_U];//·�ߵ�Ԫ����


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
			delay2[ncycle] += total_time - ncycle * T;//�������ʱ��
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