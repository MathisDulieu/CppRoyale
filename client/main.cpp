#include "GameClient.hpp"
#include "Renderer.hpp"
#include "Constants.hpp"

int main() {
    GameClient client;
    Renderer renderer;

    if (!client.connect("127.0.0.1", SERVER_PORT))
        return 1;

    while (renderer.isOpen()) {
        while (const auto event = renderer.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                return 0;

            if (event->is<sf::Event::MouseButtonPressed>()) {
                const auto &mouseEvent = event->getIf<sf::Event::MouseButtonPressed>();
                if (mouseEvent && client.getState() == ClientState::Playing) {
                    const auto clickX = static_cast<float>(mouseEvent->position.x);
                    const auto clickY = static_cast<float>(mouseEvent->position.y);
                    client.deployTroop(TroopType::Knight, clickX, clickY);
                }
            }
        }

        client.receivePackets();

        const bool gameStarted = (client.getState() == ClientState::Playing);
        const uint8_t playerId = client.getPlayerId();

        renderer.render(client.getEntities(), gameStarted, playerId);
    }

    return 0;
}
