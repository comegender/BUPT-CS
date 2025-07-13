#include"Behavior.h"
#include"Sheet.h"
#include"Business.h"
#include"Comsumer.h"
#include"Discaonut.h"
#include<fstream>
#include<iomanip>

UserBehavior::UserBehavior() {
	sheet.readInInformation();
}

UserBehavior::~UserBehavior() {
	sheet.outMessege();
}

int UserBehavior::Sign_In(string username, string password, int type) {
	if (type == 0) return 0;
	return sheet.SerchSignInMessege(username, password, type);
}

int UserBehavior::Sign_Up(string username, string password, int type, string ACP) {
	if (username == "r") return 0;
	if (sheet.findUsername(username)) {
		//cout << "该用户名已被注册，请重新注册!\n";
		return 1;
	}
	if (ACP != password) {
		//cout << "两次输入的密码不一致，注册失败!\n";
		return 2;
	}
	sheet.wrightNewMessege(username, password, type);
	return 3;
}

//void UserBehavior::chooseMode(string x) {
//	if (x == "1") Sign_Up();
//	else if (x == "2") Sign_In();
//	else if (x == "0") return;
//	else if (x == "3") changePassword();
//}

int UserBehavior::changePassword(string username, string newpassword, string ACP) {
	if (username == "r") return 0;
	if (newpassword != ACP) {
		//cout << "两次密码不一致!\n";
		return 1;
	}
	int flag = sheet.changePassword(username, newpassword);
	if (flag == 1) {
		//cout << "不存在此用户!\n";
		return 2;
	}
	else if (flag == 2) {
		//cout << "新密码不能与原密码相同!\n";
		return 3;
	}
	//cout << "修改密码成功!\n";
	return 4;
}

double UserBehavior::getUserBalance(string userName) {
	return sheet.getUserBalance(userName);
}

//void UserBehavior::chooseUseMode(string userName, string x) {
//	if (x == "0") return;
//	else if (x == "1") Balance_Operator(userName);
//	else if (x == "2") searchProduct();
//	else if (x == "3") Shopping(userName);
//	else if (x == "4") manageProduct(userName);
//	system("pause");
//}

//void UserBehavior::featurePage(string userName) {
//	for (;;) {
//		system("cls");
//		int type = getUserType(userName);
//		productPage();
//		cout << "---------------------------------------------------------\n1:余额操作\n2:搜索商品\n";
//		if (type == 1) {
//			cout << "3:购物\n";
//		}
//		if (type == 2) {
//			cout << "4:商品管理\n";
//		}
//		cout << "0:退出\n---------------------------------------------------------\n";
//		cout << "请选择您的操作:";
//		string x;
//		cin >> x;
//		if ((x != "0" && x != "1" && x != "2" && x != "3" && x != "4") || (type == 1 && x == "4") || (type == 2 && x == "3")) {
//			cout << "输入错误，请重新选择！\n";
//			system("pause");
//			continue;
//		}
//		chooseUseMode(userName, x);
//		if (x == "0") break;
//	}
//
//}

double UserBehavior::Recharge(string userName,double money) {
	if (money == 0) return -1;
	sheet.Recharge(userName, money);
	sheet.outMessege();
	return sheet.getUserBalance(userName);
}

void UserBehavior::Consume(string userName, double money) {
	if (money == 0) return;
	bool temp = sheet.Consume(userName, money);
	if (temp) cout << "交易成功!\n账户余额:" << setprecision(2) << fixed << sheet.getUserBalance(userName) << endl;
	else cout << "余额不足!\n";
	system("pause");
}

//void UserBehavior::Balance_Operator(string userName) {
//	while (1) {
//		system("cls");
//		cout << "---------------------------------------------------------\n1:查询余额\n2:充值余额\n0:退出\n---------------------------------------------------------\n";
//		cout << "请选择您的操作:";
//		string x;
//		cin >> x;
//		if (x != "0" && x != "1" && x != "2") {
//			cout << "输入错误，请重新选择！\n";
//			system("pause");
//			continue;
//		}
//		chooseBalanceUse(x, userName);
//		if (x == "0") return;
//	}
//}

//void UserBehavior::chooseBalanceUse(string x, string userName) {
//	if (x == "1") getUserBalance(userName);
//	else if (x == "2") Recharge(userName);
//	else if (x == "0") return;
//	system("pause");
//}

void UserBehavior::productPage() {
	p.productPage();
}

void UserBehavior::searchProduct() {
	p.searchProduct();
}

//void UserBehavior::manageProduct(string userName) {
//	Business buss;
//	for (;;) {
//		system("cls");
//		cout << "---------------------------------------------------------\n1:添加商品\n2:修改商品信息\n0:退出\n---------------------------------------------------------\n";
//		cout << "请选择您的操作:";
//		string x;
//		cin >> x;
//		if (x != "0" && x != "1" && x != "2") {
//			cout << "输入错误，请重新选择！\n";
//			system("pause");
//			continue;
//		}
//		buss.chooseProductMode(x, userName);
//		if (x == "0") break;
//	}
//}

//int UserBehavior::Shopping(string userName,string x) {
//	Comsumer cum;
//		//cout << "---------------------------------------------------------\n1:添加购物车\n2:移除购物车\n3:查看购物车\n4:修改数量\n5:结账\n0:退出\n---------------------------------------------------------\n";
//		cum.chooseCartMode(x, userName);
//		cum.updateSheet();
//		if (x == "0")  return 0;
//}
