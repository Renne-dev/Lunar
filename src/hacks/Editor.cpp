#include <Geode/Geode.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/EditLevelLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include <Geode/modify/EditorPauseLayer.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/SliderTouchLogic.hpp>
#include <Geode/modify/GJScaleControl.hpp>
#include <Geode/modify/SongSelectNode.hpp>
#include <Geode/modify/MoreSearchLayer.hpp>
#include "../HackState.hpp"

using namespace geode::prelude;

// ═══════════════════════════════════════════════════════
//  COPY BYPASS  —  : CopyHack.cpp
// ═══════════════════════════════════════════════════════

class $modify(HackLevelInfoLayer, LevelInfoLayer) {
    struct Fields { int password = 0; };

    bool init(GJGameLevel* level, bool challenge) {
        m_fields->password = level->m_password;
        if (HackState::get().copyBypass)
            level->m_password = 1;
        return LevelInfoLayer::init(level, challenge);
    }

    void onBack(cocos2d::CCObject* sender) {
        m_level->m_password = m_fields->password;
        LevelInfoLayer::onBack(sender);
    }

    void confirmClone(cocos2d::CCObject* sender) {
        m_level->m_password = m_fields->password;
        LevelInfoLayer::confirmClone(sender);
    }
};

// ═══════════════════════════════════════════════════════
//  VERIFY BYPASS  —  : VerifyHack.cpp
//  NO (C) MARK    —  : NoCopyMark.cpp
// ═══════════════════════════════════════════════════════

class $modify(HackEditLevelLayer, EditLevelLayer) {
    bool init(GJGameLevel* gl) {
        if (HackState::get().verifyBypass)
            gl->m_isVerified = true;
        return EditLevelLayer::init(gl);
    }

    void onShare(CCObject* sender) {
        if (HackState::get().noCopyMark)
            m_level->m_originalLevel = 0;
        EditLevelLayer::onShare(sender);
    }
};

// ═══════════════════════════════════════════════════════
//  FREE SCROLL          —  : FreeScroll.cpp
//  CUSTOM OBJECT BYPASS —  : CustomObjectBypass.cpp
//  HIDE UI              —  : HideUI.cpp
// ═══════════════════════════════════════════════════════

class $modify(HackEditorUI, EditorUI) {

    bool init(LevelEditorLayer* editorLayer) {
        if (!EditorUI::init(editorLayer)) return false;
        this->schedule(schedule_selector(HackEditorUI::onHideUITick), 0.f);
        return true;
    }

    void onHideUITick(float) {
        this->setVisible(!HackState::get().hideUI);
    }

    void constrainGameLayerPosition(float w, float h) {
        if (!HackState::get().freeScroll)
            EditorUI::constrainGameLayerPosition(w, h);
        // no-op when enabled = unlimited scroll
    }

    void onNewCustomItem(CCObject* sender) {
        if (!HackState::get().customObjectBypass) {
            EditorUI::onNewCustomItem(sender);
            return;
        }
        if (auto gm = GameManager::get()) {
            cocos2d::CCArray* objs;
            if (m_selectedObjects->count() == 0) {
                objs = cocos2d::CCArray::create();
                objs->addObject(m_selectedObject);
            } else {
                objs = m_selectedObjects;
            }
            gm->addNewCustomObject(copyObjects(objs, false, false));
            m_selectedObjectIndex = 0;
            reloadCustomItems();
        }
    }
};

// ═══════════════════════════════════════════════════════
//  RESET PERCENT ON SAVE  —  : ResetPercentOnSave.cpp
// ═══════════════════════════════════════════════════════

class $modify(HackEditorPauseLayer, EditorPauseLayer) {
    void saveLevel() {
        if (HackState::get().resetPercentOnSave)
            if (m_editorLayer->m_level->m_levelType == GJLevelType::Editor)
                m_editorLayer->m_level->m_normalPercent = 0;
        EditorPauseLayer::saveLevel();
    }
};

// ═══════════════════════════════════════════════════════
//  LEVEL EDIT  —  : LevelEdit.cpp
// ═══════════════════════════════════════════════════════

class $modify(HackPauseLayer, PauseLayer) {
    void customSetup() override {
        if (!HackState::get().levelEdit) {
            PauseLayer::customSetup();
            return;
        }
        auto level = PlayLayer::get()->m_level;
        auto orig  = level->m_levelType;
        level->m_levelType = GJLevelType::Editor;
        PauseLayer::customSetup();
        level->m_levelType = orig;
    }

    void onTryEdit(cocos2d::CCObject* sender) {
        if (!HackState::get().levelEdit) return;
        auto level = PlayLayer::get()->m_level;
        auto orig  = level->m_levelType;
        level->m_levelType = GJLevelType::Editor;
        PauseLayer::onEdit(sender);
        level->m_levelType = orig;
    }
};

// ═══════════════════════════════════════════════════════
//  HIDE TRIGGERS  —  : HideTriggers.cpp
//  SMOOTH TRAIL   —  : SmoothTrail.cpp
// ═══════════════════════════════════════════════════════

static std::vector<GameObject*> s_hiddenTriggers;

class $modify(HackLevelEditorLayer, LevelEditorLayer) {

    void onStopPlaytest() {
        LevelEditorLayer::onStopPlaytest();
        for (auto obj : s_hiddenTriggers)
            obj->setVisible(true);
        s_hiddenTriggers.clear();
    }

    void updateVisibility(float dt) override {
        LevelEditorLayer::updateVisibility(dt);
        if (!HackState::get().hideTriggers) return;
        for (auto obj : s_hiddenTriggers)
            obj->setVisible(false);
    }

    void postUpdate(float dt) override {
        if (HackState::get().smoothTrail)
            m_trailTimer = 0.1f;
        LevelEditorLayer::postUpdate(dt);
    }
};

// ═══════════════════════════════════════════════════════
//  SLIDER LIMIT  —  : SliderLimit.cpp
// ═══════════════════════════════════════════════════════

class $modify(HackSliderTouchLogic, SliderTouchLogic) {
    void ccTouchMoved(cocos2d::CCTouch* touch, cocos2d::CCEvent* event) override {
        if (!HackState::get().sliderLimit) {
            SliderTouchLogic::ccTouchMoved(touch, event);
            return;
        }
        auto pos = this->convertTouchToNodeSpace(touch) - this->m_position;
        if (this->m_rotated)
            this->m_thumb->setPosition({0.f, pos.y});
        else
            this->m_thumb->setPosition({pos.x, 0.f});
        if (this->m_activateThumb) this->m_thumb->activate();
        if (auto* s = this->m_slider) s->updateBar();
    }
};

// ═══════════════════════════════════════════════════════
//  DEFAULT SONG BYPASS  —  : DefaultSongBypass.cpp
// ═══════════════════════════════════════════════════════

class $modify(HackSongSelectNode, SongSelectNode) {
    void audioPrevious(cocos2d::CCObject* sender) {
        if (!HackState::get().defaultSongBypass) {
            SongSelectNode::audioPrevious(sender);
            return;
        }
        m_selectedSongID = std::max(0, m_selectedSongID - 1);
        this->updateAudioLabel();
    }

    void audioNext(cocos2d::CCObject* sender) {
        if (!HackState::get().defaultSongBypass) {
            SongSelectNode::audioNext(sender);
            return;
        }
        m_selectedSongID = std::max(0, m_selectedSongID + 1);
        this->updateAudioLabel();
    }
};
