#include <iostream>
#include "../lib/Server.h"

int main(int, char**) {
    Server server = Server::GetServer();
    server.Init(8099);
    server.Start();
}
