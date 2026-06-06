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
                const std::vector<TowerSnapshot> &towers,
                bool gameStarted,
                bool gameOver,
                uint8_t localPlayerId,
                uint8_t winnerId,
                TroopType selectedTroop,
                float elixir,
                float remainingTime,
                bool isOvertime);

    bool isTroopCardClicked(const sf::Vector2i &mousePosition,
                            TroopType &outTroopType) const;

    bool isArenaClicked(const sf::Vector2i &mousePosition) const;

private:
    void drawArena();

    void drawTowers(const std::vector<TowerSnapshot> &towers,
                    uint8_t localPlayerId);

    void drawTowerHpBar(const TowerSnapshot &snapshot, float renderY);

    void drawEntities(const std::vector<EntitySnapshot> &entities,
                      uint8_t localPlayerId);

    void drawHpBar(const EntitySnapshot &snapshot, float renderY);

    void drawTroopPanel(TroopType selectedTroop, float elixir);

    void drawElixirBar(float elixir);

    void drawTimer(float remainingTime, bool isOvertime);

    void drawWaitingScreen();

    void drawGameOverScreen(bool localPlayerWon, bool isDraw);

    float getRenderY(float serverY, uint8_t localPlayerId) const;

    sf::Color getTroopColor(TroopType troopType, uint8_t ownerId,
                            uint8_t localPlayerId) const;

    sf::Color getTowerColor(bool isNexus, uint8_t ownerId,
                            uint8_t localPlayerId) const;

    sf::Color getTroopCardColor(TroopType troopType, bool affordable) const;

    std::string getTroopName(TroopType troopType) const;

    sf::FloatRect getTroopCardBounds(int cardIndex) const;

    uint16_t getTroopMaxHp(TroopType troopType) const;

    uint8_t getTroopCost(TroopType troopType) const;

    sf::RenderWindow m_window;
    sf::Font m_font;
    bool m_fontLoaded;
};
