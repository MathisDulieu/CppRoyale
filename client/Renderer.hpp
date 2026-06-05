#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <optional>
#include <cstdint>
#include "Packets.hpp"

class Renderer {
public:
    Renderer();

    bool isOpen() const;

    std::optional<sf::Event> pollEvent();

    void render(const std::vector<EntitySnapshot> &entities,
                bool gameStarted,
                uint8_t localPlayerId);

private:
    static sf::Color getTroopColor(TroopType troopType, uint8_t ownerId,
                                   uint8_t localPlayerId);

    sf::RenderWindow m_window;
};
