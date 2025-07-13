#pragma once
#include"Behavior.h"
#include"Products.h"

class Business :public UserBehavior {
public:
	virtual int getUserType(string userName);

	/*void chooseProductMode(string x, string userName);*/

	int addProducts(string userName, string pname, string type, string belong, string describe, double price, ll remain);

	int changeProduct(string userName, string x, string pname, string type, string New);
};