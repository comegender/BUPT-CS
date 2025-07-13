#include<iostream>
using namespace std;

//动态分配内存
int** NewMatrix(int row, int column) {
	int** num;
	num = new int* [row];
	for (int i = 0; i < row; i++) {
		num[i] = new int[column];
	}
	return num;
}

//矩阵初始化函数
void MatrixInit(int**& num, int row, int column) {
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < column; j++) {
			cin >> num[i][j];
		}
	}
}

//矩阵输出函数
void PrintMatrix(int** num, int row, int column) {
	for (int i = 0; i < column; i++) {
		for (int j = 0; j < row; j++) {
			cout << num[i][j] << " ";
		}
		cout << endl;
	}
}

//矩阵相加函数
void AddMatrix(int** a, int** b, int **c,int row, int column) {
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < column; j++) {
			c[i][j] = a[i][j] + b[i][j];
		}
	}
}

//矩阵相减
void SubMatrix(int** a, int** b, int **c,int row, int column) {
	for (int i = 0; i < row; i++) {
		for (int j = 0; j < column; j++) {
			c[i][j] = a[i][j] - b[i][j];
		}
	}
}

//释放矩阵
void DeleteMatrix(int** a, int row, int column) {
	for (int i = 0; i < row; i++) {
		delete[] a[i];
	}
	delete[] a;
}

int main() {
	int **A, **B, **C;
	A = NewMatrix(4, 5);
	B = NewMatrix(4, 5);
	C = NewMatrix(4, 5);
	MatrixInit(A, 4, 5);
	MatrixInit(B, 4, 5);
	AddMatrix(A, B, C,4, 5);
	cout << "矩阵相加：" << endl;
	PrintMatrix(C, 4, 5);
	cout << "矩阵相减：" << endl;
	SubMatrix(A, B,C, 4, 5);
	PrintMatrix(C, 4, 5);
	DeleteMatrix(A,4,5);
	DeleteMatrix(B,4,5);
	DeleteMatrix(C,4,5);
	return 0;
}
