#include "GameClient.hpp"
#include "Renderer.hpp"
#include "Constants.hpp"
#include <string>

int main() {
    GameClient client;
    Renderer   renderer;

    bool connected = client.connect("127.0.0.1", SERVER_PORT);
    std::string status = connected ? "Connected to server!" : "Connection failed";

    sf::Clock pingClock;

    while (renderer.isOpen()) {
        while (auto event = renderer.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                return 0;
        }

        if (connected && pingClock.getElapsedTime().asSeconds() > 1.f) {
            client.sendPing();
            pingClock.restart();
        }

        if (client.receivePong())
            status = "Connected to server!  [PONG received]";

        renderer.render(status);
    }
    return 0;
}