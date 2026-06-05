#pragma once
#include <cstdint>
#include "TroopType.hpp"

constexpr uint8_t PKT_PING = 0x01;
constexpr uint8_t PKT_START = 0x03;
constexpr uint8_t PKT_DEPLOY = 0x04;
constexpr uint8_t PKT_GAME_STATE = 0x05;
constexpr uint8_t PKT_GAME_OVER = 0x06;

constexpr uint16_t MAX_ENTITIES = 64;

struct EntitySnapshot {
    uint16_t entityId;
    TroopType troopType;
    float x;
    float y;
    uint16_t hp;
    uint8_t ownerId;
};

struct TowerSnapshot {
    uint8_t towerId;
    float x;
    float y;
    uint16_t hp;
    uint8_t ownerId;
    bool isNexus;
};
