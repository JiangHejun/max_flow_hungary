#include"real_map.h"

#define NONE -1
#define ROADSIDE_UNITS "roadunits.txt"

extern vehicle cars[NUM_V];
extern unit units[NUM_U];

void readfile() {
	int i, j;
	string x, y;//���꣬����ֱ�Ӷ���double���ͣ�С����ȡ����
	string filename;
	string s1;

	//¼�복���켣����
	for (i = 0; i < NUM_V; i++) {
		filename = to_string(i + 1) + ".txt";//to_string:int-->string
		ifstream filein(filename);//���ļ�
		for (j = 0; getline(filein, s1); j++) {//�����ݴ���
			istringstream sin(s1);//����sin��
			sin >> x >> y;
			x.erase(3, 1);//ɾ��С���㣬���ַ����±�3��ʼɾ��1���ַ�
			y.erase(2, 1);
			cars[i].route[j].rx = atoi(x.c_str());//���복������
			cars[i].route[j].ry = atoi(y.c_str());
			//cout << cars[i].route[j].rx << "  " << cars[i].route[j].ry << endl;
		}
		//cout << filename << "  " << j << endl;
		//system("pause");
	}

	//¼��·�ߵ�Ԫ����
	filename = ROADSIDE_UNITS;//to_string:int-->string
	ifstream filein(filename);//���ļ�
	for (i = 0; i < NUM_U; i++) {
		getline(filein, s1);
		istringstream sin(s1);//����sin��
		sin >> x >> y;
		x.erase(3, 1);//ɾ��С���㣬���ַ����±�3��ʼɾ��1���ַ�
		y.erase(2, 1);
		units[i].rx = atoi(x.c_str());
		units[i].ry = atoi(y.c_str());
		//cout << i << ":" << units[i].rx << "  " << units[i].ry << endl;
	}
}

void getmeet() {
	int i, j, t;
	for (i = 0; i < NUM_V; i++) {//���г���
		for (j = 0; j < C*T; j++) {//����ʱ��
			//��ȡ����·�ߵ�Ԫ
			int flag = 0;
			for (t = 0; t < NUM_U; t++) {//����·�ߵ�Ԫ
				int x = cars[i].route[j].rx - units[t].rx;
				int y = cars[i].route[j].ry - units[t].ry;
				double d = sqrt(pow(x, 2) + pow(y, 2));
				if (d < UNIT_RADIUS) {
					cars[i].route[j].uid = t;//¼��·�ߵ�Ԫ���
					cars[i].route[j].u2v_distance = d;//¼��·�ߵ�Ԫ����
					flag++;
					break;
				}
			}
			if (flag == 0)
				cars[i].route[j].uid = NONE;
			//��ȡ��������
			for (t = 0; t < NUM_V; t++) {//��������
				int x = cars[i].route[j].rx - cars[t].route[j].rx;
				int y = cars[i].route[j].ry - cars[t].route[j].ry;
				double d = sqrt(pow(x, 2) + pow(y, 2));
				if (d < VEHICLE_RADIUS && i != t && cars[i].route[j].num_v < MAX_PASSING_VEHICLE) {
					cars[i].route[j].vid[cars[i].route[j].num_v] = t;//¼�복�����
					cars[i].route[j].v2v_distance[cars[i].route[j].num_v] = d;//¼�복������
					cars[i].route[j].num_v++;
					if (cars[i].route[j].num_v == MAX_PASSING_VEHICLE)//������������ǰ����
						break;
				}
			}
		}
	}
}