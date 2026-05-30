#include <Geode/Geode.hpp>
#include <Geode/modify/CCScheduler.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>
#include <Geode/modify/CCLayerColor.hpp>
#include "../HackState.hpp"

using namespace geode::prelude;

// ═══════════════════════════════════════════════════════
//  SPEED HACK  —     : Speedhack.cpp
//  Hook: CCScheduler::update — scales dt directly.
//  This is the correct approach; CCDirector::setTimeScale
//  only affects rendering, not physics.
// ═══════════════════════════════════════════════════════

class $modify(HackScheduler, cocos2d::CCScheduler) {
    void update(float dt) override {
        auto& st = HackState::get();
        if (st.speedHackEnabled) {
            float speed = st.speedHackValue;
            if (speed <= 0.f) speed = 1.f;
            dt *= speed;
        }
        CCScheduler::update(dt);
    }
};

// ═══════════════════════════════════════════════════════
//  AUDIO SPEED  —     : AudioSpeed.cpp
//  Hook: FMODAudioEngine::update
//  Sets pitch on the master channel group each frame.
//  Can sync with speedhack value or use its own value.
// ═══════════════════════════════════════════════════════

class $modify(HackFMODAudio, FMODAudioEngine) {
    void update(float dt) override {
        FMODAudioEngine::update(dt);

        auto& st = HackState::get();
        float speed = 1.f;

        if (st.audioSpeedEnabled)
            speed = st.audioSpeedValue;
        else if (st.audioSpeedSync && st.speedHackEnabled)
            speed = st.speedHackValue > 0.f ? st.speedHackValue : 1.f;
        else
            return;

        FMOD::ChannelGroup* masterGroup = nullptr;
        if (m_system->getMasterChannelGroup(&masterGroup) == FMOD_OK)
            masterGroup->setPitch(speed);
    }
};

// ═══════════════════════════════════════════════════════
//  PITCH SHIFT  —     : PitchShift.cpp
//  Applies a FMOD DSP pitch shifter to the background music channel.
//  Re-applies whenever the value changes via the slider.
// ═══════════════════════════════════════════════════════

static FMOD::DSP* s_pitchDSP = nullptr;

static void applyPitch(float pitch) {
    auto* fmod = FMODAudioEngine::get();
    if (!fmod || !fmod->m_backgroundMusicChannel) return;

    // Remove existing DSP
    if (s_pitchDSP) {
        fmod->m_backgroundMusicChannel->removeDSP(s_pitchDSP);
        s_pitchDSP->release();
        s_pitchDSP = nullptr;
    }

    if (pitch == 1.f) return;

    fmod->m_system->createDSPByType(FMOD_DSP_TYPE_PITCHSHIFT, &s_pitchDSP);
    s_pitchDSP->setParameterFloat(FMOD_DSP_PITCHSHIFT_FFTSIZE, 2048.f);
    s_pitchDSP->setParameterFloat(FMOD_DSP_PITCHSHIFT_PITCH, pitch);
    fmod->m_backgroundMusicChannel->addDSP(0, s_pitchDSP);
}

class $modify(HackFMODPitch, FMODAudioEngine) {
    void update(float dt) override {
        FMODAudioEngine::update(dt);

        auto& st = HackState::get();
        if (!st.pitchShiftEnabled) {
            if (s_pitchDSP) applyPitch(1.f); // remove DSP when disabled
            return;
        }
        applyPitch(st.pitchShiftValue);
    }
};

// ═══════════════════════════════════════════════════════
//  TRANSPARENT LISTS  —     : TransparentLists.cpp
//  Hook: CCLayerColor::initWithColor
//  Hides the two brown colors GD uses for list cell backgrounds.
// ═══════════════════════════════════════════════════════

class $modify(HackCCLayerColor, cocos2d::CCLayerColor) {
    bool initWithColor(const cocos2d::ccColor4B& color, float w, float h) {
        bool ret = CCLayerColor::initWithColor(color, w, h);
        if (HackState::get().transparentLists) {
            if (color == cocos2d::ccColor4B{161, 88, 44, 255} ||
                color == cocos2d::ccColor4B{194, 114, 62, 255})
                this->setVisible(false);
        }
        return ret;
    }

    bool initWithColor(const cocos2d::ccColor4B& color) {
        bool ret = CCLayerColor::initWithColor(color);
        if (HackState::get().transparentLists) {
            if (color == cocos2d::ccColor4B{161, 88, 44, 255} ||
                color == cocos2d::ccColor4B{194, 114, 62, 255}) {
                this->setOpacity(0);
                this->setVisible(false);
            }
        }
        return ret;
    }
};
