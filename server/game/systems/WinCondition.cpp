#include "WinCondition.hpp"
#include <iostream>
#include <limits>

WinCondition::WinCondition()
    : m_result(GameResult::Ongoing)
      , m_phase(GamePhase::Normal)
      , m_remainingTime(GAME_DURATION) {
}

void WinCondition::update(std::vector<Tower> &towers, float tickDuration) {
    if (m_result != GameResult::Ongoing) return;

    checkNexusDestroyed(towers);
    if (m_result != GameResult::Ongoing) return;

    m_remainingTime -= tickDuration;
    if (m_remainingTime <= 0.f) {
        m_remainingTime = 0.f;
        resolveTimerEnd(towers);
    }

    if (m_phase == GamePhase::Overtime)
        checkOvertimeCondition(towers);
}

void WinCondition::checkNexusDestroyed(std::vector<Tower> &towers) {
    bool player0NexusAlive = false;
    bool player1NexusAlive = false;

    for (const auto &tower: towers) {
        if (!tower.isNexus() || !tower.isAlive()) continue;
        if (tower.getOwnerId() == 0) player0NexusAlive = true;
        if (tower.getOwnerId() == 1) player1NexusAlive = true;
    }

    if (!player0NexusAlive) {
        m_result = GameResult::Player1Wins;
        return;
    }
    if (!player1NexusAlive) {
        m_result = GameResult::Player0Wins;
        return;
    }
}

void WinCondition::checkOvertimeCondition(std::vector<Tower> &towers) {
    int destroyed0 = countDestroyedTowers(towers, 0);
    int destroyed1 = countDestroyedTowers(towers, 1);

    if (destroyed0 > 0) {
        m_result = GameResult::Player1Wins;
        return;
    }
    if (destroyed1 > 0) {
        m_result = GameResult::Player0Wins;
        return;
    }
}

void WinCondition::resolveTimerEnd(std::vector<Tower> &towers) {
    if (m_phase == GamePhase::Normal) {
        int destroyed0 = countDestroyedTowers(towers, 1);
        int destroyed1 = countDestroyedTowers(towers, 0);

        std::cout << "[Server] Timer ended — towers destroyed: "
                << "player0=" << destroyed0
                << " player1=" << destroyed1 << "\n";

        if (destroyed0 > destroyed1) {
            m_result = GameResult::Player0Wins;
            m_phase = GamePhase::Ended;
        } else if (destroyed1 > destroyed0) {
            m_result = GameResult::Player1Wins;
            m_phase = GamePhase::Ended;
        } else {
            std::cout << "[Server] Equality — starting overtime\n";
            m_phase = GamePhase::Overtime;
            m_remainingTime = OVERTIME_DURATION;
        }
    } else if (m_phase == GamePhase::Overtime) {
        uint16_t lowestHp0 = getLowestTowerHp(towers, 0);
        uint16_t lowestHp1 = getLowestTowerHp(towers, 1);

        std::cout << "[Server] Overtime ended — lowest tower HP: "
                << "player0=" << lowestHp0
                << " player1=" << lowestHp1 << "\n";

        if (lowestHp0 < lowestHp1) m_result = GameResult::Player1Wins;
        else if (lowestHp1 < lowestHp0) m_result = GameResult::Player0Wins;
        else m_result = GameResult::Draw;

        m_phase = GamePhase::Ended;
    }
}

int WinCondition::countDestroyedTowers(const std::vector<Tower> &towers,
                                       uint8_t ownerId) const {
    int count = 0;
    for (const auto &tower: towers)
        if (tower.getOwnerId() == ownerId && !tower.isAlive())
            ++count;
    return count;
}

uint16_t WinCondition::getLowestTowerHp(const std::vector<Tower> &towers,
                                        uint8_t ownerId) const {
    uint16_t lowest = std::numeric_limits<uint16_t>::max();
    for (const auto &tower: towers) {
        if (tower.getOwnerId() != ownerId) continue;
        if (!tower.isAlive()) return 0;
        if (tower.getHp() < lowest) lowest = tower.getHp();
    }
    return lowest;
}
