#include "GameServer.hpp"
#include "Packets.hpp"
#include "Constants.hpp"
#include <iostream>
#include <algorithm>

GameServer::GameServer(unsigned short port)
    : m_running(true)
      , m_nextClientId(0) {
    if (m_listener.listen(port) != sf::Socket::Status::Done) {
        std::cerr << "[Server] Cannot bind port " << port << "\n";
        m_running = false;
        return;
    }
    m_selector.add(m_listener);
    std::cout << "[Server] Listening on port " << port << "\n";
}

void GameServer::run() {
    sf::Clock clock;
    float accumulator = 0.f;

    while (m_running) {
        accumulator += clock.restart().asSeconds();

        if (m_selector.wait(sf::milliseconds(5))) {
            acceptClients();
            receivePackets();
        }

        removeDisconnectedClients();

        while (accumulator >= TICK_DURATION) {
            for (auto &game: m_activeGames) {
                game.gameState.update();
                broadcastGameState(game);

                GameResult result = game.gameState.getResult();
                if (result != GameResult::Ongoing) {
                    uint8_t winnerId = 255;
                    if (result == GameResult::Player0Wins) winnerId = 0;
                    if (result == GameResult::Player1Wins) winnerId = 1;
                    broadcastGameOver(game, winnerId);
                }
            }

            m_activeGames.erase(
                std::remove_if(m_activeGames.begin(), m_activeGames.end(),
                               [](const ActiveGame &game) {
                                   return game.gameState.getResult() != GameResult::Ongoing;
                               }),
                m_activeGames.end()
            );

            accumulator -= TICK_DURATION;
        }
    }
}

void GameServer::acceptClients() {
    if (!m_selector.isReady(m_listener)) return;

    auto socket = std::make_unique<sf::TcpSocket>();
    if (m_listener.accept(*socket) != sf::Socket::Status::Done) return;

    auto address = socket->getRemoteAddress();
    std::cout << "[Server] Client connected: "
            << (address ? address->toString() : "unknown") << "\n";

    socket->setBlocking(false);
    m_selector.add(*socket);
    m_sessions.emplace_back(std::move(socket), m_nextClientId++);
}

void GameServer::receivePackets() {
    for (auto &session: m_sessions) {
        if (session.disconnected) continue;
        if (!m_selector.isReady(*session.socket)) continue;

        sf::Packet packet;
        auto status = session.socket->receive(packet);

        if (status == sf::Socket::Status::Disconnected
            || status == sf::Socket::Status::Error) {
            session.disconnected = true;
            continue;
        }

        if (status == sf::Socket::Status::Done)
            processPacket(packet, session);
    }
}

void GameServer::removeDisconnectedClients() {
    bool anyRemoved = false;

    for (auto &session: m_sessions) {
        if (!session.disconnected) continue;

        std::cout << "[Server] Client disconnected: " << session.name << "\n";

        bool hadName = session.hasSetName;

        if (session.state == SessionState::Searching)
            m_lobby.removeFromQueue(session.clientId);

        if (session.state == SessionState::ChallengeSent
            || session.state == SessionState::ChallengeReceived) {
            ClientSession *partner = findSession(session.challengePartnerId);
            if (partner) {
                sf::Packet cancelPacket;
                cancelPacket << PKT_CANCEL_CHALLENGE;
                (void) partner->socket->send(cancelPacket);
                partner->state = SessionState::Idle;
                partner->challengePartnerId = 255;
            }
        }

        if (session.state == SessionState::InGame) {
            ActiveGame *game = findGame(session.clientId);
            if (game) {
                uint8_t survivorClientId = (game->clientIds[0] == session.clientId)
                                               ? game->clientIds[1]
                                               : game->clientIds[0];
                ClientSession *survivor = findSession(survivorClientId);
                if (survivor) {
                    sf::Packet gameOverPacket;
                    gameOverPacket << PKT_GAME_OVER << survivor->gamePlayerId;
                    (void) survivor->socket->send(gameOverPacket);
                    survivor->state = SessionState::Idle;
                    survivor->gamePlayerId = 255;
                }
                m_activeGames.erase(
                    std::remove_if(m_activeGames.begin(), m_activeGames.end(),
                                   [&session](const ActiveGame &g) {
                                       return g.clientIds[0] == session.clientId
                                              || g.clientIds[1] == session.clientId;
                                   }),
                    m_activeGames.end()
                );
            }
        }

        m_selector.remove(*session.socket);
        if (hadName) anyRemoved = true;
    }

    m_sessions.erase(
        std::remove_if(m_sessions.begin(), m_sessions.end(),
                       [](const ClientSession &s) { return s.disconnected; }),
        m_sessions.end()
    );

    if (anyRemoved)
        broadcastPlayerList();
}

void GameServer::processPacket(sf::Packet &packet, ClientSession &session) {
    uint8_t packetType;
    packet >> packetType;

    switch (packetType) {
        case PKT_SET_NAME: handleSetName(packet, session);
            break;
        case PKT_FIND_MATCH: handleFindMatch(session);
            break;
        case PKT_CANCEL_MATCH: handleCancelMatch(session);
            break;
        case PKT_PLAYER_LIST: handlePlayerListRequest(session);
            break;
        case PKT_CHALLENGE: handleChallenge(packet, session);
            break;
        case PKT_CHALLENGE_RESP: handleChallengeResponse(packet, session);
            break;
        case PKT_CANCEL_CHALLENGE: handleCancelChallenge(session);
            break;
        case PKT_DEPLOY: handleDeploy(packet, session);
            break;
        case PKT_RETURN_TO_LOBBY: handleReturnToLobby(session);
            break;
        default: break;
    }
}

void GameServer::handleSetName(sf::Packet &packet, ClientSession &session) {
    std::string name;
    packet >> name;
    if (name.empty() || name.size() > MAX_NAME_LENGTH) return;

    for (const auto &other: m_sessions) {
        if (other.clientId == session.clientId) continue;
        if (!other.hasSetName) continue;
        if (other.name == name) {
            sf::Packet rejectPacket;
            rejectPacket << PKT_SET_NAME << uint8_t(0) << std::string("Name already taken");
            (void) session.socket->send(rejectPacket);
            return;
        }
    }

    session.name = name;
    session.hasSetName = true;
    sf::Packet confirmPacket;
    confirmPacket << PKT_SET_NAME << uint8_t(1) << session.clientId;
    (void) session.socket->send(confirmPacket);

    std::cout << "[Server] Client " << (int) session.clientId
            << " set name: " << name << "\n";
    broadcastPlayerList();
}

void GameServer::handleFindMatch(ClientSession &session) {
    if (session.state != SessionState::Idle) return;
    session.state = SessionState::Searching;
    m_lobby.addToQueue(session.clientId);
    std::cout << "[Server] " << session.name << " is searching\n";
    broadcastPlayerList();

    uint8_t client0, client1;
    if (m_lobby.tryMatchmake(client0, client1))
        startGame(client0, client1);
}

void GameServer::handleCancelMatch(ClientSession &session) {
    if (session.state != SessionState::Searching) return;
    session.state = SessionState::Idle;
    m_lobby.removeFromQueue(session.clientId);
    std::cout << "[Server] " << session.name << " cancelled search\n";
    broadcastPlayerList();
}

void GameServer::handlePlayerListRequest(ClientSession &session) {
    sendPlayerList(session);
}

void GameServer::handleChallenge(sf::Packet &packet, ClientSession &session) {
    if (session.state != SessionState::Idle) return;

    uint8_t targetId;
    packet >> targetId;

    if (targetId == session.clientId) return;

    ClientSession *target = findSession(targetId);
    if (!target) return;
    if (target->state != SessionState::Idle) return;

    session.state = SessionState::ChallengeSent;
    session.challengePartnerId = targetId;
    target->state = SessionState::ChallengeReceived;
    target->challengePartnerId = session.clientId;

    sf::Packet challengePacket;
    challengePacket << PKT_CHALLENGE << session.clientId << session.name;
    (void) target->socket->send(challengePacket);

    std::cout << "[Server] " << session.name
            << " challenged " << target->name << "\n";
    broadcastPlayerList();
}

void GameServer::handleChallengeResponse(sf::Packet &packet,
                                         ClientSession &session) {
    if (session.state != SessionState::ChallengeReceived) return;

    uint8_t accepted;
    packet >> accepted;

    uint8_t challengerId = session.challengePartnerId;
    ClientSession *challenger = findSession(challengerId);

    session.challengePartnerId = 255;

    if (accepted == 1) {
        if (challenger) {
            challenger->challengePartnerId = 255;
            startGame(challengerId, session.clientId);
        }
    } else {
        session.state = SessionState::Idle;
        if (challenger) {
            challenger->state = SessionState::Idle;
            challenger->challengePartnerId = 255;
            sf::Packet responsePacket;
            responsePacket << PKT_CHALLENGE_RESP << uint8_t(0);
            (void) challenger->socket->send(responsePacket);
        }
        broadcastPlayerList();
    }
}

void GameServer::handleCancelChallenge(ClientSession &session) {
    if (session.state != SessionState::ChallengeSent) return;

    ClientSession *target = findSession(session.challengePartnerId);
    if (target) {
        sf::Packet cancelPacket;
        cancelPacket << PKT_CANCEL_CHALLENGE;
        (void) target->socket->send(cancelPacket);
        target->state = SessionState::Idle;
        target->challengePartnerId = 255;
    }

    session.state = SessionState::Idle;
    session.challengePartnerId = 255;
    broadcastPlayerList();
}

void GameServer::handleDeploy(sf::Packet &packet, ClientSession &session) {
    ActiveGame *game = findGame(session.clientId);
    if (!game) return;

    uint8_t troopTypeByte;
    float x;
    float y;
    uint8_t senderPlayerId;

    packet >> troopTypeByte >> x >> y >> senderPlayerId;
    if (senderPlayerId != session.gamePlayerId) return;

    game->gameState.spawnTroop(
        static_cast<TroopType>(troopTypeByte), x, y, session.gamePlayerId
    );
}

void GameServer::handleReturnToLobby(ClientSession &session) {
    if (session.state != SessionState::GameOver) return;
    session.state = SessionState::Idle;
    session.gamePlayerId = 255;
    broadcastPlayerList();
}

void GameServer::startGame(uint8_t clientId0, uint8_t clientId1) {
    ClientSession *session0 = findSession(clientId0);
    ClientSession *session1 = findSession(clientId1);
    if (!session0 || !session1) return;

    session0->state = SessionState::InGame;
    session1->state = SessionState::InGame;
    session0->gamePlayerId = 0;
    session1->gamePlayerId = 1;
    session0->challengePartnerId = 255;
    session1->challengePartnerId = 255;

    ActiveGame game;
    game.clientIds[0] = clientId0;
    game.clientIds[1] = clientId1;
    m_activeGames.push_back(std::move(game));

    broadcastMatchFound(clientId0, clientId1);
    broadcastPlayerList();

    std::cout << "[Server] Game started: "
            << session0->name << " vs " << session1->name << "\n";
}

void GameServer::broadcastMatchFound(uint8_t clientId0, uint8_t clientId1) {
    ClientSession *session0 = findSession(clientId0);
    ClientSession *session1 = findSession(clientId1);
    if (!session0 || !session1) return;

    sf::Packet packet0;
    packet0 << PKT_MATCH_FOUND << uint8_t(0) << session1->name;
    (void) session0->socket->send(packet0);

    sf::Packet packet1;
    packet1 << PKT_MATCH_FOUND << uint8_t(1) << session0->name;
    (void) session1->socket->send(packet1);
}

void GameServer::broadcastGameState(ActiveGame &game) {
    const auto &entities = game.gameState.getEntities();
    const auto &towers = game.gameState.getTowers();

    sf::Packet packet;
    packet << PKT_GAME_STATE;
    packet << game.gameState.getTick();
    packet << game.gameState.getRemainingTime();
    packet << static_cast<uint8_t>(game.gameState.isOvertime() ? 1 : 0);

    for (uint8_t playerId = 0; playerId < MAX_PLAYERS; ++playerId)
        packet << game.gameState.getElixir(playerId);

    packet << static_cast<uint16_t>(entities.size());
    for (const auto &entity: entities) {
        packet << entity->getEntityId()
                << static_cast<uint8_t>(entity->getTroopType())
                << entity->getX()
                << entity->getY()
                << entity->getHp()
                << entity->getOwnerId();
    }

    packet << static_cast<uint8_t>(towers.size());
    for (const auto &tower: towers) {
        packet << tower.getTowerId()
                << tower.getX()
                << tower.getY()
                << tower.getHp()
                << tower.getOwnerId()
                << static_cast<uint8_t>(tower.isNexus() ? 1 : 0);
    }

    for (uint8_t clientId: game.clientIds) {
        ClientSession *session = findSession(clientId);
        if (session)
            (void) session->socket->send(packet);
    }
}

void GameServer::broadcastGameOver(ActiveGame &game, uint8_t winnerPlayerId) {
    sf::Packet packet;
    packet << PKT_GAME_OVER << winnerPlayerId;

    for (uint8_t clientId: game.clientIds) {
        ClientSession *session = findSession(clientId);
        if (session) {
            (void) session->socket->send(packet);
            session->state = SessionState::GameOver;
            session->gamePlayerId = 255;
        }
    }
    broadcastPlayerList();
}

void GameServer::broadcastPlayerList() {
    for (auto &session: m_sessions)
        sendPlayerList(session);
}

void GameServer::sendPlayerList(ClientSession &receiver) {
    sf::Packet packet;
    packet << PKT_PLAYER_LIST;

    uint8_t count = 0;
    for (const auto &session: m_sessions)
        if (session.clientId != receiver.clientId && session.hasSetName)
            ++count;

    packet << count;
    for (const auto &session: m_sessions) {
        if (session.clientId == receiver.clientId) continue;
        if (!session.hasSetName) continue;
        bool hasPending = (session.state == SessionState::ChallengeReceived);
        bool isUnavailable = (session.state == SessionState::InGame
                              || session.state == SessionState::GameOver);
        packet << session.clientId
                << session.name
                << static_cast<uint8_t>(session.state == SessionState::Searching ? 1 : 0)
                << static_cast<uint8_t>(hasPending ? 1 : 0)
                << static_cast<uint8_t>(isUnavailable ? 1 : 0);
    }

    (void) receiver.socket->send(packet);
}

ClientSession *GameServer::findSession(uint8_t clientId) {
    for (auto &session: m_sessions)
        if (session.clientId == clientId)
            return &session;
    return nullptr;
}

ActiveGame *GameServer::findGame(uint8_t clientId) {
    for (auto &game: m_activeGames)
        if (game.clientIds[0] == clientId || game.clientIds[1] == clientId)
            return &game;
    return nullptr;
}
