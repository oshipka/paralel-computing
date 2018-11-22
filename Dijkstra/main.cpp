#include <iostream>
#include <thread>
#include <chrono>

using namespace std;

const int threads = 4;
const int matrixSize = 1000;
const int infinity = 9999999;

int **m1, **m2, **resultAll;
int setter [7][7] = {
		{0, 2, 5, 0, 0, 0, 0},
		{2, 0, 0, 3, 6, 0, 0},
		{5, 0, 0, 7, 0, 9, 0},
		{0, 3, 7, 0, 1, 0, 0},
		{0, 6, 0, 1, 0, 5, 3},
		{0, 0, 9, 0, 5, 0, 7},
		{0, 0, 0, 0, 3, 7, 0}
};
void init()
{
	m1 = new int* [matrixSize];
	m2 = new int* [matrixSize];
	resultAll = new int* [matrixSize];
	for (int i=0; i<matrixSize; i++)
	{
		m1[i] = new int [matrixSize];
		m2[i] = new int [matrixSize];
		resultAll[i] = new int [matrixSize];

	}
	for (int i=0; i<matrixSize; i++) {
		for (int j = i; j < matrixSize; j++) {
			if (i == j) {
				m1[i][j] = 0;
				m2[i][j] = 0;
			} else {
				if(rand()%2 == 1) {
					m1[i][j] = rand() % 100;
				}
				else
				{

					m1[i][j] = infinity;

				}
				m1[j][i] = m1[i][j];
				m2[i][j] = m1[i][j];
				m2[i][j] = m1[i][j];
			}
		}
	}
}

void nullResult()
{
	for (int i = 0; i< matrixSize; i++)
	{
		for (int j = 0; j < matrixSize; ++j) {
			resultAll[i][j] = 0;
		}
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

int minDistance(const int* distance, const bool* included)
{
	int min = infinity, min_index = infinity;

	for (int i = 0; i < matrixSize; i++)
		if (!included[i] && distance[i] <= min)
			min = distance[i], min_index = i;

	return min_index;
}

int* findPath (int** matrix, int pointStart) {
	int *result;
	bool *included;
	result = new int[matrixSize];
	included = new bool[matrixSize];
	for (int i = 0; i< matrixSize; i++)
	{
		result[i] = infinity;
		included[i] = false;
	}

	result[pointStart] = 0;
	for (int count = 0; count < matrixSize - 1; count++) {
		int closestVertice = minDistance(result, included);

		included[closestVertice] = true;

		for (int i = 0; i < matrixSize; i++) {
			if (
					!included[i] &&
					matrix[closestVertice][i] &&
					result[closestVertice] != infinity &&
					result[closestVertice] + matrix[closestVertice][i] < result[i]) {
				result[i] = result[closestVertice] + matrix[closestVertice][i];
			}
		}
	}
	return result;
}

void linear(int** matrix, int** result)
{

	for (int i = 0; i < matrixSize; i++) {
		int* resVector = findPath(matrix, i);
		for (int j = 0; j < matrixSize; j++) {
			result[i][j] = resVector[j];
		}
	}
}

void findPaths (int** matrix, int** result, int threadNum)
{
	for (int i = threadNum; i < matrixSize; i+=threads) {
		int* resVector = findPath(matrix, i);
		for (int j = 0; j < matrixSize; j++) {
			result[i][j] = resVector[j];
		}
	}
}

void threaded (int** matrix, int** result)
{
	auto *threadsVector = new thread[threads];
	for (int i = 0; i < threads; i++)
	{
		threadsVector[i]= thread(findPaths, matrix, result, i);
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
	cout<< "Started\n";
	init();



	//print(m1);		cout << endl;

	auto startTine = std::chrono::high_resolution_clock::now();
	linear(m1, resultAll);
	auto endTime = std::chrono::high_resolution_clock::now();
	auto resT =  chrono::duration_cast<chrono::milliseconds >(endTime - startTine);
	cout << "Linear time: \t" << resT.count();		cout << endl;


	cout<< "\n";
	//print(resultAll);
	//print(m1);        cout << endl;        cout << endl;

	//print(m2);        cout << endl;

	nullResult();
	startTine = std::chrono::high_resolution_clock::now();
	threaded(m2, resultAll);
	endTime = std::chrono::high_resolution_clock::now();
	resT =  chrono::duration_cast<chrono::milliseconds >(endTime - startTine);
	cout << "Threaded time: \t" << resT.count();		cout << endl;

//	print(m2);        cout << endl;


	cout<< "\n";
	system("pause");
	return  0;
}