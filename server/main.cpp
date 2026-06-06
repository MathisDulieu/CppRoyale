#include "GameServer.hpp"
#include "Constants.hpp"

int main() {
    GameServer server(SERVER_PORT);
    server.run();
    return 0;
}
