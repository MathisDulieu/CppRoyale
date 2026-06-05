#include "GameClient.hpp"
#include "Renderer.hpp"
#include "Constants.hpp"

int main() {
    GameClient client;
    Renderer renderer;

    bool connected = client.connect("127.0.0.1", SERVER_PORT);
    if (!connected)
        return 1;

    sf::Clock pingClock;

    while (renderer.isOpen()) {
        while (auto event = renderer.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                return 0;
        }

        client.receivePackets();

        if (pingClock.getElapsedTime().asSeconds() > 1.f) {
            client.sendPing();
            pingClock.restart();
        }

        bool gameStarted = (client.getState() == ClientState::Playing);
        uint8_t playerId = client.getPlayerId();

        std::string statusText = gameStarted
                                     ? "Game started"
                                     : "Waiting for players...";

        renderer.render(statusText, gameStarted, playerId);
    }

    return 0;
}
