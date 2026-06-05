#pragma once
#include <cstdint>

constexpr uint8_t PKT_PING  = 0x01;
constexpr uint8_t PKT_PONG  = 0x02;

struct PacketPing {
    uint8_t type = PKT_PING;
};

struct PacketPong {
    uint8_t type = PKT_PONG;
};