#include<iostream>
#include<mpi.h>

#define MIN(a, b) ((a <= b) ? (a) : (b))

using namespace std;
int comm_sz = 0;
int my_rank = 0;
int arra[] = { 1,2,3,4,5 }, arrb[] = { 10,10,10,10,10 }, arrc[] = { 0,0,0,0,0 };

//获取输入，0进程从键盘读取，非0进程从0进程获取
bool get_input(double& a, double& b, int n)
{
	if (my_rank == 0) {	//用户输入
		cout << " enter a,b,n" << endl;
		cin >> a >> b >> n;
	}
	else {	//从进程0获取
		MPI_Recv(&a, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&b, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
		MPI_Recv(&n, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	}
	return true;
}

//加和数组a和数组b的对应项
int count_sum(int beg, int en)
{
	int sum = 0;
	for (int i = beg; i <= en; i++) {
		sum += arra[i] + arrb[i];
	}
	return sum;
}

int main(void)
{
	int local_sum = 0, total_sum = 0;	//local_sum为本进程使用，total_sum为计算风格进程的合使用

	MPI_Init(NULL, NULL);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);

	//get_input(a,b,n);	//测试时用的
	int n = sizeof(arra) / sizeof(int);
	int jiange = n - 1 / (comm_sz - 1) + 1;

	//if(!my_rank) cout << "number of processes:" << comm_sz << endl;

	if (!my_rank) {	//进程0收集各个进程的结果，然后加和
		cout << "n: " << n << endl;
		// from no.1 process to recv data;
		for (int source = 1; source < comm_sz; source++) {
			MPI_Recv(&local_sum, 1, MPI_INT, source, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			//cout << "receive " << local_sum << " from " << source << endl;
			total_sum += local_sum;
		}
		cout << "total_sum: " << total_sum << endl;
	}
	else
	{
		int sum = count_sum(jiange * (my_rank - 1), MIN(jiange * my_rank - 1, n - 1));
		//deadlock tag not same
		//MPI_Send(&sum, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
		
		MPI_Send(&sum, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);
	}

	////test deadlock 
	//// use "mpiexec -n 2 MPI.exe" to run 
	//if (!my_rank)
	//{
	//	//int send = 3;
	//	//int recv = 4;
	//	//MPI_Send(&send, 1, MPI_INT, 1, 0, MPI_COMM_WORLD);
	//	//MPI_Recv(&recv, 1, MPI_INT, 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//	//cout << my_rank << ": " << recv << endl;

	//	int send[100] = { 1 };
	//	int recv[100] = { 0 };
	//	MPI_Send(&send, 100, MPI_INT, 1, 0, MPI_COMM_WORLD);
	//	MPI_Recv(&recv, 100, MPI_INT, 1, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//	cout << my_rank << ": " << recv[1] << endl;
	//}
	//else
	//{
	//	//int send = 5;
	//	//int recv = 6;
	//	//MPI_Send(&send, 1, MPI_INT, 0, 1, MPI_COMM_WORLD);
	//	//MPI_Recv(&recv, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//	//cout << my_rank << ": " << recv << endl;

	//	int send[100] = { 2 };
	//	int recv[100] = { 0 };
	//	MPI_Send(&send, 100, MPI_INT, 0, 1, MPI_COMM_WORLD);
	//	MPI_Recv(&recv, 100, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
	//	cout << my_rank << ": " << recv[1] << endl;
	//}

	MPI_Finalize();
	return 0;
}
