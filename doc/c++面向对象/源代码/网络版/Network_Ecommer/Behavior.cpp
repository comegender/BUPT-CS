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
		//cout << "���û����ѱ�ע�ᣬ������ע��!\n";
		return 1;
	}
	if (ACP != password) {
		//cout << "������������벻һ�£�ע��ʧ��!\n";
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
		//cout << "�������벻һ��!\n";
		return 1;
	}
	int flag = sheet.changePassword(username, newpassword);
	if (flag == 1) {
		//cout << "�����ڴ��û�!\n";
		return 2;
	}
	else if (flag == 2) {
		//cout << "�����벻����ԭ������ͬ!\n";
		return 3;
	}
	//cout << "�޸�����ɹ�!\n";
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
//		cout << "---------------------------------------------------------\n1:������\n2:������Ʒ\n";
//		if (type == 1) {
//			cout << "3:����\n";
//		}
//		if (type == 2) {
//			cout << "4:��Ʒ����\n";
//		}
//		cout << "0:�˳�\n---------------------------------------------------------\n";
//		cout << "��ѡ�����Ĳ���:";
//		string x;
//		cin >> x;
//		if ((x != "0" && x != "1" && x != "2" && x != "3" && x != "4") || (type == 1 && x == "4") || (type == 2 && x == "3")) {
//			cout << "�������������ѡ��\n";
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
	if (temp) cout << "���׳ɹ�!\n�˻����:" << setprecision(2) << fixed << sheet.getUserBalance(userName) << endl;
	else cout << "����!\n";
	system("pause");
}

//void UserBehavior::Balance_Operator(string userName) {
//	while (1) {
//		system("cls");
//		cout << "---------------------------------------------------------\n1:��ѯ���\n2:��ֵ���\n0:�˳�\n---------------------------------------------------------\n";
//		cout << "��ѡ�����Ĳ���:";
//		string x;
//		cin >> x;
//		if (x != "0" && x != "1" && x != "2") {
//			cout << "�������������ѡ��\n";
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
//		cout << "---------------------------------------------------------\n1:�����Ʒ\n2:�޸���Ʒ��Ϣ\n0:�˳�\n---------------------------------------------------------\n";
//		cout << "��ѡ�����Ĳ���:";
//		string x;
//		cin >> x;
//		if (x != "0" && x != "1" && x != "2") {
//			cout << "�������������ѡ��\n";
//			system("pause");
//			continue;
//		}
//		buss.chooseProductMode(x, userName);
//		if (x == "0") break;
//	}
//}

//int UserBehavior::Shopping(string userName,string x) {
//	Comsumer cum;
//		//cout << "---------------------------------------------------------\n1:��ӹ��ﳵ\n2:�Ƴ����ﳵ\n3:�鿴���ﳵ\n4:�޸�����\n5:����\n0:�˳�\n---------------------------------------------------------\n";
//		cum.chooseCartMode(x, userName);
//		cum.updateSheet();
//		if (x == "0")  return 0;
//}
