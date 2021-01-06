#include "../lib/Client.h"

int main(int, char**) {
    Client c = Client();
    AbstractClient &client = c;
    client.Init(8099);
    client.Start();
    return 0;
}