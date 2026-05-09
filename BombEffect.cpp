#include "BombEffect.h"
#include "PlayerBullet.h"

BombEffect::BombEffect() {
    m_centerFlash.setRadius(m_flashRadius);
    m_centerFlash.setFillColor(sf::Color(255, 255, 255, 255));
    m_centerFlash.setOrigin(m_flashRadius, m_flashRadius);

    m_ring.setRadius(m_ringRadius);
    m_ring.setOutlineThickness(m_ringThickness);
    m_ring.setOutlineColor(m_ringColor);
    m_ring.setFillColor(sf::Color(0, 0, 0, 0));  // Hollow ring
    m_ring.setOrigin(m_ringRadius, m_ringRadius);
}

void BombEffect::trigger(const sf::Vector2f& playerPos) {
    m_active = true;
    m_timer = 0.f;
    m_playerPos = playerPos;

    // Reset flash
    m_centerFlash.setRadius(m_flashRadius);
    m_centerFlash.setFillColor(sf::Color(255, 255, 255, 255));

    // Reset ring
    m_ringRadius = 0.f;
    m_ring.setRadius(m_ringRadius);
    m_ring.setOutlineColor(m_ringColor);
}

void BombEffect::update(float deltaTime) {
    if (!m_active) return;

    m_timer += deltaTime;
    float t = m_timer / m_duration;  // 0 to 1

    if (t >= 1.f) {
        m_active = false;
        return;
    }

    // ---- Center Flash (frames 0-0.1, very fast) ----
    if (t < 0.2f) {
        float flashT = t / 0.2f;  // 0 to 1 for flash phase
        // Flash expands quickly then fades
        float radius = m_flashRadius + (m_flashMaxRadius - m_flashRadius) * flashT * 2.f;
        if (radius > m_flashMaxRadius) radius = m_flashMaxRadius;
        m_centerFlash.setRadius(radius);

        // Fade out quickly
        sf::Uint8 alpha = static_cast<sf::Uint8>(255 * (1.f - flashT));
        m_centerFlash.setFillColor(sf::Color(255, 255, 255, alpha));
        m_centerFlash.setOrigin(radius, radius);
        m_centerFlash.setPosition(m_playerPos);
    }

    // ---- Ring Shockwave (starts at t=0, expands through duration) ----
    float ringT = t * 1.5f;  // Ring expands faster than duration
    m_ringRadius = ringT * m_ringMaxRadius;
    if (m_ringRadius > m_ringMaxRadius) m_ringRadius = m_ringMaxRadius;
    m_ring.setRadius(m_ringRadius);
    m_ring.setOrigin(m_ringRadius, m_ringRadius);
    m_ring.setPosition(m_playerPos);

    // Ring fades as it expands
    sf::Uint8 ringAlpha = static_cast<sf::Uint8>(255 * (1.f - t * 0.7f));
    m_ring.setOutlineColor(sf::Color(255, 200, 100, ringAlpha));
}

void BombEffect::draw(sf::RenderWindow& window) {
    if (!m_active) return;

    float t = m_timer / m_duration;

    // Draw center flash (only in early phase)
    if (t < 0.2f) {
        window.draw(m_centerFlash);
    }

    // Draw ring shockwave
    if (m_ringRadius > 0) {
        window.draw(m_ring);
    }
}

// ---- BombSystem ----

BombSystem::BombSystem() {
    m_effect = BombEffect();
}

void BombSystem::triggerBomb(const sf::Vector2f& playerPos, PlayerBulletManager* bulletManager) {
    m_effect.trigger(playerPos);
    m_bulletManager = bulletManager;
    m_bulletsFading = true;
    m_bulletFadeFrames = 0;
}

void BombSystem::update(float deltaTime) {
    m_effect.update(deltaTime);

    // Handle bullet fading over 6 frames
    if (m_bulletsFading && m_bulletManager) {
        m_bulletFadeFrames++;
        if (m_bulletFadeFrames >= BULLET_FADE_TOTAL_FRAMES) {
            // After 6 frames, clear all bullets and stop fading
            m_bulletManager->clearAll();
            m_bulletsFading = false;
            m_bulletFadeFrames = 0;
        } else {
            // Set fade alpha based on frame progress
            float fadeT = static_cast<float>(m_bulletFadeFrames) / BULLET_FADE_TOTAL_FRAMES;
            m_bulletManager->setFadeAlpha(255 * (1.f - fadeT));
        }
    }
}

void BombSystem::draw(sf::RenderWindow& window) {
    m_effect.draw(window);
}