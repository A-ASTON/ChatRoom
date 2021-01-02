#include "../lib/Client.h"

int main(int, char**) {
    Client client = Client();
    client.Init(8099);
    client.Start();
    return 0;
}