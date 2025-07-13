#pragma once
using namespace std;
#include<unordered_map>
#include<string>
#include<iostream>

class Account {
private:
	string Username;
	string Password;
	double Balance;
	int Usertype;
public:
	Account(string username="", string password="", double balance = 0,int usertype=1) : Username(username), Password(password), Usertype(usertype), Balance(balance) {}

	 void getMessege() const;

	friend ostream & operator<<(ostream& cout, Account a);

	int getType() const;

	string getUsername() const;

	string getPassword() const;
	
	double getBalance() const;

	void changePassword(string newpassword) ;

	void Recharge(double money);

	bool Consume(double cost);
};

ostream & operator<<(ostream& cout, Account a);