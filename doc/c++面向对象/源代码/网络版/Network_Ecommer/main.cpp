#include<iostream>
#include"Server.h"
#include"Client.h"
using namespace std;

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Error: No arguments provided. Use '-help' for usage.\n";
        return 1;
    }

    string arg = argv[1]; // 获取第一个参数

    if (arg == "-server") {
        Server s;
        s.runServer();
    }
    else if (arg == "-client") {
        Client c;
        c.run();
    }
    else if (arg == "-help") {
        cout << "Usage: ./test.exe [OPTION]\n"
            << "Options:\n"
            << "  -server   Run in server mode\n"
            << "  -client   Run in client mode\n"
            << "  -help     Show this help message\n";
    }
    else {
        cerr << "Error: Unknown argument '" << arg << "'. Use '-help' for usage.\n";
        return 1;
    }

    return 0;
}
