#include"Products.h"
#include"Tourist.h"
#include"Server.h"
#include<sstream>
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
			string username, password, type;
			if (iss >> type >> username >> password) {
				int success = T.Sign_In(username, password, type);

			}
		}
	}
}