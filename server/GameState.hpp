#pragma once
#include <vector>
#include <memory>
#include <cstdint>
#include "Entity.hpp"
#include "TroopType.hpp"

class GameState {
public:
    GameState();

    void spawnTroop(TroopType troopType, float x, float y, uint8_t ownerId);

    void update();

    [[nodiscard]] const std::vector<std::unique_ptr<Entity> > &getEntities() const { return m_entities; }
    [[nodiscard]] uint32_t getTick() const { return m_tick; }

private:
    uint16_t getNextEntityId();

    static uint16_t getBaseHp(TroopType troopType);

    std::vector<std::unique_ptr<Entity> > m_entities;
    uint32_t m_tick;
    uint16_t m_nextEntityId;
};
