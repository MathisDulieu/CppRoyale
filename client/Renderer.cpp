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

void Renderer::render(const std::vector<EntitySnapshot> &entities,
                      const bool gameStarted,
                      const uint8_t localPlayerId) {
    m_window.clear(sf::Color(20, 20, 30));

    if (!gameStarted) {
        sf::RectangleShape waitingBox(sf::Vector2f(300.f, 80.f));
        waitingBox.setPosition({250.f, 260.f});
        waitingBox.setFillColor(sf::Color(200, 140, 30));
        m_window.draw(waitingBox);
        m_window.display();
        return;
    }

    sf::RectangleShape midline(sf::Vector2f(800.f, 2.f));
    midline.setPosition({0.f, 300.f});
    midline.setFillColor(sf::Color(80, 80, 80));
    m_window.draw(midline);

    for (const auto &snapshot: entities) {
        sf::RectangleShape troopShape(sf::Vector2f(24.f, 24.f));
        troopShape.setOrigin({12.f, 12.f});
        troopShape.setPosition({snapshot.x, snapshot.y});
        troopShape.setFillColor(getTroopColor(snapshot.troopType,
                                              snapshot.ownerId,
                                              localPlayerId));
        m_window.draw(troopShape);
    }

    m_window.display();
}

sf::Color Renderer::getTroopColor(TroopType troopType,
                                  const uint8_t ownerId,
                                  const uint8_t localPlayerId) {
    bool isAlly = (ownerId == localPlayerId);
    if (isAlly)
        return {60, 180, 100};
    return {200, 80, 80};
}
