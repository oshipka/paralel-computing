#include "cuda_runtime.h"
#include "device_launch_parameters.h"

#include <stdio.h>
#include <cstdlib>
#include <iostream>
#include <chrono>
using namespace std;

#define HANDLE_ERROR(error) { \
    if (error != cudaSuccess) { \
        fprintf(stderr, "%s in %s at line %d\n", \
                cudaGetErrorString(error), __FILE__, __LINE__); \
        exit(EXIT_FAILURE); \
    } \
}


const int matrixSize = 1000;
const int threads = 51;
int* m1, *m2, *resultAll;


void init()
{
	m1 = new int[matrixSize * matrixSize];
	m2 = new int[matrixSize * matrixSize];
	resultAll = new int[matrixSize * matrixSize];
	for (int i = 0; i < matrixSize; i++)
	{
		for (int j = 0; j < matrixSize; ++j)
		{
			m1[i* matrixSize + j] = rand() % 100;
			m2[i* matrixSize + j] = rand() % 100;
			resultAll[i* matrixSize + j] = 0;
		}
	}
}

void nullResult()
{
	for (int i = 0; i < matrixSize; i++)
	{
		for (int j = 0; j < matrixSize; ++j) {
			resultAll[i* matrixSize + j] = 0;
		}
	}
}

void print(int* matrix)
{

	for (int i = 0; i < matrixSize; i++)
	{
		for (int j = 0; j < matrixSize; j++)
		{

			cout << matrix[i* matrixSize + j] << "\t";

		}
		cout << endl;
	}
}

__global__ void multiplyKernel(int *c, const int *a, const int *b)
{
	
	for (int i = threadIdx.x; i < matrixSize; i += threads) 
	{
		for (int j = 0; j < matrixSize; j++)
		{
			for (int k = 0; k < matrixSize; k++)
			{
				c[i*matrixSize + j] += a[i*matrixSize + k] * b[k*matrixSize + j];
			}
		}
	}
}

void multiplyLinear(int *c, const int *a, const int *b)
{
	for (int i = 0; i < matrixSize; i++)
	{
		for (int j = 0; j < matrixSize; j++)
		{
			for (int k = 0; k < matrixSize; k++)
			{
				c[i*matrixSize + j] += a[i*matrixSize + k] * b[k*matrixSize + j];
			}
		}
	}
}

void MatrixMultWithCuda(int *c, const int *a, const int *b, unsigned int size)
{
	int *dev_a = 0;
	int *dev_b = 0;
	int *dev_c = 0;

	HANDLE_ERROR(cudaSetDevice(0));

	HANDLE_ERROR(cudaMalloc((void**)&dev_c, size * size * sizeof(int)));
	HANDLE_ERROR(cudaMalloc((void**)&dev_a, size * size * sizeof(int)));
	HANDLE_ERROR(cudaMalloc((void**)&dev_b, size * size * sizeof(int)));

	HANDLE_ERROR(cudaMemcpy(dev_a, a, size * size * sizeof(int), cudaMemcpyHostToDevice));
	HANDLE_ERROR(cudaMemcpy(dev_b, b, size * size * sizeof(int), cudaMemcpyHostToDevice));

	multiplyKernel << <1, threads >> > (dev_c, dev_a, dev_b);

	HANDLE_ERROR(cudaGetLastError());
	HANDLE_ERROR(cudaDeviceSynchronize());
	HANDLE_ERROR(cudaMemcpy(c, dev_c, size * size * sizeof(int), cudaMemcpyDeviceToHost));

	HANDLE_ERROR(cudaFree(dev_a));
	HANDLE_ERROR(cudaFree(dev_b));
	HANDLE_ERROR(cudaFree(dev_c));
}

std::chrono::high_resolution_clock::time_point currentTime()
{
	return  std::chrono::high_resolution_clock::now();
}

float timeElapsed(std::chrono::high_resolution_clock::time_point startTime)
{
	auto endTime = currentTime();
	auto resT = chrono::duration_cast<chrono::milliseconds>(endTime - startTime);
	return resT.count();
}

void main()
{
	srand((unsigned)time(NULL));

	init();
	//print(m1);
	//cout << endl << endl;

	//print(m2);
	//cout << endl;

	auto startTime = currentTime();
	//multiplyLinear(resultAll, m1, m2);
	auto Linear = timeElapsed(startTime);
	cout << "Linear time: \t" << Linear << endl << endl;

	//print(resultAll);

	nullResult();

	startTime = currentTime();
	MatrixMultWithCuda(resultAll, m1, m2, matrixSize);
	auto Threaded = timeElapsed(startTime);
	cout << "Threaded time: \t" << Threaded << endl << endl;

	//print(resultAll);

	cout << "Acceleration: " << Linear / Threaded << endl;
	cout << "Efficiency: " << Linear / (Threaded*matrixSize) << endl;


	HANDLE_ERROR(cudaDeviceReset());
}



