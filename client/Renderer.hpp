#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <optional>

class Renderer {
public:
    Renderer();
    bool isOpen() const;
    std::optional<sf::Event> pollEvent();
    void render(const std::string& statusText);

private:
    sf::RenderWindow m_window;
};