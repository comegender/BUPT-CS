#include"Comsumer.h"
#include<fstream>
#include"Discaonut.h"
#include<set>
#include<iomanip>
using namespace std;

int Comsumer::getUserType(string userName) {
	return sheet.getUserType(userName);
}

void Comsumer::participle(string pattern, string& cname, string& pname, ll& quantity, string& type, string& belong) {
	int a = -1, b = -1, c = -1, d = -1;
	int len = pattern.length();
	for (int i = 0; i < pattern.length(); i++) {
		if (pattern[i] == ',') {
			if (a == -1) a = i;
			else if (b == -1) b = i;
			else if (c == -1) c = i;
			else if (d == -1) d = i;
		}
	}
	string temp;
	temp = "";
	for (int i = 0; i < a; i++) {
		temp += pattern[i];
	}
	cname = temp;
	temp = "";
	for (int i = a + 1; i < b; i++) {
		temp += pattern[i];
	}
	pname = temp;
	temp = "";
	for (int i = b + 1; i < c; i++) {
		temp += pattern[i];
	}
	type = temp;
	temp = "";
	for (int i = c + 1; i < d; i++) {
		temp += pattern[i];
	}
	belong = temp;
	temp = "";
	for (int i = d+1; i < pattern.length(); i++) {
		temp += pattern[i];
	}
	quantity = stoll(temp);
}

void Comsumer::getCart() {
	ifstream ifs;
	ifs.open("shopping_cart.csv", ios::in);
	if (ifs.is_open()) cout << "文件成功打开!\n";
	else cout << "文件打开失败!\n";
	string messege;
	getline(ifs, messege);
	string cname, pname, type, belong;
	ll quantity;
	while (getline(ifs, messege)) {
		participle(messege, cname, pname, quantity, type, belong);
		shopping_cart[cname].push_back(productsIfo(pname, p.returnProductIfo(pname, type, belong).getPrice(), "sc", quantity, type, belong, p.returnProductIfo(pname, type, belong).getNewPrice()));
	}
	ifs.close();
}

void Comsumer::outCart() {
	ofstream ofs;
	ofs.open("shopping_cart.csv", ios::out);
	ofs << "消费者,商品名,商品类别,商家,数量" << endl;
	for (unordered_map<string, vector<productsIfo> >::iterator it = shopping_cart.begin(); it != shopping_cart.end(); it++) {
		for (auto x : it->second) {
			ofs << it->first << "," << x.getPname() << "," << x.getType() << "," << x.getBelong() << "," << x.getRemain() << endl;
		}
	}
	ofs.close();
}

Comsumer::Comsumer() {
	getCart();
}

Comsumer::~Comsumer() {
	outCart();
}

void Comsumer::chooseCartMode(string x, string cname) {
	if (x == "0") return;
	else if (x == "1") addCart(cname);
	else if (x == "2") removeCart(cname);
	else if (x == "3") showCart(cname);
	else if (x == "4") changeCart(cname);
	else if (x == "5") submitBill(cname);
	system("pause");
}

void Comsumer::addCart(string cname) {
	system("cls");
	string pname, type, belong;
	ll count;
	cout << "请输入加入购物车的商品名称(输入r退出):";
	cin >> pname;
	if (pname == "r") return;
	cout << "请输入加入购物车的商品种类:";
	cin >> type;
	cout << "请输入加入购物车的商品商家:";
	cin >> belong;
	cout << "请输入添加的数量:";
	cin >> count;
	if (p.returnProductIfo(pname, type, belong).getType() == "") {
		cout << "没有这个商品!\n";
		return;
	}
	if (count > p.returnProductIfo(pname, type, belong).getRemain()) {
		cout << "添加数量不能超过库存!\n";
		return;
	}
	productsIfo temp = p.returnProductIfo(pname, type, belong);
	temp.changeRemain(count);
	shopping_cart[cname].push_back(temp);
	cout << "加入购物车成功！\n";
}

void Comsumer::removeCart(string cname) {
	system("cls");
	string pname, type, belong;
	ll count;
	cout << "请输入移除购物车的商品名称(输入r退出):";
	cin >> pname;
	if (pname == "r") return;
	cout << "请输入移除购物车的商品种类:";
	cin >> type;
	cout << "请输入移除购物车的商品商家:";
	cin >> belong;
	cout << "请输入移除的数量:";
	cin >> count;
	productsIfo temp;
	if (findCart(cname, pname, type, belong) == -1) {
		cout << "未找到商品……\n";
		return;
	}
	else temp = shopping_cart[cname][findCart(cname, pname, type, belong)];
	if (count > temp.getRemain()) {
		cout << "移除数量过多！\n";
		return;
	}
	ll r = temp.getRemain();
	shopping_cart[cname][findCart(cname, pname, type, belong)].changeRemain(r - count);
	if (shopping_cart[cname][findCart(cname, pname, type, belong)].getRemain() == 0) shopping_cart[cname].erase(shopping_cart[cname].begin() + findCart(cname, pname, type, belong));
	cout << "移除成功！\n";
}

void Comsumer::showCart(string cname) {
	system("cls");
	if (shopping_cart[cname].size() == 0) {
		cout << "空的……\n";
		return;
	}
	for (int i = 0; i < shopping_cart[cname].size(); i++) {
		cout << "序号: " << i + 1 << endl;
		cout << "名称: " << shopping_cart[cname][i].getPname() << endl;
		cout << "类别: " << shopping_cart[cname][i].getType() << endl;
		cout << "商家: " << shopping_cart[cname][i].getBelong() << endl;
		cout << "单价: " << setprecision(2) << fixed << shopping_cart[cname][i].getPrice() << endl;
		if (IS_DISCOUNT) cout << "折后价: " << setprecision(2) << fixed << shopping_cart[cname][i].getNewPrice() << endl;
		cout << "数量: " << shopping_cart[cname][i].getRemain() << endl << endl;
	}
}

void Comsumer::submitBill(string cname) {
	showCart(cname);
	set<int> bill;
	cout << "-----------------------------------------------------------\n";
	for (;;) {
		cout << "请输入您想要结账的商品序号(输入0退出，输入-1确认订单):";
		int seq;
		cin >> seq;
		if (seq == 0) return;
		if (seq == -1) break;
		if (seq > shopping_cart[cname].size()) {
			cout << "没有这个商品！\n";
			continue;
		}
		bill.insert(seq);
	}
	cout << "------------------------------您的订单----------------------------\n";
	double sum = 0;
	for (set<int>::iterator it = bill.begin(); it != bill.end();it++) {
		cout << "名称: " << shopping_cart[cname][*it - 1].getPname() << endl;
		cout << "类别: " << shopping_cart[cname][*it - 1].getType() << endl;
		cout << "商家: " << shopping_cart[cname][*it - 1].getBelong() << endl;
		cout << "单价: " << shopping_cart[cname][*it - 1].getPrice() << endl;
		if(IS_DISCOUNT) cout << "折后价: " << setprecision(2) << fixed << shopping_cart[cname][*it - 1].getNewPrice() << endl;
		cout << "数量: " << shopping_cart[cname][*it - 1].getRemain() << endl;
		if(IS_DISCOUNT) cout << "小结: " << setprecision(2) << fixed << shopping_cart[cname][*it - 1].getRemain() * shopping_cart[cname][*it - 1].getNewPrice() << endl << endl;
		else cout << "小结: " << setprecision(2) << fixed << shopping_cart[cname][*it - 1].getRemain() * shopping_cart[cname][*it - 1].getPrice() << endl << endl;
		if (IS_DISCOUNT) sum += shopping_cart[cname][*it - 1].getRemain() * shopping_cart[cname][*it - 1].getNewPrice();
		else sum += shopping_cart[cname][*it - 1].getRemain() * shopping_cart[cname][*it - 1].getPrice();
	}
	cout << "总计: " << setprecision(2) << fixed << sum << endl;
	for (;;) {
		if (1) {
			Products temp;
			for (set<int>::iterator it = bill.begin(); it != bill.end(); it++) {
				temp.lockRemain(shopping_cart[cname][*it - 1].getPname(), shopping_cart[cname][*it - 1].getType(), shopping_cart[cname][*it - 1].getBelong(), shopping_cart[cname][*it - 1].getRemain());
			}
		}
		cout << "确认支付订单输入yes,取消订单输入no:";
		string is_confirm;
		cin >> is_confirm;
		if (is_confirm != "yes" && is_confirm != "no") {
			cout << "请重新输入……\n";
			continue;
		}
		if (is_confirm == "no") {
			Products temp;
			for (set<int>::iterator it = bill.begin(); it != bill.end(); it++) {
				temp.unlockRemain(shopping_cart[cname][*it - 1].getPname(), shopping_cart[cname][*it - 1].getType(), shopping_cart[cname][*it - 1].getBelong(), shopping_cart[cname][*it - 1].getRemain());
			}
			return;
		}
		if (is_confirm == "yes") {
			Consume(cname, sum);
			for (auto it = bill.begin(); it != bill.end(); it++) {
				if (IS_DISCOUNT) sheet.Recharge(shopping_cart[cname][*it - 1].getBelong(), shopping_cart[cname][*it - 1].getNewPrice() * shopping_cart[cname][*it - 1].getRemain());
				else sheet.Recharge(shopping_cart[cname][*it - 1].getBelong(), shopping_cart[cname][*it - 1].getPrice());
			}
			for (auto it = bill.rbegin(); it != bill.rend(); it++) {
				if (*it - 1 < shopping_cart[cname].size()) {
					shopping_cart[cname].erase(shopping_cart[cname].begin() + *it - 1);
				}
			}
			return;
		}
	}
}

void Comsumer::changeCart(string cname) {
	system("cls");
	string pname, type, belong;
	ll count;
	cout << "请输入修改购物车的商品名称(输入r退出):";
	cin >> pname;
	if (pname == "r") return;
	cout << "请修改加入购物车的商品种类:";
	cin >> type;
	cout << "请修改加入购物车的商品商家:";
	cin >> belong;
	cout << "请修改添加的数量:";
	cin >> count;
	productsIfo temp;
	if (findCart(cname, pname, type, belong) == -1) {
		cout << "未找到商品……\n";
		return;
	}
	if (count > p.returnProductIfo(pname, type, belong).getRemain()) {
		cout << "添加数量不能超过库存!\n";
		return;
	}
	shopping_cart[cname][findCart(cname, pname, type, belong)].changeRemain(count);
	if (shopping_cart[cname][findCart(cname, pname, type, belong)].getRemain() == 0) shopping_cart[cname].erase(shopping_cart[cname].begin() + findCart(cname, pname, type, belong));
	cout << "修改成功！\n";
}

int Comsumer::findCart(string cname, string pname, string type, string belong) {
	for (int i = 0; i < shopping_cart[cname].size(); i++) {
		if (shopping_cart[cname][i].getPname() == pname && shopping_cart[cname][i].getType() == type && shopping_cart[cname][i].getBelong() == belong) {
			return i;
		}
	}
	return -1;
}

void Comsumer::updateSheet() {
	p.getPsheet();
}