#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

const int threads = 4;
const int matrixSize = 1000;
const int infinity = 9999999;

int **m1, **m2;
void init()
{
	m1 = new int* [matrixSize];
	m2 = new int* [matrixSize];
	for (int i=0; i<matrixSize; i++)
	{
		m1[i] = new int [matrixSize];
		m2[i] = new int [matrixSize];
	}
	for (int i=0; i<matrixSize; i++) {
		for (int j = i; j < matrixSize; j++) {
			if (i == j) {
				m1[i][j] = 0;
				m2[i][j] = 0;
			} else {
				m1[i][j] = rand() % 100;
				m1[j][i] = infinity;
				m2[i][j] = m1[i][j];
				m2[j][i] = infinity;
			}
			//cout << m1[i][j] << " ";
		}
		//cout << endl;
	}
}

void print (int** matrix)
{
	for (int i=0; i<matrixSize; i++)
	{
		for (int j = 0; j< matrixSize; j++)
		{
			if(matrix[i][j]==infinity)
			{
				cout << "><\t";
			}
			else
			{
				cout << matrix[i][j]<< "\t";
			}
		}
		cout << endl;
	}
}

void linear (int** matrix)
{
	for (int k = 0; k < matrixSize; k++)
	{
		for (int i = 0; i < matrixSize; i++)
		{
			for (int j = 0; j < matrixSize; j++)
			{
				matrix[i][j] = static_cast<int>(fmin(matrix[i][j], matrix[i][k] + matrix[k][j]));
			}
		}
	}
}

void findPath (int** matrix, int threadNum)
{
	for (int k = 0; k < matrixSize; k++)
	{
		for (int i = threadNum; i < matrixSize; i+=threads)
		{
			for (int j = 0; j < matrixSize; j++)
			{
				matrix[i][j] = static_cast<int>(fmin(matrix[i][j], matrix[i][k] + matrix[k][j]));
			}
		}
	}
}

void threaded (int** matrix)
{
	auto *threadsVector = new thread[threads];
	for (int i = 0; i < threads; i++)
	{
		threadsVector[i]= thread(findPath, matrix, i);
	}
	for (auto i = 0; i<threads; i++)
	{
		if(threadsVector[i].joinable())
		{
			threadsVector[i].join();
		}
	}
	delete[](threadsVector);
}

int main()
{
	cout<< "Started";
	init();

	//print(m1);		cout << endl;

	auto startTine = std::chrono::high_resolution_clock::now();
	linear(m1);
	auto endTime = std::chrono::high_resolution_clock::now();
	auto resT =  chrono::duration_cast<chrono::milliseconds >(endTime - startTine);
	cout << "Linear time: \t" << resT.count();		cout << endl;


	cout<< "\n";
	//print(m1);        cout << endl;        cout << endl;

	//print(m2);        cout << endl;


	startTine = std::chrono::high_resolution_clock::now();
	threaded(m2);
	endTime = std::chrono::high_resolution_clock::now();
	resT =  chrono::duration_cast<chrono::milliseconds >(endTime - startTine);
	cout << "Threaded time: \t" << resT.count();		cout << endl;

//	print(m2);        cout << endl;


	cout<< "\n";
	system("pause");
	return  0;
}