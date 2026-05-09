#pragma once
#include <memory>
#include <map>
#include "Animation.h"
#include "PlayerBullet.h"
#include "BombEffect.h"
#include "EnemyBullet.h"
#include "Boss.h"

// ---- 动画状态 ----
enum class PlayerAnimState {
    IDLE,
    LEFT,
    RIGHT,
    MOVE_LEFT,
    MOVE_RIGHT,
    MOVE_FORWARD
};

class Player {
public:
    Player();
    ~Player();

    void update(float deltaTime, const sf::Vector2u& windowSize);
    void draw(sf::RenderWindow& window);

    static constexpr float GAME_WIDTH = 960.f;
    static constexpr float GAME_HEIGHT = 960.f;
    static constexpr float PLAYER_HALF_WIDTH = 16.0f;

    // 判定点（用于碰撞检测）
    static constexpr float HITBOX_RADIUS = 6.0f;  // 判定圈半径，缩放2倍后为12像素

    sf::Vector2f getPosition() const { return m_sprite.getPosition(); }
    sf::Vector2f getHitboxCenter() const {
        sf::Vector2f pos = m_sprite.getPosition();
        return sf::Vector2f(pos.x, pos.y + 10.f);  // 判定点略微偏下
    }
    float getHitboxRadius() const { return HITBOX_RADIUS; }
    bool isFocused() const { return m_isFocused; }

    PlayerBulletManager* getBulletManager() { return m_bulletManager.get(); }

    int getLife() const { return m_life; }
    int getBombs() const { return m_bombs; }
    bool isAlive() const { return m_life > 0; }

    void useBomb();
    void takeDamage();

    // 重置玩家状态（用于重新开始）
    void resetPlayerState();

    // 获取当前弹幕管理器
    EnemyBulletManager* getEnemyBulletManager() { return m_enemyBulletManager; }
    void setEnemyBulletManager(EnemyBulletManager* mgr) { m_enemyBulletManager = mgr; }

    // 追踪弹发射模式
    void setTrackingShot(bool enable) { m_trackingShotEnabled = enable; }

    // 设置追踪目标位置（用于玩家弹幕追踪Boss）
    void setTrackingTarget(sf::Vector2f target) { m_trackingTargetPos = target; }
    bool isTrackingShotEnabled() const { return m_trackingShotEnabled; }

    // 受到敌人弹幕伤害
    void hitByBullet();
    void hitByLaser();

    // 无敌时间
    float getInvincibleTime() const { return m_invincibleTimer; }

private:
    sf::Texture m_texture;
    sf::Sprite m_sprite;
    PlayerAnimState m_animState = PlayerAnimState::IDLE;
    PlayerAnimState m_prevAnimState = PlayerAnimState::IDLE;

    // ---- 生命和符卡 ----
    int m_life = 3;    // 初始3条命
    int m_bombs = 3;   // 初始3个符卡

    // ---- 动画系统 ----
    std::map<std::string, Animation> m_animations;
    std::string m_currentAnimName = "idle";
    float m_animTimer = 0.f;
    int m_currentFrameIndex = 0;
    bool m_animFinished = false;  // 用于非循环动画

    // ---- 射击系统 ----
    std::unique_ptr<PlayerBulletManager> m_bulletManager;
    float m_shootCooldown = 0.f;
    float m_shootInterval = 0.05f;
    bool m_isShooting = false;
    bool m_shootType = false;
    bool m_bombKeyHeld = false;

    // ---- Focus状态（用于显示判定点）----
    bool m_isFocused = false;

    // ---- 判定点图形 ----
    sf::CircleShape m_hitboxIndicator;

    // ---- 符卡系统 ----
    BombSystem m_bombSystem;

    // ---- 敌弹管理器引用 ----
    EnemyBulletManager* m_enemyBulletManager = nullptr;

    // ---- 追踪弹模式 ----
    bool m_trackingShotEnabled = false;

    // ---- 追踪目标位置（用于玩家弹幕追踪Boss）----
    sf::Vector2f m_trackingTargetPos;

    // ---- 无敌时间 ----
    float m_invincibleTimer = 0.f;
    static constexpr float INVINCIBLE_DURATION = 2.f;

    void initAnimations();
    void playAnimation(const std::string& name);
    void updateAnimation(float deltaTime);
    void handleShooting(float deltaTime);
    void checkBulletCollisions();
};