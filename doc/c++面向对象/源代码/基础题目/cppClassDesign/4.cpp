#include<iostream>
#include<cstdlib>
#include<ctime>
using namespace std;

void solve() {
	srand(time(NULL));
	int num = (rand() % 1000) + 1;
	int l=1, r=1000;
	int tnum=0;
	while (tnum != num) {
		cout << "请输入"<<l<<"-"<<r<<"的数字：" << endl;
		cin >> tnum;
		if (tnum > num) {
			r = tnum;
			cout << "猜大啦！" << endl;
		}
		else if (tnum < num) {
			l = tnum;
			cout << "猜小啦！" << endl;
		}
	}
	if (tnum == num) cout << "猜对啦！" << endl;
}

int main() {
	solve();
	return 0;
}