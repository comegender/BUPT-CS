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
	cout << "请输入您的身份(1:消费者 2:商家)(输入0退出):";
	cin >> type;
	if (type == 0) return;
	getc(stdin);
	string username;
	cout << "请输入用户名:";
	getline(cin, username);
	if (username.length() == 0 || username.find(' ') != string::npos) {
		cout << "用户名不能是空，不能有空格\n";
		system("pause");
		return;
	}
	string password;
	cout << "请输入密码:";
	getline(cin, password);
	if (sheet.SerchSignInMessege(username, password, type) == 4) {
		cout << "登录成功!\n";
		system("pause");
		system("cls");
		featurePage(username);
	}
	else if (sheet.SerchSignInMessege(username, password, type) == 2) cout << "密码不正确!\n";
	else if (sheet.SerchSignInMessege(username, password, type) == 3) cout << "身份不正确!\n";
	else {
		cout << "没有该用户，请先注册!\n";
	}
}

void UserBehavior::Sign_Up() {
	string name;
	string password;
	string ACP;
	int type;
	cout << "请输入您的用户名(输入r返回):";
	getline(cin , name);
	if (name.length() == 0 || name.find(' ') != string::npos) {
		cout << "用户名不能是空，不能有空格\n";
		system("pause");
		return;
	}
	if (name == "r") return;
	if (sheet.findUsername(name)) {
		cout << "该用户名已被注册，请重新注册!\n";
		return;
	}
	cout << "请输入您的密码:";
	getline(cin , password);
	if (password.length() == 0 || password.find(' ') != string::npos) {
		cout << "密码不能是空，不能有空格\n";
		system("pause");
		return;
	}
	cout << "请确认密码:";
	getline(cin,ACP);
	cout << "请选择您的身份:(消费者请输入1，商家请输入2):";
	cin >> type;
	if (ACP != password) {
		cout << "两次输入的密码不一致，注册失败!\n";
		return;
	}
	sheet.wrightNewMessege(name,password,type);
	cout << "注册成功!\n";
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
	cout << "请输入您的用户名(输入r返回):";
	getline(cin, name);
	if (name == "r") return;
	cout << "请输入您的新密码:";
	getline(cin, newpassword);
	if (newpassword.length() == 0 || newpassword.find(' ') != string::npos) {
		cout << "密码不能是空，不能有空格\n";
		system("pause");
		return;
	}
	cout << "请确认您的新密码:";
	getline(cin, ACP);
	if (newpassword != ACP) {
		cout << "两次密码不一致!\n";
		return;
	}
	int flag = sheet.changePassword(name, newpassword);
	if (flag == 1) {
		cout << "不存在此用户!\n";
		return;
	}
	else if (flag == 2) {
		cout << "新密码不能与原密码相同!\n";
		return;
	}
	cout << "修改密码成功!\n";
}

double UserBehavior::getUserBalance(string userName) {
	cout << "您的余额为:" << setprecision(2) << fixed << sheet.getUserBalance(userName) << endl;
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
		cout << "---------------------------------------------------------\n1:余额操作\n2:搜索商品\n";
		if (type == 1) {
			cout << "3:购物\n";
		}
		if (type == 2) {
			cout << "4:商品管理\n";
		}
		cout << "0:退出\n---------------------------------------------------------\n";
		cout << "请选择您的操作:";
		string x;
		cin >> x;
		if ((x != "0" && x != "1" && x != "2" && x != "3" && x != "4") || (type == 1 && x == "4") || (type == 2 && x == "3")) {
			cout << "输入错误，请重新选择！\n";
			system("pause");
			continue;
		}
		chooseUseMode(userName, x);
		if (x == "0") break;
	}

}

void UserBehavior::Recharge(string userName) {
	cout << "请输入您想要充值的金额(输入0返回):";
	string input;
	cin >> input;
	// 检查输入是否合法
	bool isValid = true;
	bool hasDecimal = false;
	int decimalDigits = 0;

	for (char c : input) {
		if (c == '.') {
			if (hasDecimal) {  // 已经有小数点，不能再有
				isValid = false;
				break;
			}
			hasDecimal = true;
		}
		else if (c == '-') {  // 不允许负数
			isValid = false;
			break;
		}
		else if (!isdigit(c)) {  // 非数字字符
			isValid = false;
			break;
		}
		else if (hasDecimal) {  // 小数点后位数检查
			decimalDigits++;
			if (decimalDigits > 2) {  // 超过 2 位小数
				isValid = false;
				break;
			}
		}
	}

	if (!isValid) {
		cout << "输入无效！请输入非负数且最多两位小数（如：100 或 50.25）\n";
		return;  // 重新输入
	}

	// 转换为 double 并检查是否为 0
	double money = stod(input);
	if (money <= 0) {
		cout << "充值金额必须大于 0！\n";
		return;
	}
	if (money == 0) return;
	sheet.Recharge(userName, money);
	sheet.outMessege();
	cout << "充值成功!\n账户余额:" << setprecision(2) << fixed << sheet.getUserBalance(userName) << endl;
}

void UserBehavior::Consume(string userName,double money) {
	if (money == 0) return;
	bool temp=sheet.Consume(userName,money);
	if (temp) cout << "交易成功!\n账户余额:" << setprecision(2) << fixed << sheet.getUserBalance(userName) << endl;
	else cout << "余额不足!\n";
}

void UserBehavior::Balance_Operator(string userName) {
	while (1) {
		system("cls");
		cout << "---------------------------------------------------------\n1:查询余额\n2:充值余额\n0:退出\n---------------------------------------------------------\n";
		cout << "请选择您的操作:";
		string x;
		cin >> x;
		if (x != "0" && x != "1" && x != "2") {
			cout << "输入错误，请重新选择！\n";
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
		cout << "---------------------------------------------------------\n1:添加商品\n2:修改商品信息\n0:退出\n---------------------------------------------------------\n";
		cout << "请选择您的操作:";
		string x;
		cin >> x;
		if (x != "0" && x != "1" && x != "2") {
			cout << "输入错误，请重新选择！\n";
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
		cout << "---------------------------------------------------------\n1:添加购物车\n2:移除购物车\n3:查看购物车\n4:修改数量\n5:结账\n0:退出\n---------------------------------------------------------\n";
		cout << "请选择您的操作:";
		string x;
		cin >> x;
		if (x != "0" && x != "1" && x != "2" && x != "3" && x != "4" && x != "5") {
			cout << "输入错误，请重新选择！\n";
			system("pause");
			continue;
		}
		cum.chooseCartMode(x, userName);
		cum.updateSheet();
		if (x == "0") break;
	}
}
