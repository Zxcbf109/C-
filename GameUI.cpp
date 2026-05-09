#include "GameUI.h"
#include <sstream>
#include <iomanip>

// UI面板起始X坐标（玩家区域宽度 = 960px）
static const float UI_START_X = 960.f;
static const float UI_PANEL_W = 320.f;  // 1280 - 960 = 320

// Heart icon constants (from item.png 256x64)
// Life hearts: position (32, 2), size 32x30
static const int LIFE_HEART_X = 32;
static const int LIFE_HEART_Y = 2;
static const int HEART_W = 32;
static const int HEART_H = 30;
// Spell hearts: position (96, 1), size 32x30
static const int SPELL_HEART_X = 96;
static const int SPELL_HEART_Y = 1;
static const float HEART_SCALE = 1.5f;
static const float HEART_SPACING = HEART_W * HEART_SCALE + 4.f;  // 48 + 4 = 52

// 状态栏位置和大小
static const float STATUS_BAR_X = 800.f;
static const float STATUS_BAR_WIDTH = 450.f;
static const float STATUS_BAR_HEIGHT = 25.f;

// Boss状态栏
static const float BOSS_BAR_Y = 30.f;
static const float BOSS_BAR_LABEL_Y = 10.f;

// Player状态栏
static const float PLAYER_BAR_Y = 100.f;
static const float PLAYER_BAR_LABEL_Y = 80.f;

GameUI::GameUI() {
    loadTextures();
    m_font.loadFromFile("C:/Windows/Fonts/msyh.ttc");
}

GameUI::~GameUI() {
}

void GameUI::loadTextures() {
    if (!m_lifeTexture.loadFromFile("assets/image/item/item.png")) {
        sf::Image img;
        img.create(64, 64, sf::Color::White);
        m_lifeTexture.loadFromImage(img);
    }
}

void GameUI::update(float deltaTime) {
    // 简化版，不需要更新文字
}

void GameUI::draw(sf::RenderWindow& window) {
    // 绘制简单的分隔线（左侧边界）
    sf::RectangleShape divider(sf::Vector2f(2.f, 960.f));
    divider.setFillColor(sf::Color(80, 80, 80, 100));
    divider.setPosition(960.f, 0.f);
    window.draw(divider);

    // ========== 绘制状态栏 ==========

    // Boss血条标签
    sf::Text bossLabel("BOSS", m_font, 18);
    bossLabel.setFillColor(sf::Color::Yellow);
    bossLabel.setOutlineColor(sf::Color::Black);
    bossLabel.setOutlineThickness(1.f);
    bossLabel.setPosition(STATUS_BAR_X, BOSS_BAR_LABEL_Y);
    window.draw(bossLabel);

    // Boss血条背景
    sf::RectangleShape bossBgBar(sf::Vector2f(STATUS_BAR_WIDTH, STATUS_BAR_HEIGHT));
    bossBgBar.setFillColor(sf::Color(50, 50, 50));
    bossBgBar.setOutlineColor(sf::Color(100, 100, 100));
    bossBgBar.setOutlineThickness(1.f);
    bossBgBar.setPosition(STATUS_BAR_X, BOSS_BAR_Y);
    window.draw(bossBgBar);

    // Boss血条
    float bossPercent = (m_stats.bossMaxHp > 0) ? static_cast<float>(m_stats.bossHp) / m_stats.bossMaxHp : 0.f;
    bossPercent = std::max(0.f, std::min(1.f, bossPercent));
    if (bossPercent > 0) {
        sf::RectangleShape bossHpBar(sf::Vector2f(STATUS_BAR_WIDTH * bossPercent, STATUS_BAR_HEIGHT));
        bossHpBar.setFillColor(sf::Color(255, 50, 50));  // 红色
        bossHpBar.setPosition(STATUS_BAR_X, BOSS_BAR_Y);
        window.draw(bossHpBar);
    }

    // Boss血量数字
    std::ostringstream bossOss;
    bossOss << m_stats.bossHp << " / " << m_stats.bossMaxHp;
    sf::Text bossHpText(bossOss.str(), m_font, 16);
    bossHpText.setFillColor(sf::Color::White);
    bossHpText.setOutlineColor(sf::Color::Black);
    bossHpText.setOutlineThickness(1.f);
    bossHpText.setPosition(STATUS_BAR_X + STATUS_BAR_WIDTH - 100.f, BOSS_BAR_Y + 4.f);
    window.draw(bossHpText);

    // Player血条标签
    sf::Text playerLabel("PLAYER", m_font, 18);
    playerLabel.setFillColor(sf::Color::Cyan);
    playerLabel.setOutlineColor(sf::Color::Black);
    playerLabel.setOutlineThickness(1.f);
    playerLabel.setPosition(STATUS_BAR_X, PLAYER_BAR_LABEL_Y);
    window.draw(playerLabel);

    // Player血条背景
    sf::RectangleShape playerBgBar(sf::Vector2f(STATUS_BAR_WIDTH, STATUS_BAR_HEIGHT));
    playerBgBar.setFillColor(sf::Color(50, 50, 50));
    playerBgBar.setOutlineColor(sf::Color(100, 100, 100));
    playerBgBar.setOutlineThickness(1.f);
    playerBgBar.setPosition(STATUS_BAR_X, PLAYER_BAR_Y);
    window.draw(playerBgBar);

    // Player血条
    float playerPercent = (m_stats.playerMaxHp > 0) ? static_cast<float>(m_stats.playerHp) / m_stats.playerMaxHp : 0.f;
    playerPercent = std::max(0.f, std::min(1.f, playerPercent));
    if (playerPercent > 0) {
        sf::RectangleShape playerHpBar(sf::Vector2f(STATUS_BAR_WIDTH * playerPercent, STATUS_BAR_HEIGHT));
        playerHpBar.setFillColor(sf::Color(50, 255, 50));  // 绿色
        playerHpBar.setPosition(STATUS_BAR_X, PLAYER_BAR_Y);
        window.draw(playerHpBar);
    }

    // Player血量数字
    std::ostringstream playerOss;
    playerOss << m_stats.playerHp << " / " << m_stats.playerMaxHp;
    sf::Text playerHpText(playerOss.str(), m_font, 16);
    playerHpText.setFillColor(sf::Color::White);
    playerHpText.setOutlineColor(sf::Color::Black);
    playerHpText.setOutlineThickness(1.f);
    playerHpText.setPosition(STATUS_BAR_X + STATUS_BAR_WIDTH - 100.f, PLAYER_BAR_Y + 4.f);
    window.draw(playerHpText);

    // ========== 原有UI ==========

    // 残机图标和数字 - 右上角
    for (int i = 0; i < m_stats.life && i < 5; ++i) {
        sf::Sprite heart;
        heart.setTexture(m_lifeTexture);
        heart.setTextureRect(sf::IntRect(LIFE_HEART_X, LIFE_HEART_Y, HEART_W, HEART_H));
        heart.setScale(1.2f, 1.2f);
        heart.setPosition(800.f + i * 40.f, 145.f);
        window.draw(heart);
    }

    // 符卡图标和数字 - 残机下方
    for (int i = 0; i < m_stats.bombs && i < 5; ++i) {
        sf::Sprite spell;
        spell.setTexture(m_lifeTexture);
        spell.setTextureRect(sf::IntRect(SPELL_HEART_X, SPELL_HEART_Y, HEART_W, HEART_H));
        spell.setScale(1.2f, 1.2f);
        spell.setPosition(800.f + i * 40.f, 180.f);
        window.draw(spell);
    }

    // 擦弹数 - 右下角
    sf::Text grazeText("Graze: " + std::to_string(m_stats.graze), m_font, 20);
    grazeText.setFillColor(sf::Color(200, 200, 200));
    grazeText.setOutlineColor(sf::Color::Black);
    grazeText.setOutlineThickness(1.f);
    grazeText.setPosition(800.f, 920.f);
    window.draw(grazeText);

    // 难度显示
    std::string diffName[] = {"EASY", "MEDIUM", "HARD", "LUNATIC"};
    sf::Text diffText(diffName[m_stats.difficulty], m_font, 18);
    diffText.setFillColor(sf::Color(255, 200, 100));
    diffText.setOutlineColor(sf::Color::Black);
    diffText.setOutlineThickness(1.f);
    diffText.setPosition(800.f, 220.f);
    window.draw(diffText);
}