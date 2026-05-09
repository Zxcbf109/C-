#include "Player.h"
#include "SoundManager.h"
#include <cmath>

Player::Player() {
    // 加载玩家贴图（pl00.png）
    if (!m_texture.loadFromFile("assets/image/player/player/reimu.png"))
        return;

    m_sprite.setTexture(m_texture);
    m_sprite.setScale(2.f, 2.f);

    // ---- 初始化生命和符卡 ----
    m_life = 3;   // 初始3条命
    m_bombs = 3;  // 初始3个符卡

    // ---- 初始化动画系统 ----
    initAnimations();
    playAnimation("idle");
    m_sprite.setPosition(387.5f, 800.f);  // 居中于775宽的游戏区域

    // ---- 初始化弹幕管理器 ----
    m_bulletManager = std::make_unique<PlayerBulletManager>(m_texture);

    // ---- 初始化判定点图形 ----
    m_hitboxIndicator.setRadius(HITBOX_RADIUS);
    m_hitboxIndicator.setFillColor(sf::Color(255, 100, 100, 180));
    m_hitboxIndicator.setOutlineColor(sf::Color::Red);
    m_hitboxIndicator.setOutlineThickness(1.f);
    m_hitboxIndicator.setOrigin(HITBOX_RADIUS, HITBOX_RADIUS);

    // ---- 初始化无敌时间 ----
    m_invincibleTimer = 0.f;
    m_grazeCount = 0;
}

Player::~Player() {
}

void Player::resetPlayerState() {
    m_life = 3;
    m_bombs = 3;
    m_sprite.setPosition(387.5f, 800.f);
    m_invincibleTimer = 0.f;
    m_grazeCount = 0;
    m_bulletManager->clearAll();
    m_bombSystem = BombSystem();
    playAnimation("idle");
}

void Player::initAnimations() {
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
        anim.loop = false;  // 非循环，播完就停
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

void Player::playAnimation(const std::string& name) {
    if (m_animations.find(name) == m_animations.end()) return;
    if (m_currentAnimName == name) return;

    m_currentAnimName = name;
    m_animTimer = 0.f;
    m_currentFrameIndex = 0;
    m_animFinished = false;

    const Animation& anim = m_animations[name];
    if (!anim.frames.empty()) {
        const AnimFrame& frame = anim.frames[0];
        m_sprite.setTextureRect(frame.rect);
        m_sprite.setOrigin(frame.anchorX, frame.anchorY);
    }
}

void Player::updateAnimation(float deltaTime) {
    Animation& anim = m_animations[m_currentAnimName];
    if (anim.frames.empty()) return;

    if (m_animFinished) {
        // 非循环动画播完后保持在最后一帧
        return;
    }

    m_animTimer += deltaTime;

    // 累积时间找到当前帧
    float accumulated = 0.f;
    int frameIdx = 0;
    for (size_t i = 0; i < anim.frames.size(); ++i) {
        accumulated += anim.frames[i].duration;
        if (m_animTimer < accumulated) {
            frameIdx = i;
            break;
        }
        if (i == anim.frames.size() - 1) {
            // 播完一轮
            if (anim.loop) {
                m_animTimer -= anim.totalDuration;
                if (m_animTimer < 0) m_animTimer = 0;
                frameIdx = 0;
            } else {
                m_animFinished = true;
                frameIdx = anim.frames.size() - 1;  // 停在最后一帧
            }
            break;
        }
    }

    // 如果帧变化则更新贴图区域
    if (frameIdx != m_currentFrameIndex) {
        m_currentFrameIndex = frameIdx;
        const AnimFrame& frame = anim.frames[frameIdx];
        m_sprite.setTextureRect(frame.rect);
        m_sprite.setOrigin(frame.anchorX, frame.anchorY);
    }
}

void Player::handleShooting(float deltaTime) {
    // Z 键射击
    m_isShooting = sf::Keyboard::isKeyPressed(sf::Keyboard::Z);

    if (m_shootCooldown > 0.f) {
        m_shootCooldown -= deltaTime;
    }

    if (m_isShooting && m_shootCooldown <= 0.f) {
        sf::Vector2f pos = m_sprite.getPosition();

        const float bulletSpeed = 1500.f;
        const float bulletAngle = 270.f;  // 向上

        // 交替发射两种弹幕
        std::string bulletType = m_shootType ? "normal_shoot_2" : "normal_shoot_1";
        m_shootType = !m_shootType;

        // 双发弹幕
        m_bulletManager->Spawn(bulletType, {pos.x - 10.f, pos.y - 20.f}, bulletAngle, bulletSpeed);
        m_bulletManager->Spawn(bulletType, {pos.x + 10.f, pos.y - 20.f}, bulletAngle, bulletSpeed);

        // 如果开启了追踪模式，发射追踪弹（时刻锁定Boss位置）
        if (m_trackingShotEnabled && m_enemyBulletManager) {
            // 在追踪弹模式中，在主弹幕两侧发射小追踪弹
            float trackAngle = 270.f;
            m_bulletManager->SpawnHoming("trait_bullet", {pos.x - 20.f, pos.y - 10.f}, trackAngle - 30.f, bulletSpeed * 0.8f, 200.f, m_trackingTargetPos);
            m_bulletManager->SpawnHoming("trait_bullet", {pos.x + 20.f, pos.y - 10.f}, trackAngle + 30.f, bulletSpeed * 0.8f, 200.f, m_trackingTargetPos);
        }

        m_shootCooldown = m_shootInterval;
    }
}

void Player::useBomb() {
    if (m_bombs > 0) {
        m_bombs--;
        m_bombSystem.triggerBomb(m_sprite.getPosition(), m_bulletManager.get());
        // 清空所有敌弹
        if (m_enemyBulletManager) {
            m_enemyBulletManager->clearAll();
        }
        // 重置无敌时间
        m_invincibleTimer = INVINCIBLE_DURATION;
    }
}

void Player::takeDamage() {
    if (m_invincibleTimer > 0.f) return;
    m_life--;
    m_invincibleTimer = INVINCIBLE_DURATION;
    // 播放受伤音效
    SoundManager::getInstance().playSFX("assets/sound/SFX/se_enep00.wav");
}

void Player::hitByBullet() {
    takeDamage();
}

void Player::hitByLaser() {
    takeDamage();
}

int Player::checkGraze(EnemyBulletManager* enemyBullets) {
    if (!enemyBullets) return 0;

    int grazeThisFrame = 0;
    sf::Vector2f hitboxPos = getHitboxCenter();

    // 获取敌弹数据
    auto& bullets = enemyBullets->getActiveBullets();

    for (auto& bullet : bullets) {
        if (!bullet.is_active || bullet.state != EBS_ACTIVE) continue;

        // 计算距离
        sf::Vector2f diff = bullet.position - hitboxPos;
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);

        // 擦弹范围是碰撞范围的2倍
        float grazeDistance = bullet.hitboxRadius + getHitboxRadius() * 2.f;

        if (dist < grazeDistance && dist >= bullet.hitboxRadius + getHitboxRadius()) {
            // 在擦弹范围内但不在碰撞范围内
            grazeThisFrame++;
        }
    }

    m_grazeCount += grazeThisFrame;
    return grazeThisFrame;
}

void Player::checkBulletCollisions() {
    if (!m_enemyBulletManager) return;
    if (m_invincibleTimer > 0.f) return;

    sf::Vector2f hitboxPos = getHitboxCenter();

    // 检查弹幕碰撞
    if (m_enemyBulletManager->checkPlayerCollision(hitboxPos, getHitboxRadius())) {
        // 触发击中特效
        m_enemyBulletManager->triggerHitEffect(hitboxPos);
        takeDamage();
    }
}

void Player::update(float deltaTime, const sf::Vector2u& windowSize) {
    // ---- 无敌时间更新 ----
    if (m_invincibleTimer > 0.f) {
        m_invincibleTimer -= deltaTime;
    }

    // ---- 符卡释放 (X键) ----
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
        if (!m_bombKeyHeld && m_bombs > 0) {
            useBomb();
        }
        m_bombKeyHeld = true;
    } else {
        m_bombKeyHeld = false;
    }

    // ---- Shift键切换追踪弹模式 ----
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift) || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift)) {
        if (!m_trackingShotEnabled) {
            m_trackingShotEnabled = true;
        }
    } else {
        m_trackingShotEnabled = false;
    }

    // ---- 移动 ----
    bool left  = sf::Keyboard::isKeyPressed(sf::Keyboard::Left);
    bool right = sf::Keyboard::isKeyPressed(sf::Keyboard::Right);
    bool up    = sf::Keyboard::isKeyPressed(sf::Keyboard::Up);
    bool down  = sf::Keyboard::isKeyPressed(sf::Keyboard::Down);
    bool shift = sf::Keyboard::isKeyPressed(sf::Keyboard::LShift)
              || sf::Keyboard::isKeyPressed(sf::Keyboard::RShift);

    m_isFocused = shift;  // 记录是否按住shift（低速移动/focus模式）

    float speed = shift ? 100.f : 350.f;

    float dx = 0.f, dy = 0.f;
    if (left  && !right) dx = -speed * deltaTime;
    if (right && !left)  dx =  speed * deltaTime;
    if (up    && !down)  dy = -speed * deltaTime;
    if (down  && !up)    dy =  speed * deltaTime;

    sf::Vector2f pos = m_sprite.getPosition();
    pos.x += dx;
    pos.y += dy;

    // ---- 确定动画状态 ----
    PlayerAnimState newState = PlayerAnimState::IDLE;
    bool isMoving = (dx != 0.f || dy != 0.f);

    if (left && !right) {
        if (isMoving) {
            newState = PlayerAnimState::MOVE_LEFT;
        } else {
            newState = PlayerAnimState::LEFT;
        }
    } else if (right && !left) {
        if (isMoving) {
            newState = PlayerAnimState::MOVE_RIGHT;
        } else {
            newState = PlayerAnimState::RIGHT;
        }
    } else if (up || down) {
        newState = PlayerAnimState::MOVE_FORWARD;
    } else {
        newState = PlayerAnimState::IDLE;
    }

    // ---- 状态变化时切换动画 ----
    if (newState != m_animState) {
        m_prevAnimState = m_animState;
        m_animState = newState;

        switch (newState) {
            case PlayerAnimState::IDLE:
                playAnimation("idle");
                break;
            case PlayerAnimState::LEFT:
                playAnimation("left");
                break;
            case PlayerAnimState::RIGHT:
                playAnimation("right");
                break;
            case PlayerAnimState::MOVE_LEFT:
                playAnimation("move_left");
                break;
            case PlayerAnimState::MOVE_RIGHT:
                playAnimation("move_right");
                break;
            case PlayerAnimState::MOVE_FORWARD:
                playAnimation("move_forword");
                break;
        }
    }

    // ---- 更新动画 ----
    updateAnimation(deltaTime);

    // ---- 边界限制 ----
    // 使用贴图半宽(缩放后)限制，确保贴图不会超出GAME_WIDTH
    // sprite scale=2, 原始32px -> 64px视觉宽度, 所以半宽是32
    const float SPRITE_HALF_W = 32.f;
    const float LEFT_LIMIT = SPRITE_HALF_W;
    // 判定点横向坐标限制为960，因此sprite位置最大为960
    const float RIGHT_LIMIT = GAME_WIDTH;

    // 强制限制位置，使用 std::clamp 确保无穿模
    pos.x = std::clamp(pos.x, LEFT_LIMIT, RIGHT_LIMIT);
    pos.y = std::clamp(pos.y, PLAYER_HALF_WIDTH, GAME_HEIGHT - PLAYER_HALF_WIDTH);

    m_sprite.setPosition(pos);

    // ---- 更新判定点位置（和贴图同步）----
    sf::Vector2f hitboxPos = getHitboxCenter();
    m_hitboxIndicator.setPosition(hitboxPos);

    // ---- 射击 ----
    handleShooting(deltaTime);

    // ---- 更新弹幕 ----
    m_bulletManager->Update(deltaTime, windowSize, m_trackingTargetPos);

    // ---- 更新符卡效果 ----
    m_bombSystem.update(deltaTime);

    // ---- 检查与敌弹的碰撞 ----
    checkBulletCollisions();

    // ---- 检查擦弹 ----
    if (m_enemyBulletManager) {
        checkGraze(m_enemyBulletManager);
    }
}

void Player::draw(sf::RenderWindow& window) {
    // 无敌闪烁效果
    if (m_invincibleTimer > 0.f) {
        // 闪烁：每0.1秒切换一次可见性
        if (static_cast<int>(m_invincibleTimer * 10) % 2 == 0) {
            return;  // 不可见时直接返回
        }
    }

    // 绘制弹幕
    m_bulletManager->Render(window);

    // 绘制符卡效果
    m_bombSystem.draw(window);

    // 绘制玩家
    window.draw(m_sprite);

    // 绘制判定点（focus模式时显示红色小圆圈）
    if (m_isFocused) {
        window.draw(m_hitboxIndicator);
    }
}