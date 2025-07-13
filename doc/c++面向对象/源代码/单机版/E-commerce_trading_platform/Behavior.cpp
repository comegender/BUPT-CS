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

void UserBehavior::Sign_In()  {
	int type;
	cout << "�������������(1:������ 2:�̼�)(����0�˳�):";
	cin >> type;
	if (type == 0) return;
	getc(stdin);
	string username;
	cout << "�������û���:";
	getline(cin, username);
	if (username.length() == 0 || username.find(' ') != string::npos) {
		cout << "�û��������ǿգ������пո�\n";
		system("pause");
		return;
	}
	string password;
	cout << "����������:";
	getline(cin, password);
	if (sheet.SerchSignInMessege(username, password, type) == 4) {
		cout << "��¼�ɹ�!\n";
		system("pause");
		system("cls");
		featurePage(username);
	}
	else if (sheet.SerchSignInMessege(username, password, type) == 2) cout << "���벻��ȷ!\n";
	else if (sheet.SerchSignInMessege(username, password, type) == 3) cout << "��ݲ���ȷ!\n";
	else {
		cout << "û�и��û�������ע��!\n";
	}
}

void UserBehavior::Sign_Up() {
	string name;
	string password;
	string ACP;
	int type;
	cout << "�����������û���(����r����):";
	getline(cin , name);
	if (name.length() == 0 || name.find(' ') != string::npos) {
		cout << "�û��������ǿգ������пո�\n";
		system("pause");
		return;
	}
	if (name == "r") return;
	if (sheet.findUsername(name)) {
		cout << "���û����ѱ�ע�ᣬ������ע��!\n";
		return;
	}
	cout << "��������������:";
	getline(cin , password);
	if (password.length() == 0 || password.find(' ') != string::npos) {
		cout << "���벻���ǿգ������пո�\n";
		system("pause");
		return;
	}
	cout << "��ȷ������:";
	getline(cin,ACP);
	cout << "��ѡ���������:(������������1���̼�������2):";
	cin >> type;
	if (ACP != password) {
		cout << "������������벻һ�£�ע��ʧ��!\n";
		return;
	}
	sheet.wrightNewMessege(name,password,type);
	cout << "ע��ɹ�!\n";
}

void UserBehavior::chooseMode(string x)  {
	if (x == "1") Sign_Up();
	else if (x == "2") Sign_In();
	else if (x == "0") return;
	else if (x == "3") changePassword();
}

void UserBehavior::changePassword() {
	string newpassword;
	string name;
	string ACP;
	cout << "�����������û���(����r����):";
	getline(cin, name);
	if (name == "r") return;
	cout << "����������������:";
	getline(cin, newpassword);
	if (newpassword.length() == 0 || newpassword.find(' ') != string::npos) {
		cout << "���벻���ǿգ������пո�\n";
		system("pause");
		return;
	}
	cout << "��ȷ������������:";
	getline(cin, ACP);
	if (newpassword != ACP) {
		cout << "�������벻һ��!\n";
		return;
	}
	int flag = sheet.changePassword(name, newpassword);
	if (flag == 1) {
		cout << "�����ڴ��û�!\n";
		return;
	}
	else if (flag == 2) {
		cout << "�����벻����ԭ������ͬ!\n";
		return;
	}
	cout << "�޸�����ɹ�!\n";
}

double UserBehavior::getUserBalance(string userName) {
	cout << "�������Ϊ:" << setprecision(2) << fixed << sheet.getUserBalance(userName) << endl;
	return sheet.getUserBalance(userName);
}

void UserBehavior::chooseUseMode(string userName,string x) {
	if (x == "0") return;
	else if (x == "1") Balance_Operator(userName);
	else if (x == "2") searchProduct();
	else if (x == "3") Shopping(userName);
	else if (x == "4") manageProduct(userName);
	system("pause");
}

void UserBehavior::featurePage(string userName) {
	for (;;) {
		system("cls");
		int type = getUserType(userName);
		productPage();
		cout << "---------------------------------------------------------\n1:������\n2:������Ʒ\n";
		if (type == 1) {
			cout << "3:����\n";
		}
		if (type == 2) {
			cout << "4:��Ʒ����\n";
		}
		cout << "0:�˳�\n---------------------------------------------------------\n";
		cout << "��ѡ�����Ĳ���:";
		string x;
		cin >> x;
		if ((x != "0" && x != "1" && x != "2" && x != "3" && x != "4") || (type == 1 && x == "4") || (type == 2 && x == "3")) {
			cout << "�������������ѡ��\n";
			system("pause");
			continue;
		}
		chooseUseMode(userName, x);
		if (x == "0") break;
	}

}

void UserBehavior::Recharge(string userName) {
	cout << "����������Ҫ��ֵ�Ľ��(����0����):";
	string input;
	cin >> input;
	// ��������Ƿ�Ϸ�
	bool isValid = true;
	bool hasDecimal = false;
	int decimalDigits = 0;

	for (char c : input) {
		if (c == '.') {
			if (hasDecimal) {  // �Ѿ���С���㣬��������
				isValid = false;
				break;
			}
			hasDecimal = true;
		}
		else if (c == '-') {  // ��������
			isValid = false;
			break;
		}
		else if (!isdigit(c)) {  // �������ַ�
			isValid = false;
			break;
		}
		else if (hasDecimal) {  // С�����λ�����
			decimalDigits++;
			if (decimalDigits > 2) {  // ���� 2 λС��
				isValid = false;
				break;
			}
		}
	}

	if (!isValid) {
		cout << "������Ч��������Ǹ����������λС�����磺100 �� 50.25��\n";
		return;  // ��������
	}

	// ת��Ϊ double ������Ƿ�Ϊ 0
	double money = stod(input);
	if (money <= 0) {
		cout << "��ֵ��������� 0��\n";
		return;
	}
	if (money == 0) return;
	sheet.Recharge(userName, money);
	sheet.outMessege();
	cout << "��ֵ�ɹ�!\n�˻����:" << setprecision(2) << fixed << sheet.getUserBalance(userName) << endl;
}

void UserBehavior::Consume(string userName,double money) {
	if (money == 0) return;
	bool temp=sheet.Consume(userName,money);
	if (temp) cout << "���׳ɹ�!\n�˻����:" << setprecision(2) << fixed << sheet.getUserBalance(userName) << endl;
	else cout << "����!\n";
}

void UserBehavior::Balance_Operator(string userName) {
	while (1) {
		system("cls");
		cout << "---------------------------------------------------------\n1:��ѯ���\n2:��ֵ���\n0:�˳�\n---------------------------------------------------------\n";
		cout << "��ѡ�����Ĳ���:";
		string x;
		cin >> x;
		if (x != "0" && x != "1" && x != "2") {
			cout << "�������������ѡ��\n";
			system("pause");
			continue;
		}
		chooseBalanceUse(x, userName);
		if (x == "0") return;
	}
}

void UserBehavior::chooseBalanceUse(string x,string userName) {
	if (x == "1") getUserBalance(userName);
	else if (x == "2") Recharge(userName);
	else if (x == "0") return;
	system("pause");
}

void UserBehavior::productPage()  {
	p.productPage();
}

void UserBehavior::searchProduct() {
	p.searchProduct();
}

void UserBehavior::manageProduct(string userName) {
	Business buss;
	for (;;) {
		system("cls");
		cout << "---------------------------------------------------------\n1:�����Ʒ\n2:�޸���Ʒ��Ϣ\n0:�˳�\n---------------------------------------------------------\n";
		cout << "��ѡ�����Ĳ���:";
		string x;
		cin >> x;
		if (x != "0" && x != "1" && x != "2") {
			cout << "�������������ѡ��\n";
			system("pause");
			continue;
		}
		buss.chooseProductMode(x, userName);
		if (x == "0") break;
	}
}

void UserBehavior::Shopping(string userName) {
	Comsumer cum;
	for (;;) {
		system("cls");
		cout << "---------------------------------------------------------\n1:��ӹ��ﳵ\n2:�Ƴ����ﳵ\n3:�鿴���ﳵ\n4:�޸�����\n5:����\n0:�˳�\n---------------------------------------------------------\n";
		cout << "��ѡ�����Ĳ���:";
		string x;
		cin >> x;
		if (x != "0" && x != "1" && x != "2" && x != "3" && x != "4" && x != "5") {
			cout << "�������������ѡ��\n";
			system("pause");
			continue;
		}
		cum.chooseCartMode(x, userName);
		cum.updateSheet();
		if (x == "0") break;
	}
}
