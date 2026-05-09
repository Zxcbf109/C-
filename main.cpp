#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <iostream>
#include <array>
#include <cmath>
#include "SoundManager.h"
#include "DifficultySelect.h"
#include "SaveSelect.h"
#include "Player.h"
#include "GameUI.h"
#include "EnemyBullet.h"
#include "Boss.h"

int main() {
    // 获取屏幕分辨率
    sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
    float screenWidth = static_cast<float>(desktop.width);
    float screenHeight = static_cast<float>(desktop.height);

    // 游戏区域比例 1:1 (正方形)
    const float GAME_WIDTH = 960.f;
    const float GAME_HEIGHT = 960.f;
    const float UI_WIDTH = 320.f;
    const float TOTAL_WIDTH = GAME_WIDTH + UI_WIDTH;

    // 根据屏幕高度计算窗口大小，1:1比例
    float windowSize = screenHeight * 0.95f;

    // 如果正方形窗口宽度超出屏幕，等比例缩小
    if (windowSize > screenWidth * 0.95f) {
        windowSize = screenWidth * 0.95f;
    }

    // 初始为窗口模式，可通过F11切换全屏
    sf::RenderWindow window(sf::VideoMode(static_cast<unsigned int>(windowSize), static_cast<unsigned int>(windowSize)),
                             "Ice Fairy Bullet Hell", sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);
    window.setFramerateLimit(60);
    window.setVerticalSyncEnabled(true);

    // 全屏状态
    bool isFullscreen = false;
    float currentWindowSize = windowSize;

    SoundManager& sound = SoundManager::getInstance();

    // 游戏区域坐标和尺寸常量
    const float CENTER_X = GAME_WIDTH / 2.f;  // 387.5f

    // 计算缩放比例以适配窗口
    float scaleX = windowSize / TOTAL_WIDTH;
    float scaleY = windowSize / GAME_HEIGHT;
    float scale = std::min(scaleX, scaleY);

    // 加载游戏背景 (gamebg.png 填充整个窗口)
    sf::Texture gameBgTexture;
    if (!gameBgTexture.loadFromFile("assets/gamebg.png")) {
        std::cerr << "Failed to load gamebg.png!" << std::endl;
        return 1;
    }
    sf::Sprite gameBgSprite(gameBgTexture);
    // 等比例缩放以填满整个窗口
    sf::Vector2u gameBgSize = gameBgTexture.getSize();
    float gameBgScaleX = windowSize / gameBgSize.x;
    float gameBgScaleY = windowSize / gameBgSize.y;
    float gameBgScale = std::max(gameBgScaleX, gameBgScaleY);
    gameBgSprite.setScale(gameBgScale, gameBgScale);
    gameBgSprite.setOrigin(gameBgSize.x / 2.f, gameBgSize.y / 2.f);
    gameBgSprite.setPosition(windowSize / 2.f, windowSize / 2.f);

    // 加载字体
    sf::Font font;
    if (!font.loadFromFile("C:/Windows/Fonts/msyh.ttc")) {
        std::cerr << "Failed to load font!" << std::endl;
        return 1;
    }

    // 选项枚举
    enum Option { START, CONTINUE, SOUND, EXIT, OPTION_COUNT };
    Option selected = START;

    // 难度选择页面
    DifficultySelect difficultySelect(font);

    // 存档选择页面
    SaveSelect saveSelect(font);

    // 当前难度参数
    DifficultyParams currentDifficultyParams = getEasyParams();
    int currentSaveSlot = 0;
    bool hasAnySave = anySaveExists();

    // 主菜单文字 - 改进版选择效果
    struct MenuText {
        sf::Text text;
        float currentSize = 36.f;
    };
    std::array<MenuText, OPTION_COUNT> menuItems;
    std::array<std::string, OPTION_COUNT> menuLabels = {"START", "CONTINUE", "SOUND", "EXIT"};

    // 菜单统一居中
    const float MENU_CENTER_X = 480.f;  // 居中X坐标
    const float MENU_START_Y = 380.f;    // 起始Y坐标
    const float MENU_SPACING = 100.f;    // 选项间距

    for (int i = 0; i < OPTION_COUNT; ++i) {
        menuItems[i].text.setFont(font);
        menuItems[i].text.setString(menuLabels[i]);
        menuItems[i].text.setCharacterSize(static_cast<unsigned int>(menuItems[i].currentSize));
        menuItems[i].text.setStyle(sf::Text::Bold);
        menuItems[i].text.setFillColor(sf::Color::White);
        menuItems[i].text.setOutlineColor(sf::Color::Black);
        menuItems[i].text.setOutlineThickness(2.f);

        float baseY = MENU_START_Y + i * MENU_SPACING;
        menuItems[i].text.setOrigin(
            menuItems[i].text.getLocalBounds().width / 2.f,
            menuItems[i].text.getLocalBounds().height / 2.f
        );
        menuItems[i].text.setPosition(MENU_CENTER_X, baseY);
    }

    // 选中高亮框 - 居中于菜单项
    sf::RectangleShape menuHighlight(sf::Vector2f(220.f, 60.f));
    menuHighlight.setFillColor(sf::Color(255, 255, 100, 60));
    menuHighlight.setOutlineColor(sf::Color(255, 255, 0, 200));
    menuHighlight.setOutlineThickness(3.f);
    menuHighlight.setOrigin(110.f, 30.f);
    menuHighlight.setPosition(MENU_CENTER_X, MENU_START_Y);

    // 选中箭头 - 居中于高亮框左侧
    sf::CircleShape menuArrow(8.f, 3);
    menuArrow.setFillColor(sf::Color(255, 255, 0));
    menuArrow.setOrigin(8.f, 4.f);
    menuArrow.setPosition(MENU_CENTER_X - 140.f, MENU_START_Y);
    float arrowTimer = 0.f;

    // 菜单状态：0=菜单，1=游戏页面，2=音效设置，3=难度选择，4=暂停，5=存档选择，6=退出存档
    int state = 0;

    // 当前游戏数据（用于存档）
    SaveData currentGameData;

    // 玩家角色
    Player player;

    // 敌弹管理器
    EnemyBulletManager enemyBulletManager;
    // 加载敌弹贴图（从新素材目录）
    enemyBulletManager.loadTextures();

    // Boss管理器
    BossManager bossManager;
    bossManager.initTexture();

    // 游戏UI（分数、残机、符卡等）
    GameUI gameUI;
    gameUI.getStats().difficulty = static_cast<int>(difficultySelect.getSelectedDifficulty());
    gameUI.getStats().progress = 0;
    gameUI.getStats().graze = 0;
    // life和bombs由Player初始化，这里同步到UI
    gameUI.getStats().life = player.getLife();
    gameUI.getStats().bombs = player.getBombs();

    // 关联敌弹管理器到玩家
    player.setEnemyBulletManager(&enemyBulletManager);

    // 游戏阶段
    enum GamePhase { PHASE_GAME, PHASE_BOSS, PHASE_BOSS_DEFEATED, PHASE_STAGE_CLEAR };
    GamePhase gamePhase = PHASE_GAME;

    // Boss出现计时
    float bossSpawnTimer = 5.f;  // 5秒后出现Boss
    bool bossSpawned = false;

    // ---- 关卡清除文字 ----
    sf::Text stageClearText;
    stageClearText.setFont(font);
    stageClearText.setString("STAGE CLEAR");
    stageClearText.setCharacterSize(64);
    stageClearText.setStyle(sf::Text::Bold);
    stageClearText.setFillColor(sf::Color::Yellow);
    stageClearText.setOutlineColor(sf::Color::Black);
    stageClearText.setOutlineThickness(3.f);
    float stageClearTimer = 0.f;

    // ---- 暂停菜单 ----
    enum PauseOption { CONTINUE_GAME, EXIT_TO_MENU, PAUSE_OPTION_COUNT };
    PauseOption pauseSelected = CONTINUE_GAME;
    const float pauseSelectedSize = 48.f;
    const float pauseNormalSize = 36.f;

    sf::Text pauseContinueText(L"继续", font, 36);
    sf::Text pauseExitText(L"退出", font, 36);
    pauseContinueText.setStyle(sf::Text::Bold);
    pauseContinueText.setFillColor(sf::Color::White);
    pauseContinueText.setOutlineColor(sf::Color::Black);
    pauseContinueText.setOutlineThickness(2.f);
    pauseExitText.setStyle(sf::Text::Bold);
    pauseExitText.setFillColor(sf::Color::White);
    pauseExitText.setOutlineColor(sf::Color::Black);
    pauseExitText.setOutlineThickness(2.f);

    float pauseFontSizes[PAUSE_OPTION_COUNT] = { 48.f, 36.f };

    // 暂停菜单垂直位置：居中，间距100px
    const float pauseCenterY = 480.f;
    const float pauseContinueY = pauseCenterY - 50.f;  // 继续在上方
    const float pauseExitY = pauseCenterY + 50.f;      // 退出在下方

    // 设置初始位置
    pauseContinueText.setOrigin(18.f, 18.f);  // 大约居中
    pauseContinueText.setPosition(CENTER_X, pauseContinueY);
    pauseExitText.setOrigin(18.f, 18.f);
    pauseExitText.setPosition(CENTER_X, pauseExitY);

    // 半透明遮罩 (修改为游戏区域大小)
    sf::RectangleShape pauseOverlay(sf::Vector2f(GAME_WIDTH, GAME_HEIGHT));
    pauseOverlay.setFillColor(sf::Color(0, 0, 0, 160));

    // ---- 音效设置页面的 UI ----
    sf::Text soundTitle("SOUND SETTINGS", font, 51);
    soundTitle.setPosition(270, 192);
    soundTitle.setStyle(sf::Text::Bold);
    soundTitle.setFillColor(sf::Color::Black);

    sf::Text sfxLabel("SFX Volume", font, 36);
    sfxLabel.setPosition(225, 400);
    sfxLabel.setStyle(sf::Text::Bold);
    sfxLabel.setFillColor(sf::Color::Black);
    sf::Text bgmLabel("BGM Volume", font, 36);
    bgmLabel.setPosition(225, 560);
    bgmLabel.setStyle(sf::Text::Bold);
    bgmLabel.setFillColor(sf::Color::Black);

    sf::Text sfxValue("50", font, 36);
    sfxValue.setPosition(550, 400);
    sfxValue.setStyle(sf::Text::Bold);
    sfxValue.setFillColor(sf::Color::Black);
    sf::Text bgmValue("50", font, 36);
    bgmValue.setPosition(550, 560);
    bgmValue.setStyle(sf::Text::Bold);
    bgmValue.setFillColor(sf::Color::Black);

    // 音量挡位条 (用矩形绘制)
    const float barX = 225.f;
    const float barWidth = 266.f;
    const float barHeight = 24.f;

    sf::RectangleShape sfxBarBg(sf::Vector2f(barWidth, barHeight));
    sfxBarBg.setPosition(barX, 488);
    sfxBarBg.setFillColor(sf::Color(60, 60, 60));
    sfxBarBg.setOutlineColor(sf::Color::White);
    sfxBarBg.setOutlineThickness(2.f);

    sf::RectangleShape bgmBarBg(sf::Vector2f(barWidth, barHeight));
    bgmBarBg.setPosition(barX, 648);
    bgmBarBg.setFillColor(sf::Color(60, 60, 60));
    bgmBarBg.setOutlineColor(sf::Color::White);
    bgmBarBg.setOutlineThickness(2.f);

    sf::RectangleShape sfxBarFill(sf::Vector2f(barWidth, barHeight));
    sfxBarFill.setPosition(barX, 488);
    sfxBarFill.setFillColor(sf::Color(0, 200, 100));

    sf::RectangleShape bgmBarFill(sf::Vector2f(barWidth, barHeight));
    bgmBarFill.setPosition(barX, 648);
    bgmBarFill.setFillColor(sf::Color(0, 200, 100));

    // 提示文字
    sf::Text hintText("Left/Right: Adjust   Enter=Back", font, 23);
    hintText.setPosition(252, 784);
    hintText.setStyle(sf::Text::Bold);
    hintText.setFillColor(sf::Color::Black);

    // 音效设置页面的选择（0=SFX, 1=BGM）
    int soundSelectIndex = 0;

    // 音量目标值（初始化与 SoundManager 同步）
    float sfxTarget = sound.getSFXVolume();
    float bgmTarget = sound.getBGMVolume();

    sf::Clock clock;

    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }

            // 处理窗口大小调整
            if (event.type == sf::Event::Resized) {
                sf::Vector2u newSize = window.getSize();
                float newWidth = static_cast<float>(newSize.x);
                float newHeight = static_cast<float>(newSize.y);

                // 重新计算缩放比例以适配新的窗口大小
                float newScaleX = newWidth / TOTAL_WIDTH;
                float newScaleY = newHeight / GAME_HEIGHT;
                float newScale = std::min(newScaleX, newScaleY);

                // 更新背景缩放
                sf::Vector2u gameBgSize = gameBgTexture.getSize();
                float gameBgScaleX = newWidth / gameBgSize.x;
                float gameBgScaleY = newHeight / gameBgSize.y;
                float gameBgScale = std::max(gameBgScaleX, gameBgScaleY);
                gameBgSprite.setScale(gameBgScale, gameBgScale);
                gameBgSprite.setOrigin(gameBgSize.x / 2.f, gameBgSize.y / 2.f);
                gameBgSprite.setPosition(newWidth / 2.f, newHeight / 2.f);

                // 重置默认视图以匹配新窗口大小
                window.setView(window.getDefaultView());
            }

            // F11切换全屏
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::F11) {
                isFullscreen = !isFullscreen;
                if (isFullscreen) {
                    window.create(sf::VideoMode(static_cast<unsigned int>(screenWidth), static_cast<unsigned int>(screenHeight)),
                                  "Ice Fairy Bullet Hell", sf::Style::Fullscreen);
                    currentWindowSize = screenHeight;
                } else {
                    window.create(sf::VideoMode(static_cast<unsigned int>(windowSize), static_cast<unsigned int>(windowSize)),
                                  "Ice Fairy Bullet Hell", sf::Style::Titlebar | sf::Style::Close | sf::Style::Resize);
                    currentWindowSize = windowSize;
                }
                window.setFramerateLimit(60);
                window.setVerticalSyncEnabled(true);

                // 更新背景缩放
                sf::Vector2u gameBgSize = gameBgTexture.getSize();
                float gameBgScaleX = currentWindowSize / gameBgSize.x;
                float gameBgScaleY = currentWindowSize / gameBgSize.y;
                float gameBgScale = std::max(gameBgScaleX, gameBgScaleY);
                gameBgSprite.setScale(gameBgScale, gameBgScale);
                gameBgSprite.setOrigin(gameBgSize.x / 2.f, gameBgSize.y / 2.f);
                gameBgSprite.setPosition(currentWindowSize / 2.f, currentWindowSize / 2.f);
            }

            if (state == 0) {
                // ---- 主菜单 ----
                if (event.type == sf::Event::KeyPressed) {
                    Option prev = selected;
                    if (event.key.code == sf::Keyboard::Up) {
                        int idx = static_cast<int>(selected);
                        do {
                            idx = (idx - 1 + OPTION_COUNT) % OPTION_COUNT;
                        } while (idx == CONTINUE && !hasAnySave);
                        selected = static_cast<Option>(idx);
                    } else if (event.key.code == sf::Keyboard::Down) {
                        int idx = static_cast<int>(selected);
                        do {
                            idx = (idx + 1) % OPTION_COUNT;
                        } while (idx == CONTINUE && !hasAnySave);
                        selected = static_cast<Option>(idx);
                    } else if (event.key.code == sf::Keyboard::Enter) {
                        if (selected == EXIT) {
                            window.close();
                        } else {
                            sound.playSFX("assets/sound/SFX/select.wav");
                            if (selected == START) {
                                difficultySelect.reset();
                                state = 3;
                            } else if (selected == CONTINUE && hasAnySave) {
                                saveSelect.setSaveMode(false);
                                saveSelect.refresh();
                                state = 5;
                            } else if (selected == SOUND) {
                                state = 2;
                                soundSelectIndex = 0;
                            }
                        }
                    }
                    if (selected != prev) {
                        sound.playSFX("assets/sound/SFX/select.wav");
                    }
                }
            } else if (state == 1) {
                // ---- 游戏页面 ----
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Escape) {
                        state = 4;
                        pauseSelected = CONTINUE_GAME;
                        pauseFontSizes[0] = pauseSelectedSize;
                        pauseFontSizes[1] = pauseNormalSize;
                    }
                }
            } else if (state == 2) {
                // ---- 音效设置页面 ----
                if (event.type == sf::Event::KeyPressed) {
                    if (event.key.code == sf::Keyboard::Escape ||
                        event.key.code == sf::Keyboard::Enter) {
                        state = 0;
                        hasAnySave = anySaveExists();
                        sound.playSFX("assets/sound/SFX/select.wav");
                    } else if (event.key.code == sf::Keyboard::Up) {
                        soundSelectIndex = 0;
                        sound.playSFX("assets/sound/SFX/select.wav");
                    } else if (event.key.code == sf::Keyboard::Down) {
                        soundSelectIndex = 1;
                        sound.playSFX("assets/sound/SFX/select.wav");
                    } else if (event.key.code == sf::Keyboard::Left) {
                        float* vol = (soundSelectIndex == 0)
                            ? &sfxTarget : &bgmTarget;
                        *vol = std::max(0.f, *vol - 10.f);
                        sound.playSFX("assets/sound/SFX/select.wav");
                    } else if (event.key.code == sf::Keyboard::Right) {
                        float* vol = (soundSelectIndex == 0)
                            ? &sfxTarget : &bgmTarget;
                        *vol = std::min(100.f, *vol + 10.f);
                        sound.playSFX("assets/sound/SFX/select.wav");
                    }
                }
            } else if (state == 3) {
                // ---- 难度选择 ----
                int result = difficultySelect.handleEvent(event);
                if (result == 0) {
                    state = 0;
                    selected = START;
                    hasAnySave = anySaveExists();
                } else if (result == 1) {
                    currentDifficultyParams = difficultySelect.getCurrentParams();
                    currentGameData.difficulty = static_cast<int>(difficultySelect.getSelectedDifficulty());
                    currentGameData.stage = 0;
                    currentGameData.progress = 0;
                    currentGameData.lives = currentDifficultyParams.starting_lives;
                    currentGameData.bombs = currentDifficultyParams.starting_bombs;
                    saveToFile(currentGameData, getSavePath(0));
                    currentSaveSlot = 0;
                    state = 1;

                    // 重置游戏状态
                    player.resetPlayerState();
                    enemyBulletManager.clearAll();
                    bossManager.clearAll();
                    gamePhase = PHASE_GAME;
                    bossSpawnTimer = 5.f;
                    bossSpawned = false;
                    gameUI.getStats().life = player.getLife();
                    gameUI.getStats().bombs = player.getBombs();
                    gameUI.getStats().difficulty = currentGameData.difficulty;
                    gameUI.getStats().progress = 0;
                    gameUI.getStats().graze = 0;
                }
            } else if (state == 4) {
                // ---- 暂停菜单 ----
                if (event.type == sf::Event::KeyPressed) {
                    PauseOption prev = pauseSelected;
                    if (event.key.code == sf::Keyboard::Escape) {
                        state = 1;
                    } else if (event.key.code == sf::Keyboard::Up) {
                        int idx = static_cast<int>(pauseSelected) - 1;
                        if (idx < 0) idx = PAUSE_OPTION_COUNT - 1;
                        pauseSelected = static_cast<PauseOption>(idx);
                    } else if (event.key.code == sf::Keyboard::Down) {
                        int idx = static_cast<int>(pauseSelected) + 1;
                        if (idx >= PAUSE_OPTION_COUNT) idx = 0;
                        pauseSelected = static_cast<PauseOption>(idx);
                    } else if (event.key.code == sf::Keyboard::Enter) {
                        sound.playSFX("assets/sound/SFX/select.wav");
                        if (pauseSelected == CONTINUE_GAME) {
                            state = 1;
                        } else if (pauseSelected == EXIT_TO_MENU) {
                            saveSelect.setSaveMode(true);
                            saveSelect.setSaveData(currentGameData);
                            saveSelect.refresh();
                            state = 6;
                        }
                    }
                    if (pauseSelected != prev) {
                        sound.playSFX("assets/sound/SFX/select.wav");
                    }
                }
            } else if (state == 5) {
                // ---- 存档选择 (加载模式) ----
                int result = saveSelect.handleEvent(event);
                if (result == 0) {
                    state = 0;
                    selected = CONTINUE;
                    hasAnySave = anySaveExists();
                } else if (result == 1) {
                    const SaveData& loaded = saveSelect.getLoadedSave();
                    switch (loaded.difficulty) {
                        case 0: currentDifficultyParams = getEasyParams();    break;
                        case 1: currentDifficultyParams = getMediumParams();  break;
                        case 2: currentDifficultyParams = getHardParams();    break;
                        case 3: currentDifficultyParams = getLunaticParams(); break;
                    }
                    currentGameData = loaded;
                    currentSaveSlot = saveSelect.getSelectedSlot();
                    state = 1;

                    // 重置游戏状态
                    player.resetPlayerState();
                    enemyBulletManager.clearAll();
                    bossManager.clearAll();
                    gamePhase = PHASE_GAME;
                    bossSpawnTimer = 5.f;
                    bossSpawned = false;
                    gameUI.getStats().life = player.getLife();
                    gameUI.getStats().bombs = player.getBombs();
                    gameUI.getStats().difficulty = currentGameData.difficulty;
                    gameUI.getStats().progress = currentGameData.progress;
                    gameUI.getStats().graze = 0;
                }
            } else if (state == 6) {
                // ---- 退出存档选择 (保存模式) ----
                int result = saveSelect.handleEvent(event);
                if (result == 0) {
                    state = 0;
                    selected = START;
                    hasAnySave = anySaveExists();
                    saveSelect.setSaveMode(false);
                } else if (result == 1) {
                    state = 0;
                    selected = START;
                    hasAnySave = anySaveExists();
                    saveSelect.setSaveMode(false);
                }
            }
        }

        // ---- 更新 ----
        float dt = clock.restart().asSeconds();

        if (state == 0) {
            float speed = 300.f * dt;
            for (int i = 0; i < OPTION_COUNT; ++i) {
                float targetSize = (i == selected) ? 48.f : 36.f;
                if (menuItems[i].currentSize < targetSize)
                    menuItems[i].currentSize = std::min(menuItems[i].currentSize + speed, targetSize);
                else if (menuItems[i].currentSize > targetSize)
                    menuItems[i].currentSize = std::max(menuItems[i].currentSize - speed, targetSize);

                menuItems[i].text.setCharacterSize(static_cast<unsigned int>(menuItems[i].currentSize));
                float textWidth = menuItems[i].text.getLocalBounds().width;
                float textHeight = menuItems[i].text.getLocalBounds().height;
                menuItems[i].text.setOrigin(textWidth / 2.f, textHeight / 2.f);

                float baseY = MENU_START_Y + i * MENU_SPACING;
                menuItems[i].text.setPosition(MENU_CENTER_X, baseY);

                if (i == selected) {
                    menuItems[i].text.setFillColor(sf::Color::Yellow);
                } else {
                    menuItems[i].text.setFillColor(sf::Color::White);
                }
            }

            // 高亮框跟随（居中跟随）
            float targetY = MENU_START_Y + static_cast<int>(selected) * MENU_SPACING;
            sf::Vector2f hlPos = menuHighlight.getPosition();
            menuHighlight.setPosition(MENU_CENTER_X, hlPos.y + (targetY - hlPos.y) * 0.2f);

            // 箭头跟随并抖动（居中跟随）
            arrowTimer += dt * 8.f;
            float arrowY = MENU_START_Y + static_cast<int>(selected) * MENU_SPACING;
            menuArrow.setPosition(MENU_CENTER_X - 140.f, arrowY);
            menuArrow.setScale(1.f + std::sin(arrowTimer) * 0.15f, 1.f + std::sin(arrowTimer) * 0.15f);
        }

        if (state == 1) {
            player.update(dt, sf::Vector2u(GAME_WIDTH, GAME_HEIGHT));
            gameUI.update(dt);

            // 同步Player的life和bombs到GameUI
            gameUI.getStats().life = player.getLife();
            gameUI.getStats().bombs = player.getBombs();

            // 同步擦弹数
            gameUI.getStats().graze = player.getGrazeCount();

            // 同步Player HP（每条命视为100HP）
            gameUI.setPlayerHp(player.getLife() * 100, 500);

            // 获取玩家位置
            sf::Vector2f playerPos = player.getHitboxCenter();

            // 更新敌弹
            enemyBulletManager.update(dt, sf::Vector2u(GAME_WIDTH, GAME_HEIGHT), playerPos);

            // 更新Boss
            if (bossManager.hasAliveBoss()) {
                bossManager.update(dt, playerPos, &enemyBulletManager);

                // 设置玩家追踪弹的目标为Boss位置
                Boss* boss = bossManager.getBoss(0);
                if (boss) {
                    player.setTrackingTarget(boss->getPosition());
                }

                // 检查玩家子弹是否击中Boss
                auto* bulletMgr = player.getBulletManager();
                if (bulletMgr) {
                    bossManager.checkPlayerBullets(bulletMgr->getBullets(), bulletMgr->GetActiveCount(), 8.f);
                }

                // 同步Boss HP到UI
                Boss* bossForHp = bossManager.getBoss(0);
                if (bossForHp) {
                    gameUI.setBossHp(bossForHp->getHp(), bossForHp->getMaxHp());
                }
            } else {
                // 没有Boss时清零
                gameUI.setBossHp(0, 0);
            }

            // 游戏阶段逻辑
            if (gamePhase == PHASE_GAME) {
                // 倒计时Boss出现
                bossSpawnTimer -= dt;
                if (bossSpawnTimer <= 0.f && !bossSpawned) {
                    // 生成Boss
                    int difficulty = currentGameData.difficulty;

                    // 根据难度设置Boss属性（大幅增加血量）
                    int bossHp = 10000;
                    float bulletSpeedMult = 1.0f;
                    float hpMult = 1.0f;
                    switch (difficulty) {
                        case 0: bossHp = 6000; bulletSpeedMult = 0.6f; hpMult = 0.5f; break;   // EASY
                        case 1: bossHp = 10000; bulletSpeedMult = 0.8f; hpMult = 0.8f; break;  // MEDIUM
                        case 2: bossHp = 15000; bulletSpeedMult = 1.0f; hpMult = 1.0f; break;  // HARD
                        case 3: bossHp = 25000; bulletSpeedMult = 1.3f; hpMult = 1.5f; break;  // LUNATIC
                    }

                    sf::Vector2f startPos(GAME_WIDTH / 2.f, -100.f);
                    sf::Vector2f targetPos(GAME_WIDTH / 2.f, 200.f);
                    Boss* boss1 = bossManager.createBoss(bossHp, startPos, targetPos, 0);
                    if (boss1) boss1->setDifficultyParams(bulletSpeedMult, hpMult);

                    // LUNATIC难度有两个Boss
                    if (difficulty == 3) {
                        Boss* boss2 = bossManager.createBoss(bossHp, sf::Vector2f(200.f, -100.f), sf::Vector2f(200.f, 200.f), 1);
                        if (boss2) boss2->setDifficultyParams(bulletSpeedMult, hpMult);
                    }

                    bossSpawned = true;
                    gamePhase = PHASE_BOSS;
                }
            } else if (gamePhase == PHASE_BOSS) {
                // Boss战阶段

                // 检查Boss是否被击败
                if (!bossManager.hasAliveBoss()) {
                    gamePhase = PHASE_STAGE_CLEAR;
                    gameUI.getStats().progress = 100;
                    stageClearTimer = 5.f;  // 显示5秒通关画面
                    sound.playSFX("assets/sound/SFX/se_cardget.wav");
                }
            } else if (gamePhase == PHASE_STAGE_CLEAR) {
                // 关卡通关
                stageClearTimer -= dt;
                if (stageClearTimer <= 0.f) {
                    // 通关后返回菜单
                    state = 0;
                    selected = START;
                    hasAnySave = anySaveExists();
                }
            }

            // 检测死亡
            if (!player.isAlive()) {
                state = 0;
            }
        }

        if (state == 3) {
            difficultySelect.update(dt);
        }

        if (state == 4) {
            float speed = 200.f * dt;
            for (int i = 0; i < PAUSE_OPTION_COUNT; ++i) {
                float target = (i == pauseSelected) ? pauseSelectedSize : pauseNormalSize;
                if (pauseFontSizes[i] < target)
                    pauseFontSizes[i] = std::min(pauseFontSizes[i] + speed, target);
                else if (pauseFontSizes[i] > target)
                    pauseFontSizes[i] = std::max(pauseFontSizes[i] - speed, target);
            }
            pauseContinueText.setCharacterSize(static_cast<unsigned int>(pauseFontSizes[CONTINUE_GAME]));
            pauseExitText.setCharacterSize(static_cast<unsigned int>(pauseFontSizes[EXIT_TO_MENU]));
        }

        if (state == 5) {
            saveSelect.update(dt);
        }

        if (state == 6) {
            saveSelect.update(dt);
        }

        // ---- BGM播放逻辑 ----
        if (state == 1 || state == 4) {
            // 游戏进行中或暂停：播放bgm1.mp3
            sound.playBGM("assets/sound/BGM/bgm1.mp3");
        } else {
            // 主菜单、难度选择、音量设置、存档选择：播放info.mp3
            sound.playBGM("assets/sound/BGM/info.mp3");
        }

        // ---- 居中文字（仅水平居中，垂直使用固定位置）----
        auto centerTextH = [](sf::Text& text, float x) {
            sf::FloatRect bounds = text.getLocalBounds();
            text.setOrigin(bounds.width / 2.f, 0);  // 只水平居中
            text.setPosition(x, text.getPosition().y);
        };
        centerTextH(pauseContinueText, CENTER_X);
        centerTextH(pauseExitText, CENTER_X);

        // ---- 更新音效设置页面 ----
        if (state == 2) {
            sound.setSFXVolume(sfxTarget);
            sound.setBGMVolume(bgmTarget);

            sfxBarFill.setSize(sf::Vector2f(barWidth * sfxTarget / 100.f, barHeight));
            bgmBarFill.setSize(sf::Vector2f(barWidth * bgmTarget / 100.f, barHeight));

            sfxValue.setString(std::to_string(static_cast<int>(sfxTarget)));
            bgmValue.setString(std::to_string(static_cast<int>(bgmTarget)));
        }

        // ---- 渲染 ----
        window.clear();

        // 所有状态都使用gamebg.png作为背景
        window.draw(gameBgSprite);

        if (state == 1 || state == 4) {
            // 设置游戏视图，居中并缩放游戏区域
            sf::View gameView(sf::FloatRect(0.f, 0.f, GAME_WIDTH, GAME_HEIGHT));
            gameView.setViewport(sf::FloatRect(0.f, 0.f, 1.f, 1.f));
            window.setView(gameView);

            // 绘制玩家（包含玩家子弹）
            player.draw(window);

            // 绘制敌弹
            enemyBulletManager.render(window);

            // 绘制Boss（在最上层，不会被子弹遮住）
            bossManager.render(window);

            // 绘制关卡清除文字
            if (gamePhase == PHASE_STAGE_CLEAR && stageClearTimer > 0.f) {
                stageClearText.setPosition(CENTER_X, GAME_HEIGHT / 2.f - 50.f);
                sf::FloatRect bounds = stageClearText.getLocalBounds();
                stageClearText.setOrigin(bounds.width / 2.f, bounds.height / 2.f);
                window.draw(stageClearText);
            }

            if (state == 4) {
                // 暂停菜单使用默认视图
                window.setView(window.getDefaultView());
                window.draw(pauseOverlay);
                window.draw(pauseContinueText);
                window.draw(pauseExitText);
            } else {
                window.setView(window.getDefaultView());
                // 游戏时绘制UI到右侧区域
                gameUI.draw(window);
            }
        } else if (state == 0) {
            // 菜单使用默认视图
            window.setView(window.getDefaultView());
            for (int i = 0; i < OPTION_COUNT; ++i) {
                window.draw(menuItems[i].text);
            }
            window.draw(menuHighlight);
            window.draw(menuArrow);
        } else if (state == 2) {
            window.draw(soundTitle);
            window.draw(sfxLabel);
            window.draw(bgmLabel);
            window.draw(sfxValue);
            window.draw(bgmValue);
            window.draw(sfxBarBg);
            window.draw(sfxBarFill);
            window.draw(bgmBarBg);
            window.draw(bgmBarFill);
            window.draw(hintText);
        } else if (state == 3) {
            // 难度选择界面：直接在默认视图(1280坐标系)绘制
            difficultySelect.draw(window);
        } else if (state == 5) {
            // 存档选择界面(加载)：直接在默认视图(1280坐标系)绘制
            saveSelect.draw(window);
        } else if (state == 6) {
            // 存档选择界面(保存)：直接在默认视图(1280坐标系)绘制
            saveSelect.draw(window);
        }

        window.display();
    }

    return 0;
}