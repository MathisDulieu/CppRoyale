#pragma once
#include <cstdint>
#include "TroopType.hpp"

class Entity {
public:
    Entity(uint16_t entityId,
           TroopType troopType,
           float x,
           float y,
           uint16_t maxHp,
           uint8_t ownerId);

    uint16_t getEntityId() const { return m_entityId; }
    TroopType getTroopType() const { return m_troopType; }
    float getX() const { return m_x; }
    float getY() const { return m_y; }
    uint16_t getHp() const { return m_hp; }
    uint8_t getOwnerId() const { return m_ownerId; }
    float getSpeed() const { return m_speed; }
    float getAttackRange() const { return m_attackRange; }
    uint16_t getAttackDamage() const { return m_attackDamage; }
    float getAttackCooldown() const { return m_attackCooldown; }
    float getCooldownTimer() const { return m_cooldownTimer; }
    bool isAlive() const { return m_hp > 0; }
    bool isInCombat() const { return m_inCombat; }

    void setPosition(float x, float y);

    void move(float deltaX, float deltaY);

    void takeDamage(uint16_t damage);

    void setInCombat(bool inCombat);

    void reduceCooldown(float delta);

    void resetCooldown();

private:
    float getBaseSpeed(TroopType troopType) const;

    float getBaseAttackRange(TroopType troopType) const;

    uint16_t getBaseAttackDamage(TroopType troopType) const;

    float getBaseAttackCooldown(TroopType troopType) const;

    uint16_t m_entityId;
    TroopType m_troopType;
    float m_x;
    float m_y;
    uint16_t m_hp;
    uint8_t m_ownerId;
    float m_speed;
    float m_attackRange;
    uint16_t m_attackDamage;
    float m_attackCooldown;
    float m_cooldownTimer;
    bool m_inCombat;
};
