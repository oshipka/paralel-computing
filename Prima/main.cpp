#include <iostream>
#include <thread>
#include <chrono>
#include <string>
#include <algorithm>
#include <mutex>
#include <vector>

using namespace std;

int threads = 2;
const int matrixSize = 1000;
const int inf = 9999;

int **m1, *resultAll;
int setter[7][7] = {
	{0, 1, 3, inf, inf, 2},
	{1, 0, 5, 1, inf, inf},
	{3, 5, 0, 2, 1, inf},
	{inf, 1, 2, 0, 4, inf},
	{inf, inf, 1, 4, 0, 5},
	{2, inf, inf, inf, 5, 0},
};

void init()
{
	m1 = new int*[matrixSize];
	resultAll = new int[matrixSize];
	for (int i = 0; i < matrixSize; i++)
	{
		m1[i] = new int[matrixSize];
	}
	for (int i = 0; i < matrixSize; i++)
	{
		for (int j = i; j < matrixSize; j++)
		{
			if (i == j)
			{
				m1[i][j] = inf;
			}
			else
			{
				if (rand() % 2 == 1)
				{
					m1[i][j] = rand() % 100;
				}
				else
				{
					m1[i][j] = inf;
				}
				m1[j][i] = m1[i][j];
			
			/*
				m1[i][j] = setter[i][j];

				m1[j][i] = m1[i][j];*/
			}
		}
	}
}

void nullResult()
{
	for (int i = 0; i < matrixSize; i++)
	{
		for (int j = 0; j < matrixSize; ++j)
		{
			resultAll[i] = inf;
		}
	}
}

void print(int** matrix)
{
	for (int i = 0; i < matrixSize; i++)
	{
		for (int j = 0; j < matrixSize; j++)
		{
			if (matrix[i][j] == inf)
			{
				cout << "><\t";
			}
			else
			{
				cout << matrix[i][j] << "\t";
			}
		}
		cout << endl;
	}
}

void print(int* matrix)
{
	for (int i = 0; i < matrixSize; i++)
	{
		if (matrix[i] == inf)
		{
			cout << "><\t";
		}
		else
		{
			cout << matrix[i] << "\t";
		}
	}
	cout << endl;
}

mutex myMutex;
void minVerticeSearch(bool* included, int** matrix, int& min_num, int& min_index, int from, int step)
{

	int curr_min_num = inf, curr_min_index = inf;
	for (int i = from; i < matrixSize; i += step)
	{
		if (included[i])
		{
			for (int j = 0; j < matrixSize; j++)
			{
				if (!included[j] && curr_min_num > matrix[i][j])
				{
					curr_min_num = matrix[i][j];
					curr_min_index = j;				
				}
			}
		}
	}
	myMutex.lock();
	min_num = min(min_num, curr_min_num);
	min_index = min(min_index, curr_min_index);
	myMutex.unlock();
}

int* findTree(int** matrix, int pointStart)
{
	int* result;
	bool* included;
	result = new int[matrixSize];
	included = new bool[matrixSize];
	for (int i = 0; i < matrixSize; i++)
	{
		result[i] = inf;
		included[i] = false;
	}
	result[pointStart] = 0;
	included[pointStart] = true; 
	for (int count = 0; count < matrixSize - 1; count++)
	{
		int min_num = inf, min_index = inf;

		minVerticeSearch(included, matrix, min_num, min_index, 0, 1);

		included[min_index] = true;
		result[min_index] = min_num;
	}
	return result;
}

void linear(int** matrix, int* result)
{
	int* resVector;
	resVector = findTree(matrix, 1);
	for (int j = 0; j < matrixSize; j++)
	{
		result[j] = resVector[j];
	}
}

void threaded(int** matrix, int* result)
{
	auto* threadsVector = new thread[threads];
	bool* included;
	included = new bool[matrixSize];
	for (int i = 0; i < matrixSize; i++)
	{
		included[i] = false;
	}
	result[1] = 0;
	included[1] = true;

	int length = matrixSize / threads;
	for (int count = 0; count < matrixSize - 1; count++)
	{
		int min_num = inf, min_index = inf;

		for (int i = 0; i < threads; i++)
		{
			threadsVector[i] = thread(minVerticeSearch, included, matrix, ref(min_num), ref(min_index), i, threads);
		}


		for (auto i = 0; i < threads; i++)
		{
			if (threadsVector[i].joinable())
			{
				threadsVector[i].join();
			}
		}

		included[min_index] = true;
		result[min_index] = min_num;
	}

	for (auto i = 0; i < threads; i++)
	{
		if (threadsVector[i].joinable())
		{
			threadsVector[i].join();
		}
	}
	delete[](threadsVector);
}

int main()
{
	cout << "Started\n";
	init();

	cout << endl;

	auto startTine = std::chrono::high_resolution_clock::now();
	linear(m1, resultAll);
	auto endTime = std::chrono::high_resolution_clock::now();
	auto resT = chrono::duration_cast<chrono::milliseconds>(endTime - startTine);

	double Linear = resT.count();
	cout << "Linear time: \t" << Linear;
	cout << endl;

	cout << "\n";
	
	for (int i = 0; i < 3; i++)
	{
		nullResult();
		startTine = std::chrono::high_resolution_clock::now();
		threaded(m1, resultAll);
		endTime = std::chrono::high_resolution_clock::now();
		resT = chrono::duration_cast<chrono::milliseconds>(endTime - startTine);
		double Paralel = resT.count();
		cout << "Threaded time: \t" << Paralel;
		cout << endl;
		cout << "Threads: \t" << threads;
		cout << endl;
		cout << "Acceleration: \t" << std::to_string(Linear / Paralel * 1.0);
		cout << endl;
		cout << "Efficiency: \t" << std::to_string(Linear / (Paralel * threads) * 1.0);
		cout << endl;
		threads = threads *2;
	}
	


	cout << "\n";
	system("pause");
	return 0;
}
