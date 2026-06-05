#pragma once
#include <cstdint>
#include "TroopType.hpp"

constexpr uint8_t PKT_PING = 0x01;
constexpr uint8_t PKT_START = 0x03;
constexpr uint8_t PKT_DEPLOY = 0x04;
constexpr uint8_t PKT_GAME_STATE = 0x05;

constexpr uint16_t MAX_ENTITIES = 64;

struct EntitySnapshot {
    uint16_t entityId;
    TroopType troopType;
    float x;
    float y;
    uint16_t hp;
    uint8_t ownerId;
};

struct PacketDeployTroop {
    uint8_t type = PKT_DEPLOY;
    TroopType troopType;
    float x{};
    float y{};
    uint8_t playerId{};
};

struct PacketGameState {
    uint8_t type = PKT_GAME_STATE;
    uint32_t tick{};
    uint16_t entityCount{};
    EntitySnapshot entities[MAX_ENTITIES]{};
};
