#include "Renderer.hpp"
#include "Constants.hpp"
#include <cmath>

Renderer::Renderer()
    : m_window(sf::VideoMode({800, 680}), "CppRoyale")
      , m_fontLoaded(false) {
    m_window.setFramerateLimit(60);
    if (m_font.openFromFile("C:/Windows/Fonts/arial.ttf"))
        m_fontLoaded = true;
}

bool Renderer::isOpen() const { return m_window.isOpen(); }

std::optional<sf::Event> Renderer::pollEvent() {
    return m_window.pollEvent();
}

void Renderer::render(const std::vector<EntitySnapshot> &entities,
                      const std::vector<TowerSnapshot> &towers,
                      bool gameStarted,
                      bool gameOver,
                      uint8_t localPlayerId,
                      uint8_t winnerId,
                      TroopType selectedTroop) {
    m_window.clear(sf::Color(20, 20, 30));

    if (!gameStarted) {
        drawWaitingScreen();
        m_window.display();
        return;
    }

    drawArena();
    drawTowers(towers, localPlayerId);
    drawEntities(entities, localPlayerId);

    if (!gameOver)
        drawTroopPanel(selectedTroop);

    if (gameOver)
        drawGameOverScreen(winnerId == localPlayerId);

    m_window.display();
}

void Renderer::drawArena() {
    sf::RectangleShape midline(sf::Vector2f(800.f, 2.f));
    midline.setPosition({0.f, 300.f});
    midline.setFillColor(sf::Color(80, 80, 80));
    m_window.draw(midline);
}

void Renderer::drawTowers(const std::vector<TowerSnapshot> &towers,
                          uint8_t localPlayerId) {
    for (const auto &snapshot: towers) {
        float renderY = getRenderY(snapshot.y, localPlayerId);

        float size = snapshot.isNexus ? 50.f : 36.f;

        sf::RectangleShape towerShape(sf::Vector2f(size, size));
        towerShape.setOrigin({size / 2.f, size / 2.f});
        towerShape.setPosition({snapshot.x, renderY});
        towerShape.setFillColor(getTowerColor(snapshot.isNexus,
                                              snapshot.ownerId,
                                              localPlayerId));
        towerShape.setOutlineThickness(2.f);
        towerShape.setOutlineColor(sf::Color(200, 200, 200));
        m_window.draw(towerShape);

        drawTowerHpBar(snapshot, renderY);
    }
}

void Renderer::drawTowerHpBar(const TowerSnapshot &snapshot, float renderY) {
    uint16_t maxHp = snapshot.isNexus ? 2000 : 1200;
    float hpRatio = static_cast<float>(snapshot.hp) / static_cast<float>(maxHp);
    float barWidth = snapshot.isNexus ? 54.f : 40.f;
    float offsetY = snapshot.isNexus ? 34.f : 27.f;

    sf::RectangleShape background(sf::Vector2f(barWidth, 5.f));
    background.setOrigin({barWidth / 2.f, 0.f});
    background.setPosition({snapshot.x, renderY - offsetY});
    background.setFillColor(sf::Color(80, 20, 20));
    m_window.draw(background);

    sf::RectangleShape fill(sf::Vector2f(barWidth * hpRatio, 5.f));
    fill.setOrigin({barWidth / 2.f, 0.f});
    fill.setPosition({snapshot.x, renderY - offsetY});
    fill.setFillColor(sf::Color(60, 200, 80));
    m_window.draw(fill);
}

void Renderer::drawEntities(const std::vector<EntitySnapshot> &entities,
                            uint8_t localPlayerId) {
    for (const auto &snapshot: entities) {
        float renderY = getRenderY(snapshot.y, localPlayerId);

        sf::RectangleShape troopShape(sf::Vector2f(24.f, 24.f));
        troopShape.setOrigin({12.f, 12.f});
        troopShape.setPosition({snapshot.x, renderY});
        troopShape.setFillColor(getTroopColor(snapshot.troopType,
                                              snapshot.ownerId,
                                              localPlayerId));
        m_window.draw(troopShape);
        drawHpBar(snapshot, renderY);
    }
}

void Renderer::drawHpBar(const EntitySnapshot &snapshot, float renderY) {
    uint16_t maxHp = getTroopMaxHp(snapshot.troopType);
    float hpRatio = static_cast<float>(snapshot.hp) / static_cast<float>(maxHp);
    float barWidth = 30.f;

    sf::RectangleShape background(sf::Vector2f(barWidth, 4.f));
    background.setOrigin({barWidth / 2.f, 0.f});
    background.setPosition({snapshot.x, renderY - 18.f});
    background.setFillColor(sf::Color(80, 20, 20));
    m_window.draw(background);

    sf::RectangleShape fill(sf::Vector2f(barWidth * hpRatio, 4.f));
    fill.setOrigin({barWidth / 2.f, 0.f});
    fill.setPosition({snapshot.x, renderY - 18.f});
    fill.setFillColor(sf::Color(60, 200, 80));
    m_window.draw(fill);
}

void Renderer::drawTroopPanel(TroopType selectedTroop) {
    sf::RectangleShape panel(sf::Vector2f(800.f, UI_PANEL_HEIGHT));
    panel.setPosition({0.f, 600.f});
    panel.setFillColor(sf::Color(30, 30, 45));
    m_window.draw(panel);

    for (int index = 0; index < TROOP_COUNT; ++index) {
        TroopType troopType = static_cast<TroopType>(index);
        sf::FloatRect bounds = getTroopCardBounds(index);
        bool isSelected = (troopType == selectedTroop);

        sf::RectangleShape card(sf::Vector2f(bounds.size.x, bounds.size.y));
        card.setPosition({bounds.position.x, bounds.position.y});
        card.setFillColor(getTroopCardColor(troopType));
        card.setOutlineThickness(isSelected ? 3.f : 1.f);
        card.setOutlineColor(isSelected
                                 ? sf::Color(255, 220, 50)
                                 : sf::Color(60, 60, 80));
        m_window.draw(card);

        if (m_fontLoaded) {
            sf::Text label(m_font, getTroopName(troopType), 9);
            label.setFillColor(sf::Color::White);
            label.setPosition({
                bounds.position.x + 4.f,
                bounds.position.y + 42.f
            });
            m_window.draw(label);
        }
    }
}

void Renderer::drawWaitingScreen() {
    sf::RectangleShape waitingBox(sf::Vector2f(300.f, 80.f));
    waitingBox.setPosition({250.f, 260.f});
    waitingBox.setFillColor(sf::Color(200, 140, 30));
    m_window.draw(waitingBox);
}

void Renderer::drawGameOverScreen(bool localPlayerWon) {
    sf::RectangleShape overlay(sf::Vector2f(800.f, 680.f));
    overlay.setFillColor(sf::Color(0, 0, 0, 160));
    m_window.draw(overlay);

    sf::RectangleShape resultBox(sf::Vector2f(400.f, 120.f));
    resultBox.setPosition({200.f, 240.f});
    resultBox.setFillColor(localPlayerWon
                               ? sf::Color(30, 120, 50)
                               : sf::Color(120, 30, 30));
    m_window.draw(resultBox);

    if (m_fontLoaded) {
        sf::Text resultText(m_font,
                            localPlayerWon ? "Victory!" : "Defeat",
                            48);
        resultText.setFillColor(sf::Color::White);
        resultText.setPosition({localPlayerWon ? 270.f : 300.f, 270.f});
        m_window.draw(resultText);
    }
}

float Renderer::getRenderY(float serverY, uint8_t localPlayerId) const {
    return (localPlayerId == 0)
               ? (ARENA_HEIGHT - 1.f - serverY)
               : serverY;
}

sf::Color Renderer::getTroopColor([[maybe_unused]] TroopType troopType,
                                  uint8_t ownerId,
                                  uint8_t localPlayerId) const {
    return (ownerId == localPlayerId)
               ? sf::Color(60, 180, 100)
               : sf::Color(200, 80, 80);
}

sf::Color Renderer::getTowerColor(bool isNexus,
                                  uint8_t ownerId,
                                  uint8_t localPlayerId) const {
    bool isAlly = (ownerId == localPlayerId);
    if (isNexus)
        return isAlly ? sf::Color(50, 100, 200) : sf::Color(200, 50, 50);
    return isAlly ? sf::Color(40, 140, 180) : sf::Color(180, 80, 40);
}

sf::Color Renderer::getTroopCardColor(TroopType troopType) const {
    switch (troopType) {
        case TroopType::Goblin: return sf::Color(60, 140, 60);
        case TroopType::Giant: return sf::Color(100, 80, 160);
        case TroopType::Archer: return sf::Color(160, 120, 40);
        case TroopType::Knight: return sf::Color(80, 80, 160);
        case TroopType::Bomber: return sf::Color(160, 80, 40);
        case TroopType::Dragon: return sf::Color(160, 40, 40);
        case TroopType::Golem: return sf::Color(80, 100, 80);
        case TroopType::Wizard: return sf::Color(100, 40, 160);
        default: return sf::Color(60, 60, 60);
    }
}

std::string Renderer::getTroopName(TroopType troopType) const {
    switch (troopType) {
        case TroopType::Goblin: return "Goblin";
        case TroopType::Giant: return "Giant";
        case TroopType::Archer: return "Archer";
        case TroopType::Knight: return "Knight";
        case TroopType::Bomber: return "Bomber";
        case TroopType::Dragon: return "Dragon";
        case TroopType::Golem: return "Golem";
        case TroopType::Wizard: return "Wizard";
        default: return "Unknown";
    }
}

sf::FloatRect Renderer::getTroopCardBounds(int cardIndex) const {
    float totalWidth = TROOP_COUNT * TROOP_CARD_SIZE
                       + (TROOP_COUNT - 1) * TROOP_CARD_GAP;
    float startX = (800.f - totalWidth) / 2.f;
    float x = startX + cardIndex * (TROOP_CARD_SIZE + TROOP_CARD_GAP);
    float y = 600.f + (UI_PANEL_HEIGHT - TROOP_CARD_SIZE) / 2.f;

    return sf::FloatRect(sf::Vector2f(x, y),
                         sf::Vector2f(TROOP_CARD_SIZE, TROOP_CARD_SIZE));
}

uint16_t Renderer::getTroopMaxHp(TroopType troopType) const {
    switch (troopType) {
        case TroopType::Goblin: return 150;
        case TroopType::Giant: return 800;
        case TroopType::Archer: return 125;
        case TroopType::Knight: return 400;
        case TroopType::Bomber: return 200;
        case TroopType::Dragon: return 350;
        case TroopType::Golem: return 1200;
        case TroopType::Wizard: return 300;
        default: return 100;
    }
}

bool Renderer::isTroopCardClicked(const sf::Vector2i &mousePosition,
                                  TroopType &outTroopType) const {
    for (int index = 0; index < TROOP_COUNT; ++index) {
        sf::FloatRect bounds = getTroopCardBounds(index);
        sf::Vector2f mouse(static_cast<float>(mousePosition.x),
                           static_cast<float>(mousePosition.y));
        if (bounds.contains(mouse)) {
            outTroopType = static_cast<TroopType>(index);
            return true;
        }
    }
    return false;
}

bool Renderer::isArenaClicked(const sf::Vector2i &mousePosition) const {
    return mousePosition.y < 600;
}
