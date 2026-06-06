#pragma once
#include <SFML/Graphics.hpp>
#include <string>
#include <vector>
#include "Packets.hpp"

struct UIButton {
    sf::FloatRect bounds;
    std::string label;
    sf::Color color;
    sf::Color hoverColor;
    bool hovered = false;
};

class UIScreen {
public:
    UIScreen(sf::RenderWindow &window, sf::Font &font, bool fontLoaded);

    void drawNameEntry(const std::string &currentName,
                       bool cursorVisible,
                       const std::string &errorMessage);

    void drawIdle(const std::string &playerName,
                  bool showPlayerList,
                  const std::vector<PlayerInfo> &playerList,
                  uint8_t localClientId);

    void drawSearching(const std::string &playerName);

    void drawChallengeSent(const std::string &targetName);

    void drawChallengeReceived(const std::string &challengerName);

    void drawMatchFound(const std::string &opponentName, float timer);

    void drawGameOver(bool localPlayerWon, bool isDraw);

    bool isConfirmNameClicked(sf::Vector2i mouse) const;

    bool isGenerateNameClicked(sf::Vector2i mouse) const;

    bool isFindMatchClicked(sf::Vector2i mouse) const;

    bool isCancelMatchClicked(sf::Vector2i mouse) const;

    bool isCancelChallengeClicked(sf::Vector2i mouse) const;

    bool isPlayerListClicked(sf::Vector2i mouse) const;

    bool isAcceptChallengeClicked(sf::Vector2i mouse) const;

    bool isDeclineChallengeClicked(sf::Vector2i mouse) const;

    bool isReturnToLobbyClicked(sf::Vector2i mouse) const;

    bool isChallengeClicked(sf::Vector2i mouse,
                            uint8_t &outTargetId,
                            const std::vector<PlayerInfo> &playerList,
                            uint8_t localClientId) const;

    void updateHover(sf::Vector2i mouse);

    void scrollPlayerList(int delta);

private:
    void drawButton(const UIButton &button);

    void drawText(const std::string &text, float x, float y,
                  unsigned int size, sf::Color color);

    void drawCenteredText(const std::string &text, float y,
                          unsigned int size, sf::Color color);

    sf::FloatRect getChallengeButtonBounds(int rowIndex) const;

    sf::RenderWindow &m_window;
    sf::Font &m_font;
    bool m_fontLoaded;
    int m_playerListScrollOffset = 0;

    UIButton m_confirmNameButton;
    UIButton m_generateNameButton;
    UIButton m_findMatchButton;
    UIButton m_cancelMatchButton;
    UIButton m_cancelChallengeButton;
    UIButton m_playerListButton;
    UIButton m_acceptChallengeButton;
    UIButton m_declineChallengeButton;
    UIButton m_returnToLobbyButton;
};
