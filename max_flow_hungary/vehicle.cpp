#include "vehicle.h"



void clean_vehicle(struct vehicle *v) //������Ϣ��ʼ��
{
	v->nsr = 0;
	v->nit = 0;
	v->recieving = -1;
	v->npu = 0;
	v->v2v.item = -1;
}

//��ʼ������·��
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

//���ҿ�ͨ�ų���
int find_vehicle(struct vehicle *v, int t, double x, double y){
	double distance = sqrt((x - v->route[t].x)*(x - v->route[t].x)+(y - v->route[t].y)*(y - v->route[t].y));
	if(distance < VEHICLE_RADIUS){//���С�ڳ���ͨ�Ű뾶
		return distance;//���ؾ���
	}
	else return 0;//�����㷵��0
}  


/* Get the passing units in time range [l, r].��ȡ��ͨ�ŵ�·�ߵ�Ԫ */
void get_passing_units(struct vehicle *v, int l, int r)
{
	int last_u = -1, u;
	memset(v->passing_units,-1,sizeof(v->passing_units));
	v->npu = 0;
	while(l <= r) {
		u = v->route[l].uid;
		if(u >= 0 && u != last_u) {//����Ƿ�Ϊͬһ��·�ߵ�Ԫ
			v->passing_units[v->npu++] = u;//��������·�ߵ�Ԫ����
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

//����ͨ��
extern vehicle cars[NUM_V];
void V2V(struct vehicle *v) {
	for (int t = 0; t<T; t++) {//��������ʱ��
		for (int j = 0; j<NUM_V; j++) {//�������г���
			int distance = find_vehicle(&cars[j], t, v->route[t].x, v->route[t].y);//����ͨ�ų�������
			if (distance>0 && v->route[t].num_v < MAX_PASSING_VEHICLE) {//ͨ�ž������0������ͨ������δ��
				v->route[t].vid[v->route[t].num_v] = j;//¼�������������
				v->route[t].v2v_distance[v->route[t].num_v] = distance;//¼���ͨ�ų�������
				v->route[t].num_v++;//��ǰ����������λ��һ		
			}
		}
	}
}