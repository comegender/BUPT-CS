#pragma once
#include<string>
#include<unordered_map>
#include"account.h"
using namespace std;

class Usersheet {
private:
	unordered_map<string, Account> Initial;
public:
	void readInInformation();

	void participle(string pattern, string& name, string& password, double& balance, int& type);

	void getUserSheet();

	int SerchSignInMessege(string name, string password, int type);

	bool findUsername(string name);

	int changePassword(string name, string newpassword);

	void wrightNewMessege(string name, string password, int type);

	void outMessege();

	double getUserBalance(string name);

	void Recharge(string name, double money);

	bool Consume(string name, double cost);

	int getUserType(string name);
};