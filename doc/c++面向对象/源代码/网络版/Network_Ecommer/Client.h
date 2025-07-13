#pragma once
#include<string>
#include"Products.h"
#include"Sheet.h"
using namespace std;
#define _WIN32_WINNT 0x0600
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")


class Client {
private:
	SOCKET sock;
    Products p;
public:
    Client() : sock(INVALID_SOCKET) {}

    ~Client() {
        p.getPsheet();
        p.outSheet();
        if (sock != INVALID_SOCKET) {
            closesocket(sock);
        }
        WSACleanup();
    }

    bool init();

    bool connectToServer(const string& serverIp, int port);

    string sendRequest(const string& request);

    void run();

    void Login();

    void Register();

    void changePassword();

    void featurePage(string userName, int type);

    void productPage();

    double getBalance(string userName);

    void Balance_Operator(string userName);

    void Recharge(string userName);

    void searchProduct();

    void Shopping(string userName);

    void addCart(string cname);

    void removeCart(string cname);

    void showCart(string cname);

    void changeCart(string cname);

    void submitBill(string cname);

    void manageProduct(string userName);

    void addProducts(string userName);

    void changeProduct(string userName);
};