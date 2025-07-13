#include<iostream>
#include"Behavior.h"
#include"Tourist.h"
#include"Products.h"
#include"Comsumer.h"
#include"Discaonut.h"
using namespace std;

int main() {
	cout << "欢迎来到电商交易系统Y^_^Y\n开发者:BUPT 计子毅\n";
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
		cout << "---------------------------------------------------------\n1:注册\n2:登录\n3:改密码\n0:退出\n---------------------------------------------------------\n";
		cout << "请输入您的选择:";
		cin >> x;
		if (x != "1" && x != "2" && x != "3" && x != "0") {
			cout << "没有这个选择!\n";
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