#include "Entity.hpp"
#include <cmath>

Entity::Entity(uint16_t entityId,
               TroopType troopType,
               float x,
               float y,
               uint16_t maxHp,
               uint8_t ownerId)
    : m_entityId(entityId)
      , m_troopType(troopType)
      , m_x(x)
      , m_y(y)
      , m_hp(maxHp)
      , m_ownerId(ownerId)
      , m_speed(getBaseSpeed(troopType))
      , m_attackRange(getBaseAttackRange(troopType))
      , m_attackDamage(getBaseAttackDamage(troopType))
      , m_attackCooldown(getBaseAttackCooldown(troopType))
      , m_cooldownTimer(0.f)
      , m_inCombat(false) {
}

void Entity::setPosition(float x, float y) {
    m_x = x;
    m_y = y;
}

void Entity::move(float deltaX, float deltaY) {
    m_x += deltaX;
    m_y += deltaY;
}

void Entity::takeDamage(uint16_t damage) {
    if (damage >= m_hp)
        m_hp = 0;
    else
        m_hp -= damage;
}

void Entity::setInCombat(bool inCombat) {
    m_inCombat = inCombat;
}

void Entity::reduceCooldown(float delta) {
    if (m_cooldownTimer > 0.f)
        m_cooldownTimer -= delta;
    if (m_cooldownTimer < 0.f)
        m_cooldownTimer = 0.f;
}

void Entity::resetCooldown() {
    m_cooldownTimer = m_attackCooldown;
}

float Entity::getBaseSpeed(TroopType troopType) const {
    switch (troopType) {
        case TroopType::Goblin: return 120.f;
        case TroopType::Giant: return 50.f;
        case TroopType::Archer: return 90.f;
        case TroopType::Knight: return 80.f;
        case TroopType::Bomber: return 60.f;
        case TroopType::Dragon: return 110.f;
        case TroopType::Golem: return 30.f;
        case TroopType::Wizard: return 70.f;
        default: return 60.f;
    }
}

float Entity::getBaseAttackRange(TroopType troopType) const {
    switch (troopType) {
        case TroopType::Goblin: return 40.f;
        case TroopType::Giant: return 50.f;
        case TroopType::Archer: return 120.f;
        case TroopType::Knight: return 45.f;
        case TroopType::Bomber: return 80.f;
        case TroopType::Dragon: return 100.f;
        case TroopType::Golem: return 55.f;
        case TroopType::Wizard: return 110.f;
        default: return 50.f;
    }
}

uint16_t Entity::getBaseAttackDamage(TroopType troopType) const {
    switch (troopType) {
        case TroopType::Goblin: return 40;
        case TroopType::Giant: return 30;
        case TroopType::Archer: return 35;
        case TroopType::Knight: return 55;
        case TroopType::Bomber: return 90;
        case TroopType::Dragon: return 60;
        case TroopType::Golem: return 25;
        case TroopType::Wizard: return 80;
        default: return 30;
    }
}

float Entity::getBaseAttackCooldown(TroopType troopType) const {
    switch (troopType) {
        case TroopType::Goblin: return 0.8f;
        case TroopType::Giant: return 1.5f;
        case TroopType::Archer: return 1.0f;
        case TroopType::Knight: return 1.1f;
        case TroopType::Bomber: return 2.0f;
        case TroopType::Dragon: return 1.2f;
        case TroopType::Golem: return 2.5f;
        case TroopType::Wizard: return 1.8f;
        default: return 1.0f;
    }
}
