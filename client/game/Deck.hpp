#pragma once
#include <array>
#include <vector>
#include "../../shared/TroopType.hpp"

constexpr int DECK_SIZE = 8;
constexpr int HAND_SIZE = 4;

class Deck {
public:
    Deck();

    void playCard(int handIndex);

    TroopType getCard(int handIndex) const { return m_hand[handIndex]; }
    TroopType getNextCard() const { return m_reserve.front(); }
    int getHandSize() const { return HAND_SIZE; }

private:
    std::array<TroopType, HAND_SIZE> m_hand;
    std::vector<TroopType> m_reserve;
};
