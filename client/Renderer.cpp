#include "Renderer.hpp"

Renderer::Renderer()
    : m_window(sf::VideoMode({800, 600}), "CppRoyale") {
    m_window.setFramerateLimit(60);
}

bool Renderer::isOpen() const {
    return m_window.isOpen();
}

std::optional<sf::Event> Renderer::pollEvent() {
    return m_window.pollEvent();
}

void Renderer::render(const std::string &statusText,
                      bool gameStarted,
                      uint8_t playerId) {
    m_window.clear(sf::Color(20, 20, 30));

    sf::RectangleShape statusBox(sf::Vector2f(300.f, 80.f));
    statusBox.setPosition({250.f, 260.f});
    statusBox.setFillColor(gameStarted
                               ? sf::Color(40, 180, 100)
                               : sf::Color(200, 140, 30));
    m_window.draw(statusBox);

    if (playerId != 255) {
        sf::RectangleShape playerBox(sf::Vector2f(100.f, 40.f));
        playerBox.setPosition({350.f, 360.f});
        playerBox.setFillColor(playerId == 0
                                   ? sf::Color(60, 120, 200)
                                   : sf::Color(200, 80, 80));
        m_window.draw(playerBox);
    }

    m_window.display();
}
