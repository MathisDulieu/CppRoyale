#include "Deck.hpp"
#include <algorithm>
#include <random>
#include <chrono>

Deck::Deck() {
    std::vector<TroopType> allTroops = {
        TroopType::Goblin,
        TroopType::Giant,
        TroopType::Archer,
        TroopType::Knight,
        TroopType::Bomber,
        TroopType::Dragon,
        TroopType::Golem,
        TroopType::Wizard
    };

    unsigned seed = static_cast<unsigned>(
        std::chrono::steady_clock::now().time_since_epoch().count()
    );
    std::shuffle(allTroops.begin(), allTroops.end(),
                 std::default_random_engine(seed));

    for (int index = 0; index < HAND_SIZE; ++index)
        m_hand[index] = allTroops[index];

    for (int index = HAND_SIZE; index < DECK_SIZE; ++index)
        m_reserve.push_back(allTroops[index]);
}

void Deck::playCard(int handIndex) {
    TroopType playedCard = m_hand[handIndex];
    drawFromReserve();
    m_hand[handIndex] = m_reserve.empty() ? playedCard : m_reserve.front();
    if (!m_reserve.empty())
        m_reserve.erase(m_reserve.begin());
    m_reserve.push_back(playedCard);
}

void Deck::drawFromReserve() {
}
