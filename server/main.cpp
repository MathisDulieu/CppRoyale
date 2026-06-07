#include "network/GameServer.hpp"
#include "../shared/Constants.hpp"

int main() {
    GameServer server(SERVER_PORT);
    server.run();
    return 0;
}
