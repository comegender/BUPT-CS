#include<iostream>
#include"Behavior.h"
#include"Tourist.h"
#include"Products.h"
#include"Comsumer.h"
#include"Discaonut.h"
using namespace std;

int main() {
	cout << "��ӭʹ�õ��̽���ϵͳY^_^Y\n����:BUPT �μ�\n";
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
		cout << "---------------------------------------------------------\nע��������:1\n��¼������:2\n��������������:3\n�˳�ϵͳ������:0\n---------------------------------------------------------\n";
		cout << "��ѡ�����Ĳ���:";
		cin >> x;
		if (x != "1" && x != "2" && x != "3" && x != "0") {
			cout << "û�����ѡ��������ѡ��!\n";
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