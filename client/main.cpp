#include "GameClient.hpp"
#include "Renderer.hpp"
#include "Deck.hpp"
#include "UIScreen.hpp"
#include "NameGenerator.hpp"
#include "Constants.hpp"
#include <SFML/Graphics.hpp>

int main() {
    sf::RenderWindow window(sf::VideoMode({800, 680}), "CppRoyale");
    window.setFramerateLimit(60);

    sf::Font font;
    bool fontLoaded = font.openFromFile("C:/Windows/Fonts/arial.ttf");

    GameClient client;
    Deck deck;
    UIScreen ui(window, font, fontLoaded);
    Renderer renderer(window, font, fontLoaded);

    if (!client.connect("127.0.0.1", SERVER_PORT))
        return 1;

    int selectedHandIndex = 0;
    bool showPlayerList = false;
    std::string currentNameInput = NameGenerator::generate();
    bool cursorVisible = true;
    sf::Clock cursorClock;
    sf::Clock frameClock;
    float buttonCooldown = 0.f;

    while (window.isOpen()) {
        float deltaTime = frameClock.restart().asSeconds();
        buttonCooldown = std::max(0.f, buttonCooldown - deltaTime);

        if (cursorClock.getElapsedTime().asSeconds() > 0.5f) {
            cursorVisible = !cursorVisible;
            cursorClock.restart();
        }

        sf::Vector2i mousePos = sf::Mouse::getPosition(window);
        ui.updateHover(mousePos);

        client.receivePackets();
        client.tickMatchFoundTimer(deltaTime);

        while (auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                return 0;

            if (event->is<sf::Event::TextEntered>()) {
                const auto *textEvent = event->getIf<sf::Event::TextEntered>();
                if (textEvent && client.getState() == ClientState::EnteringName) {
                    uint32_t unicode = textEvent->unicode;
                    if (unicode == 8 && !currentNameInput.empty())
                        currentNameInput.pop_back();
                    else if (unicode >= 32 && unicode < 127
                             && currentNameInput.size() < MAX_NAME_LENGTH)
                        currentNameInput += static_cast<char>(unicode);
                }
            }

            if (event->is<sf::Event::MouseWheelScrolled>()) {
                const auto *wheelEvent = event->getIf<sf::Event::MouseWheelScrolled>();
                if (wheelEvent && showPlayerList) {
                    int delta = (wheelEvent->delta > 0) ? -1 : 1;
                    ui.scrollPlayerList(delta);
                }
            }

            if (!event->is<sf::Event::MouseButtonPressed>()) continue;
            const auto *mouseEvent = event->getIf<sf::Event::MouseButtonPressed>();
            if (!mouseEvent) continue;

            sf::Vector2i mouse = mouseEvent->position;
            ClientState state = client.getState();

            if (state == ClientState::EnteringName) {
                if (ui.isConfirmNameClicked(mouse) && !currentNameInput.empty())
                    client.sendName(currentNameInput);
                else if (ui.isGenerateNameClicked(mouse))
                    currentNameInput = NameGenerator::generate();
            } else if (state == ClientState::Idle) {
                if (ui.isPlayerListClicked(mouse)) {
                    showPlayerList = !showPlayerList;
                    if (showPlayerList) {
                        client.requestPlayerList();
                        ui.scrollPlayerList(-999);
                    }
                } else if (showPlayerList) {
                    uint8_t targetId;
                    if (ui.isChallengeClicked(mouse, targetId,
                                              client.getPlayerList(),
                                              client.getClientId())) {
                        client.sendChallenge(targetId);
                        showPlayerList = false;
                    } else {
                        showPlayerList = false;
                    }
                } else if (ui.isFindMatchClicked(mouse) && buttonCooldown <= 0.f) {
                    client.sendFindMatch();
                    buttonCooldown = 1.f;
                }
            } else if (state == ClientState::Searching) {
                if (ui.isCancelMatchClicked(mouse) && buttonCooldown <= 0.f) {
                    client.sendCancelMatch();
                    buttonCooldown = 1.f;
                } else if (ui.isPlayerListClicked(mouse)) {
                    showPlayerList = !showPlayerList;
                    if (showPlayerList) {
                        client.requestPlayerList();
                        ui.scrollPlayerList(-999);
                    }
                }
            } else if (state == ClientState::ChallengeSent) {
                if (ui.isCancelChallengeClicked(mouse) && buttonCooldown <= 0.f) {
                    client.sendCancelChallenge();
                    buttonCooldown = 1.f;
                }
            } else if (state == ClientState::ChallengeReceived) {
                if (ui.isAcceptChallengeClicked(mouse))
                    client.sendChallengeResponse(true);
                else if (ui.isDeclineChallengeClicked(mouse))
                    client.sendChallengeResponse(false);
            } else if (state == ClientState::GameOver) {
                if (ui.isReturnToLobbyClicked(mouse)) {
                    client.sendReturnToLobby();
                    deck = Deck();
                    selectedHandIndex = 0;
                    showPlayerList = false;
                }
            } else if (state == ClientState::Playing) {
                int clickedHandIndex;
                if (renderer.isHandCardClicked(mouse, clickedHandIndex)) {
                    selectedHandIndex = clickedHandIndex;
                } else if (renderer.isArenaClicked(mouse)) {
                    float clickX = static_cast<float>(mouse.x);
                    float clickY = static_cast<float>(mouse.y);

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

        window.clear(sf::Color(20, 20, 30));

        ClientState state = client.getState();

        if (state == ClientState::EnteringName) {
            ui.drawNameEntry(currentNameInput, cursorVisible, client.getNameError());
        } else if (state == ClientState::Idle) {
            ui.drawIdle(client.getName(), showPlayerList,
                        client.getPlayerList(), client.getClientId());
        } else if (state == ClientState::Searching) {
            ui.drawSearching(client.getName());
        } else if (state == ClientState::ChallengeSent) {
            ui.drawChallengeSent("opponent");
        } else if (state == ClientState::ChallengeReceived) {
            ui.drawIdle(client.getName(), false,
                        client.getPlayerList(), client.getClientId());
            ui.drawChallengeReceived(client.getChallengerName());
        } else if (state == ClientState::MatchFound) {
            ui.drawMatchFound(client.getOpponentName(),
                              client.getMatchFoundTimer());
        } else if (state == ClientState::Playing) {
            renderer.render(client.getEntities(), client.getTowers(),
                            false,
                            client.getPlayerId(), client.getWinnerId(),
                            deck, selectedHandIndex,
                            client.getElixir(),
                            client.getRemainingTime(),
                            client.isOvertime());
        } else if (state == ClientState::GameOver) {
            renderer.render(client.getEntities(), client.getTowers(),
                            true,
                            client.getPlayerId(), client.getWinnerId(),
                            deck, selectedHandIndex,
                            client.getElixir(),
                            client.getRemainingTime(),
                            client.isOvertime());
            ui.drawGameOver(client.getWinnerId() == client.getPlayerId(),
                            client.getWinnerId() == 255);
        }

        window.display();
    }

    return 0;
}
