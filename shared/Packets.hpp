#pragma once
#include <cstdint>

constexpr uint8_t PKT_PING = 0x01;
constexpr uint8_t PKT_PONG = 0x02;
constexpr uint8_t PKT_START = 0x03;

struct PacketPing {
    uint8_t type = PKT_PING;
};

struct PacketPong {
    uint8_t type = PKT_PONG;
};

struct PacketStart {
    uint8_t type = PKT_START;
    uint8_t playerId{};
};
