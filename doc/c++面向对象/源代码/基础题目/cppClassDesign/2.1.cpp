#include<iostream>
#include<cmath>
using namespace std;

class Point {
private:
	int x, y;
public:
	Point(int xx = 0, int yy = 0) :x(xx), y(yy) {
		cout << "Point�Ĺ��캯������" << endl;
		system("pause");

	}
	~Point() {
		cout << "Point��������������" << endl;
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
		cout << "Circle�Ĺ��캯������" << endl;
		system("pause");
	}

	~Circle() {
		cout << "Circle��������������" << endl;
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
	cout << "�������һ��Բ��Բ�����꣺" << endl;
	cout << "x����: ";
	cin >> x;
	cout << "y����: ";
	cin >> y;
	cout << "�������һ��Բ�İ뾶��";
	cin >> r;
	Circle c1(x,y,r);
	cout << "������ڶ���Բ��Բ�����꣺" << endl;
	cout << "x����: ";
	cin >> x;
	cout << "y����: ";
	cin >> y;
	cout << "������ڶ���Բ�İ뾶��";
	cin >> r;
	Circle c2(x, y, r);
	if (isIntersect(c1, c2)) cout << "�ཻ" << endl;
	else cout << "���ཻ" << endl;
	return 0;
}