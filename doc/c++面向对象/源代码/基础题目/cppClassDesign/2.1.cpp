#include<iostream>
#include<cmath>
using namespace std;

class Point {
private:
	int x, y;
public:
	Point(int xx = 0, int yy = 0) :x(xx), y(yy) {
		cout << "Point的构造函数调用" << endl;
		system("pause");

	}
	~Point() {
		cout << "Point的析构函数调用" << endl;
		system("pause");
	}
	double Distance(Point other) {
		int dx = x - other.x;
		int dy = y - other.y;
		return sqrt(dx * dx + dy * dy);
	}
};

class Circle {
private:
	Point center;
	double radius;
public:

	Circle(int x, int y, double r) : center(x, y), radius(r) {
		cout << "Circle的构造函数调用" << endl;
		system("pause");
	}

	~Circle() {
		cout << "Circle的析构函数调用" << endl;
		system("pause");
	}

	Point getCenter() {
		return center;
	}
	double getRadius() {
		return radius;
	}
};

bool isIntersect(Circle& a, Circle& b) {
	double centerdis = a.getCenter().Distance(b.getCenter());
	double addRadius = a.getRadius() + b.getRadius();
	double subRadius = abs(a.getRadius() - b.getRadius());
	if (centerdis <= addRadius && centerdis >= subRadius) return true;
	return false;
}

int main() {
	int x, y;
	double r;
	cout << "请输入第一个圆的圆心坐标：" << endl;
	cout << "x坐标: ";
	cin >> x;
	cout << "y坐标: ";
	cin >> y;
	cout << "请输入第一个圆的半径：";
	cin >> r;
	Circle c1(x,y,r);
	cout << "请输入第二个圆的圆心坐标：" << endl;
	cout << "x坐标: ";
	cin >> x;
	cout << "y坐标: ";
	cin >> y;
	cout << "请输入第二个圆的半径：";
	cin >> r;
	Circle c2(x, y, r);
	if (isIntersect(c1, c2)) cout << "相交" << endl;
	else cout << "不相交" << endl;
	return 0;
}