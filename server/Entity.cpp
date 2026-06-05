#include "Entity.hpp"

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
      , m_speed(getBaseSpeed(troopType)) {
}

void Entity::setPosition(float x, float y) {
    m_x = x;
    m_y = y;
}

void Entity::move(float deltaX, float deltaY) {
    m_x += deltaX;
    m_y += deltaY;
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
