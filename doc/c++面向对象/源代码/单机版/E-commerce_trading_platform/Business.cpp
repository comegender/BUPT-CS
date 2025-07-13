#include"Business.h"
#include"productsIfo.h"
typedef long long ll;

int Business::getUserType(string userName) {
	return sheet.getUserType(userName);
}

void Business::addProducts(string userName) {
	system("cls");
	string pname, type, belong, describe;
	string input;
	ll remain;
	belong = userName;
	cout << "请输入商品名称(输入q退出):";
	cin >> pname;
	if (pname.length() == 0 || pname.find(' ') != string::npos) {
		cout << "名不能是空，不能有空格\n";
		system("pause");
		return;
	}
	if (pname == "q") return;
	cout << "请输入商品定价:";
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
	double price = stod(input);
	if (price <= 0) {
		cout << "充值金额必须大于 0！\n";
		return;
	}

	cout << "请输入商品数量:";
	cin >> remain;
	cout << "请输入商品类型:";
	cin >> type;
	if (type.find(' ') != string::npos) {
		cout << "不能输入空!\n";
		system("pause");
		return;
	}
	cout << "请输入商品描述:";
	cin >> describe;
	productsIfo a(pname, price, describe, remain, type, belong);
	if (p.isRepeat(a)) {
		cout << "已经存在同一商品!\n";
		return;
	}
	p.addProducts(a);
	cout << "添加商品成功!\n";
}

void Business::changeProduct(string userName) {
	p.changeProduct(userName);
}

void Business::chooseProductMode(string x,string userName) {
	if (x == "0") return;
	else if (x == "1") addProducts(userName);
	else if (x == "2") changeProduct(userName);
	system("pause");
}

