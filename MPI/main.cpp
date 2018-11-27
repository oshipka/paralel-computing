#include "mpi.h"

#include <iostream>
#include <chrono>
#include <fstream>

using namespace std;

int matrixSize = 1000;
int totalWorkNodes = 20;
const int mpiRootId = 0;
int* m1, *m2, *resultAll;
float elapsed = 0;
std::string matrixPath = "./Matrices/matrix_1000x1000";

void nullResult()
{
	for (int i = 0; i < matrixSize; i++)
	{
		for (int j = 0; j < matrixSize; j++)
		{
			resultAll[i * matrixSize + j] = 0;
		}
	}
}

void print(int* matrix)
{

	for (int i = 0; i < matrixSize; i++)
	{
		for (int j = 0; j < matrixSize; j++)
		{
			cout << matrix[i * matrixSize + j] << "\t";
		}
		cout << endl;
	}
}

void multiplyCluster(int* c, const int* a, const int* b, int nodeId)
{
	for (int i = nodeId-1; i < matrixSize; i += totalWorkNodes)
	{
		for (int j = 0; j < matrixSize; j++)
		{
			c[i * matrixSize + j] = 0;
			for (int k = 0; k < matrixSize; k++)
			{
				c[i * matrixSize + j] += a[i * matrixSize + k] * b[k * matrixSize + j];
			}
		}
	}
}

void multiplyLinear(int* c, const int* a, const int* b)
{
	for (int i = 0; i < matrixSize; i++)
	{
		for (int j = 0; j < matrixSize; j++)
		{
			c[i * matrixSize + j] = 0;
			for (int k = 0; k < matrixSize; k++)
			{
				c[i * matrixSize + j] += a[i * matrixSize + k] * b[k * matrixSize + j];
			}
		}
	}
}

void readMatrix (string path, int* destination)
{
	ifstream input_file(path.c_str());
	if (input_file.good())
	{
		int i = 0;
		input_file >> matrixSize;
		int size = matrixSize * matrixSize;
		destination = new int[size];
		int current_number;
		while (input_file >> current_number)
		{
			if (i >= size)
			{
				break;
			}
			destination[i++] = current_number;
		}
		input_file.close();
	}
}



int main(int argc, char** argv)
{
	int mpiNodesCount, mpiNodeId;
	const int mSize = matrixSize*matrixSize;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &mpiNodesCount);
	MPI_Comm_rank(MPI_COMM_WORLD, &mpiNodeId);

	std::cout << "Node #" << mpiNodeId << " initialized" << std::endl;
	chrono::high_resolution_clock::time_point start;
	elapsed = 0;
	if (mpiNodeId != mpiRootId)
	{
		m1 = new int[matrixSize * matrixSize];
		readMatrix(matrixPath, m1);
		m2 = new int[matrixSize * matrixSize];
		readMatrix(matrixPath, m2);
		resultAll = new int[matrixSize * matrixSize];

		start = chrono::high_resolution_clock::now();
		multiplyCluster(resultAll, m1, m2, mpiNodeId);
		elapsed = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count();
		MPI_Send(resultAll, mSize, MPI_FLOAT, mpiRootId, 1, MPI_COMM_WORLD);
		MPI_Send(&elapsed, 1, MPI_FLOAT, mpiRootId, 0, MPI_COMM_WORLD);
	}
	else
	{
		m1 = new int[matrixSize * matrixSize];
		readMatrix(matrixPath, m1);
		m2 = new int[matrixSize * matrixSize];
		readMatrix(matrixPath, m2);
		start = chrono::high_resolution_clock::now();
		resultAll = new int[matrixSize * matrixSize];
		multiplyLinear(resultAll, m1, m2);
		auto linear = chrono::duration_cast<chrono::milliseconds>(chrono::high_resolution_clock::now() - start).count();
		ofstream output_file("./computation_results.txt", std::ios::app);
		output_file << "Linear time: " << linear << endl;
		cout << "Linear time: " << linear << endl;
		//print(resultAll);
		cout << endl;
		nullResult();
		float maxElapsed = 0.0;
		for (int i = 1; i < mpiNodesCount; i++)
		{
			int* tempRes = new int[mSize];
			MPI_Recv(tempRes, mSize, MPI_FLOAT, i, 1, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			for (int l = i-1; l < matrixSize; l += totalWorkNodes)
			{
				for (int j = 0; j < matrixSize; j++)
				{
					resultAll[l * matrixSize + j] = tempRes[l * matrixSize + j];
				}
			}
			MPI_Recv(&elapsed, 1, MPI_FLOAT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
			if (elapsed > maxElapsed)
			{
				maxElapsed = elapsed;
			}
		}

		float acceleration = linear / maxElapsed * 1.0f;

		output_file << "Parallel time: " << maxElapsed << endl;
		output_file << endl;
		output_file <<"Acceleration: " << acceleration << endl;
		output_file <<"Efficiency: " << acceleration / (totalWorkNodes * 1.0f) << endl;

		cout << "Parallel time: " << maxElapsed << endl;
		//print(resultAll);
		cout << endl;
		cout <<"Acceleration: " << acceleration << endl;
		cout <<"Efficiency: " << acceleration / (totalWorkNodes * 1.0f) << endl;
	}
	MPI_Finalize();
}
