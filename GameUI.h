#pragma once
#include <SFML/Graphics.hpp>

struct GameStats {
    int life = 5;         // 残机数
    int bombs = 3;        // 符卡数
    int progress = 0;     // 游戏进度（百分比）
    int difficulty = 0;    // 难度 0=EASY, 1=MEDIUM, 2=HARD, 3=LUNATIC

    // Boss血量
    int bossMaxHp = 0;
    int bossHp = 0;

    // 玩家血量
    int playerMaxHp = 100;
    int playerHp = 100;
};

class GameUI {
public:
    GameUI();
    ~GameUI();

    void update(float deltaTime);
    void draw(sf::RenderWindow& window);

    void setStats(const GameStats& stats) { m_stats = stats; }
    GameStats& getStats() { return m_stats; }

    void setBossHp(int hp, int maxHp) { m_stats.bossHp = hp; m_stats.bossMaxHp = maxHp; }
    void setPlayerHp(int hp, int maxHp) { m_stats.playerHp = hp; m_stats.playerMaxHp = maxHp; }

private:
    void loadTextures();

    sf::Texture m_lifeTexture;
    sf::Font m_font;
    GameStats m_stats;
};