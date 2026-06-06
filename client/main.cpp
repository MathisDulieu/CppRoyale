#include "GameClient.hpp"
#include "Renderer.hpp"
#include "Deck.hpp"
#include "Constants.hpp"

int main() {
    GameClient client;
    Renderer renderer;
    Deck deck;

    if (!client.connect("127.0.0.1", SERVER_PORT))
        return 1;

    int selectedHandIndex = 0;

    while (renderer.isOpen()) {
        while (auto event = renderer.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                return 0;

            if (event->is<sf::Event::MouseButtonPressed>()) {
                const auto *mouseEvent =
                        event->getIf<sf::Event::MouseButtonPressed>();
                if (!mouseEvent) continue;
                if (client.getState() == ClientState::GameOver) continue;

                sf::Vector2i mousePosition = mouseEvent->position;

                int clickedHandIndex;
                if (renderer.isHandCardClicked(mousePosition, clickedHandIndex)) {
                    selectedHandIndex = clickedHandIndex;
                } else if (renderer.isArenaClicked(mousePosition)
                           && client.getState() == ClientState::Playing) {
                    float clickX = static_cast<float>(mousePosition.x);
                    float clickY = static_cast<float>(mousePosition.y);

                    if (client.getPlayerId() == 0)
                        clickY = ARENA_HEIGHT - 1.f - clickY;

                    TroopType selectedTroop = deck.getCard(selectedHandIndex);
                    uint8_t cost = 0;

                    if (selectedTroop == TroopType::Goblin) cost = 2;
                    else if (selectedTroop == TroopType::Giant) cost = 5;
                    else if (selectedTroop == TroopType::Archer) cost = 3;
                    else if (selectedTroop == TroopType::Knight) cost = 3;
                    else if (selectedTroop == TroopType::Bomber) cost = 4;
                    else if (selectedTroop == TroopType::Dragon) cost = 4;
                    else if (selectedTroop == TroopType::Golem) cost = 8;
                    else if (selectedTroop == TroopType::Wizard) cost = 5;

                    if (client.getElixir() >= static_cast<float>(cost)) {
                        client.deployTroop(selectedTroop, clickX, clickY);
                        deck.playCard(selectedHandIndex);
                    }
                }
            }
        }

        client.receivePackets();

        bool gameStarted = (client.getState() != ClientState::Waiting);
        bool gameOver = (client.getState() == ClientState::GameOver);
        uint8_t playerId = client.getPlayerId();
        uint8_t winnerId = client.getWinnerId();
        float elixir = client.getElixir();
        float remainingTime = client.getRemainingTime();
        bool isOvertime = client.isOvertime();

        renderer.render(client.getEntities(), client.getTowers(),
                        gameStarted, gameOver,
                        playerId, winnerId,
                        deck, selectedHandIndex,
                        elixir, remainingTime, isOvertime);
    }

    return 0;
}
