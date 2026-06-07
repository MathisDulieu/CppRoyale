#include "Tower.hpp"

Tower::Tower(uint8_t towerId,
             float x,
             float y,
             uint16_t maxHp,
             uint8_t ownerId,
             bool isNexus)
    : m_towerId(towerId)
      , m_x(x)
      , m_y(y)
      , m_hp(maxHp)
      , m_ownerId(ownerId)
      , m_isNexus(isNexus)
      , m_attackRange(isNexus ? 120.f : 100.f)
      , m_attackDamage(isNexus ? 60 : 45)
      , m_attackCooldown(isNexus ? 1.0f : 1.2f)
      , m_cooldownTimer(0.f) {
}

void Tower::takeDamage(uint16_t damage) {
    if (damage >= m_hp) m_hp = 0;
    else m_hp -= damage;
}

void Tower::reduceCooldown(float delta) {
    if (m_cooldownTimer > 0.f) m_cooldownTimer -= delta;
    if (m_cooldownTimer < 0.f) m_cooldownTimer = 0.f;
}

void Tower::resetCooldown() { m_cooldownTimer = m_attackCooldown; }
