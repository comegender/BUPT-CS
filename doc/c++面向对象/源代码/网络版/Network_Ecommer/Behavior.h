#pragma once
#include"Sheet.h"
#include"Products.h"

class UserBehavior {
protected:
	Usersheet sheet;
	Products p;
public:
	UserBehavior();

	~UserBehavior();

	/*void chooseMode(string x);*/

	int Sign_In(string username, string password, int type);

	int Sign_Up(string username, string password, int type, string ACP);

	int changePassword(string username, string newpassword, string ACP);

	double getUserBalance(string userName);

	void featurePage(string userName);

	/*void chooseUseMode(string userName, string x);*/

	double Recharge(string userName,double money);

	void Consume(string userName, double money);

	virtual int getUserType(string userName) = 0;

	/*void Balance_Operator(string userName);*/

	//void chooseBalanceUse(string x, string userName);

	void productPage();

	void searchProduct();

	/*void manageProduct(string userName);*/

	int Shopping(string userName,string x);
};