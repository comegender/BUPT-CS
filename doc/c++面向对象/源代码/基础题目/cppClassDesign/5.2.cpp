#include<iostream>
using namespace std;

class Point {
private:
	int x, y;
public:
	Point(int xx=0, int yy=0) :x(xx), y(yy) {};

	Point& operator++() {
		x++;
		y++;
		return *this;
	}
	Point operator++(int) {
		Point temp = *this;
		++(*this);
		return temp;
	}
	Point& operator--() {
		x--;
		y--;
		return *this;
	}
	Point operator--(int) {
		Point temp = *this;
		--(*this);
		return temp;
	}
	void print() {
		cout << x << " " << y << endl;
	}
};


int main() {
	Point P(5, 5);
	Point a;
	a=P++;
	a.print();
	a = ++P;
	a.print();
	a = P--;
	a.print();
	a = --P;
	a.print();
	return 0;
}