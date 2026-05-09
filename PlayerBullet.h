#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <string>

// ---- 玩家弹幕状态 ----
enum PlayerBulletState {
    PBS_UNUSED,
    PBS_ACTIVE,
    PBS_DESTROYING,
};

// ---- 弹幕动画帧 ----
struct BulletAnimFrame {
    sf::IntRect rect;
    float duration;
};

// ---- 弹幕配置（从 player_config.json 读取）----
struct BulletConfig {
    std::string sprite_name;      // 贴图名称（pl00）
    sf::IntRect active_rect;      // 激活状态贴图区域
    sf::Vector2f hit_box;        // 碰撞箱大小
    std::string destroy_sprite;   // 销毁动画贴图
    std::vector<BulletAnimFrame> destroy_frames;  // 销毁动画帧
    float total_destroy_duration = 0.f;
    bool random_rotation = false;
};

// ---- 单个弹幕数据 ----
struct PlayerBullet {
    sf::Vector2f position;       // 世界坐标
    sf::Vector2f velocity;       // 速度向量
    sf::Vector2f hit_box;        // 碰撞箱
    float angle;                 // 角度（度，向上为0）
    float speed;                 // 速度大小
    float angle_acc = 0.f;       // 角度加速度（用于追踪）
    float destroy_timer = 0.f;
    bool is_active = false;
    bool is_homing = false;      // 是否追踪
    float homing_rate = 0.f;     // 追踪率（度/秒）
    sf::Vector2f homing_target; // 追踪目标位置
    PlayerBulletState state = PBS_UNUSED;
    const BulletConfig* config = nullptr;
    int current_anim_frame = 0;
    float current_anim_timer = 0.f;
    float rotation = 0.f;        // 渲染时的旋转角度
};

// ---- 玩家弹幕管理器 ----
class PlayerBulletManager {
private:
    std::map<std::string, BulletConfig> m_configs;
    std::vector<PlayerBullet> m_bullets;
    sf::Texture& m_texture;
    static constexpr int MAX_BULLETS = 512;
    sf::Uint8 m_fadeAlpha = 255;  // For bomb effect fade out

    void InitDefaultConfigs();

public:
    PlayerBulletManager(sf::Texture& tex);
    ~PlayerBulletManager() = default;

    // 生成弹幕
    void Spawn(const std::string& type, sf::Vector2f pos, float angle, float speed);

    // 生成追踪弹幕
    void SpawnHoming(const std::string& type, sf::Vector2f pos, float angle, float speed, float homing_rate, sf::Vector2f target);

    // 更新所有弹幕
    void Update(float deltaTime, sf::Vector2u windowSize, sf::Vector2f homingTarget = sf::Vector2f(0.f, 0.f));

    // 渲染所有弹幕
    void Render(sf::RenderWindow& window);

    // 获取弹幕数量（调试用）
    int GetActiveCount() const;

    // 清空所有弹幕
    void clearAll();

    // 获取所有弹幕（用于碰撞检测）
    PlayerBullet* getBullets() { return m_bullets.data(); }
    const PlayerBullet* getBullets() const { return m_bullets.data(); }

    // 设置所有弹幕淡出透明度
    void setFadeAlpha(sf::Uint8 alpha);
};
