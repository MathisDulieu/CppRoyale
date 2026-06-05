#include "Entity.hpp"

Entity::Entity(const uint16_t entityId,
               const TroopType troopType,
               const float x,
               const float y,
               const uint16_t maxHp,
               const uint8_t ownerId)
    : m_entityId(entityId)
      , m_troopType(troopType)
      , m_x(x)
      , m_y(y)
      , m_hp(maxHp)
      , m_ownerId(ownerId) {
}

void Entity::setPosition(const float x, const float y) {
    m_x = x;
    m_y = y;
}
