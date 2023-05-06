// Teylor for function sinh(x)*x
#include <stdio.h>
#include <math.h>
#include <mpi.h>
#include <malloc.h>

#define SIZE 1000000 

//the teylor series
double TaylorSeries(double x)
{
	double last = x * x;
	double sum = x * x;
	for (int i = 1; i < SIZE; ++i)
	{
		double rat;

		rat = x * x / (2 * i) / (2 * i + 1);
		last *= rat;
		sum += last;
		if (sum + last == sum)
		{
			break;
		}
	}
	return sum;
}
int main(int argc, char* argv[])
{
	int rank, size;
	double start, end;// timer
	int N = 600000000;//number of steps
	double n = 1 / (double)N;//step length
	double a;// teylor value

	double arrBegin[3];//the first value
	double arrBeginFunc[3];//the first function value

	double function;// real function

	double ls1, ls2, ls3, lr1, lr2, lr3;//the last value
	double fs1, fs2, fs3, fr1, fr2, fr3;//the last function value	

	MPI_Status status;

	MPI_Init(&argc, &argv);

	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	double partialSegment = (double)rank / (double)size;//the starting point for the process
	double partiakSegmentNext = (double)(rank + 1) / (double)size;//the starting point for the next process

	int countStep = N / size;

	if (rank == 0)
	{
		start = MPI_Wtime();
		int curr = 0;
		for (double i = partialSegment; i < partiakSegmentNext; i += n)//calculation
		{
			a = TaylorSeries(i);
			function = sinh(i) * i;
			if ((curr == 0) || (curr == 1) || (curr == 2))
			{
				arrBegin[curr] = a;
				arrBeginFunc[curr] = function;
			}
			curr++;
		}

		if (size != 1)
		{
			MPI_Recv(&lr1, 1, MPI_DOUBLE, size - 1, 0, MPI_COMM_WORLD, &status);
			MPI_Recv(&lr2, 1, MPI_DOUBLE, size - 1, 1, MPI_COMM_WORLD, &status);
			MPI_Recv(&lr3, 1, MPI_DOUBLE, size - 1, 2, MPI_COMM_WORLD, &status);

			MPI_Recv(&fr1, 1, MPI_DOUBLE, size - 1, 3, MPI_COMM_WORLD, &status);
			MPI_Recv(&fr2, 1, MPI_DOUBLE, size - 1, 4, MPI_COMM_WORLD, &status);
			MPI_Recv(&fr3, 1, MPI_DOUBLE, size - 1, 5, MPI_COMM_WORLD, &status);
		}



		end = MPI_Wtime();

		printf("time = %lf\n\n", end - start);

		for (int i = 0; i < 3; i++)
		{
			printf("¹ %d value taylor = %e\nvalue function = %e\ndifferent = %e\n", i + 1, arrBegin[i], arrBeginFunc[i], arrBeginFunc[i] - arrBegin[i]);
		}
		printf("¹ %d value taylor = %e\nvalue function = %e\ndifferent = %e\n", N - 3, lr3, fr3, fr3 - lr3);
		printf("¹ %d value taylor = %e\nvalue function = %e\ndifferent = %e\n", N - 2, lr2, fr2, fr2 - lr2);
		printf("¹ %d value taylor = %e\nvalue function = %e\ndifferent = %e\n", N - 1, lr1, fr1, fr1 - lr1);

	}
	else if (rank > 0 && rank < size - 1)
	{

		for (double i = partialSegment; i < partiakSegmentNext; i += n)
		{
			a = TaylorSeries(i);
		}

	}
	else
	{
		int count = 0;

		int curr = 0;
		for (double i = partiakSegmentNext; i > partialSegment; i -= n)
		{
			a = TaylorSeries(i);
			function = sinh(i) * i;
			if (count == 0)
			{
				ls1 = a;
				fs1 = function;
			}
			if (count == 1)
			{
				ls2 = a;
				fs2 = function;
			}
			if (count == 2) {
				ls3 = a;
				fs3 = function;
			}
			count++;
		}
		if (N % size != 0)
		{
			for (int i = 0; i < N % size; i++)
			{
				int x = i * n + partiakSegmentNext;
				a = TaylorSeries(x);
				function = sinh(x) * x;
				if (i == N % size - 1)
				{
					ls1 = a;
					fs1 = function;
				}
				if (i == N % size - 2)
				{
					ls2 = a;
					fs2 = function;
				}
				if (i == N % size - 3) {
					ls3 = a;
					fs3 = function;
				}
			}
		}
		MPI_Send(&ls1, 1, MPI_DOUBLE, 0, 0, MPI_COMM_WORLD);
		MPI_Send(&ls2, 1, MPI_DOUBLE, 0, 1, MPI_COMM_WORLD);
		MPI_Send(&ls3, 1, MPI_DOUBLE, 0, 2, MPI_COMM_WORLD);

		MPI_Send(&fs1, 1, MPI_DOUBLE, 0, 3, MPI_COMM_WORLD);
		MPI_Send(&fs2, 1, MPI_DOUBLE, 0, 4, MPI_COMM_WORLD);
		MPI_Send(&fs3, 1, MPI_DOUBLE, 0, 5, MPI_COMM_WORLD);


	}



	MPI_Finalize();


	return 0;
}

//C:\Users\mv\source\repos\mpi_teylor\Debug