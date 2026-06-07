#pragma once
#include <vector>
#include <array>
#include <memory>
#include "../Tower.hpp"
#include "../../../shared/Constants.hpp"

enum class GameResult {
    Ongoing,
    Player0Wins,
    Player1Wins,
    Draw
};

enum class GamePhase {
    Normal,
    Overtime,
    Ended
};

class WinCondition {
public:
    WinCondition();

    void update(std::vector<Tower> &towers, float tickDuration);

    GameResult getResult() const { return m_result; }
    GamePhase getPhase() const { return m_phase; }
    float getRemainingTime() const { return m_remainingTime; }
    bool isOvertime() const { return m_phase == GamePhase::Overtime; }

private:
    void checkNexusDestroyed(std::vector<Tower> &towers);

    void checkOvertimeCondition(std::vector<Tower> &towers);

    void resolveTimerEnd(std::vector<Tower> &towers);

    int countDestroyedTowers(const std::vector<Tower> &towers, uint8_t ownerId) const;

    uint16_t getLowestTowerHp(const std::vector<Tower> &towers, uint8_t ownerId) const;

    GameResult m_result;
    GamePhase m_phase;
    float m_remainingTime;
};
