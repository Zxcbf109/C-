#pragma once
#include <SFML/Audio.hpp>
#include <string>
#include <map>

class SoundManager {
public:
    static SoundManager& getInstance();

    void setSFXVolume(float vol);   // 0-100
    void setBGMVolume(float vol);   // 0-100
    float getSFXVolume() const;
    float getBGMVolume() const;

    // Play SFX with current SFX volume applied. Loads + caches buffer.
    void playSFX(const std::string& path);

    // Play BGM with current BGM volume applied. Only one BGM at a time.
    void playBGM(const std::string& path);
    void stopBGM();

private:
    SoundManager() = default;
    float m_sfxVolume = 100.f;
    float m_bgmVolume = 100.f;

    std::map<std::string, sf::SoundBuffer> m_buffers;
    sf::Sound m_sfxSound;
    sf::Music m_bgm;
    std::string m_currentBGM;
};
