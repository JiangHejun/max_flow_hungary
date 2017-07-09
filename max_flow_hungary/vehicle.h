#ifndef __VEHICLE_H__
#define __VEHICLE_H__ __ZIE_IUK__

#include "map.h"
//#include <cstring>

struct vehicle {//��������
	struct coord route[C * T];//����·��
	int requests[MAX_REQ];//������������
	int passing_units[MAX_PASSING_UNIT];//����·�ߵ�Ԫ����
	//int passing_vehicle[1];
	struct v2v_pack v2v;//���ݰ�
	double speed; /* m/s. �����ٶ�*/
	int nit; /* Number of requests. �������ݵ�����*/
	int nsr; /* Number of satisfied requests.���������ݵ����� */
	int recieving; /* The item that is recieving, -1 if none. �������ڴ������ݣ�-1��ʾ��*/
	int timeout; /* Timeout for transfering one item. ���䳬ʱ*/
	int npu; /* Number of passing units. �ɴ���·�ߵ�Ԫ������*/
};

void clean_vehicle(struct vehicle *v);
void init_vehicle(struct vehicle *v);
void get_passing_units(struct vehicle *v, int l, int r);
void make_requests(struct vehicle *v);
int find_vehicle(struct vehicle *v, int t, double x, double y);
void V2V(struct vehicle *v);

#endif