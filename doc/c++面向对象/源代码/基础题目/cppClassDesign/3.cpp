#include<iostream>
using namespace std;
#define PI 3.14

class Shape {
public:
	Shape() {
		cout << "Shape���캯������" << endl;
	}

	~Shape() {
		cout << "Shape������������" << endl;
	}

	double Area() {
		return 0;
	}
};

class Rectangle :public Shape {
private:
	int len, wid;
public:
	Rectangle(int lenth=0,int width=0):len(lenth),wid(width) {
		cout << "Rectangle���캯������" << endl;
	}

	~Rectangle() {
		cout << "Rectangle������������" << endl;
	}

	double Area() {
		return len * wid;
	}
};

class Circle :public Shape {
private:
	int radius;
public:
	Circle(int r=0) :radius(r) {
		cout << "Circle���캯������" << endl;
	}

	~Circle() {
		cout << "Circle������������" << endl;
	}

	double Area() {
		return PI * radius * radius;
	}
};

class Square :public Rectangle {
public:
	Square(int l=0) :Rectangle(l, l) {
		cout << "Square���캯������" << endl;
	}

	~Square() {
		cout << "Square������������" << endl;
	}
};


int main() {
	Rectangle A(2,3);
	cout << A.Area() << endl;
	Circle B(1);
	cout << B.Area() << endl;
	Square C(2);
	cout << C.Area() << endl;
	return 0;
}