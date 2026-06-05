#include "GameState.hpp"

#include <algorithm>

GameState::GameState()
    : m_tick(0)
      , m_nextEntityId(1) {
}

void GameState::spawnTroop(TroopType troopType,
                           float x,
                           float y,
                           uint8_t ownerId) {
    uint16_t hp = getBaseHp(troopType);
    uint16_t entityId = getNextEntityId();
    m_entities.push_back(
        std::make_unique<Entity>(entityId, troopType, x, y, hp, ownerId)
    );
}

void GameState::update() {
    m_entities.erase(
        std::remove_if(m_entities.begin(), m_entities.end(),
                       [](const std::unique_ptr<Entity> &entity) {
                           return !entity->isAlive();
                       }),
        m_entities.end()
    );
    ++m_tick;
}

uint16_t GameState::getNextEntityId() {
    return m_nextEntityId++;
}

uint16_t GameState::getBaseHp(const TroopType troopType) {
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
