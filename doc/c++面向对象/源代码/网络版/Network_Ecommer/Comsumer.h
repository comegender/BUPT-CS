#pragma once
#include"Behavior.h"
#include"Products.h"
#include<set>
#include<unordered_map>

class Comsumer :public UserBehavior {
private:
	unordered_map< string, vector<productsIfo> > shopping_cart;
public:
	virtual int getUserType(string userName);

	void getCart();

	void participle(string pattern, string& cname, string& pname, ll& quantity, string& type, string& belong);

	void outCart();

	Comsumer();

	~Comsumer();

	int addCart(string cname, string pname, string type, string belong, ll count);

	int removeCart(string cname, string pname, string type, string belong, ll count);

	void showCart(string cname);

	void submitBill(string cname);

	int changeCart(string cname, string pname, string type, string belong, ll count);

	//void chooseCartMode(string x, string cname);

	int findCart(string cname, string pname, string type, string belong);

	void updateSheet();
};