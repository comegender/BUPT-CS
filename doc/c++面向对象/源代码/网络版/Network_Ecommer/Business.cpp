#include"Business.h"
#include"productsIfo.h"
typedef long long ll;

int Business::getUserType(string userName) {
	return sheet.getUserType(userName);
}

int Business::addProducts(string userName, string pname, string type, string belong, string describe,double price,ll remain) {
	if (pname == "q") return 0;
	productsIfo a(pname, price, describe, remain, type, belong);
	if (p.isRepeat(a)) {
		//cout << "已经存在同一商品!\n";
		return 1;
	}
	p.addProducts(a);
	//cout << "添加商品成功!\n";
	return 2;
}

int Business::changeProduct(string userName,string x,string pname,string type,string New) {
	return p.changeProduct(userName, x, pname, type, New);
}

//void Business::chooseProductMode(string x, string userName) {
//	if (x == "0") return;
//	else if (x == "1") addProducts(userName);
//	else if (x == "2") changeProduct(userName);
//	system("pause");
//}

