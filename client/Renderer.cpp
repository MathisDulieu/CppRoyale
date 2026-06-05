#include "Renderer.hpp"

Renderer::Renderer()
    : m_window(sf::VideoMode({800, 600}), "CppRoyale - Bootstrap")
{
    m_window.setFramerateLimit(60);
}

bool Renderer::isOpen() const { return m_window.isOpen(); }

std::optional<sf::Event> Renderer::pollEvent() {
    return m_window.pollEvent();
}

void Renderer::render(const std::string& statusText) {
    m_window.clear(sf::Color(20, 20, 30));

    sf::RectangleShape indicator(sf::Vector2f(200.f, 60.f));
    indicator.setPosition({300.f, 270.f});
    bool connected = (statusText.find("Connected") != std::string::npos);
    indicator.setFillColor(connected
        ? sf::Color(40, 180, 100)
        : sf::Color(180, 60, 60));
    m_window.draw(indicator);

    m_window.display();
}