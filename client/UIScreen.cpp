#include "UIScreen.hpp"
#include <cmath>

UIScreen::UIScreen(sf::RenderWindow &window, sf::Font &font, bool fontLoaded)
    : m_window(window)
      , m_font(font)
      , m_fontLoaded(fontLoaded) {
    m_confirmNameButton = {
        sf::FloatRect({300.f, 380.f}, {200.f, 50.f}),
        "Confirm",
        sf::Color(50, 150, 80), sf::Color(70, 180, 100)
    };
    m_generateNameButton = {
        sf::FloatRect({300.f, 450.f}, {200.f, 50.f}),
        "Random name",
        sf::Color(60, 80, 160), sf::Color(80, 100, 200)
    };
    m_findMatchButton = {
        sf::FloatRect({300.f, 300.f}, {200.f, 60.f}),
        "Find Match",
        sf::Color(50, 150, 80), sf::Color(70, 180, 100)
    };
    m_cancelMatchButton = {
        sf::FloatRect({300.f, 300.f}, {200.f, 60.f}),
        "Cancel Search",
        sf::Color(150, 50, 50), sf::Color(180, 70, 70)
    };
    m_cancelChallengeButton = {
        sf::FloatRect({300.f, 360.f}, {200.f, 50.f}),
        "Cancel",
        sf::Color(150, 50, 50), sf::Color(180, 70, 70)
    };
    m_playerListButton = {
        sf::FloatRect({690.f, 10.f}, {100.f, 36.f}),
        "Players",
        sf::Color(60, 60, 100), sf::Color(80, 80, 140)
    };
    m_acceptChallengeButton = {
        sf::FloatRect({220.f, 370.f}, {150.f, 50.f}),
        "Accept",
        sf::Color(50, 150, 80), sf::Color(70, 180, 100)
    };
    m_declineChallengeButton = {
        sf::FloatRect({430.f, 370.f}, {150.f, 50.f}),
        "Decline",
        sf::Color(150, 50, 50), sf::Color(180, 70, 70)
    };
    m_returnToLobbyButton = {
        sf::FloatRect({300.f, 420.f}, {200.f, 55.f}),
        "Back to lobby",
        sf::Color(60, 80, 160), sf::Color(80, 100, 200)
    };
}

void UIScreen::drawButton(const UIButton &button) {
    sf::RectangleShape shape(button.bounds.size);
    shape.setPosition(button.bounds.position);
    shape.setFillColor(button.hovered ? button.hoverColor : button.color);
    shape.setOutlineThickness(2.f);
    shape.setOutlineColor(sf::Color(200, 200, 200, 80));
    m_window.draw(shape);

    if (m_fontLoaded) {
        sf::Text label(m_font, button.label, 18);
        label.setFillColor(sf::Color::White);
        sf::FloatRect bounds = label.getLocalBounds();
        label.setPosition({
            button.bounds.position.x + (button.bounds.size.x - bounds.size.x) / 2.f,
            button.bounds.position.y + (button.bounds.size.y - bounds.size.y) / 2.f - 2.f
        });
        m_window.draw(label);
    }
}

void UIScreen::drawText(const std::string &text, float x, float y,
                        unsigned int size, sf::Color color) {
    if (!m_fontLoaded) return;
    sf::Text label(m_font, text, size);
    label.setFillColor(color);
    label.setPosition({x, y});
    m_window.draw(label);
}

void UIScreen::drawCenteredText(const std::string &text, float y,
                                unsigned int size, sf::Color color) {
    if (!m_fontLoaded) return;
    sf::Text label(m_font, text, size);
    label.setFillColor(color);
    sf::FloatRect bounds = label.getLocalBounds();
    label.setPosition({400.f - bounds.size.x / 2.f, y});
    m_window.draw(label);
}

void UIScreen::drawNameEntry(const std::string &currentName,
                             bool cursorVisible,
                             const std::string &errorMessage) {
    drawCenteredText("CppRoyale", 80.f, 48, sf::Color(220, 180, 60));
    drawCenteredText("Enter your name", 200.f, 24, sf::Color(200, 200, 200));

    sf::RectangleShape inputBox(sf::Vector2f(300.f, 50.f));
    inputBox.setPosition({250.f, 300.f});
    inputBox.setFillColor(sf::Color(40, 40, 60));
    inputBox.setOutlineThickness(2.f);
    inputBox.setOutlineColor(errorMessage.empty()
                                 ? sf::Color(100, 100, 180)
                                 : sf::Color(200, 60, 60));
    m_window.draw(inputBox);

    std::string displayName = currentName + (cursorVisible ? "|" : "");
    drawText(displayName, 260.f, 312.f, 18, sf::Color::White);

    if (!errorMessage.empty())
        drawCenteredText(errorMessage, 360.f, 15, sf::Color(220, 80, 80));

    drawButton(m_confirmNameButton);
    drawButton(m_generateNameButton);
}

void UIScreen::drawIdle(const std::string &playerName,
                        bool showPlayerList,
                        const std::vector<PlayerInfo> &playerList,
                        uint8_t localClientId) {
    drawCenteredText("CppRoyale", 80.f, 48, sf::Color(220, 180, 60));
    drawCenteredText("Welcome, " + playerName + "!", 160.f, 22,
                     sf::Color(180, 180, 180));
    drawButton(m_findMatchButton);
    drawButton(m_playerListButton);

    if (!showPlayerList) return;

    constexpr float PANEL_X = 150.f;
    constexpr float PANEL_Y = 130.f;
    constexpr float PANEL_W = 500.f;
    constexpr float PANEL_H = 420.f;
    constexpr float ROW_HEIGHT = 56.f;
    constexpr float LIST_START_Y = 190.f;
    constexpr float LIST_END_Y = PANEL_Y + PANEL_H - 10.f;
    constexpr int VISIBLE_ROWS = static_cast<int>((LIST_END_Y - LIST_START_Y) / ROW_HEIGHT);

    sf::RectangleShape overlay(sf::Vector2f(PANEL_W, PANEL_H));
    overlay.setPosition({PANEL_X, PANEL_Y});
    overlay.setFillColor(sf::Color(25, 25, 40, 245));
    overlay.setOutlineThickness(2.f);
    overlay.setOutlineColor(sf::Color(80, 80, 120));
    m_window.draw(overlay);

    drawCenteredText("Connected players", 145.f, 18, sf::Color(200, 200, 220));

    int totalRows = 0;
    for (const auto &player: playerList)
        if (player.clientId != localClientId)
            ++totalRows;

    int maxScroll = std::max(0, totalRows - VISIBLE_ROWS);
    m_playerListScrollOffset = std::min(m_playerListScrollOffset, maxScroll);

    if (totalRows > VISIBLE_ROWS) {
        sf::RectangleShape arrowUp(sf::Vector2f(10.f, 6.f));
        arrowUp.setPosition({633.f, LIST_START_Y + 4.f});
        arrowUp.setFillColor(sf::Color(180, 180, 220));
        m_window.draw(arrowUp);

        sf::RectangleShape arrowDown(sf::Vector2f(10.f, 6.f));
        arrowDown.setPosition({633.f, LIST_END_Y - 14.f});
        arrowDown.setFillColor(sf::Color(180, 180, 220));
        m_window.draw(arrowDown);

        float trackH = LIST_END_Y - LIST_START_Y - 40.f;
        float thumbH = trackH * VISIBLE_ROWS / totalRows;
        float thumbY = LIST_START_Y + 20.f
                       + (trackH - thumbH) * m_playerListScrollOffset / maxScroll;

        sf::RectangleShape track(sf::Vector2f(6.f, trackH));
        track.setPosition({637.f, LIST_START_Y + 20.f});
        track.setFillColor(sf::Color(60, 60, 80));
        m_window.draw(track);

        sf::RectangleShape thumb(sf::Vector2f(6.f, thumbH));
        thumb.setPosition({637.f, thumbY});
        thumb.setFillColor(sf::Color(140, 140, 200));
        m_window.draw(thumb);
    }

    int globalIndex = 0;
    int visibleIndex = 0;
    for (const auto &player: playerList) {
        if (player.clientId == localClientId) continue;

        if (globalIndex >= m_playerListScrollOffset
            && visibleIndex < VISIBLE_ROWS) {
            float rowY = LIST_START_Y + visibleIndex * ROW_HEIGHT;

            std::string statusText;
            if (player.isInGame) statusText = " [In game]";
            else if (player.isSearching) statusText = " [Searching]";
            else statusText = " [Idle]";

            drawText(player.name + statusText, 170.f, rowY + 8.f, 15,
                     sf::Color(200, 200, 200));

            bool canChallenge = !player.isSearching
                                && !player.isInGame
                                && !player.hasPendingChallenge;

            if (canChallenge) {
                sf::FloatRect challengeBounds = getChallengeButtonBounds(visibleIndex);
                sf::RectangleShape challengeBtn(challengeBounds.size);
                challengeBtn.setPosition(challengeBounds.position);
                challengeBtn.setFillColor(sf::Color(100, 60, 160));
                challengeBtn.setOutlineThickness(1.f);
                challengeBtn.setOutlineColor(sf::Color(140, 80, 220));
                m_window.draw(challengeBtn);
                drawText("Challenge", challengeBounds.position.x + 6.f,
                         challengeBounds.position.y + 8.f, 13, sf::Color::White);
            }

            ++visibleIndex;
        }
        ++globalIndex;
    }
}

void UIScreen::drawSearching(const std::string &playerName) {
    drawCenteredText("CppRoyale", 80.f, 48, sf::Color(220, 180, 60));
    drawCenteredText("Welcome, " + playerName + "!", 160.f, 22,
                     sf::Color(180, 180, 180));
    drawCenteredText("Searching for opponent...", 240.f, 20,
                     sf::Color(160, 200, 160));
    drawButton(m_cancelMatchButton);
    drawButton(m_playerListButton);
}

void UIScreen::drawChallengeSent(const std::string &targetName) {
    drawCenteredText("CppRoyale", 80.f, 48, sf::Color(220, 180, 60));
    drawCenteredText("Challenge sent to " + targetName + "...", 240.f, 20,
                     sf::Color(180, 180, 100));
    drawCenteredText("Waiting for response", 280.f, 16,
                     sf::Color(140, 140, 140));
    drawButton(m_cancelChallengeButton);
}

void UIScreen::drawChallengeReceived(const std::string &challengerName) {
    sf::RectangleShape overlay(sf::Vector2f(800.f, 680.f));
    overlay.setFillColor(sf::Color(0, 0, 0, 160));
    m_window.draw(overlay);

    sf::RectangleShape box(sf::Vector2f(360.f, 200.f));
    box.setPosition({220.f, 240.f});
    box.setFillColor(sf::Color(40, 30, 60));
    box.setOutlineThickness(2.f);
    box.setOutlineColor(sf::Color(120, 80, 200));
    m_window.draw(box);

    drawCenteredText(challengerName + " challenged you!", 265.f, 18,
                     sf::Color(200, 180, 255));
    drawCenteredText("Accept or decline?", 300.f, 15,
                     sf::Color(160, 160, 160));
    drawButton(m_acceptChallengeButton);
    drawButton(m_declineChallengeButton);
}

void UIScreen::drawMatchFound(const std::string &opponentName, float timer) {
    sf::RectangleShape overlay(sf::Vector2f(800.f, 680.f));
    overlay.setFillColor(sf::Color(0, 0, 0, 180));
    m_window.draw(overlay);

    sf::RectangleShape box(sf::Vector2f(440.f, 200.f));
    box.setPosition({180.f, 240.f});
    box.setFillColor(sf::Color(30, 60, 30));
    box.setOutlineThickness(3.f);
    box.setOutlineColor(sf::Color(80, 200, 80));
    m_window.draw(box);

    drawCenteredText("Match Found!", 260.f, 32, sf::Color(80, 220, 80));
    drawCenteredText("vs " + opponentName, 310.f, 22, sf::Color(200, 200, 200));

    int countdown = static_cast<int>(std::ceil(timer));
    drawCenteredText("Starting in " + std::to_string(countdown) + "...",
                     355.f, 18, sf::Color(160, 160, 160));
}

void UIScreen::drawGameOver(bool localPlayerWon, bool isDraw) {
    sf::RectangleShape overlay(sf::Vector2f(800.f, 680.f));
    overlay.setFillColor(sf::Color(0, 0, 0, 160));
    m_window.draw(overlay);

    sf::Color boxColor = isDraw
                             ? sf::Color(80, 80, 80)
                             : (localPlayerWon ? sf::Color(30, 120, 50) : sf::Color(120, 30, 30));

    sf::RectangleShape resultBox(sf::Vector2f(400.f, 180.f));
    resultBox.setPosition({200.f, 220.f});
    resultBox.setFillColor(boxColor);
    m_window.draw(resultBox);

    if (m_fontLoaded) {
        std::string resultText = isDraw
                                     ? "Draw"
                                     : (localPlayerWon ? "Victory!" : "Defeat");
        sf::Text label(m_font, resultText, 48);
        label.setFillColor(sf::Color::White);
        sf::FloatRect bounds = label.getLocalBounds();
        label.setPosition({400.f - bounds.size.x / 2.f, 240.f});
        m_window.draw(label);
    }

    drawButton(m_returnToLobbyButton);
}

void UIScreen::updateHover(sf::Vector2i mouse) {
    sf::Vector2f mouseF(static_cast<float>(mouse.x),
                        static_cast<float>(mouse.y));
    m_confirmNameButton.hovered = m_confirmNameButton.bounds.contains(mouseF);
    m_generateNameButton.hovered = m_generateNameButton.bounds.contains(mouseF);
    m_findMatchButton.hovered = m_findMatchButton.bounds.contains(mouseF);
    m_cancelMatchButton.hovered = m_cancelMatchButton.bounds.contains(mouseF);
    m_cancelChallengeButton.hovered = m_cancelChallengeButton.bounds.contains(mouseF);
    m_playerListButton.hovered = m_playerListButton.bounds.contains(mouseF);
    m_acceptChallengeButton.hovered = m_acceptChallengeButton.bounds.contains(mouseF);
    m_declineChallengeButton.hovered = m_declineChallengeButton.bounds.contains(mouseF);
    m_returnToLobbyButton.hovered = m_returnToLobbyButton.bounds.contains(mouseF);
}

bool UIScreen::isConfirmNameClicked(sf::Vector2i mouse) const {
    return m_confirmNameButton.bounds.contains(
        sf::Vector2f(static_cast<float>(mouse.x), static_cast<float>(mouse.y)));
}

bool UIScreen::isGenerateNameClicked(sf::Vector2i mouse) const {
    return m_generateNameButton.bounds.contains(
        sf::Vector2f(static_cast<float>(mouse.x), static_cast<float>(mouse.y)));
}

bool UIScreen::isFindMatchClicked(sf::Vector2i mouse) const {
    return m_findMatchButton.bounds.contains(
        sf::Vector2f(static_cast<float>(mouse.x), static_cast<float>(mouse.y)));
}

bool UIScreen::isCancelMatchClicked(sf::Vector2i mouse) const {
    return m_cancelMatchButton.bounds.contains(
        sf::Vector2f(static_cast<float>(mouse.x), static_cast<float>(mouse.y)));
}

bool UIScreen::isCancelChallengeClicked(sf::Vector2i mouse) const {
    return m_cancelChallengeButton.bounds.contains(
        sf::Vector2f(static_cast<float>(mouse.x), static_cast<float>(mouse.y)));
}

bool UIScreen::isPlayerListClicked(sf::Vector2i mouse) const {
    return m_playerListButton.bounds.contains(
        sf::Vector2f(static_cast<float>(mouse.x), static_cast<float>(mouse.y)));
}

bool UIScreen::isAcceptChallengeClicked(sf::Vector2i mouse) const {
    return m_acceptChallengeButton.bounds.contains(
        sf::Vector2f(static_cast<float>(mouse.x), static_cast<float>(mouse.y)));
}

bool UIScreen::isDeclineChallengeClicked(sf::Vector2i mouse) const {
    return m_declineChallengeButton.bounds.contains(
        sf::Vector2f(static_cast<float>(mouse.x), static_cast<float>(mouse.y)));
}

bool UIScreen::isReturnToLobbyClicked(sf::Vector2i mouse) const {
    return m_returnToLobbyButton.bounds.contains(
        sf::Vector2f(static_cast<float>(mouse.x), static_cast<float>(mouse.y)));
}

bool UIScreen::isChallengeClicked(sf::Vector2i mouse,
                                  uint8_t &outTargetId,
                                  const std::vector<PlayerInfo> &playerList,
                                  uint8_t localClientId) const {
    sf::Vector2f mouseF(static_cast<float>(mouse.x), static_cast<float>(mouse.y));

    constexpr int VISIBLE_ROWS = 6;
    int globalIndex = 0;
    int visibleIndex = 0;

    for (const auto &player: playerList) {
        if (player.clientId == localClientId) {
            ++globalIndex;
            continue;
        }

        if (globalIndex >= m_playerListScrollOffset && visibleIndex < VISIBLE_ROWS) {
            bool canChallenge = !player.isSearching
                                && !player.isInGame
                                && !player.hasPendingChallenge;
            if (canChallenge) {
                sf::FloatRect bounds = getChallengeButtonBounds(visibleIndex);
                if (bounds.contains(mouseF)) {
                    outTargetId = player.clientId;
                    return true;
                }
            }
            ++visibleIndex;
        }
        ++globalIndex;
    }
    return false;
}

sf::FloatRect UIScreen::getChallengeButtonBounds(int rowIndex) const {
    float rowY = 190.f + rowIndex * 56.f;
    return sf::FloatRect(sf::Vector2f(530.f, rowY), sf::Vector2f(100.f, 34.f));
}

void UIScreen::scrollPlayerList(int delta) {
    m_playerListScrollOffset = std::max(0, m_playerListScrollOffset + delta);
}
