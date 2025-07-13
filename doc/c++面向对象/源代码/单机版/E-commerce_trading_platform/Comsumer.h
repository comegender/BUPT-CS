#pragma once
#include"Behavior.h"
#include"Products.h"
#include<unordered_map>

class Comsumer:public UserBehavior {
private:
	unordered_map< string, vector<productsIfo> > shopping_cart;
public:
	int getUserType(string userName);

	void getCart();

	void participle(string pattern, string& cname, string& pname, ll& quantity, string& type, string& belong);

	void outCart();

	Comsumer();

	~Comsumer();

	void addCart(string cname);

	void removeCart(string cname);

	void showCart(string cname);

	void submitBill(string cname);

	void changeCart(string cname);

	void chooseCartMode(string x, string cname);

	int findCart(string cname,string pname, string type, string belong);

	void updateSheet();
};