#pragma once
#include <SFML/Network.hpp>
#include <vector>
#include <string>
#include <cstdint>
#include <array>
#include "../../shared/Packets.hpp"
#include "../../shared/TroopType.hpp"
#include "../../shared/Constants.hpp"

enum class ClientState {
    EnteringName,
    Idle,
    Searching,
    ChallengeSent,
    ChallengeReceived,
    MatchFound,
    Playing,
    GameOver,
    Spectating
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

    void sendSpectate(uint8_t player0Id, uint8_t player1Id);

    void sendSpectateLeave();

    void tickMatchFoundTimer(float delta);

    ClientState getState() const { return m_state; }
    uint8_t getPlayerId() const { return m_playerId; }
    uint8_t getWinnerId() const { return m_winnerId; }
    uint8_t getClientId() const { return m_clientId; }
    uint8_t getSpectatorCount() const { return m_spectatorCount; }
    uint8_t getWatchedPlayerId() const { return m_watchedPlayerId; }
    uint8_t getGamePlayer0Id() const { return m_gamePlayer0Id; }
    uint8_t getGamePlayer1Id() const { return m_gamePlayer1Id; }
    const std::string &getName() const { return m_name; }
    const std::string &getNameError() const { return m_nameError; }
    const std::string &getOpponentName() const { return m_opponentName; }
    const std::string &getChallengerName() const { return m_challengerName; }
    const std::string &getPlayer0Name() const { return m_player0Name; }
    const std::string &getPlayer1Name() const { return m_player1Name; }
    const std::vector<EntitySnapshot> &getEntities() const { return m_entities; }
    const std::vector<TowerSnapshot> &getTowers() const { return m_towers; }
    const std::vector<PlayerInfo> &getPlayerList() const { return m_playerList; }
    float getElixir() const { return m_elixir[m_playerId < MAX_PLAYERS ? m_playerId : 0]; }
    float getRemainingTime() const { return m_remainingTime; }
    float getMatchFoundTimer() const { return m_matchFoundTimer; }
    bool isOvertime() const { return m_isOvertime; }
    bool isSpectating() const { return m_state == ClientState::Spectating; }
    bool wasSpectating() const { return m_wasSpectating; }
    bool isLocalPlayer0() const { return m_isLocalPlayer0; }

    void setName(const std::string &name) { m_name = name; }

private:
    sf::TcpSocket m_socket;
    ClientState m_state = ClientState::EnteringName;
    uint8_t m_clientId = 255;
    uint8_t m_playerId = 0;
    uint8_t m_winnerId = 255;
    uint8_t m_spectatorCount = 0;
    uint8_t m_watchedPlayerId = 255;
    uint8_t m_gamePlayer0Id = 255;
    uint8_t m_gamePlayer1Id = 255;
    std::string m_name;
    std::string m_nameError;
    std::string m_opponentName;
    std::string m_challengerName;
    std::string m_player0Name;
    std::string m_player1Name;
    std::vector<EntitySnapshot> m_entities;
    std::vector<TowerSnapshot> m_towers;
    std::vector<PlayerInfo> m_playerList;
    std::array<float, MAX_PLAYERS> m_elixir = {5.f, 5.f};
    float m_remainingTime = GAME_DURATION;
    float m_matchFoundTimer = 0.f;
    bool m_isOvertime = false;
    bool m_wasSpectating = false;
    bool m_isLocalPlayer0 = true;
};
