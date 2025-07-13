#include<iostream>
#include"Behavior.h"
#include"Tourist.h"
#include"Products.h"
#include"Comsumer.h"
#include"Discaonut.h"
using namespace std;

int main() {
	cout << "欢迎使用电商交易系统Y^_^Y\n作者:BUPT 牢计\n";
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
		cout << "---------------------------------------------------------\n注册请输入:1\n登录请输入:2\n忘记密码请输入:3\n退出系统请输入:0\n---------------------------------------------------------\n";
		cout << "请选择您的操作:";
		cin >> x;
		if (x != "1" && x != "2" && x != "3" && x != "0") {
			cout << "没有这个选择，请重新选择!\n";
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