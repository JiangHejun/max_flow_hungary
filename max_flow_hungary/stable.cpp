#include "stable.h"

int U;
int Ve[MAX_PASSING_VEHICLE];

extern int maxFlow ;//�������·������
extern double totalDelay;//ʱ��
extern double _ratio;//��Ӧ��
extern double _defi_ratio;//ͨ����ȱʧ��
extern int _tran;//���ݴ���

//struct ArcType {
//	int c,f; //����������
//};

//struct{
//	int in, out;//��ȣ�����
//}vertex[MAXN];//��ĳ���ȱ�ֵ

//ArcType Edge[MAXN][MAXN];

//int **Edgec = new int[MAXN][MAXN]; //�ڽӾ���.���� 
//int Edgef[MAXN][MAXN]; //�ڽӾ���.����
int n,m; //��������ͻ���
int flag[MAXN]; //��¼����״̬��-1ΪΪ��ţ�0Ϊ�ѱ��δ��飬1Ϊ�ѱ��δ���
int _prev[MAXN]; //��ŵĵ�һ��������ָ����Ŵ��ĸ�����õ����Ա���ҿɸĽ���
int alpha[MAXN]; //��ŵĵڶ����������ɸĽ���a
int _queue[MAXN]; //����
int v; //������ȡ����ͷԪ��
int  qs, qe; //��ͷλ�ã���βλ��
int i, j; 

int path[MAXN], _flow[MAXN], start, _end;
queue<int> q;   //�Ƚ��ȳ����С������ڱ��ڵ�����˳��  

int bfs(int **Edgec, int **Edgef) {
	int i, t;
	while (!q.empty()) q.pop();//��ն���
	memset(path, -1, sizeof(path));//��ʼ��·��
	path[start] = 0;//ͷ·��Ϊ0
	_flow[start] = INF;//��ʼ����С������
	q.push(start);//��ͷ����
	while (!q.empty()) {
		t = q.front();//ȡ���е���Ԫ��  
		q.pop();//�Ѷ�����Ԫ�ص���  
		if (t == _end) break;//�����ͷλ��=���һ���ڵ�
		//cout << "bfs path=";
		for (i = 1+NUM_REQ; i <= n; i++) {//�����ڵ����
			int y = Edgec[t][i] - Edgef[t][i];//����·������
			if (path[i] == -1 && y) {
				//_flow[i] = MIN(_flow[t],y);//��¼��Сֵ�����üǣ���Сֵ����1
				q.push(i);//����
				path[i] = t;//��¼·��
				//cout << t << ", ";
			}
		}
		//cout << endl;
	}
	if (path[_end] == -1)//��ʾ��_endû��ͨ·
		return -1;
	return 1;                   //һ�α���֮�����������  _flow[_end]
}

void Edmonds_Karp(int **Edgec,int **Edgef) {
	int step, now, pre;
	int maxpath = 0;//���·������ͬһ����-->ͬһ����=���
	int path_num[NUM_V][NUM_REQ] = { 0 };//·�����飺����-->����=·������
	maxFlow = 0;//ͬһ����-->ͬһ����=һ��
	_tran = 0;
	_end = n;
	while (true) {
		start = rand_range_int(1, NUM_REQ + 1);//�����������ڣ�ͬʱҲ��ʾ������������������������
		clock_t s, e;
		s = clock();
		step = bfs(Edgec,Edgef);//��һ������·��
		e = clock();
		//cout << step << endl;
		//system("pause");
		if (step == -1) break;//�Ҳ�����·��ʱ�˳�  
		maxpath += step;
		now = _end;
		while (now != start) {//���±�����
			pre = path[now];
			Edgef[pre][now] += step;      //��������ߵ�ʵ������  
			Edgef[now][pre] -= step;      //��ӷ����  
			now = pre;
			_tran++;
		}
		int v = (path[_end] - (1 + NUM_REQ + NUM_U*T)) / T;
		path_num[v][start-1]++;//����·������
		Edgef[0][start] += step;//����src-->���ݵı�����
		cout << start << "-->" << v << "spend time:" << (double)(e - s) / CLOCKS_PER_SEC << "s" << endl;
	}
	/*--------------------------------���ݴ�����---------------------------------------*/
	//�������������ͨ�ŵ�·������
	for (int i = 0; i < NUM_V; i++)
		for (int j = 0; j < NUM_REQ; j++)
			if (path_num[i][j]) {
				maxFlow++;
			}

	//ʱ��
	double delay = 0;
	for (i = 0; i <= n; i++) {//��������ߵ���������õ��������
		for (j = 0; j <= n; j++) {
			if (Edgef[i][j]<INF && Edgef[i][j] != 0) {
				if (j == i + 1 && (i - NUM_REQ) % T != 0) { delay += 1; }
				else delay += 0.1;
			}
		}
	}
	totalDelay = delay / maxpath;//����ʱ��

	//�������������:����������������������������
	int r, re;//����������
	int remenber[NUM_REQ] = { 0 };//��������
	int req[NUM_V][NUM_REQ] = { 0 };//��������������������������������
	int sum_req = 0, sat_req = 0;//����������������������
	for (int i = NUM_V*(1 - percent); i < NUM_V; i++) {
		int t = 0;//���鶨λ
		re = r = rand_range_int(1, MAX_REQ + 1);
		while (1) {
			int flag = 0;//���
			int c = rand_range_int(0, NUM_REQ);//����������������
			for (int j = 0; j < t; j++) {
				if (c == remenber[j])
					flag = 1;
			}
			if (flag == 0) {//��ʾ������������û�й� ������������
				if (r && path_num[i][c]) {//��ʾ����������������ͨ·
					req[i][c]++;//�����һ
					r--;//�����һ
				}
				remenber[t] = c;//���¼�������
				t++;
			}
			if (t == NUM_REQ || r == 0) {//������������||�������
				break;
			}
		}
		sum_req += re;//����������������
	}
	for(int i= NUM_V*(1-percent);i<NUM_V;i++)//������������
		for (int j = 0; j < NUM_REQ; j++) {
			if (req[i][j]) {
				sat_req += req[i][j];
				//cout << "����:" << i << "->����:" << j << "��������Ϊ:" << req[i][j] << endl;//����֤
			}
		}
	_ratio = (double)(sat_req) / sum_req;//������Ӧ��

	//���㴫��·������
	_tran = _tran*((float)sat_req / maxpath);

	/*---------------------------------���ݲ鿴��--------------------------------------*/
	//�鿴ʵ���������ͨ����������֤
	/*for (int i = 1; i <= NUM_REQ; i++)
	cout << Edgef[0][i] << endl;*/

	//�鿴·������,����֤
	//�鿴��������ȱ·����һ����ͨ·����û�У��ı��ʣ�����֤����ĸ��Ӷ�&³����
	int flag;//���
	int defi_num = 0;//ȱʧ·��
	for (int i = NUM_V*(1-percent); i < NUM_V; i++) {
		flag = 0;
		for (int j = 0; j < NUM_REQ; j++)
			if (path_num[i][j]) {
				/*cout << "����" << i << "->" << "����" << j << "��������" << path_num[i][j];
				if (req[i][j])
					cout << " ��Ӧ";
				cout << endl;*/
				flag = 1;
			}
		if (flag == 0)//����ȱʧ·��
			defi_num++;
	}
	_defi_ratio = defi_num / (NUM_V*percent);

	cout << "max_tran_flow:" << maxFlow << endl;
	cout << "delay:" << totalDelay << endl;
	cout << "satified_req:" << sat_req << " summer_req:" << sum_req << endl;
	cout << "response_rate:" << _ratio << endl;
	cout << "defi_ratio:" << _defi_ratio << endl;
	cout << "tran_num:" << _tran << endl;
	//system("pause");

}

//void ford(){
//	while(1){
//		//���ֱ�������ڿɸĽ�·
//		memset(flag, 0xff, sizeof(flag));//��ʼ�� 
//		memset(_prev, 0xff, sizeof(_prev));
//		memset(alpha, 0xff, sizeof(alpha));
//		flag[0] = 0; //��Դ�ڵ���Ϊ�ѱ��δ��鶥��
//		_prev[0] = 0;
//		alpha[0] = INF;
//		qs = qe = 0 ;
//		_queue[qe] = 0;  //Դ�������
//		qe++;
//		while( qs<qe && flag[n] == -1){//qs<qe˵�����зǿ�,-1��ʾδ���
//			v = _queue[qs];//ȡ����ͷλ��
//			qs++;
//			for(i=0; i<=n; i++){
//				if(flag[i] == -1){
//					if(Edge[v][i].c<INF && Edge[v][i].f <Edge[v][i].c){ //����δʹ����
//						flag[i] = 0;
//						_prev[i] = v;
//						alpha[i] = MIN(alpha[v], Edge[v][i].c-Edge[v][i].f);
//						_queue[qe] = i;
//						qe++;
//					}
//					else if(Edge[i][v].c<INF && Edge[i][v].f>0){
//						flag[i] = 0;
//						_prev[i] = -v;
//						alpha[i] = MIN(alpha[v], Edge[i][v].f);
//						_queue[qe] = i;
//						qe++;
//					}
//				}
//			 }
//			flag[v] = 1;
//		}
//		if (flag[n] == -1 || alpha[n] == 0 )  break; //�õ�û�б�Ż��߿ɸĽ���Ϊ0
//		int k1 = n,k2 = abs(_prev[k1]);
//		int a = alpha[n];
//		while (1){
//			if(Edge[k2][k1].f<INF) Edge[k2][k1].f = Edge[k2][k1].f+a;
//			else Edge[k1][k2].f = Edge[k1][k2].f-a;
//			if(k2 == 0) break; //һֱ������Դ��0
//			k1 = k2;
//			k2 = abs(_prev[k2]);
//		}	
//	}
//	maxFlow = 0;
//	double delay = 0;
//	for(i=0;i<=n;i++){//��������ߵ���������õ��������
//		for(j=0; j<=n; j++){
//			if (i == 0 && Edge[i][j].f < INF)  maxFlow += Edge[i][j].f;// , cout << Edge[i][j].f << endl;
//			if(Edge[i][j].f<INF && Edge[i][j].f != 0)  {
//				//printf("%d->%d:%d\n", i, j, Edge[i][j].f);
//				if(j == i+1 && (i-NUM_REQ)%T != 0 ){ delay += 1;   }
//				else delay += 0.1;
//			}
//		}
//	}
//	totalDelay = delay/maxFlow;
//	printf("maxFlow:%d\n", maxFlow);
//	printf("delay:%lf\n", totalDelay);
//}


void unitAndVehicle(struct vehicle *v,int t){
	memset(Ve, -1, sizeof(Ve));//���
	U = v->route[t].uid;    //���복����·�ߵ�Ԫ �복����������Ϣ 
	//cout << U << ","<<v->route[t].u2v_distance<<"  ";//�鿴������Ϣ,����֤
	for(int k =0;k<v->route[t].num_v; k++){
		Ve[k]=v->route[t].vid[k];
		//cout << Ve[k] << "," << v->route[t].v2v_distance[k] << " ";
	}
	//cout << endl;
}

//·�ߵ�Ԫ�������Ȩֵ
int ranku(struct vehicle *v, int t) {
	for (int i = 1; i <= RANK; i++) {
		double dis = (double)i / RANK*UNIT_RADIUS;
		if (v->route[t].u2v_distance < dis) {
			return 1 + RANK - i;
		}
	}
}

//�����������Ȩֵ
int rankv(struct vehicle *v,int t,int j) {
	for (int i = 1; i <= RANK; i++) {
		double dis = (double)i / RANK*VEHICLE_RADIUS;
		if (v->route[t].v2v_distance[j] < dis) {
			return 1 + RANK - i;
		}
	}
}


int max_flow(struct vehicle *cars){
	//������̬�ڽӾ�������
	int **Edgec = new int*[MAXN];
	for (int i = 0; i < MAXN; i++) {
		Edgec[i] = new int[MAXN];
	}

	int **Edgef = new int*[MAXN];
	for (int i = 0; i < MAXN; i++) {
		Edgef[i] = new int[MAXN];
	}
	
	//memset(Ve, -1, sizeof(Ve));//��ʼ��������������
	n = NUM_REQ+NUM_U*T+NUM_V*T+1;
	//cout << n << endl;
    int i,j,t;

	//for (i = 0; i <= n; i++) {//��ʼ��vertex
	//	vertex[i].in = vertex[i].out = 0;
	//}

	for (i = 0; i <= n; i++) {//��ʼ���ڽӾ���
		for (j = 0; j <= n; j++) {
			Edgec[i][j] = Edgef[i][j] = INF;
		}
	}
	
	for(i=1;i<=NUM_REQ;i++){ //������src��C������
		Edgec[0][i] = MAXN;
		Edgef[0][i] = 0;
		//vertex[0].out++;
		//vertex[i].in++;
	}

	for(i=1;i<=NUM_REQ;i++){ //����c��u������
		for(j=NUM_REQ+1;j<=NUM_REQ+NUM_U*T;j++){
			Edgec[i][j] = 1;
			Edgef[i][j] = 0;
			//vertex[i].out++;
			//vertex[j].in++;
		}
	}
	
	for(i=0;i<NUM_V;i++){//����V��u��V������,������������//*(1-percent)
		for(t=0;t<T;t++){
			unitAndVehicle(&cars[i],t);//����֤
			if(U != -1){
			Edgec[NUM_REQ+U*T+t+1][NUM_REQ+NUM_U*T+i*T+t+1] = 1;
			Edgef[NUM_REQ+U*T+t+1][NUM_REQ+NUM_U*T+i*T+t+1] = 0;
			//int rank_u = ranku(&cars[i],t);//Ȩֵ�����ڴ������-->ģ���ȼ�
			//cout << "rank_u:" << rank_u<<" ";
			//vertex[NUM_REQ+U*T+t+1].out++;//U-->V
			//vertex[NUM_REQ+NUM_U*T+i*T+t+1].in++;
			}
			for(j=0;j<MAX_PASSING_VEHICLE;j++){
				if(Ve[j] != -1) {//��Ҫ���Ve
					Edgec[NUM_REQ+NUM_U*T+i*T+t+1][NUM_REQ+NUM_U*T+Ve[j]*T+t+1] = 1;
					Edgef[NUM_REQ+NUM_U*T+i*T+t+1][NUM_REQ+NUM_U*T+Ve[j]*T+t+1] = 0;
					//int rank_v = rankv(&cars[i],t,j);//Ȩֵ�����ڴ������-->ģ���ȼ�
					//cout << "rank_v:" << rank_v<<" ";
					//vertex[NUM_REQ+NUM_U*T+i*T+t+1].out++;
					//vertex[NUM_REQ+NUM_U*T+Ve[j]*T+t+1].in++;
				}
			}
			//cout << endl;
			//system("pause");
		}
	}

	i=NUM_REQ+1;//����ͬһ��U��V�ڲ�ͬʱ�������
	while (i<n){ //����������ʱ������//-NUM_V*percent*T
		for(j=1;j<T;j++){
			Edgec[i][i+1] = NUM_REQ;//����ʱ���ߵ�Ȩֵ
			Edgef[i][i+1] = 0;
			//vertex[i].out++;
			//vertex[i+1].in++;
			i++;
		}
		i++;
	}
		
	for(i=NUM_REQ+NUM_U*T+NUM_V*(1-percent)*T+1;i<n;i++){ //��������Vdes������
		Edgec[i][n] = MAX_REQ;
		Edgef[i][n] = 0;
		//vertex[i].out++;
		//vertex[n].in++;
	}

	//��ȡ�м��������Сֵ�����ֵ
	/*int max_in = 0, min_in = INF;
	for (int i = 1 + NUM_REQ + NUM_U*T; i < n; i++) {
		if (vertex[i].in == 0)
			continue;
		if (vertex[i].in < min_in)
			min_in = vertex[i].in;
		if (vertex[i].in > max_in)
			max_in = vertex[i].in;
	}*/
	//cout << max_in << " " << min_in << endl;
	//system("pause");

	//�м���Ȩֵ����
	//for (i = NUM_REQ + 1; i < n; i++) {//����������-->Vdes������
	//	for (j = 1; j < n; j++) {
	//		if (Edge[i][j].f != INF&&Edge[i][j].c == INF) {//Edge[i][j].c == INF������ʱ���ߵ�Ȩֵ��vertex[j].in != 0 && 
	//			//Ȩֵ������/���
	//			/*double x = vertex[j].out / vertex[j].in;
	//			int y = vertex[j].out / vertex[j].in;
	//			if (x > 1.0) { Edge[i][j].c = vertex[j].out / vertex[j].in; }
	//			else { Edge[i][j].c = 1; }*/

	//			//Ȩֵ������
	//			//Edge[i][j].c = vertex[j].out;
	//			//cout << i << "-->" << j << ":" << Edge[i][j].c << endl;

	//			//Ȩֵ����ת��ȣ�a=7��b=8��c=9��d=10,md_in=��7+10��/2 --> a_weight=7+2*(md_in-7)=min+max-a
	//			//Edge[i][j].c = max_in + min_in - vertex[j].in;
	//			//cout << i << "-->" << j << ":" << Edge[i][j].c << endl;

	//			//Ȩֵ��a*����-��1-a�����
	//			/*float a = 0.9;
	//			Edge[i][j].c = a*vertex[j].out - (1 - a)*vertex[j].in;
	//			if (Edge[i][j].c < 1)
	//				Edge[i][j].c = 1;
	//			cout << i << "-->" << j << ":" << Edge[i][j].c << endl;*/

	//			Edge[i][j].c = 1;

	//		}
	//	}
	//}
	//system("pause");
    //ford(); //��ŷ�������������
	Edmonds_Karp(Edgec,Edgef);//�������

	for (i = 0; i < MAXN; i++) delete[]Edgec[MAXN];
	delete[]Edgec;
	for (i = 0; i < MAXN; i++) delete[]Edgef[MAXN];
	delete[]Edgef;

    return 0;

}

