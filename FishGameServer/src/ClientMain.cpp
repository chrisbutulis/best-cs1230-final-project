#include "TestClient.h"

int main() {
    TestClient client("127.0.0.1", 12345); // Replace with your server's IP and port
    client.run();
    return 0;
}
