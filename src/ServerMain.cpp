#include <iostream>
#include "../lib/Server.h"

int main(int, char**) {
    std::cout << "Hello, world!\n";
    Server server = Server::GetServer();
    server.Init();
    server.Start();
}
