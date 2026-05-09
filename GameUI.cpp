#include "GameUI.h"
#include <sstream>
#include <iomanip>

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

// 窗口宽度 (右顶格用)
static const float WINDOW_WIDTH = 1920.f;

// 状态栏位置和大小 (右顶格)
static const float STATUS_BAR_X = 1450.f;
static const float STATUS_BAR_WIDTH = 450.f;
static const float STATUS_BAR_HEIGHT = 35.f;

// Boss状态栏
static const float BOSS_BAR_Y = 180.f;
static const float BOSS_BAR_LABEL_Y = 10.f;

// Player状态栏 (已移除血条，只保留标签)
static const float PLAYER_BAR_LABEL_Y = 80.f;

// 残机/符卡图标位置 (紧跟PLAYER标签右侧，左移100像素)
static const float ICON_BASE_X = 1560.f;
static const float ICON_START_Y = 260.f;
static const float ICON_SPACING_Y = 250.f;

// 难度显示位置 (右顶格，y方向居中，左移75像素)
static const float DIFFICULTY_X = 1425.f;
static const float DIFFICULTY_Y = 720.f;

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
    // ========== 绘制状态栏 ==========

    // Boss血条标签
    sf::Text bossLabel("BOSS", m_font, 50);
    bossLabel.setFillColor(sf::Color::Yellow);
    bossLabel.setOutlineColor(sf::Color::Black);
    bossLabel.setOutlineThickness(1.f);
    bossLabel.setPosition(STATUS_BAR_X - 100.f, BOSS_BAR_LABEL_Y);
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
    sf::Text bossHpText(bossOss.str(), m_font, 24);
    bossHpText.setFillColor(sf::Color::White);
    bossHpText.setOutlineColor(sf::Color::Black);
    bossHpText.setOutlineThickness(1.f);
    bossHpText.setPosition(STATUS_BAR_X + STATUS_BAR_WIDTH - 120.f, BOSS_BAR_Y + 6.f);
    window.draw(bossHpText);

    // ========== 原有UI ==========

    // 残机图标和数字 - 右上角
    for (int i = 0; i < m_stats.life && i < 5; ++i) {
        sf::Sprite heart;
        heart.setTexture(m_lifeTexture);
        heart.setTextureRect(sf::IntRect(LIFE_HEART_X, LIFE_HEART_Y, HEART_W, HEART_H));
        heart.setScale(2.88f, 2.88f);
        heart.setPosition(ICON_BASE_X + i * 100.f, ICON_START_Y);
        window.draw(heart);
    }

    // 符卡图标和数字 - 残机下方
    for (int i = 0; i < m_stats.bombs && i < 5; ++i) {
        sf::Sprite spell;
        spell.setTexture(m_lifeTexture);
        spell.setTextureRect(sf::IntRect(SPELL_HEART_X, SPELL_HEART_Y, HEART_W, HEART_H));
        spell.setScale(2.88f, 2.88f);
        spell.setPosition(ICON_BASE_X + i * 100.f, ICON_START_Y + ICON_SPACING_Y);
        window.draw(spell);
    }

    // 难度显示
    std::string diffName[] = {"EASY", "MEDIUM", "HARD", "LUNATIC"};
    sf::Text diffText(diffName[m_stats.difficulty], m_font, 75);
    diffText.setFillColor(sf::Color::White);  // 白色
    diffText.setOutlineColor(sf::Color::Black);
    diffText.setOutlineThickness(1.f);
    diffText.setPosition(DIFFICULTY_X, DIFFICULTY_Y);
    window.draw(diffText);
}