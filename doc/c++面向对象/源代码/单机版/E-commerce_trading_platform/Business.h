#pragma once
#include"Behavior.h"
#include"Products.h"

class Business :public UserBehavior {
public:
	int getUserType(string userName);

	void chooseProductMode(string x,string userName);

	void addProducts(string userName);

	void changeProduct(string userName);
};