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
        cerr << "无法创建socket: " << WSAGetLastError() << endl;
        return false;
    }

    sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(port);
    inet_pton(AF_INET, serverIp.c_str(), &serverAddr.sin_addr);

    if (connect(sock, (sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
        cerr << "连接服务器失败: " << WSAGetLastError() << endl;
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
    cout << "连接到服务器...\n";
    if (!connectToServer("127.0.0.1", 8080)) return;
    cout << "连接成功!\n";
    cout << "欢迎来到电商交易系统Y^_^Y\n开发者:BUPT 计子毅\n";
    string x;
    Tourist a;
    while (1) {
        system("pause");
        system("cls");
        a.productPage();
        cout << "---------------------------------------------------------\n1:注册\n2:登录\n3:改密码\n0:退出\n---------------------------------------------------------\n";
        cout << "请输入您的选择:";
        cin >> x;
        if (x != "1" && x != "2" && x != "3" && x != "0") {
            cout << "没有这个选择!\n";
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
    cout << "请输入您的身份(1:消费者 2:商家)(输入0退出):";
    cin >> type;
    if (type == 0) return;
    getc(stdin);
    cout << "请输入用户名:";
    string username;
    getline(cin, username);
    if (username.length() == 0 || username.find(' ') != string::npos) {
        cout << "用户名不能是空，不能有空格\n";
        system("pause");
        return;
    }
    string password;
    cout << "请输入密码:";
    getline(cin, password);
    if (password.length() == 0 || password.find(' ') != string::npos) {
        cout << "密码不能是空，不能有空格\n";
        system("pause");
        return;
    }
    request = "LOGIN " + to_string(type) + " " + username + " " + password;
    string response = sendRequest(request);
    if (response == "QUIT") return;
    else if (response == "NO_THIS_PERSON") cout << "没有该用户，请先注册!\n";
    else if (response == "PASSWORD_WRONG") cout << "密码不正确!\n";
    else if (response == "TYPE_WRONG") cout << "身份不正确!\n";
    else if (response == "LOGIN_SUCCESS") {
        cout << "登录成功!\n";
        system("pause");
        system("cls");
        featurePage(username, type);
    }
    else cout << "登录失败!\n";
}

void Client::Register() {
    system("cls");
    string name;
    string password;
    string ACP;
    int type;
    cout << "请输入您的用户名(输入r返回):";
    cin >> name;
    if (name == "r") return;
    if (name.length() == 0 || name.find(' ') != string::npos) {
        cout << "用户名不能是空，不能有空格\n";
        system("pause");
        return;
    }
    cout << "请输入您的密码:";
    cin >> password;
    if (password.length() == 0 || password.find(' ') != string::npos) {
        cout << "密码不能是空，不能有空格\n";
        system("pause");
        return;
    }
    cout << "请确认密码:";
    cin >> ACP;
    cout << "请选择您的身份:(消费者请输入1，商家请输入2):";
    cin >> type;
    string request = "REGISTER " + name + " " + password + " " + to_string(type) + " " + ACP;
    cout << request << endl;
    string response = sendRequest(request);
    if (response == "QUIT") return;
    else if (response == "HAVE_REGISTERED") cout << "该用户名已被注册，请重新注册!\n";
    else if (response == "ACP_WRONG") cout << "两次输入的密码不一致，注册失败!\n";
    else if (response == "REGISTER_SUCCESS") cout << "注册成功!\n";
    else cout << "注册失败\n";
    system("pause");
}

void Client::changePassword() {
    system("cls");
    string newpassword;
    string name;
    string ACP;
    cout << "请输入您的用户名(输入r返回):";
    cin >> name;
    if (name == "r") return;
    if (name.length() == 0 || name.find(' ') != string::npos) {
        cout << "用户名不能是空，不能有空格\n";
        system("pause");
        return;
    }
    cout << "请输入您的新密码:";
    cin >> newpassword;
    if (newpassword.length() == 0 || newpassword.find(' ') != string::npos) {
        cout << "密码不能是空，不能有空格\n";
        system("pause");
        return;
    }
    cout << "请确认您的新密码:";
    cin >> ACP;
    string request = "CHANGE_PASSWORD " + name + " " + newpassword + " " + ACP;
    string response = sendRequest(request);
    if (response == "NEWPASSWORD_SAME") cout << "新密码不能与原密码相同!\n";
    else if (response == "ACP_WRONG") cout << "两次密码不一致!\n";
    else if (response == "NO_THIS_PERSON") cout << "不存在此用户!\n";
    else if (response == "CHANGE_PASSWORD_SUCCESS") cout << "修改密码成功!\n";
    else cout << "修改密码失败!\n";
    system("pause");
}

void Client::featurePage(string userName,int type) {
    for (;;) {
        system("cls");
        productPage();
        cout << "---------------------------------------------------------\n1:余额操作\n2:搜索商品\n";
        if (type == 1) {
            cout << "3:购物\n";
        }
        if (type == 2) {
            cout << "4:商品管理\n";
        }
        cout << "0:退出\n---------------------------------------------------------\n";
        cout << "请选择您的操作:";
        string x;
        cin >> x;
        if ((x != "0" && x != "1" && x != "2" && x != "3" && x != "4") || (type == 1 && x == "4") || (type == 2 && x == "3")) {
            cout << "输入错误，请重新选择！\n";
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
        cout << "您的余额为:" << setprecision(2) << fixed << balance << endl;
        system("pause");
        return balance;
    }
    return 0;
}

void Client::Balance_Operator(string userName) {
    while (1) {
        system("cls");
        cout << "---------------------------------------------------------\n1:查询余额\n2:充值余额\n0:退出\n---------------------------------------------------------\n";
        cout << "请选择您的操作:";
        string x;
        cin >> x;
        if (x != "0" && x != "1" && x != "2") {
            cout << "输入错误，请重新选择！\n";
            system("pause");
            continue;
        }
        if (x == "1") getBalance(userName);
        else if (x == "2") Recharge(userName);
        if (x == "0") return;
    }
}

void Client::Recharge(string userName) {
    cout << "请输入您想要充值的金额(输入0返回):";
    string input;
    cin >> input;
    if (input == "0") return;
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
        system("pause");
        return;  // 重新输入
    }

    // 转换为 double 并检查是否为 0
    double money = stod(input);
    if (money <= 0) {
        cout << "充值金额必须大于 0！\n";
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
        cout << "充值成功!\n账户余额:" << balance << endl;
    }
    else cout << "充值失败!\n" << response;
    system("pause");
}

void Client::searchProduct() {
    string request = "SEARCH_PRODUCTS";
    string response = sendRequest(request);
    if (response == "SEARCH_SUCCESS") p.searchProduct();
    else cout << "搜索失败!\n";
}

void Client::Shopping(string userName) {
    for (;;) {
        system("cls");
        cout << "---------------------------------------------------------\n1:添加购物车\n2:移除购物车\n3:查看购物车\n4:修改数量\n5:结账\n0:退出\n---------------------------------------------------------\n";
        cout << "请选择您的操作:";
        string x;
        cin >> x;
        if (x != "0" && x != "1" && x != "2" && x != "3" && x != "4" && x != "5") {
            cout << "输入错误，请重新选择！\n";
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
    cout << "请输入加入购物车的商品名称(输入r退出):";
    cin >> pname;
    if (pname == "r") return;
    cout << "请输入加入购物车的商品种类:";
    cin >> type;
    cout << "请输入加入购物车的商品商家:";
    cin >> belong;
    cout << "请输入添加的数量:";
    cin >> count;
    string request = "ADD_CART " + cname + " " + pname + " " + type + " " + belong + " " + to_string(count);
    string response = sendRequest(request);
    if (response == "QUIT") return;
    else if (response == "NO_PRODUCT") cout << "没有这个商品!\n";
    else if (response == "OVER_REMAIN") cout << "添加数量不能超过库存!\n";
    else if (response == "ADD_CART_SUCCESS") cout << "加入购物车成功！\n";
    else cout << "添加失败!\n";
    system("pause");
}

void Client::removeCart(string cname) {
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
    string request = "REMOVE_CART " + cname + " " + pname + " " + type + " " + belong + " " + to_string(count);
    string response = sendRequest(request);
    if (response == "QUIT") return;
    else if (response == "NO_PRODUCT") cout << "未找到商品……\n";
    else if (response == "OVER_REMAIN") cout << "移除数量过多！\n";
    else if (response == "REMOVE_CART_SUCCESS") cout << "移除成功！\n";
    else cout << "移除失败!\n";
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
    cout << "请输入修改购物车的商品名称(输入r退出):";
    cin >> pname;
    if (pname == "r") return;
    cout << "请修改加入购物车的商品种类:";
    cin >> type;
    cout << "请修改加入购物车的商品商家:";
    cin >> belong;
    cout << "请修改添加的数量:";
    cin >> count;
    string request = "CHANGE_CART_AMOUNT " + cname + " " + pname + " " + type + " " + belong + " " + to_string(count);
    string response = sendRequest(request);
    if (response == "QUIT") return;
    else if (response == "NO_PRODUCT") cout << "未找到商品……\n";
    else if (response == "OVER_REMAIN") cout << "添加数量不能超过库存!\n";
    else if (response == "CHANGE_CART_AMOUNT_SUCCESS") cout << "修改成功！\n";
    else cout << "修改失败!\n";
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
        cout << "---------------------------------------------------------\n1:添加商品\n2:修改商品信息\n0:退出\n---------------------------------------------------------\n";
        cout << "请选择您的操作:";
        string x;
        cin >> x;
        if (x != "0" && x != "1" && x != "2") {
            cout << "输入错误，请重新选择！\n";
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
    cout << "请输入商品名称(输入q退出):";
    cin >> pname;
    if (pname == "q") return;
    if (pname.length() == 0 || pname.find(' ') != string::npos) {
        cout << "名字不能是空，不能有空格\n";
        system("pause");
        return;
    }
    cout << "请输入商品定价:";
    cin >> input;
    if (input == "0") return;
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
        system("pause");
        return;  // 重新输入
    }

    // 转换为 double 并检查是否为 0
    double price = stod(input);
    if (price <= 0) {
        cout << "充值金额必须大于 0！\n";
        system("pause");
        return;
    }
    cout << "请输入商品数量:";
    cin >> remain;
    cout << "请输入商品类型:";
    cin >> type;
    cin.ignore();
    if (type.find(' ') != string::npos) {
        cout << "不能输入空!\n";
        system("pause");
        return;
    }
    cout << "请输入商品描述:";
    getline(cin, describe);
    string request = "ADD_PRODUCT " + userName + " " + pname + " " + type + " " + to_string(price) + " " + to_string(remain) + " \n" + describe + "\n";
    string response = sendRequest(request);
    if (response == "QUIT") return;
    else if (response == "PRODUCT_EXIST") cout << "已经存在同一商品!\n";
    else if (response == "ADD_PRODUCT_SUCCESS") cout << "添加商品成功!\n";
    else cout << "添加商品失败!\n";
    system("pause");
}

void Client::changeProduct(string userName) {
        system("cls");
        cout << "请输入您想要修改的商品的商品名(输入q退出):";
        string pname;
        cin >> pname;
        if (pname == "q") return;
        if (pname.length() == 0 || pname.find(' ') != string::npos) {
            cout << "名字不能是空，不能有空格\n";
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
        string x;
        for (;;) {
            system("cls");
            cout << "---------------------------------------------------------\n1:修改商品名\n2:修改价格\n3:修改商品类别\n4:修改商品描述\n5:修改商品数量\n0:退出\n---------------------------------------------------------\n";
            cout << "请选择您的操作:";
            cin >> x;
            if (x != "0" && x != "1" && x != "2" && x != "3" && x != "4" && x != "5") {
                cout << "输入错误，请重新选择！\n";
                system("pause");
                continue;
            }
            break;
        }
        string New;
        cout << "请输入您想要修改的内容:";
        cin >> New;
        string request = "CHANGE_PRODUCT " + userName + " " + pname + " " + type + " " + x + " \n" + New + "\n";
        string response = sendRequest(request);
        if (response == "QUIT") return;
        else if (response == "NO_PRODUCT") cout << "该商品不存在!\n";
        else if (response == "CHANGE_PRODUCT_SUCCESS") cout << "修改成功!\n";
        else cout << "修改失败!\n";
        system("pause");
        if (x == "0") return;
}