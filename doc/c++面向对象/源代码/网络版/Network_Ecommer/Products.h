#pragma once
#include"ProductsIfo.h"
#include<vector>
#include<unordered_map>
#include<string>

class Products {
private:
	vector<productsIfo> psheet;
	unordered_map<string, vector<int>> pname_key;
	unordered_map<string, vector<int>> type_key;
	unordered_map<string, vector<int>> belong_key;
public:
	void getPsheet();

	void participle(string pattern, string& pname, double& price, string& describe, ll& remain, string& type, string& belong, double& newPrice);

	bool isRepeat(productsIfo a);

	void addProducts(productsIfo a);

	void outSheet();

	Products();

	~Products();

	int changeProduct(string userName, string x, string pname, string type, string New);

	void chooseChangeProductMode(string x, string userName, int insert,string New);

	void productPage();

	void searchProduct();

	void chooseSearchMode(string x);

	productsIfo returnProductIfo(string pname, string type, string belong);

	int returnProductIfoInsert(string pname, string type, string belong);

	virtual double getPrice();

	void Discount();

	void lockRemain(string pname, string type, string belong, ll cost);

	void unlockRemain(string pname, string type, string belong, ll cost);
};

class Book :public Products {
private:
	string pname, belong;
public:
	Book(string pname, string belong) :pname(pname), belong(belong) {}

	virtual double getPrice();
};

class Food :public Products {
private:
	string pname, belong;
public:
	Food(string pname, string belong) :pname(pname), belong(belong) {}

	virtual double getPrice();
};

class Cloth :public Products {
private:
	string pname, belong;
public:
	Cloth(string pname, string belong) :pname(pname), belong(belong) {}

	virtual double getPrice();
};

