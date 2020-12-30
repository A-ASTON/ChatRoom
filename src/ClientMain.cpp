#include "../lib/Client.h"

int main(int, char**) {
    Client client = Client();
    client.Init();
    client.Start();
    return 0;
}