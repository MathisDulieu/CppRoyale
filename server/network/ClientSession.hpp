#pragma once
#include <SFML/Network.hpp>
#include <string>
#include <memory>
#include <cstdint>

enum class SessionState {
    Idle,
    Searching,
    ChallengeSent,
    ChallengeReceived,
    InGame,
    GameOver,
    Spectating
};

struct ClientSession {
    std::unique_ptr<sf::TcpSocket> socket;
    uint8_t clientId;
    std::string name;
    SessionState state;
    uint8_t gamePlayerId;
    uint8_t challengePartnerId;
    bool disconnected;
    bool hasSetName;

    ClientSession(std::unique_ptr<sf::TcpSocket> sock, uint8_t id)
        : socket(std::move(sock))
          , clientId(id)
          , name("")
          , state(SessionState::Idle)
          , gamePlayerId(255)
          , challengePartnerId(255)
          , disconnected(false)
          , hasSetName(false) {
    }
};
