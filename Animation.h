#pragma once
#include <SFML/Graphics.hpp>
#include <vector>
#include <string>

// ---- 动画帧 ----
struct AnimFrame {
    sf::IntRect rect;   // 贴图区域
    float duration;     // 持续时间（秒）
    float anchorX;      // 锚点X
    float anchorY;      // 锚点Y
};

// ---- 动画序列 ----
struct Animation {
    std::string name;
    std::vector<AnimFrame> frames;
    bool loop = true;
    float totalDuration = 0.f;
};