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

    [[nodiscard]] uint16_t getEntityId() const { return m_entityId; }
    [[nodiscard]] TroopType getTroopType() const { return m_troopType; }
    [[nodiscard]] float getX() const { return m_x; }
    [[nodiscard]] float getY() const { return m_y; }
    [[nodiscard]] uint16_t getHp() const { return m_hp; }
    [[nodiscard]] uint8_t getOwnerId() const { return m_ownerId; }
    [[nodiscard]] bool isAlive() const { return m_hp > 0; }

    void setPosition(float x, float y);

private:
    uint16_t m_entityId;
    TroopType m_troopType;
    float m_x;
    float m_y;
    uint16_t m_hp;
    uint8_t m_ownerId;
};
