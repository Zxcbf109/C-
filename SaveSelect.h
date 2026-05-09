#pragma once
#include <SFML/Graphics.hpp>
#include <string>

// ---- Save Data ----
struct SaveData {
    int difficulty;  // 0=EASY, 1=MEDIUM, 2=HARD, 3=LUNATIC
    int stage;
    int progress;    // 游戏进度（百分比）
    int lives;
    int bombs;
};

static constexpr int NUM_SAVE_SLOTS = 3;
static constexpr const char* SAVE_DIR = "assets/saves/";

bool saveFileExists(const std::string& path);
bool saveToFile(const SaveData& data, const std::string& path);
bool loadFromFile(SaveData& data, const std::string& path);
bool anySaveExists();
std::string getSavePath(int slot);

// ---- Save Selection Screen ----
class SaveSelect {
public:
    explicit SaveSelect(const sf::Font& font);

    void refresh();  // re-scan save files
    int  handleEvent(const sf::Event& event);  // -1=none, 0=back, 1=load/save
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);

    // For loading
    const SaveData& getLoadedSave() const { return m_saves[m_selected]; }
    int  getSelectedSlot() const { return m_selected; }

    // For saving
    void setSaveData(const SaveData& data) { m_saveDataToSave = data; }
    bool isSaveMode() const { return m_isSaveMode; }
    void setSaveMode(bool saveMode) { m_isSaveMode = saveMode; }

private:
    int m_selected = 0;
    bool m_saveExists[NUM_SAVE_SLOTS];
    SaveData m_saves[NUM_SAVE_SLOTS];
    SaveData m_saveDataToSave;  // Data to save when in save mode
    bool m_isSaveMode = false;  // true=save mode, false=load mode

    sf::Text m_title;
    sf::Text m_slotNames[NUM_SAVE_SLOTS];
    sf::Text m_slotInfos[NUM_SAVE_SLOTS];
    float m_fontSizes[NUM_SAVE_SLOTS];

    // Highlight box and arrow
    sf::RectangleShape m_highlight;
    sf::CircleShape m_arrow;
    float m_arrowTimer = 0.f;

    static constexpr float TITLE_SIZE = 48.f;       // 标题字号
    static constexpr float SLOT_SIZE = 40.f;        // Slot文字字号
    static constexpr float INFO_SIZE = 24.f;        // 说明文字字号
    static constexpr float SELECTED_SIZE = 40.f;     // 选中时字号(不变)
    static constexpr float NORMAL_SIZE = 40.f;       // 普通时字号(不变)
};
