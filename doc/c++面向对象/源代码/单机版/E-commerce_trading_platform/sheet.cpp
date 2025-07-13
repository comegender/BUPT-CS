#include"Sheet.h"
#include<iostream>
#include<fstream>
using namespace std;

void Usersheet::participle(string pattern,string& name, string& password, double& balance, int& type) {
	int a=-1, b=-1, c=-1;
	for (int i = 0; i < pattern.length(); i++) {
		if (pattern[i] == ',') {
			if (a == -1) a = i;
			else if (b == -1) b = i;
			else if (c == -1) c = i;
		}
	}
	string temp;
	temp = "";
	for (int i = 0; i < a; i++) {
		temp += pattern[i];
	}
	name = temp;	
	temp = "";
	for (int i = a+1; i < b; i++) {
		temp += pattern[i];
	}
	password = temp;	
	temp = "";
	for (int i = b+1; i < c; i++) {
		temp += pattern[i];
	}
	balance = stold(temp);	
	temp = "";
	for (int i = c+1; i < pattern.length(); i++) {
		temp += pattern[i];
	}
	type = temp[0] - '0';
}

void Usersheet::readInInformation() {
	ifstream f;
	f.open("Sheets.csv", ios::in);
	if (f.is_open()) cout << "文件打开成功!\n";
	else cout << "文件打开失败!\n";
	string messeges;
	string key;
	getline(f, messeges);
	while (getline(f, messeges)) {
		string name, password;
		double balance;
		int type;
		participle(messeges, name, password, balance, type);
		Account user(name, password, balance, type);
		Initial[name] = user;
	}
	f.close();
}

void Usersheet::getUserSheet() {
	for (unordered_map<string, Account>::iterator it = Initial.begin(); it != Initial.end(); it++) {
		cout << it->first << " " << it->second;
	}
}

int Usersheet::SerchSignInMessege(string name, string password, int type)  {
	for (unordered_map<string, Account>::iterator it = Initial.begin(); it != Initial.end(); it++) {
		if (it->first == name) {
			if (it->second.getPassword() != password) return 2;
			else if (it->second.getType() != type) return 3;
			else return 4;
		}
	}
	return 1;
}

bool Usersheet::findUsername(string name) const {
	if (Initial.find(name) == Initial.end()) return false;
	return true;
}

int Usersheet::changePassword(string name, string newpassword) {
	if (Initial.find(name) == Initial.end()) return 1;
	else if (Initial[name].getPassword() == newpassword) return 2;
	Initial[name].changePassword(newpassword);
	return 0;
}

void Usersheet::wrightNewMessege(string name, string password, int type) {
	Account newUser(name,password,0,type);
	Initial[name] = newUser;
}

void Usersheet::outMessege() {
	ofstream ofs;
	ofs.open("Sheets.csv", ios::out);
	ofs << "账号,密码,余额,类型" << endl;
	for (unordered_map<string, Account>::iterator it = Initial.begin(); it != Initial.end(); it++) {
		ofs << it->first << ',' << it->second.getPassword() << ',' << it->second.getBalance() << ',' << it->second.getType() << endl;
	}
	ofs.close();
}

double Usersheet::getUserBalance(string name) {
	return Initial[name].getBalance();
}

void Usersheet::Recharge(string name, double money) {
	Initial[name].Recharge(money);
}

bool Usersheet::Consume(string name, double cost) {
	bool temp = Initial[name].Consume(cost);
	if (temp) return true;
	return false;
}

int Usersheet::getUserType(string name)  {
	return Initial[name].getType();
}