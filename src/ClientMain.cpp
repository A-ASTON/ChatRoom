#include "../lib/Client.h"

int main(int, char**) {
    Client client = Client();
    client.test();
    client.Init();
    client.Start();
    return 0;
}