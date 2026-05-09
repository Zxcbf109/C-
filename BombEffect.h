#pragma once
#include <SFML/Graphics.hpp>
#include <vector>

class PlayerBulletManager;  // Forward declare

class BombEffect {
public:
    BombEffect();

    void trigger(const sf::Vector2f& playerPos);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);

    bool isActive() const { return m_active; }

private:
    bool m_active = false;
    float m_timer = 0.f;
    float m_duration = 0.5f;  // Total duration in seconds

    // Center flash
    sf::CircleShape m_centerFlash;
    float m_flashRadius = 50.f;
    float m_flashMaxRadius = 200.f;

    // Ring shockwave
    sf::CircleShape m_ring;
    float m_ringRadius = 0.f;
    float m_ringMaxRadius = 600.f;
    float m_ringThickness = 8.f;

    // Player position (for centering effects)
    sf::Vector2f m_playerPos;

    // Color scheme (绯红色/金色)
    sf::Color m_ringColor = sf::Color(255, 200, 100, 255);
};

class BombSystem {
public:
    BombSystem();

    void triggerBomb(const sf::Vector2f& playerPos, PlayerBulletManager* bulletManager);
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);

    bool isActive() const { return m_effect.isActive(); }

private:
    BombEffect m_effect;
    PlayerBulletManager* m_bulletManager = nullptr;
    bool m_bulletsFading = false;
    int m_bulletFadeFrames = 0;
    static const int BULLET_FADE_TOTAL_FRAMES = 6;
};