#include "EnemyBullet.h"
#include <cmath>
#include <algorithm>
#include <iostream>

// 手动列出所有子弹贴图文件
namespace BulletFiles {
    // assets/dbullet/a/ - 13 files
    const char* a_files[] = {"a.png", "b.png", "c.png", "f.png", "g.png", "h.png", "j.png", "k.png", "l.png", "m.png", "q.png", "s.png", "x.png"};
    const int a_count = 13;

    // assets/dbullet/d/ - 13 files
    const char* d_files[] = {"a.png", "aa.png", "aaa.png", "aaaa.png", "e.png", "q.png", "r.png", "rr.png", "s.png", "t.png", "u.png", "w.png", "y.png"};
    const int d_count = 13;

    // assets/dbullet/f/ - 14 files
    const char* f_files[] = {"a.png", "aa.png", "aaa.png", "aaaa.png", "e.png", "ee.png", "eee.png", "f.png", "ff.png", "fff.png", "ffff.png", "v.png", "vvv.png"};
    const int f_count = 13;  // actual is 13, not 14

    // assets/dbullet/g/ - 15 files
    const char* g_files[] = {"a.png", "aa.png", "aaa.png", "aaaa.png", "c.png", "v.png", "x.png", "xx.png", "xxx.png", "xxxx.png", "z.png", "zz.png", "zzz.png"};
    const int g_count = 13;

    // assets/dbullet/h/ - 14 files
    const char* h_files[] = {"q.png", "qq.png", "qqq.png", "r.png", "rr.png", "rrr.png", "rrrr.png", "v.png", "vv.png", "vvv.png", "vvvv.png", "ww.png", "wwww.png"};
    const int h_count = 13;

    // assets/dbullet/j/ - 16 files
    const char* j_files[] = {"a.png", "aa.png", "aaa.png", "d.png", "dd.png", "ddd.png", "dddd.png", "mm.png", "q.png", "qq.png", "qqq.png", "qqqq.png", "qqqqq.png"};
    const int j_count = 13;

    // assets/dbullet/n/ - 15 files
    const char* n_files[] = {"ee.png", "eee.png", "eeeee.png", "f.png", "ff.png", "fff.png", "ll.png", "qqqq.png", "qqqqq.png", "rr.png", "rrr.png", "w.png", "ww.png"};
    const int n_count = 13;

    // assets/dbullet/s/ - 16 files
    const char* s_files[] = {"a.png", "aaa.png", "e.png", "ee.png", "eee.png", "q.png", "w.png", "ww.png", "www.png", "z.png", "zz.png", "zzz.png", "zzzz.png"};
    const int s_count = 13;

    // assets/dbullet/v/ - 15 files
    const char* v_files[] = {"ee.png", "eee.png", "q.png", "qq.png", "qqq.png", "r.png", "rr.png", "rrr.png", "rrrr.png", "t.png", "w.png", "ww.png", "www.png"};
    const int v_count = 13;

    // 基础类型目录（移除x）
    const char* x_files[] = {};  // 不再使用
    const int x_count = 0;

    // 自动旋转子弹 - assets/dbullet/aa/
    const char* aa_files[] = {"a.png", "aa.png", "aaa.png", "aaaaa.png", "d.png", "q.png", "qq.png", "qqq.png", "qqqq.png", "s.png", "ss.png", "sss.png", "sssss.png"};
    const int aa_count = 13;

    // 自动旋转子弹 - assets/dbullet/ss/
    const char* ss_files[] = {"a.png", "aa.png", "aaa.png", "aaaaa.png", "d.png", "q.png", "qq.png", "qqq.png", "qqqq.png", "s.png", "ss.png", "sss.png", "sssss.png"};
    const int ss_count = 13;
}

EnemyBulletManager::EnemyBulletManager() {
    m_bullets.resize(MAX_BULLETS);
    m_bulletTypeTextures.resize(BULLET_TYPE_COUNT);
    m_autoRotateTypeTextures.resize(AUTO_ROTATE_TYPE_COUNT);
}

bool EnemyBulletManager::loadTextures() {
    bool loaded = true;

    // 加载普通子弹贴图：assets/dbullet/a, d, f, g, h, j, n, s, v, x
    // 类型0: a
    for (int i = 0; i < BulletFiles::a_count; ++i) {
        std::string path = "assets/dbullet/a/" + std::string(BulletFiles::a_files[i]);
        sf::Texture tex;
        if (tex.loadFromFile(path)) {
            m_bulletTypeTextures[0].push_back(tex);
        }
    }

    // 类型1: d
    for (int i = 0; i < BulletFiles::d_count; ++i) {
        std::string path = "assets/dbullet/d/" + std::string(BulletFiles::d_files[i]);
        sf::Texture tex;
        if (tex.loadFromFile(path)) {
            m_bulletTypeTextures[1].push_back(tex);
        }
    }

    // 类型2: f
    for (int i = 0; i < BulletFiles::f_count; ++i) {
        std::string path = "assets/dbullet/f/" + std::string(BulletFiles::f_files[i]);
        sf::Texture tex;
        if (tex.loadFromFile(path)) {
            m_bulletTypeTextures[2].push_back(tex);
        }
    }

    // 类型3: g
    for (int i = 0; i < BulletFiles::g_count; ++i) {
        std::string path = "assets/dbullet/g/" + std::string(BulletFiles::g_files[i]);
        sf::Texture tex;
        if (tex.loadFromFile(path)) {
            m_bulletTypeTextures[3].push_back(tex);
        }
    }

    // 类型4: h
    for (int i = 0; i < BulletFiles::h_count; ++i) {
        std::string path = "assets/dbullet/h/" + std::string(BulletFiles::h_files[i]);
        sf::Texture tex;
        if (tex.loadFromFile(path)) {
            m_bulletTypeTextures[4].push_back(tex);
        }
    }

    // 类型5: j
    for (int i = 0; i < BulletFiles::j_count; ++i) {
        std::string path = "assets/dbullet/j/" + std::string(BulletFiles::j_files[i]);
        sf::Texture tex;
        if (tex.loadFromFile(path)) {
            m_bulletTypeTextures[5].push_back(tex);
        }
    }

    // 类型6: n
    for (int i = 0; i < BulletFiles::n_count; ++i) {
        std::string path = "assets/dbullet/n/" + std::string(BulletFiles::n_files[i]);
        sf::Texture tex;
        if (tex.loadFromFile(path)) {
            m_bulletTypeTextures[6].push_back(tex);
        }
    }

    // 类型7: s
    for (int i = 0; i < BulletFiles::s_count; ++i) {
        std::string path = "assets/dbullet/s/" + std::string(BulletFiles::s_files[i]);
        sf::Texture tex;
        if (tex.loadFromFile(path)) {
            m_bulletTypeTextures[7].push_back(tex);
        }
    }

    // 类型8: v
    for (int i = 0; i < BulletFiles::v_count; ++i) {
        std::string path = "assets/dbullet/v/" + std::string(BulletFiles::v_files[i]);
        sf::Texture tex;
        if (tex.loadFromFile(path)) {
            m_bulletTypeTextures[8].push_back(tex);
        }
    }

    // 加载自动旋转子弹贴图：assets/dbullet/aa, ss
    // 类型0: aa
    for (int i = 0; i < BulletFiles::aa_count; ++i) {
        std::string path = "assets/dbullet/aa/" + std::string(BulletFiles::aa_files[i]);
        sf::Texture tex;
        if (tex.loadFromFile(path)) {
            m_autoRotateTypeTextures[0].push_back(tex);
        }
    }

    // 类型1: ss
    for (int i = 0; i < BulletFiles::ss_count; ++i) {
        std::string path = "assets/dbullet/ss/" + std::string(BulletFiles::ss_files[i]);
        sf::Texture tex;
        if (tex.loadFromFile(path)) {
            m_autoRotateTypeTextures[1].push_back(tex);
        }
    }

    // 加载击中特效贴图（使用白色方块作为后备）
    sf::Image img;
    img.create(32, 32, sf::Color::White);
    m_hitEffectTexture.loadFromImage(img);

    m_texturesLoaded = loaded;
    return loaded;
}

// 获取一个空闲的子弹
EnemyBullet* EnemyBulletManager::getInactiveBullet() {
    for (auto& b : m_bullets) {
        if (!b.is_active) {
            return &b;
        }
    }
    return nullptr;
}

// ========== 基础弹幕实现 ==========

// 1. 基础单发
void EnemyBulletManager::spawnSingle(sf::Vector2f pos, float angle, float speed, float radius, int texType, int texFile) {
    EnemyBullet* bullet = getInactiveBullet();
    if (!bullet) return;

    bullet->position = pos;
    bullet->angle = angle;
    bullet->speed = speed;
    bullet->hitboxRadius = radius;
    bullet->renderRadius = radius;
    bullet->is_active = true;
    bullet->state = EBS_ACTIVE;
    bullet->homingRate = 0.f;
    bullet->lifeTime = 0.f;
    bullet->rotation = 0.f;
    bullet->rotationSpeed = 0.f;
    bullet->autoRotate = false;
    bullet->texFileIndex = texFile;

    // 设置类型
    if (texType < BULLET_TYPE_COUNT) {
        bullet->type = static_cast<EnemyBulletType>(texType);
        bullet->autoRotate = false;
    } else {
        bullet->type = static_cast<EnemyBulletType>(texType);
        bullet->autoRotate = true;
        bullet->rotationSpeed = 200.f;
    }

    float angleRad = angle * 3.14159f / 180.f;
    bullet->velocity.x = std::cos(angleRad) * speed;
    bullet->velocity.y = std::sin(angleRad) * speed;
}

// 2. 扇形弹幕 - 遍历所有10种类型
void EnemyBulletManager::spawnFan(sf::Vector2f pos, float baseAngle, int count, float spreadAngle, float speed, float radius, int texType) {
    if (count <= 0) return;
    float halfSpread = spreadAngle / 2.f;
    float angleStep = count > 1 ? spreadAngle / (count - 1) : 0.f;
    for (int i = 0; i < count; ++i) {
        float angle = baseAngle - halfSpread + angleStep * i;
        // 在所有10种类型中轮换
        int typeIdx = i % BULLET_TYPE_COUNT;
        int fileIdx = i % 5;  // 每种类型用前5个文件
        spawnSingle(pos, angle, speed, radius, typeIdx, fileIdx);
    }
}

// 3. 圆形弹幕（一圈）
void EnemyBulletManager::spawnCircle(sf::Vector2f pos, int count, float speed, float radius) {
    if (count <= 0) return;
    float angleStep = 360.f / count;
    for (int i = 0; i < count; ++i) {
        float angle = angleStep * i;
        int typeIdx = i % BULLET_TYPE_COUNT;
        int fileIdx = (i / BULLET_TYPE_COUNT) % 5;
        spawnSingle(pos, angle, speed, radius, typeIdx, fileIdx);
    }
}

// 4. 双圈圆形弹幕（两圈，以不同角度错开）
void EnemyBulletManager::spawnDoubleCircle(sf::Vector2f pos, int count, float speed, float radius) {
    if (count <= 0) return;
    float angleStep = 360.f / count;

    // 第一圈 - 使用前10种类型的文件0
    for (int i = 0; i < count; ++i) {
        float angle = angleStep * i;
        int typeIdx = i % BULLET_TYPE_COUNT;
        spawnSingle(pos, angle, speed, radius, typeIdx, 0);
    }

    // 第二圈（错开半步角度）- 使用不同文件索引
    float angleOffset = angleStep / 2.f;
    for (int i = 0; i < count; ++i) {
        float angle = angleStep * i + angleOffset;
        int typeIdx = (i + 5) % BULLET_TYPE_COUNT;
        spawnSingle(pos, angle, speed, radius, typeIdx, 1);
    }
}

// ========== 螺旋与环形 ==========

// 5. 螺旋弹幕
void EnemyBulletManager::spawnSpiral(sf::Vector2f pos, float startAngle, float angleStep, float speed, float radius, int texType) {
    static int spiralCount = 0;
    float angle = startAngle + angleStep;
    int typeIdx = spiralCount % BULLET_TYPE_COUNT;
    int fileIdx = (spiralCount / BULLET_TYPE_COUNT) % 5;
    spawnSingle(pos, angle, speed, radius, typeIdx, fileIdx);
    spiralCount++;
}

// 6. 环形弹幕
void EnemyBulletManager::spawnRing(sf::Vector2f pos, int count, float ringRadius, float speed, float radius) {
    if (count <= 0) return;
    float angleStep = 360.f / count;
    for (int i = 0; i < count; ++i) {
        float angle = angleStep * i;
        float bx = pos.x + std::cos(angle * 3.14159f / 180.f) * ringRadius;
        float by = pos.y + std::sin(angle * 3.14159f / 180.f) * ringRadius;
        // 向圆心飞
        int typeIdx = i % BULLET_TYPE_COUNT;
        int fileIdx = (i / BULLET_TYPE_COUNT) % 5;
        spawnSingle(sf::Vector2f(bx, by), angle + 180.f, speed, radius, typeIdx, fileIdx);
    }
}

// 7. 旋转环形弹幕 - 使用自动旋转贴图
void EnemyBulletManager::spawnRotatingRing(sf::Vector2f pos, int count, float ringRadius, float speed, float rotSpeed, float radius) {
    if (count <= 0) return;
    static float globalAngle = 0.f;
    globalAngle += rotSpeed;
    float angleStep = 360.f / count;

    for (int i = 0; i < count; ++i) {
        float angle = globalAngle + angleStep * i;
        float bx = pos.x + std::cos(angle * 3.14159f / 180.f) * ringRadius;
        float by = pos.y + std::sin(angle * 3.14159f / 180.f) * ringRadius;
        // 向切线方向飞，使用自动旋转贴图
        int autoTypeIdx = i % AUTO_ROTATE_TYPE_COUNT;
        int fileIdx = i % 5;
        spawnSingle(sf::Vector2f(bx, by), angle + 90.f, speed, radius, BULLET_TYPE_COUNT + autoTypeIdx, fileIdx);
    }
}

// ========== 直线/曲线弹幕阵 ==========

// 8. 直线弹幕阵
void EnemyBulletManager::spawnGrid(sf::Vector2f pos, int rows, int cols, float spacingX, float spacingY, float speed, float radius) {
    for (int r = 0; r < rows; ++r) {
        for (int c = 0; c < cols; ++c) {
            float x = pos.x + c * spacingX - (cols - 1) * spacingX / 2.f;
            float y = pos.y + r * spacingY;
            int typeIdx = (r + c) % BULLET_TYPE_COUNT;
            int fileIdx = r % 5;
            spawnSingle(sf::Vector2f(x, y), 90.f, speed, radius, typeIdx, fileIdx);
        }
    }
}

// 9. 波浪形弹幕
void EnemyBulletManager::spawnWave(sf::Vector2f pos, float baseAngle, float speed, float amplitude, float frequency, int count, float radius) {
    for (int i = 0; i < count; ++i) {
        float offsetAngle = baseAngle + amplitude * std::sin(frequency * i);
        int typeIdx = i % BULLET_TYPE_COUNT;
        int fileIdx = (i / BULLET_TYPE_COUNT) % 5;
        spawnSingle(pos, offsetAngle, speed, radius, typeIdx, fileIdx);
    }
}

// 10. 交叉弹幕
void EnemyBulletManager::spawnCross(sf::Vector2f pos, float angle1, float angle2, int count, float spread, float speed, float radius) {
    spawnFan(pos, angle1, count, spread, speed, radius, 0);
    spawnFan(pos, angle2, count, spread, speed, radius, 5);  // 从类型5开始
}

// ========== 追踪与特殊弹幕 ==========

// 11. 追踪弹
void EnemyBulletManager::spawnHoming(sf::Vector2f pos, float speed, float radius, float homingRate) {
    EnemyBullet* bullet = getInactiveBullet();
    if (!bullet) return;

    bullet->position = pos;
    bullet->angle = 90.f;
    bullet->speed = speed;
    bullet->hitboxRadius = radius;
    bullet->renderRadius = radius;
    bullet->type = EnemyBulletType::TYPE_S; // 使用TYPE_S作为追踪弹外观
    bullet->is_active = true;
    bullet->state = EBS_ACTIVE;
    bullet->homingRate = homingRate;
    bullet->lifeTime = 0.f;
    bullet->rotation = 0.f;
    bullet->rotationSpeed = 200.f;
    bullet->autoRotate = false;
    bullet->texFileIndex = 0;

    float angleRad = bullet->angle * 3.14159f / 180.f;
    bullet->velocity.x = std::cos(angleRad) * speed;
    bullet->velocity.y = std::sin(angleRad) * speed;
}

// 12. 预瞄追踪弹
void EnemyBulletManager::spawnPredictiveHoming(sf::Vector2f pos, sf::Vector2f playerPos, sf::Vector2f playerVel, float speed, float radius, float homingRate) {
    // 计算预判位置
    float dist = std::sqrt((playerPos.x - pos.x) * (playerPos.x - pos.x) + (playerPos.y - pos.y) * (playerPos.y - pos.y));
    float travelTime = dist / speed;
    sf::Vector2f predictedPos = playerPos + playerVel * travelTime * 0.5f;

    // 角度朝向预判位置
    sf::Vector2f dir = predictedPos - pos;
    float angle = std::atan2(dir.y, dir.x) * 180.f / 3.14159f;

    EnemyBullet* bullet = getInactiveBullet();
    if (!bullet) return;

    bullet->position = pos;
    bullet->angle = angle;
    bullet->speed = speed;
    bullet->hitboxRadius = radius;
    bullet->renderRadius = radius;
    bullet->type = EnemyBulletType::TYPE_G; // 使用TYPE_G作为预瞄弹外观
    bullet->is_active = true;
    bullet->state = EBS_ACTIVE;
    bullet->homingRate = homingRate * 0.5f;
    bullet->lifeTime = 0.f;
    bullet->rotation = 0.f;
    bullet->rotationSpeed = 150.f;
    bullet->autoRotate = false;
    bullet->texFileIndex = 0;

    float angleRad = angle * 3.14159f / 180.f;
    bullet->velocity.x = std::cos(angleRad) * speed;
    bullet->velocity.y = std::sin(angleRad) * speed;
}

// 13. 反弹弹幕
void EnemyBulletManager::spawnBounce(sf::Vector2f pos, float angle, float speed, int bounceCount, float radius) {
    EnemyBullet* bullet = getInactiveBullet();
    if (!bullet) return;

    bullet->position = pos;
    bullet->angle = angle;
    bullet->speed = speed;
    bullet->hitboxRadius = radius;
    bullet->renderRadius = radius;
    bullet->type = EnemyBulletType::TYPE_V; // 使用TYPE_V作为反弹弹外观
    bullet->is_active = true;
    bullet->state = EBS_ACTIVE;
    bullet->homingRate = 0.f;
    bullet->lifeTime = 0.f;
    bullet->rotation = 0.f;
    bullet->rotationSpeed = 100.f;
    bullet->autoRotate = false;
    bullet->texFileIndex = 0;

    float angleRad = angle * 3.14159f / 180.f;
    bullet->velocity.x = std::cos(angleRad) * speed;
    bullet->velocity.y = std::sin(angleRad) * speed;
}

// ========== 定时/序列弹幕 ==========

// 14. 弹幕序列
void EnemyBulletManager::spawnSequence(sf::Vector2f pos, float baseAngle, float speed, float interval, int count, float radius) {
    for (int i = 0; i < count; ++i) {
        int typeIdx = i % BULLET_TYPE_COUNT;
        int fileIdx = (i / BULLET_TYPE_COUNT) % 5;
        spawnSingle(pos, baseAngle + (i - count/2) * 10.f, speed, radius, typeIdx, fileIdx);
    }
}

// 15. 弹幕波次
void EnemyBulletManager::spawnWave(sf::Vector2f pos, float baseAngle, float speed, int waves, int startCount, int endCount, float waveInterval, float spreadAngle, float radius) {
    int bulletCount = startCount;
    for (int w = 0; w < waves; ++w) {
        int typeIdx = w % BULLET_TYPE_COUNT;
        spawnFan(pos, baseAngle, bulletCount, spreadAngle, speed, radius, typeIdx);
        bulletCount = startCount + (endCount - startCount) * w / waves;
    }
}

// 16. 随机弹幕
void EnemyBulletManager::spawnRandom(sf::Vector2f pos, float minAngle, float maxAngle, float minSpeed, float maxSpeed, int count, float radius) {
    for (int i = 0; i < count; ++i) {
        float angle = minAngle + static_cast<float>(rand()) / RAND_MAX * (maxAngle - minAngle);
        float speed = minSpeed + static_cast<float>(rand()) / RAND_MAX * (maxSpeed - minSpeed);
        int typeIdx = rand() % BULLET_TYPE_COUNT;
        int fileIdx = rand() % 5;
        spawnSingle(pos, angle, speed, radius, typeIdx, fileIdx);
    }
}

// ========== 更新与渲染 ==========

void EnemyBulletManager::update(float deltaTime, const sf::Vector2u& windowSize, sf::Vector2f playerPos) {
    for (auto& bullet : m_bullets) {
        if (!bullet.is_active) continue;

        if (bullet.state == EBS_DESTROYING) {
            bullet.is_active = false;
            continue;
        }

        // 追踪弹：根据玩家位置转向
        if (bullet.homingRate > 0.f) {
            sf::Vector2f toPlayer = playerPos - bullet.position;
            float targetAngle = std::atan2(toPlayer.y, toPlayer.x) * 180.f / 3.14159f;

            float angleDiff = targetAngle - bullet.angle;
            while (angleDiff > 180.f) angleDiff -= 360.f;
            while (angleDiff < -180.f) angleDiff += 360.f;

            float turn = bullet.homingRate * deltaTime;
            if (std::abs(angleDiff) < turn) {
                bullet.angle = targetAngle;
            } else {
                bullet.angle += (angleDiff > 0 ? turn : -turn);
            }

            float angleRad = bullet.angle * 3.14159f / 180.f;
            bullet.velocity.x = std::cos(angleRad) * bullet.speed;
            bullet.velocity.y = std::sin(angleRad) * bullet.speed;
        }

        // 移动
        bullet.position.x += bullet.velocity.x * deltaTime;
        bullet.position.y += bullet.velocity.y * deltaTime;

        // 自动旋转子弹
        if (bullet.autoRotate) {
            bullet.rotation += bullet.rotationSpeed * deltaTime;
        } else {
            // 非自动旋转子弹：rotation跟随移动方向 + 90度修正
            bullet.rotation = bullet.angle + 90.f;
        }
        bullet.lifeTime += deltaTime;

        // 超出屏幕检测
        float margin = 100.f;
        if (bullet.position.y < -margin || bullet.position.y > windowSize.y + margin ||
            bullet.position.x < -margin || bullet.position.x > windowSize.x + margin) {
            bullet.state = EBS_DESTROYING;
            bullet.is_active = false;
        }
    }

    // 更新击中特效
    for (auto& effect : m_hitEffects) {
        if (!effect.active) continue;
        effect.timer -= deltaTime;
        if (effect.timer <= 0.f) {
            effect.active = false;
        }
    }
}

void EnemyBulletManager::render(sf::RenderWindow& window) {
    // 渲染击中特效
    for (const auto& effect : m_hitEffects) {
        if (!effect.active) continue;
        sf::Sprite sprite(m_hitEffectTexture);
        int frameX = (effect.frame % 8) * 64;
        sprite.setTextureRect(sf::IntRect(frameX, 0, 64, 64));
        sprite.setOrigin(32.f, 32.f);
        sprite.setPosition(effect.position);
        sprite.setScale(0.8f, 0.8f);
        sf::Uint8 alpha = static_cast<sf::Uint8>(255 * (1.f - effect.timer / 0.3f));
        sprite.setColor(sf::Color(255, 255, 255, alpha));
        window.draw(sprite);
    }

    // 渲染弹幕 - 使用 assets/dbullet 目录下的贴图，按类型分组
    for (const auto& bullet : m_bullets) {
        if (!bullet.is_active) continue;

        int typeIdx = static_cast<int>(bullet.type);
        int fileIdx = bullet.texFileIndex;

        if (typeIdx < BULLET_TYPE_COUNT) {
            // 普通子弹
            if (typeIdx < static_cast<int>(m_bulletTypeTextures.size()) &&
                fileIdx < static_cast<int>(m_bulletTypeTextures[typeIdx].size())) {
                sf::Sprite sprite(m_bulletTypeTextures[typeIdx][fileIdx]);
                sf::Vector2u texSize = m_bulletTypeTextures[typeIdx][fileIdx].getSize();
                sprite.setOrigin(texSize.x / 2.f, texSize.y / 2.f);
                sprite.setPosition(bullet.position);
                sprite.setRotation(bullet.rotation);
                sprite.setScale(1.5f, 1.5f);
                window.draw(sprite);
            }
        } else {
            // 自动旋转子弹
            int autoIdx = typeIdx - BULLET_TYPE_COUNT;
            if (autoIdx < static_cast<int>(m_autoRotateTypeTextures.size()) &&
                fileIdx < static_cast<int>(m_autoRotateTypeTextures[autoIdx].size())) {
                sf::Sprite sprite(m_autoRotateTypeTextures[autoIdx][fileIdx]);
                sf::Vector2u texSize = m_autoRotateTypeTextures[autoIdx][fileIdx].getSize();
                sprite.setOrigin(texSize.x / 2.f, texSize.y / 2.f);
                sprite.setPosition(bullet.position);
                sprite.setRotation(bullet.rotation);
                sprite.setScale(1.5f, 1.5f);
                window.draw(sprite);
            }
        }
    }
}

void EnemyBulletManager::triggerHitEffect(sf::Vector2f pos) {
    for (auto& effect : m_hitEffects) {
        if (!effect.active) {
            effect.position = pos;
            effect.timer = 0.3f;
            effect.active = true;
            effect.frame = 0;
            break;
        }
    }
}

int EnemyBulletManager::getTotalTextureCount() const {
    int count = 0;
    for (const auto& typeVec : m_bulletTypeTextures) {
        count += static_cast<int>(typeVec.size());
    }
    return count;
}

int EnemyBulletManager::getAutoRotateTextureCount() const {
    int count = 0;
    for (const auto& typeVec : m_autoRotateTypeTextures) {
        count += static_cast<int>(typeVec.size());
    }
    return count;
}

int EnemyBulletManager::getActiveCount() const {
    int count = 0;
    for (const auto& b : m_bullets) {
        if (b.is_active) ++count;
    }
    return count;
}

void EnemyBulletManager::clearAll() {
    for (auto& bullet : m_bullets) {
        bullet.is_active = false;
        bullet.state = EBS_UNUSED;
    }
    for (auto& effect : m_hitEffects) {
        effect.active = false;
    }
}

bool EnemyBulletManager::checkPlayerCollision(sf::Vector2f playerPos, float playerRadius) {
    for (const auto& bullet : m_bullets) {
        if (!bullet.is_active || bullet.state != EBS_ACTIVE) continue;

        sf::Vector2f diff = bullet.position - playerPos;
        float dist = std::sqrt(diff.x * diff.x + diff.y * diff.y);
        if (dist < bullet.hitboxRadius + playerRadius) {
            return true;
        }
    }
    return false;
}