#include<iostream>
#include"Behavior.h"
#include"Tourist.h"
#include"Products.h"
#include"Comsumer.h"
#include"Discaonut.h"
using namespace std;

int main() {
	cout << "��ӭ�������̽���ϵͳY^_^Y\n������:BUPT ������\n";
	if (IS_DISCOUNT) {
		Products p;
		p.Discount();
	}
	Tourist a;
	string x;
	while (1) {
		system("pause");
		system("cls");
		a.productPage();
		cout << "---------------------------------------------------------\n1:ע��\n2:��¼\n3:������\n0:�˳�\n---------------------------------------------------------\n";
		cout << "����������ѡ��:";
		cin >> x;
		if (x != "1" && x != "2" && x != "3" && x != "0") {
			cout << "û�����ѡ��!\n";
			continue;
		}
		getc(stdin);
		system("cls");
		a.chooseMode(x);
		a.updateSheet();
		if (x == "0") break;
	}
	return 0;
}