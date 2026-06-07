#pragma once
#include <vector>
#include <memory>
#include <array>
#include <cstdint>
#include "Entity.hpp"
#include "Tower.hpp"
#include "systems/CombatSystem.hpp"
#include "systems/MovementSystem.hpp"
#include "systems/WinCondition.hpp"
#include "../../shared/TroopType.hpp"
#include "../../shared/Constants.hpp"

class GameState {
public:
    GameState();

    bool spawnTroop(TroopType troopType, float x, float y, uint8_t ownerId);

    void update();

    const std::vector<std::unique_ptr<Entity> > &getEntities() const { return m_entities; }
    const std::vector<Tower> &getTowers() const { return m_towers; }
    uint32_t getTick() const { return m_tick; }
    GameResult getResult() const { return m_winCondition.getResult(); }
    float getRemainingTime() const { return m_winCondition.getRemainingTime(); }
    bool isOvertime() const { return m_winCondition.isOvertime(); }

    float getElixir(uint8_t playerId) const;

private:
    void initTowers();

    void regenElixir();

    uint16_t getNextEntityId();

    uint16_t getBaseHp(TroopType troopType) const;

    uint8_t getTroopCost(TroopType troopType) const;

    std::vector<std::unique_ptr<Entity> > m_entities;
    std::vector<Tower> m_towers;
    std::array<float, MAX_PLAYERS> m_elixir;
    uint32_t m_tick;
    uint16_t m_nextEntityId;

    CombatSystem m_combatSystem;
    MovementSystem m_movementSystem;
    WinCondition m_winCondition;
};
