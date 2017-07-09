#include "stable.h"

int U;
int Ve[MAX_PASSING_VEHICLE];

extern int maxFlow ;//最大流：路径条数
extern double totalDelay;//时延
extern double _ratio;//响应率
extern double _defi_ratio;//通信流缺失率
extern int _tran;//传递次数

//struct ArcType {
//	int c,f; //容量，流量
//};

//struct{
//	int in, out;//入度，出度
//}vertex[MAXN];//点的出入度比值

//ArcType Edge[MAXN][MAXN];

//int **Edgec = new int[MAXN][MAXN]; //邻接矩阵.容量 
//int Edgef[MAXN][MAXN]; //邻接矩阵.流量
int n,m; //顶点个数和弧数
int flag[MAXN]; //记录顶点状态：-1为为标号，0为已标号未检查，1为已标号未检查
int _prev[MAXN]; //标号的第一个分量：指明标号从哪个顶点得到，以便查找可改进量
int alpha[MAXN]; //标号的第二个分量：可改进量a
int _queue[MAXN]; //队列
int v; //队列中取出来头元素
int  qs, qe; //队头位置，队尾位置
int i, j; 

int path[MAXN], _flow[MAXN], start, _end;
queue<int> q;   //先进先出队列——用于表达节点搜索顺序  

int bfs(int **Edgec, int **Edgef) {
	int i, t;
	while (!q.empty()) q.pop();//清空队列
	memset(path, -1, sizeof(path));//初始化路径
	path[start] = 0;//头路径为0
	_flow[start] = INF;//初始化最小流数组
	q.push(start);//队头进队
	while (!q.empty()) {
		t = q.front();//取队列的首元素  
		q.pop();//把队列首元素弹出  
		if (t == _end) break;//如果对头位置=最后一个节点
		//cout << "bfs path=";
		for (i = 1+NUM_REQ; i <= n; i++) {//遍历节点个数
			int y = Edgec[t][i] - Edgef[t][i];//增广路径余量
			if (path[i] == -1 && y) {
				//_flow[i] = MIN(_flow[t],y);//记录最小值；不用记，最小值就是1
				q.push(i);//进队
				path[i] = t;//记录路径
				//cout << t << ", ";
			}
		}
		//cout << endl;
	}
	if (path[_end] == -1)//表示到_end没有通路
		return -1;
	return 1;                   //一次遍历之后的流量增量  _flow[_end]
}

void Edmonds_Karp(int **Edgec,int **Edgef) {
	int step, now, pre;
	int maxpath = 0;//最大路径数：同一车辆-->同一数据=多个
	int path_num[NUM_V][NUM_REQ] = { 0 };//路径数组：车辆-->数据=路径条数
	maxFlow = 0;//同一车辆-->同一数据=一个
	_tran = 0;
	_end = n;
	while (true) {
		start = rand_range_int(1, NUM_REQ + 1);//随机化数据入口，同时也表示：随机化车辆的数据请求入口
		clock_t s, e;
		s = clock();
		step = bfs(Edgec,Edgef);//找一条增广路径
		e = clock();
		//cout << step << endl;
		//system("pause");
		if (step == -1) break;//找不到增路径时退出  
		maxpath += step;
		now = _end;
		while (now != start) {//更新边容量
			pre = path[now];
			Edgef[pre][now] += step;      //更新正向边的实际容量  
			Edgef[now][pre] -= step;      //添加反向边  
			now = pre;
			_tran++;
		}
		int v = (path[_end] - (1 + NUM_REQ + NUM_U*T)) / T;
		path_num[v][start-1]++;//更新路径数组
		Edgef[0][start] += step;//更新src-->数据的变容量
		cout << start << "-->" << v << "spend time:" << (double)(e - s) / CLOCKS_PER_SEC << "s" << endl;
	}
	/*--------------------------------数据处理区---------------------------------------*/
	//计算最大流：可通信的路径条数
	for (int i = 0; i < NUM_V; i++)
		for (int j = 0; j < NUM_REQ; j++)
			if (path_num[i][j]) {
				maxFlow++;
			}

	//时延
	double delay = 0;
	for (i = 0; i <= n; i++) {//输出各弧线的流量及求得的最大流量
		for (j = 0; j <= n; j++) {
			if (Edgef[i][j]<INF && Edgef[i][j] != 0) {
				if (j == i + 1 && (i - NUM_REQ) % T != 0) { delay += 1; }
				else delay += 0.1;
			}
		}
	}
	totalDelay = delay / maxpath;//计算时延

	//随机化数据请求:随机化车辆请求数、遍历数据入口
	int r, re;//车辆请求数
	int remenber[NUM_REQ] = { 0 };//记忆数组
	int req[NUM_V][NUM_REQ] = { 0 };//请求数据总数，已满足请求数据数量
	int sum_req = 0, sat_req = 0;//总请求数、已满足请求数
	for (int i = NUM_V*(1 - percent); i < NUM_V; i++) {
		int t = 0;//数组定位
		re = r = rand_range_int(1, MAX_REQ + 1);
		while (1) {
			int flag = 0;//标记
			int c = rand_range_int(0, NUM_REQ);//随机化数据请求入口
			for (int j = 0; j < t; j++) {
				if (c == remenber[j])
					flag = 1;
			}
			if (flag == 0) {//表示记忆数组里面没有过 随机化这个数据
				if (r && path_num[i][c]) {//表示这个数据入口里面有通路
					req[i][c]++;//满足加一
					r--;//请求减一
				}
				remenber[t] = c;//更新记忆数组
				t++;
			}
			if (t == NUM_REQ || r == 0) {//记忆数组已满||请求完毕
				break;
			}
		}
		sum_req += re;//更新请求数据数量
	}
	for(int i= NUM_V*(1-percent);i<NUM_V;i++)//计算总请求数
		for (int j = 0; j < NUM_REQ; j++) {
			if (req[i][j]) {
				sat_req += req[i][j];
				//cout << "车辆:" << i << "->数据:" << j << "的请求数为:" << req[i][j] << endl;//已验证
			}
		}
	_ratio = (double)(sat_req) / sum_req;//计算响应率

	//计算传输路径次数
	_tran = _tran*((float)sat_req / maxpath);

	/*---------------------------------数据查看区--------------------------------------*/
	//查看实际数据入口通行量，已验证
	/*for (int i = 1; i <= NUM_REQ; i++)
	cout << Edgef[0][i] << endl;*/

	//查看路径数组,已验证
	//查看请求车辆空缺路径（一条可通路径都没有）的比率，可验证网络的复杂度&鲁棒性
	int flag;//标记
	int defi_num = 0;//缺失路径
	for (int i = NUM_V*(1-percent); i < NUM_V; i++) {
		flag = 0;
		for (int j = 0; j < NUM_REQ; j++)
			if (path_num[i][j]) {
				/*cout << "车辆" << i << "->" << "数据" << j << "的条数：" << path_num[i][j];
				if (req[i][j])
					cout << " 响应";
				cout << endl;*/
				flag = 1;
			}
		if (flag == 0)//计算缺失路径
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
//		//标号直至不存在可改进路
//		memset(flag, 0xff, sizeof(flag));//初始化 
//		memset(_prev, 0xff, sizeof(_prev));
//		memset(alpha, 0xff, sizeof(alpha));
//		flag[0] = 0; //将源节点标号为已标号未检查顶点
//		_prev[0] = 0;
//		alpha[0] = INF;
//		qs = qe = 0 ;
//		_queue[qe] = 0;  //源点入队列
//		qe++;
//		while( qs<qe && flag[n] == -1){//qs<qe说明队列非空,-1表示未标号
//			v = _queue[qs];//取出队头位置
//			qs++;
//			for(i=0; i<=n; i++){
//				if(flag[i] == -1){
//					if(Edge[v][i].c<INF && Edge[v][i].f <Edge[v][i].c){ //容量未使用完
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
//		if (flag[n] == -1 || alpha[n] == 0 )  break; //该点没有标号或者可改进量为0
//		int k1 = n,k2 = abs(_prev[k1]);
//		int a = alpha[n];
//		while (1){
//			if(Edge[k2][k1].f<INF) Edge[k2][k1].f = Edge[k2][k1].f+a;
//			else Edge[k1][k2].f = Edge[k1][k2].f-a;
//			if(k2 == 0) break; //一直调整到源点0
//			k1 = k2;
//			k2 = abs(_prev[k2]);
//		}	
//	}
//	maxFlow = 0;
//	double delay = 0;
//	for(i=0;i<=n;i++){//输出各弧线的流量及求得的最大流量
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
	memset(Ve, -1, sizeof(Ve));//清空
	U = v->route[t].uid;    //读入车辆与路边单元 与车辆的相遇信息 
	//cout << U << ","<<v->route[t].u2v_distance<<"  ";//查看相遇信息,已验证
	for(int k =0;k<v->route[t].num_v; k++){
		Ve[k]=v->route[t].vid[k];
		//cout << Ve[k] << "," << v->route[t].v2v_distance[k] << " ";
	}
	//cout << endl;
}

//路边单元传输距离权值
int ranku(struct vehicle *v, int t) {
	for (int i = 1; i <= RANK; i++) {
		double dis = (double)i / RANK*UNIT_RADIUS;
		if (v->route[t].u2v_distance < dis) {
			return 1 + RANK - i;
		}
	}
}

//车辆传输距离权值
int rankv(struct vehicle *v,int t,int j) {
	for (int i = 1; i <= RANK; i++) {
		double dis = (double)i / RANK*VEHICLE_RADIUS;
		if (v->route[t].v2v_distance[j] < dis) {
			return 1 + RANK - i;
		}
	}
}


int max_flow(struct vehicle *cars){
	//创建动态邻接矩阵数组
	int **Edgec = new int*[MAXN];
	for (int i = 0; i < MAXN; i++) {
		Edgec[i] = new int[MAXN];
	}

	int **Edgef = new int*[MAXN];
	for (int i = 0; i < MAXN; i++) {
		Edgef[i] = new int[MAXN];
	}
	
	//memset(Ve, -1, sizeof(Ve));//初始化相遇车辆数组
	n = NUM_REQ+NUM_U*T+NUM_V*T+1;
	//cout << n << endl;
    int i,j,t;

	//for (i = 0; i <= n; i++) {//初始化vertex
	//	vertex[i].in = vertex[i].out = 0;
	//}

	for (i = 0; i <= n; i++) {//初始化邻接矩阵
		for (j = 0; j <= n; j++) {
			Edgec[i][j] = Edgef[i][j] = INF;
		}
	}
	
	for(i=1;i<=NUM_REQ;i++){ //构建是src与C的连线
		Edgec[0][i] = MAXN;
		Edgef[0][i] = 0;
		//vertex[0].out++;
		//vertex[i].in++;
	}

	for(i=1;i<=NUM_REQ;i++){ //构建c与u的连线
		for(j=NUM_REQ+1;j<=NUM_REQ+NUM_U*T;j++){
			Edgec[i][j] = 1;
			Edgef[i][j] = 0;
			//vertex[i].out++;
			//vertex[j].in++;
		}
	}
	
	for(i=0;i<NUM_V;i++){//构建V与u或V的连线,请求车辆不连线//*(1-percent)
		for(t=0;t<T;t++){
			unitAndVehicle(&cars[i],t);//已验证
			if(U != -1){
			Edgec[NUM_REQ+U*T+t+1][NUM_REQ+NUM_U*T+i*T+t+1] = 1;
			Edgef[NUM_REQ+U*T+t+1][NUM_REQ+NUM_U*T+i*T+t+1] = 0;
			//int rank_u = ranku(&cars[i],t);//权值：基于传输距离-->模糊等级
			//cout << "rank_u:" << rank_u<<" ";
			//vertex[NUM_REQ+U*T+t+1].out++;//U-->V
			//vertex[NUM_REQ+NUM_U*T+i*T+t+1].in++;
			}
			for(j=0;j<MAX_PASSING_VEHICLE;j++){
				if(Ve[j] != -1) {//需要清空Ve
					Edgec[NUM_REQ+NUM_U*T+i*T+t+1][NUM_REQ+NUM_U*T+Ve[j]*T+t+1] = 1;
					Edgef[NUM_REQ+NUM_U*T+i*T+t+1][NUM_REQ+NUM_U*T+Ve[j]*T+t+1] = 0;
					//int rank_v = rankv(&cars[i],t,j);//权值：基于传输距离-->模糊等级
					//cout << "rank_v:" << rank_v<<" ";
					//vertex[NUM_REQ+NUM_U*T+i*T+t+1].out++;
					//vertex[NUM_REQ+NUM_U*T+Ve[j]*T+t+1].in++;
				}
			}
			//cout << endl;
			//system("pause");
		}
	}

	i=NUM_REQ+1;//构建同一个U或V在不同时间的连线
	while (i<n){ //请求车辆不与时间连线//-NUM_V*percent*T
		for(j=1;j<T;j++){
			Edgec[i][i+1] = NUM_REQ;//定义时间线的权值
			Edgef[i][i+1] = 0;
			//vertex[i].out++;
			//vertex[i+1].in++;
			i++;
		}
		i++;
	}
		
	for(i=NUM_REQ+NUM_U*T+NUM_V*(1-percent)*T+1;i<n;i++){ //请求车辆与Vdes的连线
		Edgec[i][n] = MAX_REQ;
		Edgef[i][n] = 0;
		//vertex[i].out++;
		//vertex[n].in++;
	}

	//获取中继网入度最小值和最大值
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

	//中继网权值定义
	//for (i = NUM_REQ + 1; i < n; i++) {//不包括请求-->Vdes的连线
	//	for (j = 1; j < n; j++) {
	//		if (Edge[i][j].f != INF&&Edge[i][j].c == INF) {//Edge[i][j].c == INF：隔离时间线的权值，vertex[j].in != 0 && 
	//			//权值：初度/入度
	//			/*double x = vertex[j].out / vertex[j].in;
	//			int y = vertex[j].out / vertex[j].in;
	//			if (x > 1.0) { Edge[i][j].c = vertex[j].out / vertex[j].in; }
	//			else { Edge[i][j].c = 1; }*/

	//			//权值：出度
	//			//Edge[i][j].c = vertex[j].out;
	//			//cout << i << "-->" << j << ":" << Edge[i][j].c << endl;

	//			//权值：翻转入度：a=7、b=8、c=9、d=10,md_in=（7+10）/2 --> a_weight=7+2*(md_in-7)=min+max-a
	//			//Edge[i][j].c = max_in + min_in - vertex[j].in;
	//			//cout << i << "-->" << j << ":" << Edge[i][j].c << endl;

	//			//权值：a*出度-（1-a）入度
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
    //ford(); //标号法求解网络最大流
	Edmonds_Karp(Edgec,Edgef);//广度搜索

	for (i = 0; i < MAXN; i++) delete[]Edgec[MAXN];
	delete[]Edgec;
	for (i = 0; i < MAXN; i++) delete[]Edgef[MAXN];
	delete[]Edgef;

    return 0;

}

