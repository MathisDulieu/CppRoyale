#include "GameState.hpp"
#include "Constants.hpp"
#include <algorithm>
#include <cmath>
#include <limits>

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
    resolveCombat();
    moveEntities();
    removeDeadEntities();
    ++m_tick;
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

            float distance = distanceBetween(*attacker, *target);
            if (distance < closestDistance) {
                closestDistance = distance;
                closestEnemy = target.get();
            }
        }

        if (closestEnemy == nullptr) continue;
        if (closestDistance > attacker->getAttackRange()) continue;

        attacker->setInCombat(true);

        if (attacker->getCooldownTimer() <= 0.f) {
            closestEnemy->takeDamage(attacker->getAttackDamage());
            attacker->resetCooldown();
        }
    }
}

void GameState::moveEntities() {
    for (auto &entity: m_entities) {
        if (entity->isInCombat()) continue;

        float targetY = (entity->getOwnerId() == 0)
                            ? ARENA_HEIGHT
                            : 0.f;

        float directionY = targetY - entity->getY();
        float length = std::abs(directionY);

        if (length > 0.f)
            directionY /= length;

        float distancePerTick = entity->getSpeed() * TICK_DURATION;
        entity->move(0.f, directionY * distancePerTick);
    }
}

void GameState::removeDeadEntities() {
    m_entities.erase(
        std::remove_if(m_entities.begin(), m_entities.end(),
                       [](const std::unique_ptr<Entity> &entity) {
                           if (!entity->isAlive()) return true;
                           if (entity->getY() < 0.f) return true;
                           if (entity->getY() > ARENA_HEIGHT) return true;
                           return false;
                       }),
        m_entities.end()
    );
}

float GameState::distanceBetween(const Entity &first,
                                 const Entity &second) const {
    float deltaX = first.getX() - second.getX();
    float deltaY = first.getY() - second.getY();
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
