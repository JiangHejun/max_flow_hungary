#include "stable.h"
#include "hungary.h"

/*��ʹ�öԱ��㷨���������㷨�����������������ݽ��бȽϣ�
��ʵ����û�ж�cars,units�����Խ��и���*/

//�������������
struct vehicle cars[NUM_V];//��������
struct unit units[NUM_U];//·�ߵ�Ԫ����

int ncycle,total_time;//��ǰ����,��ǰʱ��

int flow[C];//���������
double flowDelay[C];//ʱ������
double ratio[C];//������Ӧ������
double defi_ratio[C];//ͨ����ȱʧ�����飺�ɱ�������³����
int tran[C];//���ݴ��ݴ�������

int maxFlow;//�������ת������ʵ�������·����
double totalDelay;//ʱ��
double _ratio;//������Ӧ��
double _defi_ratio;//ͨ����ȱʧ��
int _tran;//���δ��ݴ���

//����ͼ��������
struct vehicle cars2[NUM_V];//��������
struct unit units2[NUM_U];//·�ߵ�Ԫ����

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
	int nreq = 0,//������������
		nrec = 0,//����������������
		l = ncycle * T,//��ǰ�����Ŀ�ʼ����
		r = l + T - 1,//��ǰ������ĩβ����
		i;

	if (IS_HUNGARY) {//��������������㷨	
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
		if (IS_HUNGARY) {//��������������㷨
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
				if (i > NUM_V*(1 - percent))//����maxflow����������������ֲ�
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
					cars[i].route[j] = cars[i].route[j + l];//���ǳ����켣
				}
		else{
			//�ڳ�ʼ��·�ߣ�������ʼ��˳��:���maxFlow�Ľ�������
			for (i = 0; i < NUM_V; i++) {
				get_passing_units(&cars[i], l, r);//��ȡ��ͨ�ŵ�·�ߵ�Ԫ
				for (int j = 0; j < T; j++)
					cars[i].route[j] = cars[i].route[j + l];//���ǳ����켣
				V2V(&cars[i]);//��ȡ��ͨ�ŵĳ���
			}
		}
		
		max_flow(cars);//��������
		flow[ncycle] = maxFlow;//�����
		flowDelay[ncycle] = totalDelay;//ʱ��
		ratio[ncycle] = _ratio;//��Ӧ��
		defi_ratio[ncycle] = _defi_ratio;//ͨ����ȱʧ��
		tran[ncycle] = _tran;//���ݴ���

		cout << "maxflow Cycle " << ncycle + 1 << "/" << C << endl;
	}
}

void run()
{
	int i, t;

	set_seed();//��ʼ���������

	if (REAL_MAP) {
		cout << "Coordinate acquiring..." << endl;
		readfile();//��ȡ����&·�ߵ�Ԫ����
		getmeet();//��ȡ���������·�ߵ�Ԫ&����
		//for (int i = 0; i < NUM_V; i++) {//�鿴�������
		//	for (int j = 0; j < C*T; j++) {
		//		cout << i << "-->" << j << endl;
		//		cout << "  ������Ԫ��" << cars[i].route[j].uid << "����Ϊ��" << cars[i].route[j].u2v_distance << endl;
		//		for (int t = 0; t < cars[i].route[j].num_v; t++) {
		//			cout << "   ����������" << cars[i].route[j].vid[t] << "����Ϊ��" << cars[i].route[j].v2v_distance[t] << endl;
		//		}
		//	}
		//}
		//system("pause");
	}
	else {
		for (i = 0; i < NUM_V; i++) {
			init_vehicle(&cars[i]);//���� ·�ߵ�Ԫ��ʼ����������켣
			if (IS_HUNGARY) {//��ʱrealmapΪ0�ſ���
				cars2[i] = cars[i];
			}
		}

		if (IS_HUNGARY) {//��ʱrealmapΪ0�ſ���
			for (i = 0; i < NUM_U; i++) init_unit(&units2[i]);
			for (i = 0; i < C; i++)     ratio2[i] = 0.0;
		}

		cout << "Initializing done." << endl;

		memset(flow, 0, sizeof(flow));//��ʼ��
		memset(flowDelay, 0, sizeof(flowDelay));
		memset(ratio, 0, sizeof(ratio));
		memset(defi_ratio, 0, sizeof(defi_ratio));
		memset(tran, 0, sizeof(tran));
		clock_t start, end;
		for (ncycle = 0; ncycle < C; ncycle++) {
			start = clock();
			cycle_startup();//�������ں���
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
		cycle_startup();//Ϊ�˼������һ�ε�ratio2
	}
}

void main(){
	run();
	int i;
	double tr;
	int total;
	char filename[11] = "result.txt"; 
	FILE *out;
	fopen_s(&out, filename, "at");//at:д��ʽ���ļ�����������ݣ�ָ�붨λ��ĩβ��w:д��ʽ���ļ����������
	
	//��ӡʵ����Ϣ
	if (IS_HUNGARY) {
		fprintf(out, "����Hungary�㷨������T��%d��·�γ���%d\n", T, HOR_LEN);

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
		fprintf(out, "recieved: ");//���յ�������������
		total = 0;
		for (i = 0; i < C; i++) {
			fprintf(out, "%8d\t", crec2[i]);
			total += crec2[i];
		}
		fprintf(out, "\t%.1f\n", (float)total / (float)C);
		fprintf(out, "requested:");//������������
		total = 0;
		for (i = 0; i < C; i++) {
			fprintf(out, "%8d\t", creq2[i]);
			total += creq2[i];
		}
		fprintf(out, "\t%.1f\n", (float)total / (float)C);
		fprintf(out, "delay:    ");//ʱ��
		tr = 0;
		for (i = 0; i < C; i++) {
			fprintf(out, "%.6f\t", (float)delay2[i]/ crec2[i]);
			tr += (float)delay2[i] / crec2[i];
		}
		fprintf(out, "\t%.6f\n", (float)tr / (float)C);

		fprintf(out, "ratio:    ");//������Ӧ��
		tr = 0;
		for (i = 0; i < C; i++) {
			fprintf(out, "%.6f\t", ratio2[i]);
			tr += ratio2[i];
		}
		fprintf(out, "\t%.6f\n", tr / (float)C);
	}
	if(REAL_MAP)
		fprintf(out, "������ʵ�����켣����--����--maxflow\n");
	else
		fprintf(out, "����Edmonds_Karp�㷨������T��%d��·�γ���%d\n", T, HOR_LEN);

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