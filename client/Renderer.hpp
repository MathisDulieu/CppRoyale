#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <optional>
#include <cstdint>
#include "Packets.hpp"
#include "TroopType.hpp"

constexpr int TROOP_COUNT = 8;
constexpr float UI_PANEL_HEIGHT = 80.f;
constexpr float TROOP_CARD_SIZE = 60.f;
constexpr float TROOP_CARD_GAP = 10.f;

class Renderer {
public:
    Renderer();

    bool isOpen() const;

    std::optional<sf::Event> pollEvent();

    void render(const std::vector<EntitySnapshot> &entities,
                bool gameStarted,
                uint8_t localPlayerId,
                TroopType selectedTroop);

    bool isTroopCardClicked(const sf::Vector2i &mousePosition,
                            TroopType &outTroopType) const;

    bool isArenaClicked(const sf::Vector2i &mousePosition) const;

private:
    void drawArena();

    void drawEntities(const std::vector<EntitySnapshot> &entities,
                      uint8_t localPlayerId);

    void drawHpBar(const EntitySnapshot &snapshot, float renderY);

    void drawTroopPanel(TroopType selectedTroop);

    void drawWaitingScreen();

    sf::Color getTroopColor(TroopType troopType, uint8_t ownerId,
                            uint8_t localPlayerId) const;

    sf::Color getTroopCardColor(TroopType troopType) const;

    std::string getTroopName(TroopType troopType) const;

    sf::FloatRect getTroopCardBounds(int cardIndex) const;

    uint16_t getTroopMaxHp(TroopType troopType) const;

    sf::RenderWindow m_window;
    sf::Font m_font;
    bool m_fontLoaded;
};
