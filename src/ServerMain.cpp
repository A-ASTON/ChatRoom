#include "../lib/Server.h"

int main(int, char**) {
    AbstractServer &server = Server::GetServer();
    server.Init(8099);
    server.Start();
    return 0;
}
