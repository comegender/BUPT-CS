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
		cout << "������"<<l<<"-"<<r<<"�����֣�" << endl;
		cin >> tnum;
		if (tnum > num) {
			r = tnum;
			cout << "�´�����" << endl;
		}
		else if (tnum < num) {
			l = tnum;
			cout << "��С����" << endl;
		}
	}
	if (tnum == num) cout << "�¶�����" << endl;
}

int main() {
	solve();
	return 0;
}