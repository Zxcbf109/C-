#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <map>
#include <string>
#include <functional>
#include <array>

// ---- 敌弹状态 ----
enum EnemyBulletState {
    EBS_UNUSED,
    EBS_ACTIVE,
    EBS_DESTROYING,
};

// ---- 敌弹类型 ----
// 使用 assets/dbullet 目录下的贴图
// 普通弹幕（不自动旋转）：assets/dbullet/a, d, f, g, h, j, n, s, v, x
// 自动旋转弹幕：assets/dbullet/aa, ss
enum class EnemyBulletType {
    TYPE_A = 0,    // assets/dbullet/a
    TYPE_D = 1,    // assets/dbullet/d
    TYPE_F = 2,    // assets/dbullet/f
    TYPE_G = 3,    // assets/dbullet/g
    TYPE_H = 4,    // assets/dbullet/h
    TYPE_J = 5,    // assets/dbullet/j
    TYPE_N = 6,    // assets/dbullet/n
    TYPE_S = 7,    // assets/dbullet/s
    TYPE_V = 8,    // assets/dbullet/v
    TYPE_AA = 9,   // assets/dbullet/aa - 自动旋转
    TYPE_SS = 10,  // assets/dbullet/ss - 自动旋转
    LARGE = 11,    // 大型弹
    TYPE_COUNT
};

// ---- 单个敌弹数据 ----
struct EnemyBullet {
    sf::Vector2f position;
    sf::Vector2f velocity;
    float speed;
    float angle;           // 移动方向角度（度）
    float hitboxRadius;    // 碰撞半径
    float renderRadius;    // 渲染半径（可以是不同大小）
    bool is_active = false;
    EnemyBulletState state = EBS_UNUSED;
    EnemyBulletType type = EnemyBulletType::TYPE_A;
    int texFileIndex = 0;  // 该类型下的哪个文件
    float rotation = 0.f;  // 渲染旋转
    float homingRate = 0.f; // 追踪率（用于追踪弹）
    float lifeTime = 0.f;  // 存活时间

    // 旋转动画（用于自动旋转子弹）
    float rotationSpeed = 0.f;
    bool autoRotate = false;  // 是否自动旋转
};

// ---- 敌弹管理器 ----
class EnemyBulletManager {
private:
    static constexpr int MAX_BULLETS = 3000;

    std::vector<EnemyBullet> m_bullets;

    // 弹幕贴图（从assets/dbullet目录加载）
    // 普通子弹：a, d, f, g, h, j, n, s, v (9种类型，不再使用x)
    static constexpr int BULLET_TYPE_COUNT = 9;  // 9种基础类型
    static constexpr int AUTO_ROTATE_TYPE_COUNT = 2;  // aa, ss 自动旋转

    // 基础类型目录（移除x）
    std::string m_bulletDirs[BULLET_TYPE_COUNT] = {"a", "d", "f", "g", "h", "j", "n", "s", "v"};
    // 每个目录下的文件数量
    int m_bulletFileCounts[BULLET_TYPE_COUNT] = {13, 13, 13, 13, 13, 13, 13, 13, 13};

    // 自动旋转类型目录
    std::string m_autoRotateDirs[AUTO_ROTATE_TYPE_COUNT] = {"aa", "ss"};
    int m_autoRotateFileCounts[AUTO_ROTATE_TYPE_COUNT] = {15, 13};

    // 实际加载的贴图 - 按类型分组
    std::vector<std::vector<sf::Texture>> m_bulletTypeTextures;  // m_bulletTypeTextures[type][file]
    std::vector<std::vector<sf::Texture>> m_autoRotateTypeTextures;  // m_autoRotateTypeTextures[type][file]

    sf::Texture m_hitEffectTexture;
    bool m_texturesLoaded = false;

    // 击中特效
    static constexpr int MAX_HIT_EFFECTS = 64;
    struct HitEffect {
        sf::Vector2f position;
        float timer = 0.f;
        bool active = false;
        int frame = 0;
    };
    std::array<HitEffect, MAX_HIT_EFFECTS> m_hitEffects;

    // 回调函数：当子弹击中玩家时调用
    std::function<void()> m_onHitCallback;

    // 内部辅助函数
    EnemyBullet* getInactiveBullet();

public:
    EnemyBulletManager();
    ~EnemyBulletManager() = default;

    // 加载弹幕贴图
    bool loadTextures();
    void setHitCallback(std::function<void()> cb) { m_onHitCallback = cb; }

    // ========== 基础弹幕 ==========
    // 1. 基础单发：固定方向、指定速度发射单颗子弹
    // texType: 0-9 for a/d/f/g/h/j/n/s/v/x, 10-11 for aa/ss
    // texFile: 该类型下的第几个文件
    void spawnSingle(sf::Vector2f pos, float angle, float speed, float radius = 8.f, int texType = 0, int texFile = 0);

    // 2. 多向散射（扇形弹幕）：按角度均匀分布发射N颗子弹
    void spawnFan(sf::Vector2f pos, float baseAngle, int count, float spreadAngle, float speed, float radius = 8.f, int texType = 0);

    // 3. 全向散射（圆形弹幕）：360度均匀分布发射N颗子弹（一圈）
    void spawnCircle(sf::Vector2f pos, int count, float speed, float radius = 8.f);

    // 4. 双圈圆形弹幕：两圈子弹，以不同角度错开
    void spawnDoubleCircle(sf::Vector2f pos, int count, float speed, float radius = 8.f);

    // ========== 螺旋与环形 ==========
    // 5. 螺旋弹幕：子弹随时间递增角度，形成旋转螺旋轨迹
    void spawnSpiral(sf::Vector2f pos, float startAngle, float angleStep, float speed, float radius = 8.f, int texType = 0);

    // 6. 环形弹幕：子弹在圆周上按固定间隔生成，形成完整圆环
    void spawnRing(sf::Vector2f pos, int count, float radius, float speed, float radius2 = 8.f);

    // 7. 旋转环形弹幕：环形弹幕整体随时间旋转
    void spawnRotatingRing(sf::Vector2f pos, int count, float ringRadius, float speed, float rotSpeed, float radius = 8.f);

    // ========== 直线/曲线弹幕阵 ==========
    // 8. 直线弹幕阵：多排子弹平行直线发射
    void spawnGrid(sf::Vector2f pos, int rows, int cols, float spacingX, float spacingY, float speed, float radius = 8.f);

    // 9. 波浪形弹幕：子弹带横向正弦波偏移
    void spawnWave(sf::Vector2f pos, float baseAngle, float speed, float amplitude, float frequency, int count, float radius = 8.f);

    // 10. 交叉弹幕：两组弹幕以不同角度发射
    void spawnCross(sf::Vector2f pos, float angle1, float angle2, int count, float spread, float speed, float radius = 8.f);

    // ========== 追踪与特殊弹幕 ==========
    // 11. 追踪弹：子弹朝向玩家位置飞行
    void spawnHoming(sf::Vector2f pos, float speed, float radius = 8.f, float homingRate = 180.f);

    // 12. 预瞄追踪弹：向预判位置发射
    void spawnPredictiveHoming(sf::Vector2f pos, sf::Vector2f playerPos, sf::Vector2f playerVel, float speed, float radius = 8.f, float homingRate = 180.f);

    // 13. 反弹弹幕：碰到边界后反弹
    void spawnBounce(sf::Vector2f pos, float angle, float speed, int bounceCount = 3, float radius = 8.f);

    // ========== 定时/序列弹幕 ==========
    // 14. 弹幕序列：按固定时间间隔依次发射不同类型的弹幕
    void spawnSequence(sf::Vector2f pos, float baseAngle, float speed, float interval, int count, float radius = 8.f);

    // 15. 弹幕波次：多轮弹幕按波次发射，支持数量递增/递减
    void spawnWave(sf::Vector2f pos, float baseAngle, float speed, int waves, int startCount, int endCount, float waveInterval, float spreadAngle, float radius = 8.f);

    // 16. 随机弹幕：在指定角度/速度范围内随机发射
    void spawnRandom(sf::Vector2f pos, float minAngle, float maxAngle, float minSpeed, float maxSpeed, int count, float radius = 8.f);

    // ========== 更新与渲染 ==========
    void update(float deltaTime, const sf::Vector2u& windowSize, sf::Vector2f playerPos);
    void render(sf::RenderWindow& window);
    int getActiveCount() const;
    std::vector<EnemyBullet>& getActiveBullets() { return m_bullets; }
    const std::vector<EnemyBullet>& getActiveBullets() const { return m_bullets; }
    void clearAll();
    bool checkPlayerCollision(sf::Vector2f playerPos, float playerRadius);

    // 触发击中特效
    void triggerHitEffect(sf::Vector2f pos);

    // 获取贴图总数
    int getTotalTextureCount() const;
    int getAutoRotateTextureCount() const;
};