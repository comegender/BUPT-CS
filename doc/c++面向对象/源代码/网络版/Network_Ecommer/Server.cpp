#include"Tourist.h"
#include"Server.h"
#include<sstream>
#include"Business.h"
#include"Comsumer.h"
#include"Discaonut.h"
using namespace std;

void Server::handleClient(SOCKET clientSocket) {
	char buffer[1024] = { 0 };
	string response;
	Tourist T;

	while (1) {
		memset(buffer, 0, sizeof(buffer));
		int bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
		if (bytesRead <= 0) break;

		string request(buffer);
		istringstream iss(request);
		string commend;
		iss >> commend;

		if (commend == "LOGIN") {
			string username, password;
			int type;
			if (iss >> type >> username >> password) {
				int success = T.Sign_In(username, password, type);
				if (success == 0) response = "QUIT";
				else if (success == 1) response = "NO_THIS_PERSON";
				else if (success == 2) response = "PASSWORD_WRONG";
				else if (success == 3) response = "TYPE_WRONG";
				else response = "LOGIN_SUCCESS";
			}
			else response = "LOGIN_FAILED";
		}
		else if (commend == "REGISTER") {
			string username, password, ACP;
			int type;
			if (iss >> username >> password >> type >> ACP) {
				int success = T.Sign_Up(username, password, type, ACP);
				switch (success)
				{
				case 0:
					response = "QUIT";
					break;
				case 1:
					response = "HAVE_REGISTERED";
					break;
				case 2:
					response = "ACP_WRONG";
					break;
				case 3:
					response = "REGISTER_SUCCESS";
					break;
				default:
					break;
				}
			}
			else response = "REGISTER_FAILED";
			cout << response << endl;
		}
		else if (commend == "CHANGE_PASSWORD") {
			string newpassword, username, ACP;
			if (iss >> username >> newpassword >> ACP) {
				int success = T.changePassword(username, newpassword, ACP);
				switch (success)
				{
				case 1:
					response = "ACP_WRONG"; 
					break;
				case 2:
					response = "NO_THIS_PERSON";
					break;
				case 3:
					response = "NEWPASSWORD_SAME";
					break;
				case 4:
					response = "CHANGE_PASSWORD_SUCCESS";
					break;
				default:
					break;
				}
			}
			else {
				response = "CHANGE_PASSWORD_FAILED";
			}
		}
		else if (commend == "GET_BALANCE") {
			string username;
			if (iss >> username) {
				response = "BALANCE " + to_string(T.getUserBalance(username));
			}
			else response = "BALANCE_FAILED";
		}
		else if (commend == "RECHARGE") {
			string username;
			double newBalance;
			if (iss >> username >> newBalance) {
				double success = T.Recharge(username, newBalance);
				if (success != -1) response = "RECHARGE_SUCCESS " + to_string(success);
				else response = "QUIT";
			}
			else response = "RECHARGE_FAILED";
		}
		else if (commend == "SEARCH_PRODUCTS") {
			response = "SEARCH_SUCCESS";
		}
		else if (commend == "ADD_PRODUCT") {
			string username, pname, type, belong, describe;
			double price;
			ll remain;
			if (iss >> username >> pname >> type >> price >> remain) {
				getline(iss, describe);
				getline(iss, describe);
				belong = username;
				Business B;
				int success = B.addProducts(username, pname, type, belong, describe, price, remain);
				switch (success)
				{
				case 0:
					response = "QUIT";
					break;
				case 1:
					response = "PRODUCT_EXIST";
					break;
				case 2:
					response = "ADD_PRODUCT_SUCCESS";
				default:
					break;
				}
			}
			else response = "ADD_PRODUCT_FAILED";
		}
		else if (commend == "CHANGE_PRODUCT") {
			string username, pname, type;
			string mode;
			string New;
			if (iss >> username >> pname >> type >> mode) {
				getline(iss, New);
				getline(iss, New);
				Business B;
				int success = B.changeProduct(username, mode, pname, type, New);
				switch (success)
				{
				case 0:
					response = "QUIT";
					break;
				case 1:
				case 2:
					response = "NO_PRODUCT";
					break;
				case 3:
					response = "CHANGE_PRODUCT_SUCCESS";
				default:
					break;
				}
			}
			else response = "CHANGE_PRODUCT_FAILED";
		}
		else if (commend == "ADD_CART") {
			string cname, pname, type, belong; 
			ll count;
			if (iss >> cname >> pname >> type >> belong >> count) {
				Comsumer C;
				int success = C.addCart(cname, pname, type, belong, count);
				switch (success)
				{
				case 0:
					response = "QUIT";
					break;
				case 1:
					response = "NO_PRODUCT";
					break;
				case 2:
					response = "OVER_REMAIN";
					break;
				case 3:
					response = "ADD_CART_SUCCESS";
				default:
					break;
				}
			}
			else response = "ADD_CART_FAILED";
		}
		else if (commend == "REMOVE_CART") {
			string cname, pname, type, belong;
			ll count;
			if (iss >> cname >> pname >> type >> belong >> count) {
				Comsumer C;
				int success = C.removeCart(cname, pname, type, belong, count);
				switch (success)
				{
				case 0:
					response = "QUIT";
					break;
				case 1:
					response = "NO_PRODUCT";
					break;
				case 2:
					response = "OVER_REMAIN";
					break;
				case 3:
					response = "REMOVE_CART_SUCCESS";
				default:
					break;
				}
			}
			else response = "REMOVE_CART_FAILED";
		}
		else if (commend == "SHOW_CART") {
			response = "SHOW_CART_SUCCESS";
		}
		else if (commend == "CHANGE_CART_AMOUNT") {
			string cname, pname, type, belong;
			ll count;
			if (iss >> cname >> pname >> type >> belong >> count) {
				Comsumer C;
				int success = C.changeCart(cname, pname, type, belong, count);
				switch (success)
				{
				case 0:
					response = "QUIT";
					break;
				case 1:
					response = "NO_PRODUCT";
					break;
				case 2:
					response = "OVER_REMAIN";
					break;
				case 3:
					response = "CHANGE_CART_AMOUNT_SUCCESS";
				default:
					break;
				}
			}
			else response = "CHANGE_CART_AMOUNT_FAILED";
		}
		else if (commend == "SHOW_BILL") {
			response = "SHOW_BILL_SUCCESS";
		}
		else if (commend == "SUBMIT_BILL") {
			response = "SUBMIT_BILL_SUCCESS";
		}
		else {
			response = "UNKOWN_COMMAND";
		}

		send(clientSocket, response.c_str(), (int)response.size(), 0);

	}
	T.updateSheet();
	closesocket(clientSocket);
}

void Server::runServer() {
	if (IS_DISCOUNT) {
		Products p;
		p.Discount();
	}
	// 初始化Winsock
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
		cerr << "WSAStartup failed" << endl;
		return ;
	}

	// 创建服务器socket
	SOCKET serverSocket = socket(AF_INET, SOCK_STREAM, 0);
	if (serverSocket == INVALID_SOCKET) {
		cerr << "无法创建socket: " << WSAGetLastError() << endl;
		WSACleanup();
		return ;
	}

	// 设置socket选项
	int opt = 1;
	if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&opt, sizeof(opt)) == SOCKET_ERROR) {
		cerr << "设置socket选项失败: " << WSAGetLastError() << endl;
		closesocket(serverSocket);
		WSACleanup();
		return ;
	}

	// 绑定socket到端口
	sockaddr_in serverAddr;
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = INADDR_ANY;
	serverAddr.sin_port = htons(8080);

	if (bind(serverSocket, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) {
		cerr << "绑定端口失败: " << WSAGetLastError() << endl;
		closesocket(serverSocket);
		WSACleanup();
		return ;
	}

	// 开始监听
	if (listen(serverSocket, 5) == SOCKET_ERROR) {
		cerr << "监听失败: " << WSAGetLastError() << endl;
		closesocket(serverSocket);
		WSACleanup();
		return ;
	}

	cout << "服务器启动，监听端口8080..." << endl;

	// 接受客户端连接
	while (true) {
		sockaddr_in clientAddr;
		int clientAddrLen = sizeof(clientAddr);
		SOCKET clientSocket = accept(serverSocket, (struct sockaddr*)&clientAddr, &clientAddrLen);
		if (clientSocket == INVALID_SOCKET) {
			cerr << "接受客户端连接失败: " << WSAGetLastError() << endl;
			continue;
		}

		char clientIP[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &(clientAddr.sin_addr), clientIP, INET_ADDRSTRLEN);
		cout << "新客户端连接: " << clientIP << ":" << ntohs(clientAddr.sin_port) << endl;

		// 创建新线程处理客户端
		thread(&Server::handleClient, this, clientSocket).detach();
	}

	closesocket(serverSocket);
	WSACleanup();
	return ;
}