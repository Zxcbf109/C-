#include "SoundManager.h"

SoundManager& SoundManager::getInstance() {
    static SoundManager instance;
    return instance;
}

void SoundManager::setSFXVolume(float vol) {
    if (vol < 0.f) vol = 0.f;
    if (vol > 100.f) vol = 100.f;
    m_sfxVolume = vol;
}

void SoundManager::setBGMVolume(float vol) {
    if (vol < 0.f) vol = 0.f;
    if (vol > 50.f) vol = 50.f;
    m_bgmVolume = vol;
    m_bgm.setVolume(m_bgmVolume);
}

float SoundManager::getSFXVolume() const { return m_sfxVolume; }
float SoundManager::getBGMVolume() const { return m_bgmVolume; }

void SoundManager::playSFX(const std::string& path) {
    // Always apply current SFX volume
    if (m_sfxVolume <= 0.f) return;

    // Cache buffer
    auto it = m_buffers.find(path);
    if (it == m_buffers.end()) {
        sf::SoundBuffer buf;
        if (!buf.loadFromFile(path)) return;
        it = m_buffers.emplace(path, std::move(buf)).first;
    }

    m_sfxSound.setBuffer(it->second);
    m_sfxSound.setVolume(m_sfxVolume);
    m_sfxSound.play();
}

void SoundManager::playBGM(const std::string& path) {
    if (path == m_currentBGM) return;
    m_bgm.stop();
    if (!m_bgm.openFromFile(path)) return;
    m_currentBGM = path;
    m_bgm.setVolume(m_bgmVolume);
    m_bgm.setLoop(true);
    m_bgm.play();
}

void SoundManager::stopBGM() {
    m_bgm.stop();
    m_currentBGM.clear();
}
