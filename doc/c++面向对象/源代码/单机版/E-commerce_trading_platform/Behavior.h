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

	void chooseMode(string x) ;

	void Sign_In() ;

	void Sign_Up() ;

	void changePassword();

	double getUserBalance(string userName) ;

	void featurePage(string userName) ;

	void chooseUseMode(string userName, string x) ;

	void Recharge(string userName);

	void Consume(string userName, double money);

	virtual int getUserType(string userName) = 0;

	void Balance_Operator(string userName);

	void chooseBalanceUse(string x, string userName);

	void productPage() ;

	void searchProduct();

	void manageProduct(string userName);

	void Shopping(string userName);
};