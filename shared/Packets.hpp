#pragma once
#include <cstdint>
#include <string>
#include "TroopType.hpp"

constexpr uint8_t PKT_PING = 0x01;
constexpr uint8_t PKT_START = 0x03;
constexpr uint8_t PKT_DEPLOY = 0x04;
constexpr uint8_t PKT_GAME_STATE = 0x05;
constexpr uint8_t PKT_GAME_OVER = 0x06;
constexpr uint8_t PKT_SET_NAME = 0x07;
constexpr uint8_t PKT_FIND_MATCH = 0x08;
constexpr uint8_t PKT_CANCEL_MATCH = 0x09;
constexpr uint8_t PKT_PLAYER_LIST = 0x0A;
constexpr uint8_t PKT_CHALLENGE = 0x0B;
constexpr uint8_t PKT_CHALLENGE_RESP = 0x0C;
constexpr uint8_t PKT_MATCH_FOUND = 0x0D;
constexpr uint8_t PKT_CANCEL_CHALLENGE = 0x0E;
constexpr uint8_t PKT_RETURN_TO_LOBBY = 0x0F;
constexpr uint8_t PKT_SPECTATE = 0x10;
constexpr uint8_t PKT_SPECTATE_LEAVE = 0x11;
constexpr uint8_t PKT_SPECTATE_COUNT = 0x12;

constexpr uint16_t MAX_ENTITIES = 64;
constexpr uint8_t MAX_NAME_LENGTH = 32;

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

struct PlayerInfo {
    uint8_t clientId;
    std::string name;
    bool isSearching;
    bool hasPendingChallenge;
    bool isInGame;
    bool isSpectating;
    uint8_t gamePlayer0Id;
    uint8_t gamePlayer1Id;
};
