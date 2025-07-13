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
	if (ifs.is_open()) cout << "��Ʒ�ļ���\n";
	else cout << "��Ʒ�ļ���ʧ��\n";
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
	ofs << "����,�۸�,ʣ��,���,����,�ۺ��,����" << endl;
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
		cout << "����Ҫ�ĳ�ʲô����:";
		cin >> newPname;
		if (newPname.length() == 0 || newPname.find(' ') != string::npos) {
			cout << "�������ǿգ������пո�\n";
			system("pause");
			return;
		}
		psheet[insert].changePname(newPname);
	}
	else if (x == "2") {
		string input;
		cout << "����Ҫ�޸�Ϊʲô�۸�:";
		cin >> input;
		// ��������Ƿ�Ϸ�
		bool isValid = true;
		bool hasDecimal = false;
		int decimalDigits = 0;

		for (char c : input) {
			if (c == '.') {
				if (hasDecimal) {  // �Ѿ���С���㣬��������
					isValid = false;
					break;
				}
				hasDecimal = true;
			}
			else if (c == '-') {  // ��������
				isValid = false;
				break;
			}
			else if (!isdigit(c)) {  // �������ַ�
				isValid = false;
				break;
			}
			else if (hasDecimal) {  // С�����λ�����
				decimalDigits++;
				if (decimalDigits > 2) {  // ���� 2 λС��
					isValid = false;
					break;
				}
			}
		}

		if (!isValid) {
			cout << "������Ч��������Ǹ����������λС�����磺100 �� 50.25��\n";
			return;  // ��������
		}

		// ת��Ϊ double ������Ƿ�Ϊ 0
		double newPrice = stod(input);
		if (newPrice <= 0) {
			cout << "��ֵ��������� 0��\n";
			return;
		}
		psheet[insert].changePrice(newPrice);
	}
	else if (x == "3") {
		string newType;
		cout << "����Ҫ�޸�Ϊʲô���:";
		cin >> newType;
		if (newType.find(' ') != string::npos) {
			cout << "���������!\n";
			system("pause");
			return;
		}
		psheet[insert].changeType(newType);
	}
	else if (x == "4") {
		string newdes;
		cout << "����������Ҫ��������:";
		cin >> newdes;
		psheet[insert].changeDescribe(newdes);
	}
	else if (x == "5") {
		ll newRemain;
		cout << "����������Ҫ����ӵ���Ʒ����:";
		cin >> newRemain;
		psheet[insert].changeRemain(newRemain);
	}
}

void Products::changeProduct(string userName) {
	for (;;) {
		system("cls");
		cout << "����������Ҫ�޸ĵ���Ʒ����Ʒ��(����q�˳�):";
		string pname;
		cin >> pname;
		if (pname == "q") return;
		if (pname.length() == 0 || pname.find(' ') != string::npos) {
			cout << "�������ǿգ������пո�\n";
			system("pause");
			return;
		}
		cout << "���������Ʒ�����:";
		string type;
		cin >> type;
		if (type.find(' ') != string::npos) {
			cout << "���������!\n";
			system("pause");
			return;
		}
		string belong = userName;
		if (pname_key[pname].size() == 0) {
			cout << "��δ�����Ʒ�����������Ʒ!\n";
			system("pause");
			return;
		}
		for (int i = 0; i < belong_key[belong].size(); i++) {
			if (psheet[belong_key[belong][i]].getPname() == pname && psheet[belong_key[belong][i]].getType() == type) {
				for (;;) {
					system("cls");
					cout << "---------------------------------------------------------\n1:�޸���Ʒ��\n2:�޸ļ۸�\n3:�޸���Ʒ���\n4:�޸���Ʒ����\n5:�޸���Ʒ����\n0:�˳�\n---------------------------------------------------------\n";
					cout << "��ѡ�����Ĳ���:";
					string x;
					cin >> x;
					if (x != "0" && x != "1" && x != "2" && x != "3" && x != "4" && x != "5") {
						cout << "�������������ѡ��\n";
						system("pause");
						continue;
					}
					chooseChangeProductMode(x, userName, belong_key[belong][i]);
					if (x == "0") break;
				}
			}
			else if (i == belong_key[belong].size() - 1) { cout << "����Ʒ������!\n"; system("pause");}
		}
		
	}
}

void Products::productPage() {
	cout << "--------------------------------------------------------��Ʒչʾ--------------------------------------------------------\n";
	if (IS_DISCOUNT) {
		cout << "���ھ��д��ۻ:" << DISTYPE << "ȫ����" << DIS * 10 << "��\n" << endl;
	}
	int range;
	if (psheet.size() < 10) range = psheet.size();
	else range = 10;
	for (int i = 0; i < range; i++) {
		cout << "����: " << psheet[i].getPname() << endl;
		cout << "�۸�: " << setprecision(2) << fixed << psheet[i].getPrice() << endl;
		if (IS_DISCOUNT) cout << "�ۺ��: " << setprecision(2) << fixed << psheet[i].getNewPrice() << endl;
		cout << "����: " << psheet[i].getType() << "    " << "ʣ��: " << psheet[i].getRemain() << endl;
		cout << "�̼�: " << psheet[i].getBelong() << endl;
		cout << "��Ʒ����: " << psheet[i].getDescribe() << endl;
		cout << endl;
	}
}

void Products::chooseSearchMode(string x) {
	if (x == "0") return;
	else if (x == "1") {
		cout << "����������Ҫ���������� (����r�˳�):";
		string stype;
		cin >> stype;
		if (stype == "r") return;
		if (type_key.find(stype) == type_key.end()) {
			cout << "�������������\n";
			return;
		}
		for (int i = 0; i < type_key[stype].size(); i++) {
			cout << endl;
			cout << "����: " << psheet[type_key[stype][i]].getPname() << endl;
			cout << "�۸�: " << setprecision(2) << fixed << psheet[type_key[stype][i]].getPrice() << endl;
			if (IS_DISCOUNT) cout << "�ۺ��: " << setprecision(2) << fixed << psheet[i].getNewPrice() << endl;
			cout << "����: " << psheet[type_key[stype][i]].getType() << "    " << "ʣ��: " << psheet[i].getRemain() << endl;
			cout << "�̼�: " << psheet[type_key[stype][i]].getBelong() << endl;
			cout << "��Ʒ����: " << psheet[type_key[stype][i]].getDescribe() << endl;
			cout << endl;
		}
	}
	else if (x == "2") {
		cout << "����������Ҫ��������Ʒ�� (����r�˳�):";
		string spname;
		if (spname == "r") return;
		cin >> spname;
		if (pname_key.find(spname) == pname_key.end()) {
			cout << "�������������\n";
			return;
		}
		for (int i = 0; i < pname_key[spname].size(); i++) {
			cout << endl;
			cout << "����: " << psheet[pname_key[spname][i]].getPname() << endl;
			cout << "�۸�: " << setprecision(2) << fixed << psheet[pname_key[spname][i]].getPrice() << endl;
			if (IS_DISCOUNT) cout << "�ۺ��: " << setprecision(2) << fixed << psheet[i].getNewPrice() << endl;
			cout << "����: " << psheet[pname_key[spname][i]].getType() << "    " << "ʣ��: " << psheet[i].getRemain() << endl;
			cout << "�̼�: " << psheet[pname_key[spname][i]].getBelong() << endl;
			cout << "��Ʒ����: " << psheet[pname_key[spname][i]].getDescribe() << endl;
			cout << endl;
		}
	}
	else if (x == "3") {
		cout << "����������Ҫ�������̼� (����r�˳�):";
		string sbelong;
		if (sbelong == "r") return;
		cin >> sbelong;
		if (belong_key.find(sbelong) == belong_key.end()) {
			cout << "�������������\n";
			return;
		}
		for (int i = 0; i < belong_key[sbelong].size(); i++) {
			cout << endl;
			cout << "����: " << psheet[belong_key[sbelong][i]].getPname() << endl;
			cout << "�۸�: " << setprecision(2) << fixed << psheet[belong_key[sbelong][i]].getPrice() << endl;
			if (IS_DISCOUNT) cout << "�ۺ��: " << setprecision(2) << fixed << psheet[i].getNewPrice() << endl;
			cout << "����: " << psheet[belong_key[sbelong][i]].getType() << "    " << "ʣ��: " << psheet[i].getRemain() << endl;
			cout << "�̼�: " << psheet[belong_key[sbelong][i]].getBelong() << endl;
			cout << "��Ʒ����: " << psheet[belong_key[sbelong][i]].getDescribe() << endl;
			cout << endl;
		}
	}
	system("pause");
}

void Products::searchProduct() {
	for (;;) {
		system("cls");
		productPage();
		cout << "---------------------------------------------------------\n1:����������\n2:����Ʒ������\n3:���̼�����\n0:�˳�\n---------------------------------------------------------\n";
		string x;
		cout << "����������ѡ��:";
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
		cout << "δ�ҵ�����Ʒ����\n";
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
		cout << "δ�ҵ�����Ʒ����\n";
		return -1;
	}
	return result[0];
}

double Products::getPrice() {
	return 0;
}

double Book::getPrice() {
	if (returnProductIfo(pname, "��", belong) == productsIfo()) {
		return -1;
	}
	else {
		return (returnProductIfo(pname, "��", belong).getPrice() * DIS);
	}
}

double Food::getPrice() {
	if (returnProductIfo(pname, "ʳƷ", belong) == productsIfo()) {
		return -1;
	}
	else {
		return (returnProductIfo(pname, "ʳƷ", belong).getPrice() * DIS);
	}
}

double Cloth::getPrice() {
	if (returnProductIfo(pname, "��װ", belong) == productsIfo()) {
		return -1;
	}
	else {
		return (returnProductIfo(pname, "��װ", belong).getPrice() * DIS);
	}
}

void Products::Discount() {
	for (int i = 0; i < DISTYPE_COUNT; i++) {
		switch (distype[i])
		{
		case BOOK:
			for (int j = 0; j < type_key["��"].size(); j++) {
				psheet[type_key["��"][j]].changeNewPrice(Book(psheet[type_key["��"][j]].getPname(), psheet[type_key["��"][j]].getBelong()).getPrice());
			}
			break;
		case FOOD:
			for (int j = 0; j < type_key["ʳƷ"].size(); j++) {
				psheet[type_key["ʳƷ"][j]].changeNewPrice(Food(psheet[type_key["ʳƷ"][j]].getPname(), psheet[type_key["ʳƷ"][j]].getBelong()).getPrice());
			}
			break;
		case CLOTH:
			for (int j = 0; j < type_key["��װ"].size(); j++) {
				psheet[type_key["��װ"][j]].changeNewPrice(Cloth(psheet[type_key["��װ"][j]].getPname(), psheet[type_key["��װ"][j]].getBelong()).getPrice());
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