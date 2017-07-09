#include"real_map.h"

#define NONE -1
#define ROADSIDE_UNITS "roadunits.txt"

extern vehicle cars[NUM_V];
extern unit units[NUM_U];

void readfile() {
	int i, j;
	string x, y;//坐标，不能直接定义double类型，小数点取不完
	string filename;
	string s1;

	//录入车辆轨迹坐标
	for (i = 0; i < NUM_V; i++) {
		filename = to_string(i + 1) + ".txt";//to_string:int-->string
		ifstream filein(filename);//打开文件
		for (j = 0; getline(filein, s1); j++) {//行数据处理
			istringstream sin(s1);//定义sin流
			sin >> x >> y;
			x.erase(3, 1);//删除小数点，从字符串下标3开始删除1个字符
			y.erase(2, 1);
			cars[i].route[j].rx = atoi(x.c_str());//存入车辆数组
			cars[i].route[j].ry = atoi(y.c_str());
			//cout << cars[i].route[j].rx << "  " << cars[i].route[j].ry << endl;
		}
		//cout << filename << "  " << j << endl;
		//system("pause");
	}

	//录入路边单元坐标
	filename = ROADSIDE_UNITS;//to_string:int-->string
	ifstream filein(filename);//打开文件
	for (i = 0; i < NUM_U; i++) {
		getline(filein, s1);
		istringstream sin(s1);//定义sin流
		sin >> x >> y;
		x.erase(3, 1);//删除小数点，从字符串下标3开始删除1个字符
		y.erase(2, 1);
		units[i].rx = atoi(x.c_str());
		units[i].ry = atoi(y.c_str());
		//cout << i << ":" << units[i].rx << "  " << units[i].ry << endl;
	}
}

void getmeet() {
	int i, j, t;
	for (i = 0; i < NUM_V; i++) {//所有车辆
		for (j = 0; j < C*T; j++) {//所有时刻
			//获取相遇路边单元
			int flag = 0;
			for (t = 0; t < NUM_U; t++) {//遍历路边单元
				int x = cars[i].route[j].rx - units[t].rx;
				int y = cars[i].route[j].ry - units[t].ry;
				double d = sqrt(pow(x, 2) + pow(y, 2));
				if (d < UNIT_RADIUS) {
					cars[i].route[j].uid = t;//录入路边单元编号
					cars[i].route[j].u2v_distance = d;//录入路边单元距离
					flag++;
					break;
				}
			}
			if (flag == 0)
				cars[i].route[j].uid = NONE;
			//获取相遇车辆
			for (t = 0; t < NUM_V; t++) {//遍历车辆
				int x = cars[i].route[j].rx - cars[t].route[j].rx;
				int y = cars[i].route[j].ry - cars[t].route[j].ry;
				double d = sqrt(pow(x, 2) + pow(y, 2));
				if (d < VEHICLE_RADIUS && i != t && cars[i].route[j].num_v < MAX_PASSING_VEHICLE) {
					cars[i].route[j].vid[cars[i].route[j].num_v] = t;//录入车辆编号
					cars[i].route[j].v2v_distance[cars[i].route[j].num_v] = d;//录入车辆距离
					cars[i].route[j].num_v++;
					if (cars[i].route[j].num_v == MAX_PASSING_VEHICLE)//数组已满，提前结束
						break;
				}
			}
		}
	}
}