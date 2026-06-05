#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <optional>
#include <cstdint>

class Renderer {
public:
    Renderer();

    bool isOpen() const;

    std::optional<sf::Event> pollEvent();

    void render(const std::string &statusText,
                bool gameStarted,
                uint8_t playerId);

private:
    sf::RenderWindow m_window;
};
