#pragma once
#include <cstdint>

class Tower {
public:
    Tower(uint8_t towerId,
          float x,
          float y,
          uint16_t maxHp,
          uint8_t ownerId,
          bool isNexus);

    uint8_t getTowerId() const { return m_towerId; }
    float getX() const { return m_x; }
    float getY() const { return m_y; }
    uint16_t getHp() const { return m_hp; }
    uint8_t getOwnerId() const { return m_ownerId; }
    bool isNexus() const { return m_isNexus; }
    bool isAlive() const { return m_hp > 0; }
    float getAttackRange() const { return m_attackRange; }
    uint16_t getAttackDamage() const { return m_attackDamage; }
    float getCooldownTimer() const { return m_cooldownTimer; }

    void takeDamage(uint16_t damage);

    void reduceCooldown(float delta);

    void resetCooldown();

private:
    uint8_t m_towerId;
    float m_x;
    float m_y;
    uint16_t m_hp;
    uint8_t m_ownerId;
    bool m_isNexus;
    float m_attackRange;
    uint16_t m_attackDamage;
    float m_attackCooldown;
    float m_cooldownTimer;
};
