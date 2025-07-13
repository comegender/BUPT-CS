#pragma once
#include<string>
using namespace std;
typedef long long ll;

class productsIfo {
private:
	string pname;
	double price;
	double newPrice;// ÕÛºó¼Û
	string describe;
	ll  remain;
	string type;
	string belong;
public:
	productsIfo(string pname = "", double price = 0, string describe = "", ll remain = 0, string type = "", string belong = "", double newPrice = 0) :pname(pname), price(price), describe(describe), remain(remain), type(type), belong(belong) {
		if (newPrice == 0) this->newPrice = price;
		else this->newPrice = newPrice;
	}

	double getPrice();

	bool&& operator==(productsIfo b);

	string getPname() const;

	string getType() const;

	string getBelong() const;

	string getDescribe() const;

	ll getRemain() const;

	double getNewPrice() const;

	void changePname(string New);

	void changeType(string New);

	void changeDescribe(string New);

	void changeRemain(ll New);

	void changePrice(double New);

	void changeNewPrice(double New); 

};

