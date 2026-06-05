#pragma once
#include <vector>
#include <memory>
#include <cstdint>
#include <array>

#include "Constants.hpp"
#include "Entity.hpp"
#include "Tower.hpp"
#include "TroopType.hpp"

enum class GameResult {
    Ongoing,
    Player0Wins,
    Player1Wins
};

class GameState {
public:
    GameState();

    bool spawnTroop(TroopType troopType, float x, float y, uint8_t ownerId);

    void update();

    const std::vector<std::unique_ptr<Entity> > &getEntities() const { return m_entities; }
    const std::vector<Tower> &getTowers() const { return m_towers; }
    uint32_t getTick() const { return m_tick; }
    GameResult getResult() const { return m_result; }

    float getElixir(uint8_t playerId) const;

private:
    void initTowers();

    void regenElixir();

    void moveEntities();

    void resolveCombat();

    void resolveTowerCombat();

    void checkWinCondition();

    Tower *findClosestEnemyTower(float x, float y, uint8_t ownerId);

    float distanceBetween(float x1, float y1, float x2, float y2) const;

    uint16_t getNextEntityId();

    uint16_t getBaseHp(TroopType troopType) const;

    uint8_t getTroopCost(TroopType troopType) const;

    std::vector<std::unique_ptr<Entity> > m_entities;
    std::vector<Tower> m_towers;
    std::array<float, MAX_PLAYERS> m_elixir;
    uint32_t m_tick;
    uint16_t m_nextEntityId;
    GameResult m_result;
};
