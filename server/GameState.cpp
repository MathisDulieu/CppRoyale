#include "GameState.hpp"
#include "Constants.hpp"
#include <algorithm>
#include <cmath>
#include <limits>
#include <iostream>

GameState::GameState()
    : m_tick(0)
      , m_nextEntityId(1)
      , m_result(GameResult::Ongoing)
      , m_phase(GamePhase::Normal)
      , m_remainingTime(GAME_DURATION) {
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
    if (m_elixir[ownerId] < static_cast<float>(cost))
        return false;

    m_elixir[ownerId] -= static_cast<float>(cost);
    m_entities.push_back(std::make_unique<Entity>(
        getNextEntityId(), troopType, x, y, getBaseHp(troopType), ownerId
    ));
    return true;
}

void GameState::update() {
    if (m_result != GameResult::Ongoing) return;

    updateTimer();
    if (m_result != GameResult::Ongoing) return;

    regenElixir();
    resolveCombat();
    resolveTowerCombat();
    moveEntities();

    m_entities.erase(
        std::remove_if(m_entities.begin(), m_entities.end(),
                       [](const std::unique_ptr<Entity> &entity) {
                           return !entity->isAlive()
                                  || entity->getY() < 0.f
                                  || entity->getY() > ARENA_HEIGHT;
                       }),
        m_entities.end()
    );

    checkWinCondition();
    ++m_tick;
}

void GameState::updateTimer() {
    m_remainingTime -= TICK_DURATION;

    if (m_remainingTime <= 0.f) {
        m_remainingTime = 0.f;
        resolveTimerEnd();
    }
}

void GameState::resolveTimerEnd() {
    if (m_phase == GamePhase::Normal) {
        int destroyed0 = countDestroyedTowers(1);
        int destroyed1 = countDestroyedTowers(0);

        std::cout << "[Server] Timer ended — towers destroyed: "
                << "player0=" << destroyed0
                << " player1=" << destroyed1 << "\n";

        if (destroyed0 > destroyed1) {
            m_result = GameResult::Player0Wins;
            m_phase = GamePhase::Ended;
        } else if (destroyed1 > destroyed0) {
            m_result = GameResult::Player1Wins;
            m_phase = GamePhase::Ended;
        } else {
            std::cout << "[Server] Equality — starting overtime\n";
            m_phase = GamePhase::Overtime;
            m_remainingTime = OVERTIME_DURATION;
        }
    } else if (m_phase == GamePhase::Overtime) {
        uint16_t lowestHp0 = getLowestTowerHp(0);
        uint16_t lowestHp1 = getLowestTowerHp(1);

        std::cout << "[Server] Overtime ended — lowest tower HP: "
                << "player0=" << lowestHp0
                << " player1=" << lowestHp1 << "\n";

        if (lowestHp0 < lowestHp1)
            m_result = GameResult::Player1Wins;
        else if (lowestHp1 < lowestHp0)
            m_result = GameResult::Player0Wins;
        else
            m_result = GameResult::Draw;

        m_phase = GamePhase::Ended;
    }
}

uint16_t GameState::getLowestTowerHp(uint8_t ownerId) const {
    uint16_t lowest = std::numeric_limits<uint16_t>::max();
    for (const auto& tower : m_towers) {
        if (tower.getOwnerId() != ownerId) continue;
        if (!tower.isAlive()) return 0;
        if (tower.getHp() < lowest)
            lowest = tower.getHp();
    }
    return lowest;
}

int GameState::countDestroyedTowers(uint8_t ownerId) const {
    int destroyed = 0;
    for (const auto &tower: m_towers) {
        if (tower.getOwnerId() == ownerId && !tower.isAlive())
            ++destroyed;
    }
    return destroyed;
}

void GameState::regenElixir() {
    for (float &elixir: m_elixir) {
        elixir += ELIXIR_REGEN_RATE * TICK_DURATION;
        if (elixir > MAX_ELIXIR)
            elixir = MAX_ELIXIR;
    }
}

float GameState::getElixir(uint8_t playerId) const {
    if (playerId >= MAX_PLAYERS) return 0.f;
    return m_elixir[playerId];
}

void GameState::resolveCombat() {
    for (auto &attacker: m_entities) {
        attacker->setInCombat(false);
        attacker->reduceCooldown(TICK_DURATION);
    }

    for (auto &attacker: m_entities) {
        if (!attacker->isAlive()) continue;

        Entity *closestEnemy = nullptr;
        float closestDistance = std::numeric_limits<float>::max();

        for (auto &target: m_entities) {
            if (!target->isAlive()) continue;
            if (target->getOwnerId() == attacker->getOwnerId()) continue;

            float distance = distanceBetween(attacker->getX(), attacker->getY(),
                                             target->getX(), target->getY());
            if (distance < closestDistance) {
                closestDistance = distance;
                closestEnemy = target.get();
            }
        }

        if (closestEnemy && closestDistance <= attacker->getAttackRange()) {
            attacker->setInCombat(true);
            if (attacker->getCooldownTimer() <= 0.f) {
                closestEnemy->takeDamage(attacker->getAttackDamage());
                attacker->resetCooldown();
            }
            continue;
        }

        Tower *closestTower = findClosestEnemyTower(attacker->getX(),
                                                    attacker->getY(),
                                                    attacker->getOwnerId());
        if (closestTower) {
            float towerDistance = distanceBetween(attacker->getX(), attacker->getY(),
                                                  closestTower->getX(), closestTower->getY());
            if (towerDistance <= attacker->getAttackRange()) {
                attacker->setInCombat(true);
                if (attacker->getCooldownTimer() <= 0.f) {
                    closestTower->takeDamage(attacker->getAttackDamage());
                    attacker->resetCooldown();
                }
            }
        }
    }
}

void GameState::resolveTowerCombat() {
    for (auto &tower: m_towers) {
        if (!tower.isAlive()) continue;
        tower.reduceCooldown(TICK_DURATION);

        Entity *closestEnemy = nullptr;
        float closestDistance = std::numeric_limits<float>::max();

        for (auto &entity: m_entities) {
            if (!entity->isAlive()) continue;
            if (entity->getOwnerId() == tower.getOwnerId()) continue;

            float distance = distanceBetween(tower.getX(), tower.getY(),
                                             entity->getX(), entity->getY());
            if (distance < closestDistance) {
                closestDistance = distance;
                closestEnemy = entity.get();
            }
        }

        if (closestEnemy && closestDistance <= tower.getAttackRange()) {
            if (tower.getCooldownTimer() <= 0.f) {
                closestEnemy->takeDamage(tower.getAttackDamage());
                tower.resetCooldown();
            }
        }
    }
}

void GameState::moveEntities() {
    for (auto &entity: m_entities) {
        if (entity->isInCombat()) continue;

        Tower *targetTower = findClosestEnemyTower(entity->getX(),
                                                   entity->getY(),
                                                   entity->getOwnerId());
        if (!targetTower) continue;

        float deltaX = targetTower->getX() - entity->getX();
        float deltaY = targetTower->getY() - entity->getY();
        float length = std::sqrt(deltaX * deltaX + deltaY * deltaY);

        if (length <= 0.f) continue;

        float distancePerTick = entity->getSpeed() * TICK_DURATION;
        entity->move((deltaX / length) * distancePerTick,
                     (deltaY / length) * distancePerTick);
    }
}

void GameState::checkWinCondition() {
    bool player0NexusAlive = false;
    bool player1NexusAlive = false;

    for (const auto &tower: m_towers) {
        if (!tower.isNexus() || !tower.isAlive()) continue;
        if (tower.getOwnerId() == 0) player0NexusAlive = true;
        if (tower.getOwnerId() == 1) player1NexusAlive = true;
    }

    if (!player0NexusAlive) {
        m_result = GameResult::Player1Wins;
        return;
    }
    if (!player1NexusAlive) {
        m_result = GameResult::Player0Wins;
        return;
    }

    if (m_phase == GamePhase::Overtime) {
        int destroyed0 = countDestroyedTowers(0);
        int destroyed1 = countDestroyedTowers(1);

        if (destroyed0 > 0) {
            m_result = GameResult::Player1Wins;
            return;
        }
        if (destroyed1 > 0) {
            m_result = GameResult::Player0Wins;
            return;
        }
    }
}

Tower *GameState::findClosestEnemyTower(float x, float y, uint8_t ownerId) {
    Tower *closest = nullptr;
    float closestDistance = std::numeric_limits<float>::max();

    for (auto &tower: m_towers) {
        if (!tower.isAlive()) continue;
        if (tower.getOwnerId() == ownerId) continue;

        float distance = distanceBetween(x, y, tower.getX(), tower.getY());
        if (distance < closestDistance) {
            closestDistance = distance;
            closest = &tower;
        }
    }
    return closest;
}

float GameState::distanceBetween(float x1, float y1,
                                 float x2, float y2) const {
    float deltaX = x1 - x2;
    float deltaY = y1 - y2;
    return std::sqrt(deltaX * deltaX + deltaY * deltaY);
}

uint16_t GameState::getNextEntityId() {
    return m_nextEntityId++;
}

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
