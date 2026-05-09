#include "SaveSelect.h"
#include "SoundManager.h"
#include "DifficultySelect.h"
#include <fstream>
#include <sstream>
#include <cmath>

// ---- Save file paths ----
std::string getSavePath(int slot) {
    return std::string(SAVE_DIR) + "save" + std::to_string(slot) + ".dat";
}

// ---- Save/Load functions ----
bool saveFileExists(const std::string& path) {
    std::ifstream f(path);
    return f.good();
}

bool saveToFile(const SaveData& data, const std::string& path) {
    std::ofstream out(path, std::ios::binary);
    if (!out) return false;
    out.write(reinterpret_cast<const char*>(&data), sizeof(data));
    return out.good();
}

bool loadFromFile(SaveData& data, const std::string& path) {
    std::ifstream in(path, std::ios::binary);
    if (!in) return false;
    in.read(reinterpret_cast<char*>(&data), sizeof(data));
    return in.good();
}

bool anySaveExists() {
    for (int i = 0; i < NUM_SAVE_SLOTS; ++i) {
        if (saveFileExists(getSavePath(i)))
            return true;
    }
    return false;
}

// ---- Difficulty name lookup ----
static const char* diffName(int d) {
    switch (d) {
        case 0: return "EASY";
        case 1: return "MEDIUM";
        case 2: return "HARD";
        case 3: return "LUNATIC";
        default: return "???";
    }
}

// ---- Layout constants ----
static constexpr float TITLE_Y = 120.f;        // 标题Y位置
static constexpr float SLOT_Y[NUM_SAVE_SLOTS] = { 220.f, 380.f, 540.f }; // Slot Y位置（文字中心）
static constexpr float INFO_OFFSET_Y = 50.f;  // 说明文字在Slot下方偏移（50px间距）
static constexpr float SCREEN_CENTER_X = 960.f; // 窗口总宽度1920的水平居中

// ---- SaveSelect implementation ----
SaveSelect::SaveSelect(const sf::Font& font) {
    // 标题：字号48，加粗，白色描边3px
    m_title = sf::Text("SELECT SLOT TO SAVE", font, 48);
    m_title.setStyle(sf::Text::Bold);
    m_title.setFillColor(sf::Color::White);
    m_title.setOutlineColor(sf::Color::Black);
    m_title.setOutlineThickness(3.f);

    for (int i = 0; i < NUM_SAVE_SLOTS; ++i) {
        m_saveExists[i] = false;
        m_fontSizes[i] = SLOT_SIZE;

        // 存档选项文字：字号40，加粗，白色描边3px
        m_slotNames[i] = sf::Text("", font, static_cast<unsigned int>(SLOT_SIZE));
        m_slotNames[i].setStyle(sf::Text::Bold);
        m_slotNames[i].setFillColor(sf::Color::White);
        m_slotNames[i].setOutlineColor(sf::Color::Black);
        m_slotNames[i].setOutlineThickness(3.f);

        // 说明文字：字号24，普通字重，白色描边3px
        m_slotInfos[i] = sf::Text("", font, 24);
        m_slotInfos[i].setFillColor(sf::Color::White);
        m_slotInfos[i].setOutlineColor(sf::Color::Black);
        m_slotInfos[i].setOutlineThickness(3.f);
    }

    m_fontSizes[0] = SELECTED_SIZE;
    m_slotNames[0].setFillColor(sf::Color::Yellow);

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

    refresh();
}

void SaveSelect::refresh() {
    // Update title based on mode
    if (m_isSaveMode) {
        m_title.setString("SELECT SLOT TO SAVE");
    } else {
        m_title.setString("SELECT SAVE");
    }

    for (int i = 0; i < NUM_SAVE_SLOTS; ++i) {
        std::string path = getSavePath(i);
        m_saveExists[i] = loadFromFile(m_saves[i], path);
        if (m_isSaveMode) {
            // Save mode: show slot info as normal, but can overwrite any slot
            m_slotNames[i].setString("Slot " + std::to_string(i + 1));
            if (m_saveExists[i]) {
                m_slotInfos[i].setString(
                    std::string(diffName(m_saves[i].difficulty))
                    + " | Stage " + std::to_string(m_saves[i].stage)
                    + " | Progress " + std::to_string(m_saves[i].progress) + "%"
                    + " [OVERWRITE]");
            } else {
                m_slotInfos[i].setString("--- Empty ---");
            }
        } else {
            // Load mode: only show existing saves
            m_slotNames[i].setString("Slot " + std::to_string(i + 1));
            if (m_saveExists[i]) {
                m_slotInfos[i].setString(
                    std::string(diffName(m_saves[i].difficulty))
                    + " | Stage " + std::to_string(m_saves[i].stage)
                    + " | Progress " + std::to_string(m_saves[i].progress) + "%");
            } else {
                m_slotInfos[i].setString("--- Empty ---");
            }
        }
    }
}

int SaveSelect::handleEvent(const sf::Event& event) {
    if (event.type != sf::Event::KeyPressed)
        return -1;

    if (event.key.code == sf::Keyboard::Escape)
        return 0;

    if (event.key.code == sf::Keyboard::Up) {
        int idx = m_selected - 1;
        if (idx < 0) idx = NUM_SAVE_SLOTS - 1;
        m_selected = idx;
        SoundManager::getInstance().playSFX("assets/sound/SFX/select.wav");
    } else if (event.key.code == sf::Keyboard::Down) {
        int idx = m_selected + 1;
        if (idx >= NUM_SAVE_SLOTS) idx = 0;
        m_selected = idx;
        SoundManager::getInstance().playSFX("assets/sound/SFX/select.wav");
    } else if (event.key.code == sf::Keyboard::Enter) {
        if (m_isSaveMode) {
            // Save mode: save to selected slot
            std::string path = getSavePath(m_selected);
            if (saveToFile(m_saveDataToSave, path)) {
                SoundManager::getInstance().playSFX("assets/sound/SFX/select.wav");
                return 1;  // Save successful
            }
        } else {
            // Load mode: only load from existing saves
            if (m_saveExists[m_selected]) {
                SoundManager::getInstance().playSFX("assets/sound/SFX/select.wav");
                return 1;
            }
        }
    }
    return -1;
}

void SaveSelect::update(float deltaTime) {
    float speed = 300.f * deltaTime;
    for (int i = 0; i < NUM_SAVE_SLOTS; ++i) {
        float target = (i == m_selected) ? SELECTED_SIZE : NORMAL_SIZE;
        if (m_fontSizes[i] < target)
            m_fontSizes[i] = std::min(m_fontSizes[i] + speed, target);
        else if (m_fontSizes[i] > target)
            m_fontSizes[i] = std::max(m_fontSizes[i] - speed, target);
        m_slotNames[i].setCharacterSize(static_cast<unsigned int>(m_fontSizes[i]));

        // Color: selected = yellow, others = white
        if (i == m_selected) {
            m_slotNames[i].setFillColor(sf::Color::Yellow);
        } else {
            m_slotNames[i].setFillColor(sf::Color::White);
        }
    }

    // Update highlight box - 完整包裹Slot文字+说明文字
    sf::FloatRect nameBounds = m_slotNames[m_selected].getLocalBounds();
    sf::FloatRect infoBounds = m_slotInfos[m_selected].getLocalBounds();
    float totalWidth = std::max(nameBounds.width, infoBounds.width) + 40.f;
    float totalHeight = nameBounds.height + infoBounds.height + 40.f;  // 上下各20px边距
    m_highlight.setSize(sf::Vector2f(totalWidth, totalHeight));
    m_highlight.setOrigin(totalWidth / 2.f, totalHeight / 2.f);

    // Highlight box follows selection smoothly
    // 框中心Y = Slot文字中心Y + 20（文字和说明文字中间的位置）
    float boxCenterY = SLOT_Y[m_selected] + 20.f;
    sf::Vector2f hlPos = m_highlight.getPosition();
    m_highlight.setPosition(SCREEN_CENTER_X, hlPos.y + (boxCenterY - hlPos.y) * 0.2f);

    // Arrow wobble animation
    m_arrowTimer += deltaTime * 8.f;
    float arrowY = SLOT_Y[m_selected];
    m_arrow.setPosition(SCREEN_CENTER_X - totalWidth / 2.f - 30.f, arrowY);
    m_arrow.setScale(1.f + std::sin(m_arrowTimer) * 0.15f, 1.f + std::sin(m_arrowTimer) * 0.15f);
}

void SaveSelect::draw(sf::RenderWindow& window) {
    // 计算标题位置（水平居中）
    sf::FloatRect titleBounds = m_title.getLocalBounds();
    m_title.setOrigin(titleBounds.width / 2.f, titleBounds.height / 2.f);
    m_title.setPosition(SCREEN_CENTER_X, TITLE_Y);

    // 绘制标题
    window.draw(m_title);

    // 绘制选中框和箭头（背后）
    window.draw(m_highlight);
    window.draw(m_arrow);

    // 绘制每个存档选项及其说明
    for (int i = 0; i < NUM_SAVE_SLOTS; ++i) {
        float slotY = SLOT_Y[i];

        // 存档选项文字：水平居中
        sf::FloatRect nameBounds = m_slotNames[i].getLocalBounds();
        m_slotNames[i].setOrigin(nameBounds.width / 2.f, nameBounds.height / 2.f);
        m_slotNames[i].setPosition(SCREEN_CENTER_X, slotY);
        window.draw(m_slotNames[i]);

        // 说明文字：在选项下方40像素，水平居中
        sf::FloatRect infoBounds = m_slotInfos[i].getLocalBounds();
        m_slotInfos[i].setOrigin(infoBounds.width / 2.f, infoBounds.height / 2.f);
        m_slotInfos[i].setPosition(SCREEN_CENTER_X, slotY + INFO_OFFSET_Y);
        window.draw(m_slotInfos[i]);
    }
}
