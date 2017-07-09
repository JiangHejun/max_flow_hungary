#ifndef __SETTING_H__
#define __SETTING_H__ __ZIE_IUK__

#include <ctime>

/* System will run C cycles of periods that lasts for T unit time each. */
#define C 20//ʵ������
#define T 60//��������s,
#define NUM_V 259//��������
/* Number of data items. */
#define NUM_REQ 300//��������������hungary�㷨�У����Ϊһ�����Ĵ�С����Ӱ��
/* The maximum number of requests a vehicle can make during one period. */
#define MAX_REQ 3//���������������
/* Coverage radius of each vehicle. */
#define VEHICLE_RADIUS 150//����ͨ�Ű뾶
/* Coverage radius of each unit. */
#define UNIT_RADIUS 250//·�ߵ�Ԫͨ�Ű뾶
/* Storage size of each unit. */
#define UNIT_SIZE 30//�洢��Ԫ��С
/* Maximum number of v2v packets a unit can hold. */
#define MAX_V2V 10//·�ߵ�Ԫ���洢���ݰ�����
#define V2V_ENABLED 1//����ʹ��
/* Speed constraints in m/s. */
#define MIN_SPEED 10//������С�ٶ�
#define MAX_SPEED 20//��������ٶ�
/* Time of a transmission in unit time. */
#define T_TRAN 5//���������Ч����
#define MAX_PASSING_UNIT 5//����ͨ��·�ߵ�Ԫ����
#define MAX_PASSING_VEHICLE 30//����ͨ�ų�������

#define REAL_MAP 0 //1:������ʵ���ݣ�0:�������������
#define IS_HUNGARY 1//1:�����������㷨��0��������

struct coord {//������Ϣ
	double x;//����λ�� 
	double y;
	int rx, ry;//��ʵ��ͼ��������
	int dir;//����
	int uid;//������·�ߵ�Ԫ���
	double u2v_distance;//����·�ߵ�Ԫ�ͳ����ľ���
	int vid[MAX_PASSING_VEHICLE]; //�����ĳ������洢����֮����
	int num_v;//Ŀǰ��λ��ͨ�ŵĳ����ı��
	double v2v_distance[MAX_PASSING_VEHICLE]; //����֮��ľ��룬�洢֮��ľ���
};

struct v2v_pack {//����Ϣ
	struct vehicle *target_v;//Ŀ�공��
	int dir;//����
	int item;//��������
	int pickup_unit;
};

#endif