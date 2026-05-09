#pragma once
#include <vector>
#include <string>
#include <map>
#include "Animation.h"
#include "EnemyBullet.h"
#include "PlayerBullet.h"

// ---- Boss状态 ----
enum class BossState {
    ENTERING,    // 入场
    FIGHTING,    // 战斗中
    SPELL_CARD,  // 符卡攻击
    DEFEATED,    // 被击败
    EXITING,     // 退场
};

// ---- Boss动画状态 ----
enum class BossAnimState {
    IDLE,
    LEFT,
    RIGHT,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_FORWARD
};

// ---- 攻击阶段 ----
struct AttackPhase {
    std::string name;           // 符卡名
    float duration;             // 持续时间
    int patternType;            // 攻击模式类型
    float bulletSpeed;          // 弹幕速度
    float bulletInterval;       // 弹幕发射间隔
    int bulletCountPerWave;     // 每波弹幕数量
    float spreadAngle;         // 扇形展开角度
    bool isSpellCard;           // 是否为符卡（需要全部躲避）
};

// ---- Boss类 ----
class Boss {
public:
    Boss(const sf::Texture* texture);
    ~Boss() = default;

    void init(int hp, const sf::Vector2f& startPos, const sf::Vector2f& targetPos);
    void setDifficultyParams(float bulletSpeedMult, float hpMult);
    void update(float deltaTime, sf::Vector2f playerPos, EnemyBulletManager* bulletManager);
    void render(sf::RenderWindow& window);

    sf::Vector2f getPosition() const { return m_sprite.getPosition(); }
    float getHitboxRadius() const { return m_hitboxRadius; }
    bool isAlive() const { return m_hp > 0 && m_state != BossState::DEFEATED; }
    bool canBeHit() const { return m_state == BossState::FIGHTING || m_state == BossState::SPELL_CARD; }
    void takeDamage(int damage);
    std::string getCurrentSpellName() const;
    bool isInSpellCard() const { return m_state == BossState::SPELL_CARD; }
    float getHpPercent() const { return static_cast<float>(m_hp) / static_cast<float>(m_maxHp); }
    int getHp() const { return m_hp; }
    int getMaxHp() const { return m_maxHp; }

private:
    sf::Texture const* m_texture;
    sf::Sprite m_sprite;
    BossAnimState m_animState = BossAnimState::IDLE;
    BossAnimState m_prevAnimState = BossAnimState::IDLE;

    int m_maxHp;
    int m_hp;
    float m_hitboxRadius = 30.f;
    float m_bulletSpeedMult = 1.0f;
    float m_hpMult = 1.0f;

    BossState m_state = BossState::ENTERING;
    float m_stateTimer = 0.f;

    sf::Vector2f m_startPos;
    sf::Vector2f m_targetPos;
    float m_moveSpeed = 100.f;

    std::vector<AttackPhase> m_phases;
    int m_currentPhase = 0;
    float m_phaseTimer = 0.f;

    float m_attackTimer = 0.f;

    std::string m_currentSpellName;

    // ---- 动画系统 ----
    std::map<std::string, Animation> m_animations;
    std::string m_currentAnimName = "idle";
    float m_animTimer = 0.f;
    int m_currentFrameIndex = 0;
    bool m_animFinished = false;

    sf::Vector2f m_prevPosition;
    sf::Vector2f m_velocity;

    float m_moveAngle = 0.f;
    float m_moveAmplitude = 50.f;
    float m_moveFrequency = 1.f;

    void initAnimations();
    void playAnimation(const std::string& name);
    void updateAnimation(float deltaTime);

    void updateEntering(float deltaTime, EnemyBulletManager* bulletManager);
    void updateFighting(float deltaTime, sf::Vector2f playerPos, EnemyBulletManager* bulletManager);
    void updateDefeated(float deltaTime);

    void executeAttackPattern(int patternType, sf::Vector2f playerPos, EnemyBulletManager* bulletManager);

    // 15种弹幕发射模式
    void patternDoubleCircle(sf::Vector2f playerPos, EnemyBulletManager* bulletManager);   // 0: 双圈圆形
    void patternFan(sf::Vector2f playerPos, EnemyBulletManager* bulletManager);           // 1: 扇形弹幕
    void patternHoming(sf::Vector2f playerPos, EnemyBulletManager* bulletManager);        // 2: 追踪弹
    void patternCircle(sf::Vector2f playerPos, EnemyBulletManager* bulletManager);        // 3: 单圈圆形
    void patternPredictiveHoming(sf::Vector2f playerPos, EnemyBulletManager* bulletManager); // 4: 预瞄追踪弹
    void patternRotatingRing(sf::Vector2f playerPos, EnemyBulletManager* bulletManager);   // 5: 旋转环形
    void patternWave(sf::Vector2f playerPos, EnemyBulletManager* bulletManager);           // 6: 波浪弹幕
    void patternDoubleFan(sf::Vector2f playerPos, EnemyBulletManager* bulletManager);     // 7: 双方向扇形
    void patternRandom(sf::Vector2f playerPos, EnemyBulletManager* bulletManager);        // 8: 随机弹幕
    void patternSequence(sf::Vector2f playerPos, EnemyBulletManager* bulletManager);       // 9: 弹幕序列
    void patternLargeHoming(sf::Vector2f playerPos, EnemyBulletManager* bulletManager);   // 10: 大型追踪弹
    void patternRing(sf::Vector2f playerPos, EnemyBulletManager* bulletManager);          // 11: 环形弹幕
    void patternCombo(sf::Vector2f playerPos, EnemyBulletManager* bulletManager);         // 12: 组合弹幕
    void patternSpiral(sf::Vector2f playerPos, EnemyBulletManager* bulletManager);         // 13: 螺旋弹幕
    void patternDenseFan(sf::Vector2f playerPos, EnemyBulletManager* bulletManager);      // 14: 密集扇形
};

// ---- Boss管理器 ----
class BossManager {
private:
    static constexpr int MAX_BOSSES = 2;
    std::vector<Boss> m_bosses;
    sf::Texture m_bossTexture;  // 使用玩家角色贴图 reimu.png

public:
    BossManager();
    ~BossManager() = default;

    bool initTexture();
    Boss* createBoss(int hp, const sf::Vector2f& startPos, const sf::Vector2f& targetPos, int bossIndex = 0);
    sf::Texture& getBossTexture(int index) { return m_bossTexture; }
    void update(float deltaTime, sf::Vector2f playerPos, EnemyBulletManager* bulletManager);
    void render(sf::RenderWindow& window);
    void checkPlayerBullets(const PlayerBullet* playerBullets, int bulletCount, float playerBulletRadius);
    void clearAll();
    bool hasAliveBoss() const;
    int getBossCount() const { return static_cast<int>(m_bosses.size()); }
    Boss* getBoss(int index);
};