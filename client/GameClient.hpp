#pragma once
#include <SFML/Network.hpp>
#include <vector>
#include <string>
#include <cstdint>
#include <array>
#include "Packets.hpp"
#include "TroopType.hpp"
#include "Constants.hpp"

enum class ClientState {
    EnteringName,
    Idle,
    Searching,
    ChallengeSent,
    ChallengeReceived,
    MatchFound,
    Playing,
    GameOver
};

class GameClient {
public:
    bool connect(const std::string &host, unsigned short port);

    void receivePackets();

    void sendName(const std::string &name);

    void sendFindMatch();

    void sendCancelMatch();

    void sendChallenge(uint8_t targetId);

    void sendChallengeResponse(bool accepted);

    void sendCancelChallenge();

    void requestPlayerList();

    void deployTroop(TroopType troopType, float x, float y);

    void sendReturnToLobby();

    void tickMatchFoundTimer(float delta);

    ClientState getState() const { return m_state; }
    uint8_t getPlayerId() const { return m_playerId; }
    uint8_t getWinnerId() const { return m_winnerId; }
    uint8_t getClientId() const { return m_clientId; }
    const std::string &getName() const { return m_name; }
    const std::string &getOpponentName() const { return m_opponentName; }
    const std::string &getChallengerName() const { return m_challengerName; }
    const std::vector<EntitySnapshot> &getEntities() const { return m_entities; }
    const std::vector<TowerSnapshot> &getTowers() const { return m_towers; }
    const std::vector<PlayerInfo> &getPlayerList() const { return m_playerList; }
    const std::string& getNameError() const { return m_nameError; }
    float getElixir() const { return m_elixir[m_playerId]; }
    float getRemainingTime() const { return m_remainingTime; }
    float getMatchFoundTimer() const { return m_matchFoundTimer; }
    bool isOvertime() const { return m_isOvertime; }

    void setName(const std::string &name) { m_name = name; }

private:
    sf::TcpSocket m_socket;
    ClientState m_state = ClientState::EnteringName;
    uint8_t m_clientId = 255;
    uint8_t m_playerId = 0;
    uint8_t m_winnerId = 255;
    std::string m_name;
    std::string m_opponentName;
    std::string m_challengerName;
    std::vector<EntitySnapshot> m_entities;
    std::vector<TowerSnapshot> m_towers;
    std::vector<PlayerInfo> m_playerList;
    std::array<float, MAX_PLAYERS> m_elixir = {5.f, 5.f};
    std::string m_nameError;
    float m_remainingTime = GAME_DURATION;
    float m_matchFoundTimer = 0.f;
    bool m_isOvertime = false;
};
