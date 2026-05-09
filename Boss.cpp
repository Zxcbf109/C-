#include "Boss.h"
#include "PlayerBullet.h"
#include "SoundManager.h"
#include <cmath>

Boss::Boss(const sf::Texture* texture) : m_texture(texture) {
    if (m_texture) {
        m_sprite.setTexture(*m_texture);
        m_sprite.setScale(2.f, 2.f);
    }

    initAnimations();
    playAnimation("idle");

    // 初始化攻击阶段 - 优化弹幕密度，增加一圈/两圈放射式和追踪弹
    m_phases = {
        // Phase 0: 双圈圆形弹幕 - 两圈均匀分布
        {"符卡「双圈弹」", 4.f, 0, 200.f, 0.4f, 8, 0.f, false},
        // Phase 1: 扇形弹幕 - 较少数量
        {"符卡「扇形弹」", 3.5f, 1, 220.f, 0.35f, 5, 80.f, false},
        // Phase 2: 追踪弹 - 向玩家方向发射
        {"符卡「追踪弹」", 4.f, 2, 280.f, 0.4f, 3, 0.f, false},
        // Phase 3: 一圈圆形弹幕
        {"符卡「单圈弹」", 3.f, 3, 180.f, 0.5f, 10, 360.f, false},
        // Phase 4: 预瞄追踪弹
        {"符卡「预瞄弹」", 4.f, 4, 300.f, 0.5f, 3, 0.f, false},
        // Phase 5: 旋转环形弹幕（使用自动旋转贴图）
        {"符卡「旋转环」", 5.f, 5, 180.f, 0.35f, 8, 0.f, false},
        // Phase 6: 波浪弹幕 - 较少数量
        {"符卡「波浪弹」", 3.5f, 6, 200.f, 0.4f, 6, 60.f, false},
        // Phase 7: 双方向扇形
        {"符卡「双向弹」", 4.f, 7, 240.f, 0.35f, 5, 60.f, false},
        // Phase 8: 随机弹幕
        {"符卡「随机弹」", 3.5f, 8, 260.f, 0.3f, 8, 0.f, false},
        // Phase 9: 弹幕序列
        {"符卡「序列弹」", 4.f, 9, 220.f, 0.35f, 8, 0.f, false},
        // Phase 10: 大型追踪弹
        {"符卡「大型追踪」", 4.5f, 10, 200.f, 0.5f, 2, 0.f, false},
        // Phase 11: 环形弹幕
        {"符卡「环形弹」", 4.f, 11, 160.f, 0.45f, 10, 0.f, false},
        // Phase 12: 双圈+单发组合
        {"符卡「组合弹」", 5.f, 12, 200.f, 0.4f, 12, 0.f, false},
        // Phase 13: 螺旋弹幕
        {"符卡「螺旋弹」", 4.5f, 13, 250.f, 0.25f, 6, 0.f, false},
        // Phase 14: 密集扇形
        {"符卡「密集扇形」", 3.f, 14, 180.f, 0.5f, 7, 120.f, false},
    };
}

void Boss::initAnimations() {
    // ---- idle: y=0, 4帧，每帧0.1秒 ----
    {
        Animation anim;
        anim.name = "idle";
        anim.loop = true;
        anim.frames = {
            {sf::IntRect(0, 0, 32, 48), 0.1f, 16.f, 24.f},
            {sf::IntRect(32, 0, 32, 48), 0.1f, 16.f, 24.f},
            {sf::IntRect(64, 0, 32, 48), 0.1f, 16.f, 24.f},
            {sf::IntRect(96, 0, 32, 48), 0.1f, 16.f, 24.f}
        };
        for (auto& f : anim.frames) anim.totalDuration += f.duration;
        m_animations["idle"] = anim;
    }

    // ---- move_forword: y=0, 4帧，每帧0.1秒 ----
    {
        Animation anim;
        anim.name = "move_forword";
        anim.loop = true;
        anim.frames = {
            {sf::IntRect(128, 0, 32, 48), 0.1f, 16.f, 24.f},
            {sf::IntRect(160, 0, 32, 48), 0.1f, 16.f, 24.f},
            {sf::IntRect(192, 0, 32, 48), 0.1f, 16.f, 24.f},
            {sf::IntRect(224, 0, 32, 48), 0.1f, 16.f, 24.f}
        };
        for (auto& f : anim.frames) anim.totalDuration += f.duration;
        m_animations["move_forword"] = anim;
    }

    // ---- left (转身): y=48, 4帧，每帧0.05秒 ----
    {
        Animation anim;
        anim.name = "left";
        anim.loop = false;
        anim.frames = {
            {sf::IntRect(0, 48, 32, 48), 0.05f, 16.f, 24.f},
            {sf::IntRect(32, 48, 32, 48), 0.05f, 16.f, 24.f},
            {sf::IntRect(64, 48, 32, 48), 0.05f, 16.f, 24.f},
            {sf::IntRect(96, 48, 32, 48), 0.05f, 16.f, 24.f}
        };
        for (auto& f : anim.frames) anim.totalDuration += f.duration;
        m_animations["left"] = anim;
    }

    // ---- move_left (向左移动): y=48, 4帧，每帧0.1秒 ----
    {
        Animation anim;
        anim.name = "move_left";
        anim.loop = true;
        anim.frames = {
            {sf::IntRect(128, 48, 32, 48), 0.1f, 16.f, 24.f},
            {sf::IntRect(160, 48, 32, 48), 0.1f, 16.f, 24.f},
            {sf::IntRect(192, 48, 32, 48), 0.1f, 16.f, 24.f},
            {sf::IntRect(224, 48, 32, 48), 0.1f, 16.f, 24.f}
        };
        for (auto& f : anim.frames) anim.totalDuration += f.duration;
        m_animations["move_left"] = anim;
    }

    // ---- right (转身): y=96, 4帧，每帧0.05秒 ----
    {
        Animation anim;
        anim.name = "right";
        anim.loop = false;
        anim.frames = {
            {sf::IntRect(0, 96, 32, 48), 0.05f, 16.f, 24.f},
            {sf::IntRect(32, 96, 32, 48), 0.05f, 16.f, 24.f},
            {sf::IntRect(64, 96, 32, 48), 0.05f, 16.f, 24.f},
            {sf::IntRect(96, 96, 32, 48), 0.05f, 16.f, 24.f}
        };
        for (auto& f : anim.frames) anim.totalDuration += f.duration;
        m_animations["right"] = anim;
    }

    // ---- move_right (向右移动): y=96, 4帧，每帧0.1秒 ----
    {
        Animation anim;
        anim.name = "move_right";
        anim.loop = true;
        anim.frames = {
            {sf::IntRect(128, 96, 32, 48), 0.1f, 16.f, 24.f},
            {sf::IntRect(160, 96, 32, 48), 0.1f, 16.f, 24.f},
            {sf::IntRect(192, 96, 32, 48), 0.1f, 16.f, 24.f},
            {sf::IntRect(224, 96, 32, 48), 0.1f, 16.f, 24.f}
        };
        for (auto& f : anim.frames) anim.totalDuration += f.duration;
        m_animations["move_right"] = anim;
    }
}

void Boss::playAnimation(const std::string& name) {
    if (m_currentAnimName == name) return;

    auto it = m_animations.find(name);
    if (it == m_animations.end()) return;

    m_currentAnimName = name;
    m_animTimer = 0.f;
    m_currentFrameIndex = 0;
    m_animFinished = false;

    const Animation& anim = it->second;
    if (!anim.frames.empty()) {
        const AnimFrame& frame = anim.frames[0];
        m_sprite.setTextureRect(frame.rect);
        m_sprite.setOrigin(frame.anchorX, frame.anchorY);
    }
}

void Boss::updateAnimation(float deltaTime) {
    auto it = m_animations.find(m_currentAnimName);
    if (it == m_animations.end()) return;

    const Animation& anim = it->second;
    if (anim.frames.empty()) return;

    // 非循环动画播完后保持最后一帧
    if (m_animFinished) return;

    m_animTimer += deltaTime;

    // 累计时间找到当前帧
    float accumulated = 0.f;
    int frameIndex = 0;
    for (size_t i = 0; i < anim.frames.size(); ++i) {
        accumulated += anim.frames[i].duration;
        if (m_animTimer < accumulated) {
            frameIndex = static_cast<int>(i);
            break;
        }
        if (i == anim.frames.size() - 1) {
            if (anim.loop) {
                m_animTimer -= anim.totalDuration;
                frameIndex = 0;
            } else {
                m_animFinished = true;
                frameIndex = static_cast<int>(anim.frames.size() - 1);
            }
        }
    }

    if (frameIndex != m_currentFrameIndex) {
        m_currentFrameIndex = frameIndex;
        const AnimFrame& frame = anim.frames[frameIndex];
        m_sprite.setTextureRect(frame.rect);
        m_sprite.setOrigin(frame.anchorX, frame.anchorY);
    }
}

void Boss::init(int hp, const sf::Vector2f& startPos, const sf::Vector2f& targetPos) {
    m_maxHp = hp;
    m_hp = hp;
    m_startPos = startPos;
    m_targetPos = targetPos;
    m_state = BossState::ENTERING;
    m_stateTimer = 0.f;
    m_currentPhase = 0;
    m_phaseTimer = 0.f;
    m_attackTimer = 0.f;

    m_sprite.setPosition(startPos);
    m_prevPosition = startPos;
    m_velocity = sf::Vector2f(0.f, 0.f);

    playAnimation("idle");
}

void Boss::setDifficultyParams(float bulletSpeedMult, float hpMult) {
    m_bulletSpeedMult = bulletSpeedMult;
    m_hpMult = hpMult;
    m_maxHp = static_cast<int>(m_maxHp * hpMult);
    m_hp = m_maxHp;
}

void Boss::update(float deltaTime, sf::Vector2f playerPos, EnemyBulletManager* bulletManager) {
    m_stateTimer += deltaTime;

    switch (m_state) {
        case BossState::ENTERING:
            updateEntering(deltaTime, bulletManager);
            break;
        case BossState::FIGHTING:
        case BossState::SPELL_CARD:
            updateFighting(deltaTime, playerPos, bulletManager);
            break;
        case BossState::DEFEATED:
            updateDefeated(deltaTime);
            break;
        case BossState::EXITING:
            m_sprite.move(0.f, 200.f * deltaTime);
            break;
    }

    // 更新动画
    updateAnimation(deltaTime);

    // 计算速度用于动画状态判断
    sf::Vector2f pos = m_sprite.getPosition();
    m_velocity = pos - m_prevPosition;
    m_prevPosition = pos;

    // 根据移动方向更新动画状态
    float velX = m_velocity.x;
    float velThreshold = 0.5f;

    if (m_state == BossState::ENTERING) {
        playAnimation("move_forword");
    } else if (m_state == BossState::DEFEATED) {
        playAnimation("idle");
    } else if (m_state == BossState::EXITING) {
        playAnimation("move_forword");
    } else {
        // 战斗中：根据水平移动方向选择动画
        if (velX < -velThreshold) {
            // 向左移动
            if (m_animState != BossAnimState::MOVE_LEFT) {
                m_animState = BossAnimState::MOVE_LEFT;
                playAnimation("move_left");
            }
        } else if (velX > velThreshold) {
            // 向右移动
            if (m_animState != BossAnimState::MOVE_RIGHT) {
                m_animState = BossAnimState::MOVE_RIGHT;
                playAnimation("move_right");
            }
        } else {
            // 基本静止或微小移动
            if (m_animState != BossAnimState::IDLE) {
                m_animState = BossAnimState::IDLE;
                playAnimation("idle");
            }
        }
    }
}

void Boss::updateEntering(float deltaTime, EnemyBulletManager* bulletManager) {
    sf::Vector2f pos = m_sprite.getPosition();
    sf::Vector2f dir = m_targetPos - pos;
    float dist = std::sqrt(dir.x * dir.x + dir.y * dir.y);

    if (dist > 5.f) {
        float speed = 150.f * deltaTime;
        pos += dir * (speed / dist);
        m_sprite.setPosition(pos);
    } else {
        m_state = BossState::FIGHTING;
        m_stateTimer = 0.f;
        m_currentPhase = 0;
        m_animState = BossAnimState::IDLE;
    }
}

void Boss::updateFighting(float deltaTime, sf::Vector2f playerPos, EnemyBulletManager* bulletManager) {
    // 移动模式：8字形移动
    sf::Vector2f pos = m_sprite.getPosition();
    float targetX = m_targetPos.x + std::sin(m_stateTimer * 0.5f) * 150.f;
    float targetY = m_targetPos.y + std::sin(m_stateTimer * 1.f) * 50.f;
    pos.x += (targetX - pos.x) * 0.015f;
    pos.y += (targetY - pos.y) * 0.01f;
    m_sprite.setPosition(pos);

    // 攻击
    if (m_currentPhase < static_cast<int>(m_phases.size())) {
        const AttackPhase& phase = m_phases[m_currentPhase];
        m_phaseTimer += deltaTime;

        m_attackTimer += deltaTime;
        if (m_attackTimer >= phase.bulletInterval) {
            m_attackTimer = 0.f;
            executeAttackPattern(phase.patternType, playerPos, bulletManager);
        }

        // 符卡时间到，切换到下一阶段
        if (m_phaseTimer >= phase.duration) {
            m_phaseTimer = 0.f;
            m_currentPhase++;
            m_attackTimer = 0.f;

            if (m_currentPhase >= static_cast<int>(m_phases.size())) {
                m_currentPhase = 0;
            }
        }
    }
}

void Boss::updateDefeated(float deltaTime) {
    m_stateTimer += deltaTime;
    if (m_stateTimer > 3.f) {
        m_state = BossState::EXITING;
    }
}

void Boss::executeAttackPattern(int patternType, sf::Vector2f playerPos, EnemyBulletManager* bulletManager) {
    switch (patternType) {
        case 0: patternDoubleCircle(playerPos, bulletManager); break;     // 双圈圆形
        case 1: patternFan(playerPos, bulletManager); break;             // 扇形
        case 2: patternHoming(playerPos, bulletManager); break;          // 追踪弹
        case 3: patternCircle(playerPos, bulletManager); break;          // 单圈圆形
        case 4: patternPredictiveHoming(playerPos, bulletManager); break; // 预瞄追踪
        case 5: patternRotatingRing(playerPos, bulletManager); break;     // 旋转环形
        case 6: patternWave(playerPos, bulletManager); break;            // 波浪
        case 7: patternDoubleFan(playerPos, bulletManager); break;        // 双方向扇形
        case 8: patternRandom(playerPos, bulletManager); break;          // 随机
        case 9: patternSequence(playerPos, bulletManager); break;          // 序列
        case 10: patternLargeHoming(playerPos, bulletManager); break;    // 大型追踪
        case 11: patternRing(playerPos, bulletManager); break;           // 环形
        case 12: patternCombo(playerPos, bulletManager); break;          // 组合
        case 13: patternSpiral(playerPos, bulletManager); break;         // 螺旋
        case 14: patternDenseFan(playerPos, bulletManager); break;       // 密集扇形
    }
}

// 0: 双圈圆形弹幕
void Boss::patternDoubleCircle(sf::Vector2f playerPos, EnemyBulletManager* bulletManager) {
    const AttackPhase& phase = m_phases[m_currentPhase];
    sf::Vector2f pos = m_sprite.getPosition();
    bulletManager->spawnDoubleCircle(pos, phase.bulletCountPerWave, phase.bulletSpeed * m_bulletSpeedMult, 8.f);
}

// 1: 扇形弹幕
void Boss::patternFan(sf::Vector2f playerPos, EnemyBulletManager* bulletManager) {
    const AttackPhase& phase = m_phases[m_currentPhase];
    sf::Vector2f pos = m_sprite.getPosition();
    bulletManager->spawnFan(pos, 90.f, phase.bulletCountPerWave, phase.spreadAngle, phase.bulletSpeed * m_bulletSpeedMult, 8.f, 0);
}

// 2: 追踪弹
void Boss::patternHoming(sf::Vector2f playerPos, EnemyBulletManager* bulletManager) {
    const AttackPhase& phase = m_phases[m_currentPhase];
    sf::Vector2f pos = m_sprite.getPosition();

    sf::Vector2f dir = playerPos - pos;
    float angle = std::atan2(dir.y, dir.x) * 180.f / 3.14159f;

    for (int i = 0; i < phase.bulletCountPerWave; ++i) {
        float offset = (i - phase.bulletCountPerWave / 2) * 10.f;
        // texType = i % 10 遍历所有10种类型，texFile = 0
        bulletManager->spawnSingle(pos, angle + offset, phase.bulletSpeed * m_bulletSpeedMult, 8.f, i % 9, 0);
    }
}

// 3: 单圈圆形弹幕
void Boss::patternCircle(sf::Vector2f playerPos, EnemyBulletManager* bulletManager) {
    const AttackPhase& phase = m_phases[m_currentPhase];
    sf::Vector2f pos = m_sprite.getPosition();
    bulletManager->spawnCircle(pos, phase.bulletCountPerWave, phase.bulletSpeed * m_bulletSpeedMult, 8.f);
}

// 4: 预瞄追踪弹
void Boss::patternPredictiveHoming(sf::Vector2f playerPos, EnemyBulletManager* bulletManager) {
    const AttackPhase& phase = m_phases[m_currentPhase];
    sf::Vector2f pos = m_sprite.getPosition();
    sf::Vector2f playerVel(0.f, 0.f);
    for (int i = 0; i < phase.bulletCountPerWave; ++i) {
        bulletManager->spawnPredictiveHoming(pos, playerPos, playerVel, phase.bulletSpeed * m_bulletSpeedMult, 8.f, 100.f);
    }
}

// 5: 旋转环形弹幕
void Boss::patternRotatingRing(sf::Vector2f playerPos, EnemyBulletManager* bulletManager) {
    const AttackPhase& phase = m_phases[m_currentPhase];
    sf::Vector2f pos = m_sprite.getPosition();
    static float rotSpeed = 30.f;
    rotSpeed += 5.f * 0.016f;
    bulletManager->spawnRotatingRing(pos, phase.bulletCountPerWave, 40.f, phase.bulletSpeed * m_bulletSpeedMult, rotSpeed, 8.f);
}

// 6: 波浪弹幕
void Boss::patternWave(sf::Vector2f playerPos, EnemyBulletManager* bulletManager) {
    const AttackPhase& phase = m_phases[m_currentPhase];
    sf::Vector2f pos = m_sprite.getPosition();
    bulletManager->spawnWave(pos, 90.f, phase.bulletSpeed * m_bulletSpeedMult, 30.f, 0.5f, phase.bulletCountPerWave, 8.f);
}

// 7: 双方向扇形
void Boss::patternDoubleFan(sf::Vector2f playerPos, EnemyBulletManager* bulletManager) {
    const AttackPhase& phase = m_phases[m_currentPhase];
    sf::Vector2f pos = m_sprite.getPosition();
    bulletManager->spawnFan(pos, 70.f, phase.bulletCountPerWave, phase.spreadAngle, phase.bulletSpeed * m_bulletSpeedMult, 8.f, 0);
    bulletManager->spawnFan(pos, 110.f, phase.bulletCountPerWave, phase.spreadAngle, phase.bulletSpeed * m_bulletSpeedMult, 8.f, 2);
}

// 8: 随机弹幕
void Boss::patternRandom(sf::Vector2f playerPos, EnemyBulletManager* bulletManager) {
    const AttackPhase& phase = m_phases[m_currentPhase];
    sf::Vector2f pos = m_sprite.getPosition();
    bulletManager->spawnRandom(pos, 45.f, 135.f, phase.bulletSpeed * 0.7f, phase.bulletSpeed * 1.3f, phase.bulletCountPerWave, 8.f);
}

// 9: 弹幕序列
void Boss::patternSequence(sf::Vector2f playerPos, EnemyBulletManager* bulletManager) {
    const AttackPhase& phase = m_phases[m_currentPhase];
    sf::Vector2f pos = m_sprite.getPosition();
    bulletManager->spawnSequence(pos, 90.f, phase.bulletSpeed * m_bulletSpeedMult, 0.1f, phase.bulletCountPerWave, 8.f);
}

// 10: 大型追踪弹
void Boss::patternLargeHoming(sf::Vector2f playerPos, EnemyBulletManager* bulletManager) {
    const AttackPhase& phase = m_phases[m_currentPhase];
    sf::Vector2f pos = m_sprite.getPosition();
    for (int i = 0; i < phase.bulletCountPerWave; ++i) {
        // 使用类型0-9的后面几个文件（更大的子弹）
        bulletManager->spawnSingle(pos, 90.f, phase.bulletSpeed * m_bulletSpeedMult, 15.f, i % 9, 4 + i % 3);
    }
}

// 11: 环形弹幕
void Boss::patternRing(sf::Vector2f playerPos, EnemyBulletManager* bulletManager) {
    const AttackPhase& phase = m_phases[m_currentPhase];
    sf::Vector2f pos = m_sprite.getPosition();
    bulletManager->spawnRing(pos, phase.bulletCountPerWave, 30.f, phase.bulletSpeed * m_bulletSpeedMult, 8.f);
}

// 12: 组合弹幕（双圈+单发）
void Boss::patternCombo(sf::Vector2f playerPos, EnemyBulletManager* bulletManager) {
    const AttackPhase& phase = m_phases[m_currentPhase];
    sf::Vector2f pos = m_sprite.getPosition();
    bulletManager->spawnDoubleCircle(pos, phase.bulletCountPerWave / 2, phase.bulletSpeed * m_bulletSpeedMult, 8.f);
    bulletManager->spawnSingle(pos, 90.f, phase.bulletSpeed * m_bulletSpeedMult * 1.2f, 10.f, 7, 2);  // 类型7，文件2
}

// 13: 螺旋弹幕
void Boss::patternSpiral(sf::Vector2f playerPos, EnemyBulletManager* bulletManager) {
    const AttackPhase& phase = m_phases[m_currentPhase];
    sf::Vector2f pos = m_sprite.getPosition();
    float angle = m_stateTimer * 60.f;
    bulletManager->spawnSpiral(pos, angle, 15.f, phase.bulletSpeed * m_bulletSpeedMult, 8.f, 1);
}

// 14: 密集扇形
void Boss::patternDenseFan(sf::Vector2f playerPos, EnemyBulletManager* bulletManager) {
    const AttackPhase& phase = m_phases[m_currentPhase];
    sf::Vector2f pos = m_sprite.getPosition();
    bulletManager->spawnFan(pos, 90.f, phase.bulletCountPerWave, phase.spreadAngle, phase.bulletSpeed * m_bulletSpeedMult, 8.f, 3);
}

void Boss::takeDamage(int damage) {
    if (!canBeHit()) return;
    m_hp -= damage;
    if (m_hp <= 0) {
        m_hp = 0;
        m_state = BossState::DEFEATED;
        m_stateTimer = 0.f;
    }
}

std::string Boss::getCurrentSpellName() const {
    if (m_currentPhase < static_cast<int>(m_phases.size())) {
        return m_phases[m_currentPhase].name;
    }
    return "";
}

void Boss::render(sf::RenderWindow& window) {
    // 淡出效果
    if (m_state == BossState::EXITING) {
        sf::Color color = m_sprite.getColor();
        color.a = 0;
        m_sprite.setColor(color);
    } else {
        // 闪烁效果（被击中时）
        if (m_state == BossState::DEFEATED && static_cast<int>(m_stateTimer * 10) % 2 == 0) {
            m_sprite.setColor(sf::Color(255, 255, 255, 128));
        } else {
            m_sprite.setColor(sf::Color(255, 255, 255, 255));
        }
    }

    window.draw(m_sprite);

    // 绘制Boss血条
    if (m_state == BossState::SPELL_CARD || m_state == BossState::FIGHTING) {
        sf::RectangleShape bgBar(sf::Vector2f(200.f, 12.f));
        bgBar.setFillColor(sf::Color(50, 50, 50));
        bgBar.setOrigin(100.f, 6.f);
        bgBar.setPosition(m_sprite.getPosition().x, m_sprite.getPosition().y + 70.f);
        window.draw(bgBar);

        sf::RectangleShape hpBar(sf::Vector2f(200.f * getHpPercent(), 12.f));
        hpBar.setFillColor(sf::Color(255, 50, 50));
        hpBar.setOrigin(100.f, 6.f);
        hpBar.setPosition(m_sprite.getPosition().x, m_sprite.getPosition().y + 70.f);
        window.draw(hpBar);
    }
}

// ---- BossManager ----

BossManager::BossManager() {
}

bool BossManager::initTexture() {
    // 加载Boss贴图：使用玩家角色贴图 reimu.png
    if (!m_bossTexture.loadFromFile("assets/image/player/player/reimu.png")) {
        // 如果加载失败，创建一个备用彩色贴图
        sf::Image img;
        img.create(64, 64, sf::Color(200, 50, 50, 200));
        m_bossTexture.loadFromImage(img);
    }
    return true;
}

Boss* BossManager::createBoss(int hp, const sf::Vector2f& startPos, const sf::Vector2f& targetPos, int bossIndex) {
    if (bossIndex >= MAX_BOSSES) return nullptr;

    sf::Texture* tex = &m_bossTexture;
    Boss boss(tex);
    boss.init(hp, startPos, targetPos);

    if (bossIndex >= static_cast<int>(m_bosses.size())) {
        m_bosses.push_back(boss);
    } else {
        m_bosses[bossIndex] = boss;
    }

    return &m_bosses[bossIndex];
}

void BossManager::update(float deltaTime, sf::Vector2f playerPos, EnemyBulletManager* bulletManager) {
    for (auto& boss : m_bosses) {
        if (boss.isAlive()) {
            boss.update(deltaTime, playerPos, bulletManager);
        }
    }
}

void BossManager::render(sf::RenderWindow& window) {
    for (auto& boss : m_bosses) {
        if (boss.isAlive() || boss.getPosition().y < 1200.f) {
            boss.render(window);
        }
    }
}

void BossManager::checkPlayerBullets(const PlayerBullet* playerBullets, int bulletCount, float playerBulletRadius) {
    for (auto& boss : m_bosses) {
        if (!boss.canBeHit()) continue;

        sf::Vector2f bossPos = boss.getPosition();
        float bossRadius = boss.getHitboxRadius();

        for (int i = 0; i < bulletCount; ++i) {
            if (!playerBullets[i].is_active) continue;

            sf::Vector2f diff = playerBullets[i].position - bossPos;
            float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
            if (dist < bossRadius + playerBulletRadius) {
                boss.takeDamage(10);
                SoundManager::getInstance().playSFX("assets/sound/SFX/enemy_damage.wav");
                const_cast<PlayerBullet&>(playerBullets[i]).is_active = false;
            }
        }
    }
}

void BossManager::clearAll() {
    m_bosses.clear();
}

bool BossManager::hasAliveBoss() const {
    for (const auto& boss : m_bosses) {
        if (boss.isAlive()) return true;
    }
    return false;
}

Boss* BossManager::getBoss(int index) {
    if (index >= 0 && index < static_cast<int>(m_bosses.size())) {
        return &m_bosses[index];
    }
    return nullptr;
}