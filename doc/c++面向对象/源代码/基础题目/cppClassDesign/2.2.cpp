#include<iostream>
using namespace std;

class Matrix {
private:
	int line, row;
	int** num;
public:
	Matrix(int l = 0, int r = 0) :line(l), row(r) {
		num = new int* [l];
		for (int i = 0; i < l; i++) {
			num[i] = new int[r];
		}
	}

	~Matrix() {
		for (int i = 0; i < line; i++) {
			delete[] num[i];
		}
		delete[] num;
	}

	Matrix(const Matrix& a) {
		line = a.line;
		row = a.row;
		num = new int* [line];
		for (int i = 0; i < line; i++) {
			num[i] = new int[row];
		}
		for (int i = 0; i < line; i++) {
			for (int j = 0; j < row; j++) {
				num[i][j] = a.num[i][j];
			}
		}
	}

	void getMatrix() {
		for (int i = 0; i < line; i++) {
			for (int j = 0; j < row; j++) {
				cin >> num[i][j];
			}
		}
	}

	void outMatrix() {
		for (int i = 0; i < line; i++) {
			for (int j = 0; j < row; j++) {
				cout << num[i][j] << " ";
			}
			cout << endl;
		}
	}

	Matrix addMatrix(const Matrix& a) {
		if (line != a.line || row != a.row) {
			cerr << "矩阵大小不匹配，无法相加\n";
			exit(114514);
		}
		Matrix temp(line,row);
		for (int i = 0; i < line; i++) {
			for (int j = 0; j < row; j++) {
				temp.num[i][j] = num[i][j] + a.num[i][j];
			}
		}
		return temp;
	}

	Matrix subMatrix(const Matrix& a) {
		if (line != a.line || row != a.row) {
			cerr << "矩阵大小不匹配，无法相减\n";
			exit(114514);
		}
		Matrix temp(line, row);
		for (int i = 0; i < line; i++) {
			for (int j = 0; j < row; j++) {
				temp.num[i][j] = num[i][j] - a.num[i][j];
			}
		}
		return temp;
	}

	Matrix& operator=(const Matrix& a) {
		if (this == &a) {
			return *this;
		}
		//释放旧元素
		for (int i = 0; i < line; i++) {
			delete[] this->num[i];
		}
		delete[] this->num;

		line = a.line;
		row = a.row;
		num = new int* [line];
		for (int i = 0; i < line; i++) {
			num[i] = new int[row];
			for (int j = 0; j < row; j++) {
				num[i][j] = a.num[i][j];
			}
		}
		return *this;
	}

};


int main() {
	Matrix A1(2,2), A2(2,2), A3(2,2);
	A1.getMatrix();
	A2.getMatrix();
	A3 = A1.addMatrix(A2);
	A3.outMatrix();
	A3 = A1.subMatrix(A2);
	A3.outMatrix();
	Matrix *pA1, *pA2, *pA3;
	pA1 = new Matrix(2, 2);
	pA2 = new Matrix(2, 2);
	pA3 = new Matrix(2, 2);
	pA1->getMatrix();
	pA2->getMatrix();
	*pA3 = pA1->addMatrix(*pA2);
	pA3->outMatrix();
	*pA3 = pA1->subMatrix(*pA2);
	pA3->outMatrix();
	delete pA1;
	delete pA2;
	delete pA3;
	return 0;
}