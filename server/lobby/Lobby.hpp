#pragma once
#include <vector>
#include <cstdint>

class Lobby {
public:
    void addToQueue(uint8_t clientId);

    void removeFromQueue(uint8_t clientId);

    bool tryMatchmake(uint8_t &outClient0, uint8_t &outClient1);

private:
    std::vector<uint8_t> m_queue;
};
