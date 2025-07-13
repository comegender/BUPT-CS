#pragma once
#define _WIN32_WINNT 0x0600
#include <winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "ws2_32.lib")
using namespace std;

class Server {
private:
	void handleClient(SOCKET clientSocket);

	void runServer();
};