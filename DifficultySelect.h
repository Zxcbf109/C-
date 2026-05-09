#pragma once
#include <SFML/Graphics.hpp>

enum Difficulty { EASY = 0, MEDIUM = 1, HARD = 2, LUNATIC = 3, DIFFICULTY_COUNT = 4 };

struct DifficultyParams {
    float bullet_speed_mult;
    float enemy_hp_mult;
    int   starting_lives;
    int   starting_bombs;
};

DifficultyParams getEasyParams();
DifficultyParams getMediumParams();
DifficultyParams getHardParams();
DifficultyParams getLunaticParams();

class DifficultySelect {
public:
    explicit DifficultySelect(const sf::Font& font);

    void reset();
    int  handleEvent(const sf::Event& event);   // returns: -1=no change, 0=back to menu, 1=start game
    void update(float deltaTime);
    void draw(sf::RenderWindow& window);

    Difficulty getSelectedDifficulty() const { return m_selected; }
    const DifficultyParams& getCurrentParams() const { return m_currentParams; }

private:
    Difficulty m_selected = EASY;
    DifficultyParams m_currentParams;

    sf::Text m_names[DIFFICULTY_COUNT];
    sf::Text m_subtitles[DIFFICULTY_COUNT];
    float m_fontSizes[DIFFICULTY_COUNT];
    float m_subFontSizes[DIFFICULTY_COUNT];

    // Highlight box and arrow
    sf::RectangleShape m_highlight;
    sf::CircleShape m_arrow;
    float m_arrowTimer;

    static constexpr float SELECTED_SIZE = 52.f;   // 选中时字号放大
    static constexpr float NORMAL_SIZE = 44.f;     // 普通时字号
    static constexpr float SUB_SELECTED_SIZE = 32.f;
    static constexpr float SUB_NORMAL_SIZE = 26.f;
};
