#include "Lobby.hpp"
#include <algorithm>

void Lobby::addToQueue(uint8_t clientId) {
    auto it = std::find(m_queue.begin(), m_queue.end(), clientId);
    if (it == m_queue.end())
        m_queue.push_back(clientId);
}

void Lobby::removeFromQueue(uint8_t clientId) {
    m_queue.erase(
        std::remove(m_queue.begin(), m_queue.end(), clientId),
        m_queue.end()
    );
}

bool Lobby::tryMatchmake(uint8_t &outClient0, uint8_t &outClient1) {
    if (m_queue.size() < 2) return false;
    outClient0 = m_queue[0];
    outClient1 = m_queue[1];
    m_queue.erase(m_queue.begin(), m_queue.begin() + 2);
    return true;
}
