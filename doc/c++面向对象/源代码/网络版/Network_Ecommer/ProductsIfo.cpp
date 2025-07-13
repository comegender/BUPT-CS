#include"ProductsIfo.h"
using namespace std;

double productsIfo::getPrice() const {
	return price;
}

bool&& productsIfo::operator==(productsIfo b) {
	return (pname == b.pname) && (type == b.type) && (belong == b.belong);
}

string productsIfo::getPname() const {
	return pname;
}

string productsIfo::getType() const {
	return type;
}

string productsIfo::getBelong() const {
	return belong;
}

string productsIfo::getDescribe() const {
	return describe;
}

ll productsIfo::getRemain() const {
	return remain;
}

void productsIfo::changePname(string New) {
	pname = New;
}

void productsIfo::changeType(string New) {
	type = New;
}

void productsIfo::changeDescribe(string New) {
	describe = New;
}

void productsIfo::changeRemain(ll New) {
	remain = New;
}

void productsIfo::changePrice(double New) {
	price = New;
}


void productsIfo::changeNewPrice(double New) {
	newPrice = New;
}

double productsIfo::getNewPrice() const {
	return newPrice;
}