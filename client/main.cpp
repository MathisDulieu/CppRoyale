#include "GameClient.hpp"
#include "Renderer.hpp"
#include "Constants.hpp"

int main() {
    GameClient client;
    Renderer renderer;

    if (!client.connect("127.0.0.1", SERVER_PORT))
        return 1;

    TroopType selectedTroop = TroopType::Knight;

    while (renderer.isOpen()) {
        while (auto event = renderer.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                return 0;

            if (event->is<sf::Event::MouseButtonPressed>()) {
                const auto *mouseEvent =
                        event->getIf<sf::Event::MouseButtonPressed>();
                if (!mouseEvent) continue;

                sf::Vector2i mousePosition = mouseEvent->position;

                if (client.getState() == ClientState::GameOver) continue;

                TroopType clickedTroop;
                if (renderer.isTroopCardClicked(mousePosition, clickedTroop)) {
                    selectedTroop = clickedTroop;
                } else if (renderer.isArenaClicked(mousePosition)
                           && client.getState() == ClientState::Playing) {
                    float clickX = static_cast<float>(mousePosition.x);
                    float clickY = static_cast<float>(mousePosition.y);

                    if (client.getPlayerId() == 0)
                        clickY = ARENA_HEIGHT - 1.f - clickY;

                    client.deployTroop(selectedTroop, clickX, clickY);
                }
            }
        }

        client.receivePackets();

        bool gameStarted = (client.getState() != ClientState::Waiting);
        bool gameOver = (client.getState() == ClientState::GameOver);
        uint8_t playerId = client.getPlayerId();
        uint8_t winnerId = client.getWinnerId();

        renderer.render(client.getEntities(), client.getTowers(),
                        gameStarted, gameOver,
                        playerId, winnerId, selectedTroop);
    }

    return 0;
}
