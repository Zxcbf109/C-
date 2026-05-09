#include "DifficultySelect.h"
#include "SoundManager.h"
#include <cmath>

DifficultyParams getEasyParams()    { return {0.6f, 0.5f, 5, 4}; }
DifficultyParams getMediumParams()  { return {0.8f, 0.8f, 4, 3}; }
DifficultyParams getHardParams()    { return {1.0f, 1.0f, 3, 2}; }
DifficultyParams getLunaticParams() { return {1.3f, 1.5f, 2, 1}; }

// ---- Layout constants ----
static constexpr float SLOT_Y[DIFFICULTY_COUNT] = { 200.f, 360.f, 520.f, 680.f }; // 难度选项Y位置
static constexpr float INFO_OFFSET_Y = 50.f;  // 说明文字在选项下方偏移（50px间距）
static constexpr float SCREEN_CENTER_X = 640.f; // 窗口总宽度1280的水平居中

static const char* NAMES[DIFFICULTY_COUNT]      = { "EASY", "MEDIUM", "HARD", "LUNATIC" };
static const wchar_t* SUBTITLES[DIFFICULTY_COUNT] = {
    L"面向弹幕苦手的难度",
    L"面向普通玩家的难度",
    L"面向高手的难度",
    L"面向prooo的难度"
};

DifficultySelect::DifficultySelect(const sf::Font& font) {
    for (int i = 0; i < DIFFICULTY_COUNT; ++i) {
        m_names[i]       = sf::Text(NAMES[i],     font, static_cast<unsigned int>(NORMAL_SIZE));
        m_subtitles[i]   = sf::Text(SUBTITLES[i], font, static_cast<unsigned int>(SUB_NORMAL_SIZE));
        m_fontSizes[i]   = NORMAL_SIZE;
        m_subFontSizes[i] = SUB_NORMAL_SIZE;
        m_names[i].setStyle(sf::Text::Bold);
        m_names[i].setFillColor(sf::Color::White);
        m_names[i].setOutlineColor(sf::Color::Black);
        m_names[i].setOutlineThickness(3.f);
        m_subtitles[i].setFillColor(sf::Color::White);
        m_subtitles[i].setOutlineColor(sf::Color::Black);
        m_subtitles[i].setOutlineThickness(3.f);
    }
    m_fontSizes[0] = SELECTED_SIZE;
    m_subFontSizes[0] = SUB_SELECTED_SIZE;
    m_names[0].setFillColor(sf::Color::Yellow);
    m_currentParams = getEasyParams();
    m_arrowTimer = 0.f;

    // 黄色选中框：动态大小，初始时先设为0
    m_highlight.setSize(sf::Vector2f(0.f, 0.f));
    m_highlight.setFillColor(sf::Color(255, 255, 100, 60));
    m_highlight.setOutlineColor(sf::Color(255, 255, 0, 200));
    m_highlight.setOutlineThickness(3.f);

    // 选中箭头
    m_arrow.setRadius(8.f);
    m_arrow.setPointCount(3);
    m_arrow.setFillColor(sf::Color(255, 255, 0));
    m_arrow.setOrigin(8.f, 4.f);
}

void DifficultySelect::reset() {
    m_selected = EASY;
    m_currentParams = getEasyParams();
    for (int i = 0; i < DIFFICULTY_COUNT; ++i) {
        m_fontSizes[i]    = (i == 0) ? SELECTED_SIZE : NORMAL_SIZE;
        m_subFontSizes[i] = (i == 0) ? SUB_SELECTED_SIZE : SUB_NORMAL_SIZE;
        m_names[i].setFillColor(sf::Color::White);
    }
    m_names[0].setFillColor(sf::Color::Yellow);
    m_arrowTimer = 0.f;
}

int DifficultySelect::handleEvent(const sf::Event& event) {
    if (event.type != sf::Event::KeyPressed)
        return -1;

    if (event.key.code == sf::Keyboard::Escape)
        return 0;

    if (event.key.code == sf::Keyboard::Up) {
        int idx = static_cast<int>(m_selected) - 1;
        if (idx < 0) idx = DIFFICULTY_COUNT - 1;
        m_selected = static_cast<Difficulty>(idx);
        SoundManager::getInstance().playSFX("assets/sound/SFX/select.wav");
    } else if (event.key.code == sf::Keyboard::Down) {
        int idx = static_cast<int>(m_selected) + 1;
        if (idx >= DIFFICULTY_COUNT) idx = 0;
        m_selected = static_cast<Difficulty>(idx);
        SoundManager::getInstance().playSFX("assets/sound/SFX/select.wav");
    } else if (event.key.code == sf::Keyboard::Enter) {
        switch (m_selected) {
            case EASY:    m_currentParams = getEasyParams();    break;
            case MEDIUM:  m_currentParams = getMediumParams();  break;
            case HARD:    m_currentParams = getHardParams();    break;
            case LUNATIC: m_currentParams = getLunaticParams(); break;
        }
        SoundManager::getInstance().playSFX("assets/sound/SFX/select.wav");
        return 1;
    }
    return -1;
}

void DifficultySelect::update(float deltaTime) {
    float speed = 300.f * deltaTime;
    for (int i = 0; i < DIFFICULTY_COUNT; ++i) {
        float target = (i == m_selected) ? SELECTED_SIZE : NORMAL_SIZE;
        if (m_fontSizes[i] < target)
            m_fontSizes[i] = std::min(m_fontSizes[i] + speed, target);
        else if (m_fontSizes[i] > target)
            m_fontSizes[i] = std::max(m_fontSizes[i] - speed, target);
        m_names[i].setCharacterSize(static_cast<unsigned int>(m_fontSizes[i]));

        float subTarget = (i == m_selected) ? SUB_SELECTED_SIZE : SUB_NORMAL_SIZE;
        if (m_subFontSizes[i] < subTarget)
            m_subFontSizes[i] = std::min(m_subFontSizes[i] + speed, subTarget);
        else if (m_subFontSizes[i] > subTarget)
            m_subFontSizes[i] = std::max(m_subFontSizes[i] - speed, subTarget);
        m_subtitles[i].setCharacterSize(static_cast<unsigned int>(m_subFontSizes[i]));

        // Color: selected = yellow, others = white (难度和描述都要变)
        if (i == m_selected) {
            m_names[i].setFillColor(sf::Color::Yellow);
            m_subtitles[i].setFillColor(sf::Color::Yellow);
        } else {
            m_names[i].setFillColor(sf::Color::White);
            m_subtitles[i].setFillColor(sf::Color::White);
        }
    }

    // Update highlight box - 固定高度110px，完全包裹难度文字+说明文字
    sf::FloatRect nameBounds = m_names[m_selected].getLocalBounds();
    sf::FloatRect subBounds = m_subtitles[m_selected].getLocalBounds();
    float totalWidth = std::max(nameBounds.width, subBounds.width) + 40.f;
    float totalHeight = 110.f;  // 固定高度110px
    m_highlight.setSize(sf::Vector2f(totalWidth, totalHeight));
    m_highlight.setOrigin(totalWidth / 2.f, totalHeight / 2.f);

    // Highlight box follows selection smoothly
    // 框中心位于：难度Y + 文字总高度/2，使框完全包裹文字
    float textCenterY = SLOT_Y[m_selected] + (nameBounds.height + INFO_OFFSET_Y + subBounds.height) / 2.f;
    sf::Vector2f hlPos = m_highlight.getPosition();
    m_highlight.setPosition(SCREEN_CENTER_X, hlPos.y + (textCenterY - hlPos.y) * 0.2f);

    // Arrow wobble animation
    m_arrowTimer += deltaTime * 8.f;
    float arrowY = SLOT_Y[m_selected];
    m_arrow.setPosition(SCREEN_CENTER_X - totalWidth / 2.f - 30.f, arrowY);
    m_arrow.setScale(1.f + std::sin(m_arrowTimer) * 0.15f, 1.f + std::sin(m_arrowTimer) * 0.15f);
}

void DifficultySelect::draw(sf::RenderWindow& window) {
    // 先绘制每个难度选项及其说明（文字在高亮框下面）
    for (int i = 0; i < DIFFICULTY_COUNT; ++i) {
        float slotY = SLOT_Y[i];

        // 难度选项文字：水平居中
        sf::FloatRect nameBounds = m_names[i].getLocalBounds();
        m_names[i].setOrigin(nameBounds.width / 2.f, nameBounds.height / 2.f);
        m_names[i].setPosition(SCREEN_CENTER_X, slotY);
        window.draw(m_names[i]);

        // 说明文字：在选项下方40像素，水平居中
        sf::FloatRect subBounds = m_subtitles[i].getLocalBounds();
        m_subtitles[i].setOrigin(subBounds.width / 2.f, subBounds.height / 2.f);
        m_subtitles[i].setPosition(SCREEN_CENTER_X, slotY + INFO_OFFSET_Y);
        window.draw(m_subtitles[i]);
    }

    // 再绘制选中框和箭头（在文字上面）
    window.draw(m_highlight);
    window.draw(m_arrow);
}
