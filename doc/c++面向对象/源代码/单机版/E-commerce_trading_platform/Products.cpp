#include"Products.h"
#include"Discaonut.h"
#include<fstream>
#include<iostream>
#include<string>
#include<algorithm>
#include<string>
#include<iomanip>
using namespace std;

void Products::participle(string pattern, string& pname, double& price, string& describe, ll& remain, string &type, string &belong,double &newPrice) {
	int a = -1, b = -1, c = -1, d = -1, e = -1, f = -1;
	int l = pattern.length();
	for (int i = 0; i < pattern.length(); i++) {
		if (pattern[i] == ',') {
			if (a == -1) a = i;
			else if (b == -1) b = i;
			else if (c == -1) c = i;
			else if (d == -1) d = i;
			else if (e == -1) e = i;
			else if (f == -1) f = i;
		}
	}
	string temp;
	temp = "";
	for (int i = 0; i < a; i++) {
		temp += pattern[i];
	}
	pname = temp;
	temp = "";
	for (int i = a + 1; i < b; i++) {
		temp += pattern[i];
	}
	price = stold(temp);
	temp = "";
	for (int i = b + 1; i < c; i++) {
		temp += pattern[i];
	}
	remain = stoll(temp);
	temp = "";
	for (int i = c + 1; i < d; i++) {
		temp += pattern[i];
	}
	type = temp;
	temp = "";
	for (int i = d + 1; i < e; i++) {
		temp += pattern[i];
	}
	belong = temp;
	temp = "";
	for (int i = e + 1; i < f; i++) {
		temp += pattern[i];
	}
	newPrice = stod(temp);
	temp = "";
	for (int i = f + 1; i < pattern.size(); i++) {
		temp += pattern[i];
	}
	describe = temp;
}

void Products::getPsheet() {
	ifstream ifs;
	string pname;
	double price, newPrice;
	string describe;
	ll remain;
	string type, belong;
	ifs.open("Products.csv", ios::in);
	if (ifs.is_open()) cout << "商品文件打开\n";
	else cout << "商品文件打开失败\n";
	string messege;
	getline(ifs,messege);
	psheet.clear();
	pname_key[pname].clear();
	type_key[type].clear();
	belong_key[belong].clear();
	while (getline(ifs, messege)) {
		participle(messege, pname, price, describe, remain, type, belong, newPrice);
		psheet.push_back(productsIfo(pname, price, describe, remain, type, belong,newPrice));
		pname_key[pname].push_back(psheet.size() - 1);
		type_key[type].push_back(psheet.size() - 1);
		belong_key[belong].push_back(psheet.size() - 1);
	}
	ifs.close();
}

bool Products::isRepeat(productsIfo a) {
	vector<int> temp;
	temp = pname_key[a.getPname()];
	for (int i = 0; i < temp.size(); i++) {
		if (a == psheet[temp[i]]) return true;
	}
	return false;
}

void Products::addProducts(productsIfo a) {
	psheet.push_back(a);
	pname_key[a.getPname()].push_back(psheet.size() - 1);
	type_key[a.getType()].push_back(psheet.size() - 1);
	belong_key[a.getBelong()].push_back(psheet.size() - 1);
}

void Products::outSheet() {
	ofstream ofs;
	ofs.open("Products.csv", ios::out);
	ofs << "名称,价格,剩余,类别,属于,折后价,描述" << endl;
	for (int i = 0; i < psheet.size(); i++) {
		ofs << psheet[i].getPname() << "," << psheet[i].getPrice() << "," << psheet[i].getRemain() << "," << psheet[i].getType() << "," << psheet[i].getBelong() << "," << psheet[i].getNewPrice() << "," << psheet[i].getDescribe() << endl;
	}
	ofs.close();
}

Products::Products() {
	getPsheet();
}

Products::~Products() {
	outSheet();
}

void Products::chooseChangeProductMode(string x, string userName, int insert) {
	system("cls");
	if (x == "0") return;
	else if (x == "1") {
		string newPname;
		cout << "您想要改成什么名字:";
		cin >> newPname;
		if (newPname.length() == 0 || newPname.find(' ') != string::npos) {
			cout << "名不能是空，不能有空格\n";
			system("pause");
			return;
		}
		psheet[insert].changePname(newPname);
	}
	else if (x == "2") {
		string input;
		cout << "您想要修改为什么价格:";
		cin >> input;
		// 检查输入是否合法
		bool isValid = true;
		bool hasDecimal = false;
		int decimalDigits = 0;

		for (char c : input) {
			if (c == '.') {
				if (hasDecimal) {  // 已经有小数点，不能再有
					isValid = false;
					break;
				}
				hasDecimal = true;
			}
			else if (c == '-') {  // 不允许负数
				isValid = false;
				break;
			}
			else if (!isdigit(c)) {  // 非数字字符
				isValid = false;
				break;
			}
			else if (hasDecimal) {  // 小数点后位数检查
				decimalDigits++;
				if (decimalDigits > 2) {  // 超过 2 位小数
					isValid = false;
					break;
				}
			}
		}

		if (!isValid) {
			cout << "输入无效！请输入非负数且最多两位小数（如：100 或 50.25）\n";
			return;  // 重新输入
		}

		// 转换为 double 并检查是否为 0
		double newPrice = stod(input);
		if (newPrice <= 0) {
			cout << "充值金额必须大于 0！\n";
			return;
		}
		psheet[insert].changePrice(newPrice);
	}
	else if (x == "3") {
		string newType;
		cout << "您想要修改为什么类别:";
		cin >> newType;
		if (newType.find(' ') != string::npos) {
			cout << "不能输入空!\n";
			system("pause");
			return;
		}
		psheet[insert].changeType(newType);
	}
	else if (x == "4") {
		string newdes;
		cout << "请输入您想要的新描述:";
		cin >> newdes;
		psheet[insert].changeDescribe(newdes);
	}
	else if (x == "5") {
		ll newRemain;
		cout << "请输入您想要新添加的商品数量:";
		cin >> newRemain;
		psheet[insert].changeRemain(newRemain);
	}
}

void Products::changeProduct(string userName) {
	for (;;) {
		system("cls");
		cout << "请输入您想要修改的商品的商品名(输入q退出):";
		string pname;
		cin >> pname;
		if (pname == "q") return;
		if (pname.length() == 0 || pname.find(' ') != string::npos) {
			cout << "名不能是空，不能有空格\n";
			system("pause");
			return;
		}
		cout << "请输入该商品的类别:";
		string type;
		cin >> type;
		if (type.find(' ') != string::npos) {
			cout << "不能输入空!\n";
			system("pause");
			return;
		}
		string belong = userName;
		if (pname_key[pname].size() == 0) {
			cout << "尚未添加商品，请先添加商品!\n";
			system("pause");
			return;
		}
		for (int i = 0; i < belong_key[belong].size(); i++) {
			if (psheet[belong_key[belong][i]].getPname() == pname && psheet[belong_key[belong][i]].getType() == type) {
				for (;;) {
					system("cls");
					cout << "---------------------------------------------------------\n1:修改商品名\n2:修改价格\n3:修改商品类别\n4:修改商品描述\n5:修改商品数量\n0:退出\n---------------------------------------------------------\n";
					cout << "请选择您的操作:";
					string x;
					cin >> x;
					if (x != "0" && x != "1" && x != "2" && x != "3" && x != "4" && x != "5") {
						cout << "输入错误，请重新选择！\n";
						system("pause");
						continue;
					}
					chooseChangeProductMode(x, userName, belong_key[belong][i]);
					if (x == "0") break;
				}
			}
			else if (i == belong_key[belong].size() - 1) { cout << "该商品不存在!\n"; system("pause");}
		}
		
	}
}

void Products::productPage() {
	cout << "--------------------------------------------------------商品展示--------------------------------------------------------\n";
	if (IS_DISCOUNT) {
		cout << "正在举行打折活动:" << DISTYPE << "全场打" << DIS * 10 << "折\n" << endl;
	}
	int range;
	if (psheet.size() < 10) range = psheet.size();
	else range = 10;
	for (int i = 0; i < range; i++) {
		cout << "名称: " << psheet[i].getPname() << endl;
		cout << "价格: " << setprecision(2) << fixed << psheet[i].getPrice() << endl;
		if (IS_DISCOUNT) cout << "折后价: " << setprecision(2) << fixed << psheet[i].getNewPrice() << endl;
		cout << "类型: " << psheet[i].getType() << "    " << "剩余: " << psheet[i].getRemain() << endl;
		cout << "商家: " << psheet[i].getBelong() << endl;
		cout << "商品描述: " << psheet[i].getDescribe() << endl;
		cout << endl;
	}
}

void Products::chooseSearchMode(string x) {
	if (x == "0") return;
	else if (x == "1") {
		cout << "请输入您想要搜索的种类 (输入r退出):";
		string stype;
		cin >> stype;
		if (stype == "r") return;
		if (type_key.find(stype) == type_key.end()) {
			cout << "无搜索结果……\n";
			return;
		}
		for (int i = 0; i < type_key[stype].size(); i++) {
			cout << endl;
			cout << "名称: " << psheet[type_key[stype][i]].getPname() << endl;
			cout << "价格: " << setprecision(2) << fixed << psheet[type_key[stype][i]].getPrice() << endl;
			if (IS_DISCOUNT) cout << "折后价: " << setprecision(2) << fixed << psheet[i].getNewPrice() << endl;
			cout << "类型: " << psheet[type_key[stype][i]].getType() << "    " << "剩余: " << psheet[i].getRemain() << endl;
			cout << "商家: " << psheet[type_key[stype][i]].getBelong() << endl;
			cout << "商品描述: " << psheet[type_key[stype][i]].getDescribe() << endl;
			cout << endl;
		}
	}
	else if (x == "2") {
		cout << "请输入您想要搜索的商品名 (输入r退出):";
		string spname;
		if (spname == "r") return;
		cin >> spname;
		if (pname_key.find(spname) == pname_key.end()) {
			cout << "无搜索结果……\n";
			return;
		}
		for (int i = 0; i < pname_key[spname].size(); i++) {
			cout << endl;
			cout << "名称: " << psheet[pname_key[spname][i]].getPname() << endl;
			cout << "价格: " << setprecision(2) << fixed << psheet[pname_key[spname][i]].getPrice() << endl;
			if (IS_DISCOUNT) cout << "折后价: " << setprecision(2) << fixed << psheet[i].getNewPrice() << endl;
			cout << "类型: " << psheet[pname_key[spname][i]].getType() << "    " << "剩余: " << psheet[i].getRemain() << endl;
			cout << "商家: " << psheet[pname_key[spname][i]].getBelong() << endl;
			cout << "商品描述: " << psheet[pname_key[spname][i]].getDescribe() << endl;
			cout << endl;
		}
	}
	else if (x == "3") {
		cout << "请输入您想要搜索的商家 (输入r退出):";
		string sbelong;
		if (sbelong == "r") return;
		cin >> sbelong;
		if (belong_key.find(sbelong) == belong_key.end()) {
			cout << "无搜索结果……\n";
			return;
		}
		for (int i = 0; i < belong_key[sbelong].size(); i++) {
			cout << endl;
			cout << "名称: " << psheet[belong_key[sbelong][i]].getPname() << endl;
			cout << "价格: " << setprecision(2) << fixed << psheet[belong_key[sbelong][i]].getPrice() << endl;
			if (IS_DISCOUNT) cout << "折后价: " << setprecision(2) << fixed << psheet[i].getNewPrice() << endl;
			cout << "类型: " << psheet[belong_key[sbelong][i]].getType() << "    " << "剩余: " << psheet[i].getRemain() << endl;
			cout << "商家: " << psheet[belong_key[sbelong][i]].getBelong() << endl;
			cout << "商品描述: " << psheet[belong_key[sbelong][i]].getDescribe() << endl;
			cout << endl;
		}
	}
	system("pause");
}

void Products::searchProduct() {
	for (;;) {
		system("cls");
		productPage();
		cout << "---------------------------------------------------------\n1:按种类搜索\n2:按商品名搜索\n3:按商家搜索\n0:退出\n---------------------------------------------------------\n";
		string x;
		cout << "请输入您的选择:";
		cin >> x;
		chooseSearchMode(x);
		if (x == "0") break;
	}
}

productsIfo Products::returnProductIfo(string pname, string type, string belong) {
	sort(pname_key[pname].begin(), pname_key[pname].end());
	sort(type_key[type].begin(), type_key[type].end());
	sort(belong_key[belong].begin(), belong_key[belong].end());
	vector<int> temp;
	set_intersection(pname_key[pname].begin(), pname_key[pname].end(),
		type_key[type].begin(), type_key[type].end(),
		back_inserter(temp));
	vector<int> result;
	set_intersection(temp.begin(), temp.end(),
		belong_key[belong].begin(), belong_key[belong].end(),
		back_inserter(result));
	if (result.size() == 0) {
		cout << "未找到该商品……\n";
		return productsIfo();
	}
	return psheet[result[0]];
}

int Products::returnProductIfoInsert(string pname, string type, string belong) {
	sort(pname_key[pname].begin(), pname_key[pname].end());
	sort(type_key[type].begin(), type_key[type].end());
	sort(belong_key[belong].begin(), belong_key[belong].end());
	vector<int> temp;
	set_intersection(pname_key[pname].begin(), pname_key[pname].end(),
		type_key[type].begin(), type_key[type].end(),
		back_inserter(temp));
	vector<int> result;
	set_intersection(temp.begin(), temp.end(),
		belong_key[belong].begin(), belong_key[belong].end(),
		back_inserter(result));
	if (result.size() == 0) {
		cout << "未找到该商品……\n";
		return -1;
	}
	return result[0];
}

double Products::getPrice() {
	return 0;
}

double Book::getPrice() {
	if (returnProductIfo(pname, "书", belong) == productsIfo()) {
		return -1;
	}
	else {
		return (returnProductIfo(pname, "书", belong).getPrice() * DIS);
	}
}

double Food::getPrice() {
	if (returnProductIfo(pname, "食品", belong) == productsIfo()) {
		return -1;
	}
	else {
		return (returnProductIfo(pname, "食品", belong).getPrice() * DIS);
	}
}

double Cloth::getPrice() {
	if (returnProductIfo(pname, "服装", belong) == productsIfo()) {
		return -1;
	}
	else {
		return (returnProductIfo(pname, "服装", belong).getPrice() * DIS);
	}
}

void Products::Discount() {
	for (int i = 0; i < DISTYPE_COUNT; i++) {
		switch (distype[i])
		{
		case BOOK:
			for (int j = 0; j < type_key["书"].size(); j++) {
				psheet[type_key["书"][j]].changeNewPrice(Book(psheet[type_key["书"][j]].getPname(), psheet[type_key["书"][j]].getBelong()).getPrice());
			}
			break;
		case FOOD:
			for (int j = 0; j < type_key["食品"].size(); j++) {
				psheet[type_key["食品"][j]].changeNewPrice(Food(psheet[type_key["食品"][j]].getPname(), psheet[type_key["食品"][j]].getBelong()).getPrice());
			}
			break;
		case CLOTH:
			for (int j = 0; j < type_key["服装"].size(); j++) {
				psheet[type_key["服装"][j]].changeNewPrice(Cloth(psheet[type_key["服装"][j]].getPname(), psheet[type_key["服装"][j]].getBelong()).getPrice());
			}
			break;
		default:
			break;
		}
	}
}

void Products::lockRemain(string pname, string type, string belong,ll cost) {
	int i = returnProductIfoInsert(pname, type, belong);
	ll temp = psheet[i].getRemain();
	psheet[i].changeRemain(temp - cost);
}

void Products::unlockRemain(string pname, string type, string belong, ll cost) {
	int i = returnProductIfoInsert(pname, type, belong);
	ll temp = psheet[i].getRemain();
	psheet[i].changeRemain(temp + cost);
}