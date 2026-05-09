#include "PlayerBullet.h"
#include <iostream>
#include <cmath>

PlayerBulletManager::PlayerBulletManager(sf::Texture& tex) : m_texture(tex) {
    m_bullets.resize(MAX_BULLETS);
    InitDefaultConfigs();
}

void PlayerBulletManager::InitDefaultConfigs() {
    // ---- normal_shoot_1：主射击弹幕1 ----
    {
        BulletConfig cfg;
        cfg.active_rect = sf::IntRect(192, 144, 64, 16);
        cfg.hit_box = sf::Vector2f(32.f, 32.f);
        cfg.total_destroy_duration = 0.25f;
        cfg.random_rotation = false;
        // 销毁动画帧
        cfg.destroy_frames.push_back({sf::IntRect(128, 160, 16, 16), 0.05f});
        cfg.destroy_frames.push_back({sf::IntRect(144, 160, 16, 16), 0.05f});
        cfg.destroy_frames.push_back({sf::IntRect(160, 160, 16, 16), 0.1f});
        cfg.destroy_frames.push_back({sf::IntRect(176, 160, 16, 16), 0.1f});
        m_configs["normal_shoot_1"] = cfg;
    }

    // ---- normal_shoot_2：主射击弹幕2 ----
    {
        BulletConfig cfg;
        cfg.active_rect = sf::IntRect(192, 176, 64, 16);
        cfg.hit_box = sf::Vector2f(32.f, 32.f);
        cfg.total_destroy_duration = 0.25f;
        cfg.random_rotation = false;
        cfg.destroy_frames.push_back({sf::IntRect(128, 160, 16, 16), 0.05f});
        cfg.destroy_frames.push_back({sf::IntRect(144, 160, 16, 16), 0.05f});
        cfg.destroy_frames.push_back({sf::IntRect(160, 160, 16, 16), 0.1f});
        cfg.destroy_frames.push_back({sf::IntRect(176, 160, 16, 16), 0.1f});
        m_configs["normal_shoot_2"] = cfg;
    }

    // ---- trait_bullet：符卡支援弹幕 ----
    {
        BulletConfig cfg;
        cfg.active_rect = sf::IntRect(0, 160, 16, 16);
        cfg.hit_box = sf::Vector2f(32.f, 32.f);
        cfg.total_destroy_duration = 0.2f;
        cfg.random_rotation = true;
        cfg.destroy_frames.push_back({sf::IntRect(64, 160, 16, 16), 0.05f});
        cfg.destroy_frames.push_back({sf::IntRect(80, 160, 16, 16), 0.05f});
        cfg.destroy_frames.push_back({sf::IntRect(96, 160, 16, 16), 0.1f});
        m_configs["trait_bullet"] = cfg;
    }
}

void PlayerBulletManager::Spawn(const std::string& type, sf::Vector2f pos, float angle, float speed) {
    auto it = m_configs.find(type);
    if (it == m_configs.end()) {
        return;
    }

    // 找一个空闲的弹幕
    PlayerBullet* bullet = nullptr;
    for (auto& b : m_bullets) {
        if (!b.is_active) {
            bullet = &b;
            break;
        }
    }
    if (!bullet) {
        bullet = &m_bullets[0];  // 复用第一个
    }

    const BulletConfig& cfg = it->second;
    bullet->config = &cfg;
    bullet->position = pos;
    bullet->angle = angle;
    bullet->speed = speed;
    bullet->angle_acc = 0.f;
    bullet->is_active = true;
    bullet->is_homing = false;
    bullet->homing_rate = 0.f;
    bullet->state = PBS_ACTIVE;
    bullet->destroy_timer = 0.f;
    bullet->current_anim_frame = 0;
    bullet->current_anim_timer = 0.f;
    bullet->rotation = 0.f;

    // 计算速度向量
    // SFML坐标系：Y轴向下为正，向上飞需要负的Y速度
    // 角度270度 = 向上，对应SFML中需要Y速度为负
    float angleRad = angle * 3.14159f / 180.f;
    bullet->velocity.x = std::cos(angleRad) * speed;
    bullet->velocity.y = std::sin(angleRad) * speed;  // 向上飞时sin(270)=-1，乘以speed得负值
}

void PlayerBulletManager::SpawnHoming(const std::string& type, sf::Vector2f pos, float angle, float speed, float homing_rate, sf::Vector2f target) {
    auto it = m_configs.find(type);
    if (it == m_configs.end()) {
        return;
    }

    // 找一个空闲的弹幕
    PlayerBullet* bullet = nullptr;
    for (auto& b : m_bullets) {
        if (!b.is_active) {
            bullet = &b;
            break;
        }
    }
    if (!bullet) {
        bullet = &m_bullets[0];  // 复用第一个
    }

    const BulletConfig& cfg = it->second;
    bullet->config = &cfg;
    bullet->position = pos;
    bullet->angle = angle;
    bullet->speed = speed;
    bullet->angle_acc = 0.f;
    bullet->is_active = true;
    bullet->is_homing = true;
    bullet->homing_rate = homing_rate;
    bullet->homing_target = target;
    bullet->state = PBS_ACTIVE;
    bullet->destroy_timer = 0.f;
    bullet->current_anim_frame = 0;
    bullet->current_anim_timer = 0.f;
    bullet->rotation = 0.f;

    // 计算速度向量
    float angleRad = angle * 3.14159f / 180.f;
    bullet->velocity.x = std::cos(angleRad) * speed;
    bullet->velocity.y = std::sin(angleRad) * speed;
}

void PlayerBulletManager::Update(float deltaTime, sf::Vector2u windowSize, sf::Vector2f homingTarget) {
    for (auto& bullet : m_bullets) {
        if (!bullet.is_active) continue;

        if (bullet.state == PBS_DESTROYING) {
            bullet.destroy_timer -= deltaTime;
            if (bullet.destroy_timer <= 0.f) {
                bullet.state = PBS_UNUSED;
                bullet.is_active = false;
            }
            continue;
        }

        // 追踪弹：更新目标位置并调整角度
        if (bullet.is_homing && bullet.homing_rate > 0.f) {
            // 更新追踪目标
            bullet.homing_target = homingTarget;

            // 计算朝向目标的角度
            sf::Vector2f dir = bullet.homing_target - bullet.position;
            float targetAngle = std::atan2(dir.y, dir.x) * 180.f / 3.14159f;

            // 角度差（归一化到 -180 ~ 180）
            float angleDiff = targetAngle - bullet.angle;
            while (angleDiff > 180.f) angleDiff -= 360.f;
            while (angleDiff < -180.f) angleDiff += 360.f;

            // 根据追踪率转向
            float maxTurn = bullet.homing_rate * deltaTime;
            if (std::abs(angleDiff) < maxTurn) {
                bullet.angle = targetAngle;
            } else {
                bullet.angle += (angleDiff > 0 ? maxTurn : -maxTurn);
            }

            // 重新计算速度向量
            float angleRad = bullet.angle * 3.14159f / 180.f;
            bullet.velocity.x = std::cos(angleRad) * bullet.speed;
            bullet.velocity.y = std::sin(angleRad) * bullet.speed;
        }

        // 移动弹幕
        bullet.position.x += bullet.velocity.x * deltaTime;
        bullet.position.y += bullet.velocity.y * deltaTime;

        // 更新渲染旋转（普通弹幕旋转跟随角度）
        bullet.rotation = bullet.angle + 180.f;  // +180度让子弹头朝上

        // 超出屏幕检测
        float margin = 100.f;
        if (bullet.position.y < -margin ||
            bullet.position.y > windowSize.y + margin ||
            bullet.position.x < -margin ||
            bullet.position.x > windowSize.x + margin) {
            bullet.state = PBS_DESTROYING;
            bullet.destroy_timer = bullet.config ? bullet.config->total_destroy_duration : 0.1f;
        }
    }
}

void PlayerBulletManager::Render(sf::RenderWindow& window) {
    for (const auto& bullet : m_bullets) {
        if (!bullet.is_active || !bullet.config) continue;

        const BulletConfig* cfg = bullet.config;
        sf::IntRect drawRect;
        float rotation;
        float scale = 2.f;  // 放大倍数，与玩家角色一致

        if (bullet.state == PBS_ACTIVE) {
            drawRect = cfg->active_rect;
            rotation = bullet.rotation;
        } else if (bullet.state == PBS_DESTROYING) {
            // 播放销毁动画
            float elapsed = cfg->total_destroy_duration - bullet.destroy_timer;
            float cumulative = 0.f;
            for (size_t i = 0; i < cfg->destroy_frames.size(); ++i) {
                cumulative += cfg->destroy_frames[i].duration;
                if (elapsed <= cumulative) {
                    drawRect = cfg->destroy_frames[i].rect;
                    break;
                }
            }
            rotation = cfg->random_rotation ? bullet.rotation + (std::rand() % 180) : 0.f;
        }

        // 创建精灵
        sf::Sprite sprite(m_texture, drawRect);
        sprite.setOrigin(drawRect.width / 2.f, drawRect.height / 2.f);
        sprite.setPosition(bullet.position);
        sprite.setRotation(rotation);
        sprite.setScale(scale, scale);

        // Apply fade alpha for bomb effect
        if (m_fadeAlpha < 255) {
            sprite.setColor(sf::Color(255, 255, 255, m_fadeAlpha));
        }

        window.draw(sprite);
    }
}

int PlayerBulletManager::GetActiveCount() const {
    int count = 0;
    for (const auto& b : m_bullets) {
        if (b.is_active) ++count;
    }
    return count;
}

void PlayerBulletManager::setFadeAlpha(sf::Uint8 alpha) {
    m_fadeAlpha = alpha;
}

void PlayerBulletManager::clearAll() {
    for (auto& bullet : m_bullets) {
        bullet.is_active = false;
        bullet.state = PBS_UNUSED;
    }
    m_fadeAlpha = 255;  // Reset fade
}
