#include "stable.h"
#include "hungary.h"

/*如使用对比算法：匈牙利算法，则须采用随机化数据进行比较；
真实数据没有对cars,units的属性进行更新*/

//最大流变量定义
struct vehicle cars[NUM_V];//车辆数组
struct unit units[NUM_U];//路边单元数组

int ncycle,total_time;//当前轮数,当前时刻

int flow[C];//最大流数组
double flowDelay[C];//时延数组
double ratio[C];//数据响应率数组
double defi_ratio[C];//通信流缺失率数组：可表明网络鲁棒性
int tran[C];//数据传递次数数组

int maxFlow;//最大流，转化：其实是最大传输路径数
double totalDelay;//时延
double _ratio;//数据响应率
double _defi_ratio;//通信流缺失率
int _tran;//单次传递次数

//二部图变量定义
struct vehicle cars2[NUM_V];//车辆数组
struct unit units2[NUM_U];//路边单元数组

int nureq2[C];
int nv2vreq2[C];
int tu2v2[C];
int tv2v2[C];

int pass2[C];
int delay2[C];
int v2v_num2[C];
int lost_num2[C];
int creq2[C];
int crec2[C];
double ratio2[C];



void cycle_startup()
{
	int nreq = 0,//请求数据数量
		nrec = 0,//满足请求数据数量
		l = ncycle * T,//当前轮数的开始跳数
		r = l + T - 1,//当前轮数的末尾跳数
		i;

	if (IS_HUNGARY) {//如果采用匈牙利算法	
		for (i = 0; i < NUM_V; i++) {
			nreq += cars2[i].nit;
			nrec += cars2[i].nsr;
		}

		if (ncycle > 0) {
			creq2[ncycle - 1] = nreq;
			crec2[ncycle - 1] = nrec;
			ratio2[ncycle - 1] = (double)nrec / (double)nreq;
		}
	}
	
	if(ncycle < C) {
		if (IS_HUNGARY) {//如果采用匈牙利算法
			nureq2[ncycle] = 0;
			nv2vreq2[ncycle] = 0;
			tu2v2[ncycle] = 0;
			tv2v2[ncycle] = 0;
			delay2[ncycle] = 0;
			v2v_num2[ncycle] = 0;
			lost_num2[ncycle] = 0;
			pass2[ncycle] = 0;
			for (i = 0; i < NUM_V; i++) clean_vehicle(&cars2[i]);
			for (i = 0; i < NUM_U; i++) init_unit(&units2[i]);

			for (i = 0; i < NUM_V; i++) {
				get_passing_units(&cars2[i], l, r);
				if (i > NUM_V*(1 - percent))//按照maxflow的请求率生成请求分布
					make_requests(&cars2[i]);
			}

			hungary_assign(cars2, units2);
			//easy_assign(cars2, units2);

			for (i = 0; i < NUM_U; i++) nureq2[ncycle] += units2[i].nitem;

			cout << "two part Cycle " << ncycle + 1 << "/" << C << endl;
		}
		if (REAL_MAP)
			for (int i = 0; i < NUM_V; i++) 
				for (int j = 0; j < T; j++) {
					cars[i].route[j] = cars[i].route[j + l];//覆盖车辆轨迹
				}
		else{
			//在初始化路线，调整初始化顺序:解决maxFlow的降序问题
			for (i = 0; i < NUM_V; i++) {
				get_passing_units(&cars[i], l, r);//获取可通信的路边单元
				for (int j = 0; j < T; j++)
					cars[i].route[j] = cars[i].route[j + l];//覆盖车辆轨迹
				V2V(&cars[i]);//获取可通信的车辆
			}
		}
		
		max_flow(cars);//求解最大流
		flow[ncycle] = maxFlow;//最大流
		flowDelay[ncycle] = totalDelay;//时延
		ratio[ncycle] = _ratio;//响应率
		defi_ratio[ncycle] = _defi_ratio;//通信流缺失率
		tran[ncycle] = _tran;//传递次数

		cout << "maxflow Cycle " << ncycle + 1 << "/" << C << endl;
	}
}

void run()
{
	int i, t;

	set_seed();//初始化随机种子

	if (REAL_MAP) {
		cout << "Coordinate acquiring..." << endl;
		readfile();//获取车辆&路边单元坐标
		getmeet();//获取相遇情况：路边单元&车辆
		//for (int i = 0; i < NUM_V; i++) {//查看相遇情况
		//	for (int j = 0; j < C*T; j++) {
		//		cout << i << "-->" << j << endl;
		//		cout << "  相遇单元：" << cars[i].route[j].uid << "距离为：" << cars[i].route[j].u2v_distance << endl;
		//		for (int t = 0; t < cars[i].route[j].num_v; t++) {
		//			cout << "   相遇车辆：" << cars[i].route[j].vid[t] << "距离为：" << cars[i].route[j].v2v_distance[t] << endl;
		//		}
		//	}
		//}
		//system("pause");
	}
	else {
		for (i = 0; i < NUM_V; i++) {
			init_vehicle(&cars[i]);//车辆 路边单元初始化，随机化轨迹
			if (IS_HUNGARY) {//此时realmap为0才可以
				cars2[i] = cars[i];
			}
		}

		if (IS_HUNGARY) {//此时realmap为0才可以
			for (i = 0; i < NUM_U; i++) init_unit(&units2[i]);
			for (i = 0; i < C; i++)     ratio2[i] = 0.0;
		}

		cout << "Initializing done." << endl;

		memset(flow, 0, sizeof(flow));//初始化
		memset(flowDelay, 0, sizeof(flowDelay));
		memset(ratio, 0, sizeof(ratio));
		memset(defi_ratio, 0, sizeof(defi_ratio));
		memset(tran, 0, sizeof(tran));
		clock_t start, end;
		for (ncycle = 0; ncycle < C; ncycle++) {
			start = clock();
			cycle_startup();//核心周期函数
			if (IS_HUNGARY) {
				for (t = 0; t < T; t++) {
					for (i = 0; i < NUM_V; i++)
						vehicle_thread(&cars2[i], total_time,ncycle);
					total_time++;
				}
			}
			end = clock();
			cout << "total spend time:" << (double)(end - start) / CLOCKS_PER_SEC << "s" << endl;
		}
		cycle_startup();//为了计算最后一次的ratio2
	}
}

void main(){
	run();
	int i;
	double tr;
	int total;
	char filename[11] = "result.txt"; 
	FILE *out;
	fopen_s(&out, filename, "at");//at:写方式打开文件，不清楚内容，指针定位到末尾。w:写方式打开文件，清楚内容
	
	//打印实验信息
	if (IS_HUNGARY) {
		fprintf(out, "采用Hungary算法，周期T：%d，路段长：%d\n", T, HOR_LEN);

		fprintf(out, "cycle     ");
		for (i = 1; i <= C; i++)
			fprintf(out, "%8d\t", i);
		fprintf(out, "\tavg\n");

		/*fprintf(out, "replicates in units:    ");
		total = 0;
		for(i = 0; i < C; i++) {
		fprintf(out, "%8d\t", nureq2[i]);
		total += nureq2[i];
		}
		fprintf(out, "\t%.1f\n", (float)total / (float)C);
		fprintf(out, "replicates in vehicles: ");
		total = 0;
		for(i = 0; i < C; i++) {
		fprintf(out, "%8d\t", nv2vreq2[i]);
		total += nv2vreq2[i];
		}
		fprintf(out, "\t%.1f\n", (float)total / (float)C);
		fprintf(out, "transimission u2v:      ");
		total = 0;
		for(i = 0; i < C; i++) {
		fprintf(out, "%8d\t", tu2v2[i]);
		total += tu2v2[i];
		}
		fprintf(out, "\t%.1f\n", (float)total / (float)C);
		fprintf(out, "transimission v2v:      ");
		total = 0;
		for(i = 0; i < C; i++) {
		fprintf(out, "%8d\t", tv2v2[i]);
		total += tv2v2[i];
		}
		fprintf(out, "\t%.1f\n", (float)total / (float)C);
		fprintf(out, "number of items lost:   ");
		total = 0;
		for(i = 0; i < C; i++) {
		fprintf(out, "%8d\t", lost_num2[i]);
		total += lost_num2[i];
		}
		fprintf(out, "\t%.1f\n", (float)total / (float)C);
		fprintf(out, "items recieved by v2v:  ");
		total = 0;
		for(i = 0; i < C; i++) {
		fprintf(out, "%8d\t", v2v_num2[i]);
		total += v2v_num2[i];
		}
		fprintf(out, "\t%.1f\n", (float)total / (float)C);*/
		fprintf(out, "recieved: ");//接收到的请求数据量
		total = 0;
		for (i = 0; i < C; i++) {
			fprintf(out, "%8d\t", crec2[i]);
			total += crec2[i];
		}
		fprintf(out, "\t%.1f\n", (float)total / (float)C);
		fprintf(out, "requested:");//请求总数据量
		total = 0;
		for (i = 0; i < C; i++) {
			fprintf(out, "%8d\t", creq2[i]);
			total += creq2[i];
		}
		fprintf(out, "\t%.1f\n", (float)total / (float)C);
		fprintf(out, "delay:    ");//时延
		tr = 0;
		for (i = 0; i < C; i++) {
			fprintf(out, "%.6f\t", (float)delay2[i]/ crec2[i]);
			tr += (float)delay2[i] / crec2[i];
		}
		fprintf(out, "\t%.6f\n", (float)tr / (float)C);

		fprintf(out, "ratio:    ");//数据响应率
		tr = 0;
		for (i = 0; i < C; i++) {
			fprintf(out, "%.6f\t", ratio2[i]);
			tr += ratio2[i];
		}
		fprintf(out, "\t%.6f\n", tr / (float)C);
	}
	if(REAL_MAP)
		fprintf(out, "采用真实车辆轨迹数据--三亚--maxflow\n");
	else
		fprintf(out, "采用Edmonds_Karp算法，周期T：%d，路段长：%d\n", T, HOR_LEN);

	//cycle
	fprintf(out, "cycle     ");
	for(i = 1; i <= C; i++)
		fprintf(out, "%8d\t", i);
	fprintf(out, "\tavg\n");

	//flow 
	fprintf(out, "flow:     ");
	total = 0;
	for(i = 0; i < C; i++) {
		fprintf(out, "%8d\t", flow[i]);
		total += flow[i];
	}
	fprintf(out, "\t%.1f\n", (float)total / C);

	//delay
	fprintf(out, "delay:    ");
	tr = 0;
	for(i = 0; i < C; i++) {
		fprintf(out, "%.6f\t", flowDelay[i]);
		tr += flowDelay[i];
	}
	fprintf(out, "\t%.6f\n", tr / (float)C);

	//ratio
	fprintf(out, "ratio:    ");
	tr = 0;
	for (i = 0; i < C; i++) {
		fprintf(out, "%.6f\t", ratio[i]);
		tr += ratio[i];
	}
	fprintf(out, "\t%.6f\n", tr / (float)C);

	//defi_ratio
	fprintf(out, "defratio: ");
	tr = 0;
	for (i = 0; i < C; i++) {
		fprintf(out, "%.6f\t", defi_ratio[i]);
		tr += defi_ratio[i];
	}
	fprintf(out, "\t%.6f\n", tr / (float)C);

	//tran 
	fprintf(out, "tran:     ");
	total = 0;
	for (i = 0; i < C; i++) {
		fprintf(out, "%8d\t", tran[i]);
		total += tran[i];
	}
	fprintf(out, "\t%.1f\n\n", (float)total / C);
	
	fclose(out);
	//return 0;
}