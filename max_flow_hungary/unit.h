#ifndef __UNIT_H__
#define __UNIT_H__ __ZIE_IUK__

#include "settting.h"
#include "stdafx.h"

struct unit {//·�ߵ�Ԫ��Ϣ
	int items[UNIT_SIZE];//ÿ���洢�������ݵĴ�С
	struct v2v_pack v2v[MAX_V2V];//��Ҫ�������ݰ�����
	int nitem;//Ŀǰ��������������Ķ�λ
	int broadcasting; /* The index of the very item that is being broadcasted by one unit.�㲥״̬����ʾ�Ƿ���������Ҫ�㲥 */
	int nreciever; /* Number of vehicles now recieving from this unit.Ŀǰ·�ߵ�Ԫ���ܵ���������ĳ����� */
	int nv2v;//Ŀǰ���������Ķ�λ
	int rx;//����·�ߵ�Ԫ���꣬����real_map��
	int ry;
};

void init_unit(struct unit *u);//��ʼ��·�ߵ�Ԫ

#endif