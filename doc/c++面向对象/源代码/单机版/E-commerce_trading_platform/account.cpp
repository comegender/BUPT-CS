#include<iostream>
#include"account.h"
using namespace std;

void Account::getMessege() const{
	cout << Password << " " << Balance << " " << Usertype << endl;
}

ostream & operator<<(ostream& cout, Account a) {
	a.getMessege();
	return cout;
}

int Account::getType() const {
	return Usertype;
}

string Account::getUsername() const {
	return Username;
}

string Account::getPassword() const {
	return Password;
}

double Account::getBalance() const {
	return Balance;
}

void Account::changePassword(string newpassword) {
	Password = newpassword;
}

void Account::Recharge(double money) {
	Balance += money;
}

bool Account::Consume(double cost) {
	if (cost > Balance) return false;
	Balance -= cost;
	return true;
}
