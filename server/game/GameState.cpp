#include "GameState.hpp"
#include <algorithm>
#include <cmath>

GameState::GameState()
    : m_tick(0)
      , m_nextEntityId(1) {
    m_elixir.fill(5.f);
    initTowers();
}

void GameState::initTowers() {
    m_towers.emplace_back(0, 150.f, 60.f, 1200, 0, false);
    m_towers.emplace_back(1, 650.f, 60.f, 1200, 0, false);
    m_towers.emplace_back(2, 400.f, 60.f, 2000, 0, true);
    m_towers.emplace_back(3, 150.f, 540.f, 1200, 1, false);
    m_towers.emplace_back(4, 650.f, 540.f, 1200, 1, false);
    m_towers.emplace_back(5, 400.f, 540.f, 2000, 1, true);
}

bool GameState::spawnTroop(TroopType troopType,
                           float x,
                           float y,
                           uint8_t ownerId) {
    uint8_t cost = getTroopCost(troopType);
    if (m_elixir[ownerId] < static_cast<float>(cost)) return false;

    m_elixir[ownerId] -= static_cast<float>(cost);
    m_entities.push_back(std::make_unique<Entity>(
        getNextEntityId(), troopType, x, y, getBaseHp(troopType), ownerId
    ));
    return true;
}

void GameState::update() {
    if (m_winCondition.getResult() != GameResult::Ongoing) return;

    regenElixir();
    m_combatSystem.resolveTroopCombat(m_entities, m_towers, TICK_DURATION);
    m_combatSystem.resolveTowerCombat(m_towers, m_entities, TICK_DURATION);
    m_movementSystem.moveEntities(m_entities, m_towers, TICK_DURATION);

    m_entities.erase(
        std::remove_if(m_entities.begin(), m_entities.end(),
                       [](const std::unique_ptr<Entity> &e) {
                           return !e->isAlive()
                                  || e->getY() < 0.f
                                  || e->getY() > ARENA_HEIGHT;
                       }),
        m_entities.end()
    );

    m_winCondition.update(m_towers, TICK_DURATION);
    ++m_tick;
}

void GameState::regenElixir() {
    for (float &elixir: m_elixir) {
        elixir += ELIXIR_REGEN_RATE * TICK_DURATION;
        if (elixir > MAX_ELIXIR) elixir = MAX_ELIXIR;
    }
}

float GameState::getElixir(uint8_t playerId) const {
    if (playerId >= MAX_PLAYERS) return 0.f;
    return m_elixir[playerId];
}

uint16_t GameState::getNextEntityId() { return m_nextEntityId++; }

uint16_t GameState::getBaseHp(TroopType troopType) const {
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

uint8_t GameState::getTroopCost(TroopType troopType) const {
    switch (troopType) {
        case TroopType::Goblin: return 2;
        case TroopType::Giant: return 5;
        case TroopType::Archer: return 3;
        case TroopType::Knight: return 3;
        case TroopType::Bomber: return 4;
        case TroopType::Dragon: return 4;
        case TroopType::Golem: return 8;
        case TroopType::Wizard: return 5;
        default: return 3;
    }
}
