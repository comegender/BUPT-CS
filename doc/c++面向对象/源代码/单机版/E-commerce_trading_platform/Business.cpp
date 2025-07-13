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
	cout << "��������Ʒ����(����q�˳�):";
	cin >> pname;
	if (pname.length() == 0 || pname.find(' ') != string::npos) {
		cout << "�������ǿգ������пո�\n";
		system("pause");
		return;
	}
	if (pname == "q") return;
	cout << "��������Ʒ����:";
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
	double price = stod(input);
	if (price <= 0) {
		cout << "��ֵ��������� 0��\n";
		return;
	}

	cout << "��������Ʒ����:";
	cin >> remain;
	cout << "��������Ʒ����:";
	cin >> type;
	if (type.find(' ') != string::npos) {
		cout << "���������!\n";
		system("pause");
		return;
	}
	cout << "��������Ʒ����:";
	cin >> describe;
	productsIfo a(pname, price, describe, remain, type, belong);
	if (p.isRepeat(a)) {
		cout << "�Ѿ�����ͬһ��Ʒ!\n";
		return;
	}
	p.addProducts(a);
	cout << "�����Ʒ�ɹ�!\n";
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

