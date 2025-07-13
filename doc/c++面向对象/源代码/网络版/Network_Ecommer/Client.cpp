#include"Client.h"
#include"Tourist.h"
#include"Comsumer.h"
#include<iostream>
#include<sstream>
#include<iomanip>

bool Client::init() {
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		cerr << "WSAStartup failed" << endl;
		return false;
	}
	return true;
}

bool Client::connectToServer(const string& serverIp, int port) {
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (sock == INVALID_SOCKET) {
        cerr << "�޷�����socket: " << WSAGetLastError() << endl;
        return false;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, serverIp.c_str(), &serverAddr.sin_addr);

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "���ӷ�����ʧ��: " << WSAGetLastError() << endl;
        closesocket(sock);
        sock = INVALID_SOCKET;
        return false;
    }

    return true;
}

string Client::sendRequest(const string& request) {
    if (send(sock, request.c_str(), (int)request.size(), 0) == SOCKET_ERROR) {
        return "SEND_ERROR";
    }

    char buffer[1024] = { 0 };
    int bytesRead = recv(sock, buffer, sizeof(buffer), 0);
    if (bytesRead <= 0) {
        return "RECV_ERROR";
    }

    return string(buffer, bytesRead);
}

void Client::run() {
    if (!init()) return;
    cout << "���ӵ�������...\n";
    if (!connectToServer("127.0.0.1", 8080)) return;
    cout << "���ӳɹ�!\n";
    cout << "��ӭ�������̽���ϵͳY^_^Y\n������:BUPT ������\n";
    string x;
    Tourist a;
    while (1) {
        system("pause");
        system("cls");
        a.productPage();
        cout << "---------------------------------------------------------\n1:ע��\n2:��¼\n3:������\n0:�˳�\n---------------------------------------------------------\n";
        cout << "����������ѡ��:";
        cin >> x;
        if (x != "1" && x != "2" && x != "3" && x != "0") {
            cout << "û�����ѡ��!\n";
            continue;
        }
        if (x == "1") Register();
        else if (x == "2") Login();
        else if (x == "3") changePassword();
        system("cls");
        a.updateSheet();
        if (x == "0") break;
    }

}

void Client::Login() {
    system("cls");
    string request;
    int type;
    cout << "�������������(1:������ 2:�̼�)(����0�˳�):";
    cin >> type;
    if (type == 0) return;
    getc(stdin);
    cout << "�������û���:";
    string username;
    getline(cin, username);
    if (username.length() == 0 || username.find(' ') != string::npos) {
        cout << "�û��������ǿգ������пո�\n";
        system("pause");
        return;
    }
    string password;
    cout << "����������:";
    getline(cin, password);
    if (password.length() == 0 || password.find(' ') != string::npos) {
        cout << "���벻���ǿգ������пո�\n";
        system("pause");
        return;
    }
    request = "LOGIN " + to_string(type) + " " + username + " " + password;
    string response = sendRequest(request);
    if (response == "QUIT") return;
    else if (response == "NO_THIS_PERSON") cout << "û�и��û�������ע��!\n";
    else if (response == "PASSWORD_WRONG") cout << "���벻��ȷ!\n";
    else if (response == "TYPE_WRONG") cout << "��ݲ���ȷ!\n";
    else if (response == "LOGIN_SUCCESS") {
        cout << "��¼�ɹ�!\n";
        system("pause");
        system("cls");
        featurePage(username, type);
    }
    else cout << "��¼ʧ��!\n";
}

void Client::Register() {
    system("cls");
    string name;
    string password;
    string ACP;
    int type;
    cout << "�����������û���(����r����):";
    cin >> name;
    if (name == "r") return;
    if (name.length() == 0 || name.find(' ') != string::npos) {
        cout << "�û��������ǿգ������пո�\n";
        system("pause");
        return;
    }
    cout << "��������������:";
    cin >> password;
    if (password.length() == 0 || password.find(' ') != string::npos) {
        cout << "���벻���ǿգ������пո�\n";
        system("pause");
        return;
    }
    cout << "��ȷ������:";
    cin >> ACP;
    cout << "��ѡ���������:(������������1���̼�������2):";
    cin >> type;
    string request = "REGISTER " + name + " " + password + " " + to_string(type) + " " + ACP;
    cout << request << endl;
    string response = sendRequest(request);
    if (response == "QUIT") return;
    else if (response == "HAVE_REGISTERED") cout << "���û����ѱ�ע�ᣬ������ע��!\n";
    else if (response == "ACP_WRONG") cout << "������������벻һ�£�ע��ʧ��!\n";
    else if (response == "REGISTER_SUCCESS") cout << "ע��ɹ�!\n";
    else cout << "ע��ʧ��\n";
    system("pause");
}

void Client::changePassword() {
    system("cls");
    string newpassword;
    string name;
    string ACP;
    cout << "�����������û���(����r����):";
    cin >> name;
    if (name == "r") return;
    if (name.length() == 0 || name.find(' ') != string::npos) {
        cout << "�û��������ǿգ������пո�\n";
        system("pause");
        return;
    }
    cout << "����������������:";
    cin >> newpassword;
    if (newpassword.length() == 0 || newpassword.find(' ') != string::npos) {
        cout << "���벻���ǿգ������пո�\n";
        system("pause");
        return;
    }
    cout << "��ȷ������������:";
    cin >> ACP;
    string request = "CHANGE_PASSWORD " + name + " " + newpassword + " " + ACP;
    string response = sendRequest(request);
    if (response == "NEWPASSWORD_SAME") cout << "�����벻����ԭ������ͬ!\n";
    else if (response == "ACP_WRONG") cout << "�������벻һ��!\n";
    else if (response == "NO_THIS_PERSON") cout << "�����ڴ��û�!\n";
    else if (response == "CHANGE_PASSWORD_SUCCESS") cout << "�޸�����ɹ�!\n";
    else cout << "�޸�����ʧ��!\n";
    system("pause");
}

void Client::featurePage(string userName,int type) {
    for (;;) {
        system("cls");
        productPage();
        cout << "---------------------------------------------------------\n1:������\n2:������Ʒ\n";
        if (type == 1) {
            cout << "3:����\n";
        }
        if (type == 2) {
            cout << "4:��Ʒ����\n";
        }
        cout << "0:�˳�\n---------------------------------------------------------\n";
        cout << "��ѡ�����Ĳ���:";
        string x;
        cin >> x;
        if ((x != "0" && x != "1" && x != "2" && x != "3" && x != "4") || (type == 1 && x == "4") || (type == 2 && x == "3")) {
            cout << "�������������ѡ��\n";
            system("pause");
            continue;
        }
        if (x == "1") Balance_Operator(userName);
        else if (x == "2") searchProduct();
        else if (x == "3") Shopping(userName);
        else if (x == "4") manageProduct(userName);
        if (x == "0") break;
    }
}

void Client::productPage() {
    p.productPage();
}

double Client::getBalance(string userName) {
    string request = "GET_BALANCE " + userName;
    string response = sendRequest(request);
    if (response.find("BALANCE") == 0) {
        istringstream iss(response);
        string dummy;
        double balance;
        iss >> dummy >> balance;
        cout << "�������Ϊ:" << setprecision(2) << fixed << balance << endl;
        system("pause");
        return balance;
    }
    return 0;
}

void Client::Balance_Operator(string userName) {
    while (1) {
        system("cls");
        cout << "---------------------------------------------------------\n1:��ѯ���\n2:��ֵ���\n0:�˳�\n---------------------------------------------------------\n";
        cout << "��ѡ�����Ĳ���:";
        string x;
        cin >> x;
        if (x != "0" && x != "1" && x != "2") {
            cout << "�������������ѡ��\n";
            system("pause");
            continue;
        }
        if (x == "1") getBalance(userName);
        else if (x == "2") Recharge(userName);
        if (x == "0") return;
    }
}

void Client::Recharge(string userName) {
    cout << "����������Ҫ��ֵ�Ľ��(����0����):";
    string input;
    cin >> input;
    if (input == "0") return;
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
        system("pause");
        return;  // ��������
    }

    // ת��Ϊ double ������Ƿ�Ϊ 0
    double money = stod(input);
    if (money <= 0) {
        cout << "��ֵ��������� 0��\n";
        system("pause");
        return;
    }

    string request = "RECHARGE " + userName + " " + to_string(money);
    string response = sendRequest(request);
    if (response.find("RECHARGE_SUCCESS") == 0) {
        istringstream iss(response);
        string dummy;
        double balance;
        iss >> dummy >> balance;
        cout << "��ֵ�ɹ�!\n�˻����:" << balance << endl;
    }
    else cout << "��ֵʧ��!\n" << response;
    system("pause");
}

void Client::searchProduct() {
    string request = "SEARCH_PRODUCTS";
    string response = sendRequest(request);
    if (response == "SEARCH_SUCCESS") p.searchProduct();
    else cout << "����ʧ��!\n";
}

void Client::Shopping(string userName) {
    for (;;) {
        system("cls");
        cout << "---------------------------------------------------------\n1:��ӹ��ﳵ\n2:�Ƴ����ﳵ\n3:�鿴���ﳵ\n4:�޸�����\n5:����\n0:�˳�\n---------------------------------------------------------\n";
        cout << "��ѡ�����Ĳ���:";
        string x;
        cin >> x;
        if (x != "0" && x != "1" && x != "2" && x != "3" && x != "4" && x != "5") {
            cout << "�������������ѡ��\n";
            system("pause");
            continue;
        }
        if (x == "1") addCart(userName);
        else if (x == "2") removeCart(userName);
        else if (x == "3") showCart(userName);
        else if (x == "4") changeCart(userName);
        else if (x == "5") submitBill(userName);

        {
            Tourist t;
            t.updateSheet();
        }
        if (x == "0") break;
    }
}

void Client::addCart(string cname) {
    system("cls");
    string pname, type, belong;
    ll count;
    cout << "��������빺�ﳵ����Ʒ����(����r�˳�):";
    cin >> pname;
    if (pname == "r") return;
    cout << "��������빺�ﳵ����Ʒ����:";
    cin >> type;
    cout << "��������빺�ﳵ����Ʒ�̼�:";
    cin >> belong;
    cout << "��������ӵ�����:";
    cin >> count;
    string request = "ADD_CART " + cname + " " + pname + " " + type + " " + belong + " " + to_string(count);
    string response = sendRequest(request);
    if (response == "QUIT") return;
    else if (response == "NO_PRODUCT") cout << "û�������Ʒ!\n";
    else if (response == "OVER_REMAIN") cout << "����������ܳ������!\n";
    else if (response == "ADD_CART_SUCCESS") cout << "���빺�ﳵ�ɹ���\n";
    else cout << "���ʧ��!\n";
    system("pause");
}

void Client::removeCart(string cname) {
    system("cls");
    string pname, type, belong;
    ll count;
    cout << "�������Ƴ����ﳵ����Ʒ����(����r�˳�):";
    cin >> pname;
    if (pname == "r") return;
    cout << "�������Ƴ����ﳵ����Ʒ����:";
    cin >> type;
    cout << "�������Ƴ����ﳵ����Ʒ�̼�:";
    cin >> belong;
    cout << "�������Ƴ�������:";
    cin >> count;
    string request = "REMOVE_CART " + cname + " " + pname + " " + type + " " + belong + " " + to_string(count);
    string response = sendRequest(request);
    if (response == "QUIT") return;
    else if (response == "NO_PRODUCT") cout << "δ�ҵ���Ʒ����\n";
    else if (response == "OVER_REMAIN") cout << "�Ƴ��������࣡\n";
    else if (response == "REMOVE_CART_SUCCESS") cout << "�Ƴ��ɹ���\n";
    else cout << "�Ƴ�ʧ��!\n";
}

void Client::showCart(string cname) {
    string request = "SHOW_CART";
    string response = sendRequest(request);
    if (response == "SHOW_CART_SUCCESS") {
        Comsumer C;
        C.showCart(cname);
    }
}

void Client::changeCart(string cname) {
    system("cls");
    string pname, type, belong;
    ll count;
    cout << "�������޸Ĺ��ﳵ����Ʒ����(����r�˳�):";
    cin >> pname;
    if (pname == "r") return;
    cout << "���޸ļ��빺�ﳵ����Ʒ����:";
    cin >> type;
    cout << "���޸ļ��빺�ﳵ����Ʒ�̼�:";
    cin >> belong;
    cout << "���޸���ӵ�����:";
    cin >> count;
    string request = "CHANGE_CART_AMOUNT " + cname + " " + pname + " " + type + " " + belong + " " + to_string(count);
    string response = sendRequest(request);
    if (response == "QUIT") return;
    else if (response == "NO_PRODUCT") cout << "δ�ҵ���Ʒ����\n";
    else if (response == "OVER_REMAIN") cout << "����������ܳ������!\n";
    else if (response == "CHANGE_CART_AMOUNT_SUCCESS") cout << "�޸ĳɹ���\n";
    else cout << "�޸�ʧ��!\n";
}

void Client::submitBill(string cname) {
    string request = "SUBMIT_BILL";
    string response = sendRequest(request);
    if (response == "SUBMIT_BILL_SUCCESS") {
        Comsumer C;
        C.submitBill(cname);
        C.updateSheet();
    }
}

void Client::manageProduct(string userName) {
    for (;;) {
        system("cls");
        cout << "---------------------------------------------------------\n1:�����Ʒ\n2:�޸���Ʒ��Ϣ\n0:�˳�\n---------------------------------------------------------\n";
        cout << "��ѡ�����Ĳ���:";
        string x;
        cin >> x;
        if (x != "0" && x != "1" && x != "2") {
            cout << "�������������ѡ��\n";
            system("pause");
            continue;
        }
        if (x == "1") addProducts(userName);
        else if (x == "2") changeProduct(userName);
        if (x == "0") break;
    }
}

void Client::addProducts(string userName) {
    system("cls");
    string pname, type, belong, describe;
    string input;
    ll remain;
    belong = userName;
    cout << "��������Ʒ����(����q�˳�):";
    cin >> pname;
    if (pname == "q") return;
    if (pname.length() == 0 || pname.find(' ') != string::npos) {
        cout << "���ֲ����ǿգ������пո�\n";
        system("pause");
        return;
    }
    cout << "��������Ʒ����:";
    cin >> input;
    if (input == "0") return;
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
        system("pause");
        return;  // ��������
    }

    // ת��Ϊ double ������Ƿ�Ϊ 0
    double price = stod(input);
    if (price <= 0) {
        cout << "��ֵ��������� 0��\n";
        system("pause");
        return;
    }
    cout << "��������Ʒ����:";
    cin >> remain;
    cout << "��������Ʒ����:";
    cin >> type;
    cin.ignore();
    if (type.find(' ') != string::npos) {
        cout << "���������!\n";
        system("pause");
        return;
    }
    cout << "��������Ʒ����:";
    getline(cin, describe);
    string request = "ADD_PRODUCT " + userName + " " + pname + " " + type + " " + to_string(price) + " " + to_string(remain) + " \n" + describe + "\n";
    string response = sendRequest(request);
    if (response == "QUIT") return;
    else if (response == "PRODUCT_EXIST") cout << "�Ѿ�����ͬһ��Ʒ!\n";
    else if (response == "ADD_PRODUCT_SUCCESS") cout << "�����Ʒ�ɹ�!\n";
    else cout << "�����Ʒʧ��!\n";
    system("pause");
}

void Client::changeProduct(string userName) {
        system("cls");
        cout << "����������Ҫ�޸ĵ���Ʒ����Ʒ��(����q�˳�):";
        string pname;
        cin >> pname;
        if (pname == "q") return;
        if (pname.length() == 0 || pname.find(' ') != string::npos) {
            cout << "���ֲ����ǿգ������пո�\n";
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
        string x;
        for (;;) {
            system("cls");
            cout << "---------------------------------------------------------\n1:�޸���Ʒ��\n2:�޸ļ۸�\n3:�޸���Ʒ���\n4:�޸���Ʒ����\n5:�޸���Ʒ����\n0:�˳�\n---------------------------------------------------------\n";
            cout << "��ѡ�����Ĳ���:";
            cin >> x;
            if (x != "0" && x != "1" && x != "2" && x != "3" && x != "4" && x != "5") {
                cout << "�������������ѡ��\n";
                system("pause");
                continue;
            }
            break;
        }
        string New;
        cout << "����������Ҫ�޸ĵ�����:";
        cin >> New;
        string request = "CHANGE_PRODUCT " + userName + " " + pname + " " + type + " " + x + " \n" + New + "\n";
        string response = sendRequest(request);
        if (response == "QUIT") return;
        else if (response == "NO_PRODUCT") cout << "����Ʒ������!\n";
        else if (response == "CHANGE_PRODUCT_SUCCESS") cout << "�޸ĳɹ�!\n";
        else cout << "�޸�ʧ��!\n";
        system("pause");
        if (x == "0") return;
}