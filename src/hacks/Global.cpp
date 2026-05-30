#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/LevelInfoLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/EditorUI.hpp>
#include "../HackState.hpp"

using namespace geode::prelude;

// ═══════════════════════════════════════════════════════
//  ACCURATE PERCENTAGE  —       : AccuratePercentage.cpp
// ═══════════════════════════════════════════════════════

class $modify(HackAccuratePctPL, PlayLayer) {
    void updateProgressbar() {
        PlayLayer::updateProgressbar();
        auto& st = HackState::get();
        if (!st.accuratePercentage || !m_percentageLabel) return;

        if (m_level->isPlatformer()) return; // time display left to GD

        float percent = getCurrentPercent();
        int digits = st.accuratePercentageDigits;
        if (digits > 0)
            m_percentageLabel->setString(fmt::format("{:.{}f}%", percent, digits).c_str());
        else
            m_percentageLabel->setString(fmt::format("{}%", (int)std::floor(percent)).c_str());
    }
};

// ═══════════════════════════════════════════════════════
//  AUTO SONG DOWNLOAD  —       : AutoSongDownload.cpp
// ═══════════════════════════════════════════════════════

class $modify(HackAutoSongDL, LevelInfoLayer) {
    void tryAutoDownload() {
        if (!HackState::get().autoSongDownload) return;
        if (!GameManager::get()->getGameVariable(GameVar::ShownNewgroundsDisclaimer)) return;
        if (m_songWidget && m_songWidget->m_downloadBtn &&
            m_songWidget->m_downloadBtn->isVisible())
            m_songWidget->m_downloadBtn->activate();
    }

    void levelDownloadFinished(GJGameLevel* level) override {
        LevelInfoLayer::levelDownloadFinished(level);
        tryAutoDownload();
    }

    void onEnterTransitionDidFinish() override {
        LevelInfoLayer::onEnterTransitionDidFinish();
        if (m_level->m_dailyID > 0) tryAutoDownload();
    }
};

// ═══════════════════════════════════════════════════════
//  FORCE PLATFORMER  —       : ForcePlatformer.cpp
// ═══════════════════════════════════════════════════════

class $modify(HackForcePlatformerBGL, GJBaseGameLayer) {
    void loadLevelSettings() {
        auto& st = HackState::get();
        if (!st.forcePlatformer || !PlayLayer::get() || m_levelSettings->m_platformerMode)
            return GJBaseGameLayer::loadLevelSettings();

        m_levelSettings->m_platformerMode = true;
        int origLen = m_levelSettings->m_level->m_levelLength;
        GJBaseGameLayer::loadLevelSettings();
        m_levelSettings->m_level->m_levelLength = origLen;
        m_levelSettings->m_platformerMode = false;
    }
};

class $modify(HackForcePlatformerPL, PlayLayer) {
    void resetLevel() {
        auto& st = HackState::get();
        if (st.forcePlatformer != m_isPlatformer)
            this->loadLevelSettings();
        PlayLayer::resetLevel();
    }
};

// ═══════════════════════════════════════════════════════
//  HIDE PAUSE MENU  —       : HidePauseMenu.cpp
// ═══════════════════════════════════════════════════════

class $modify(HackHidePause, PauseLayer) {
    void customSetup() override {
        PauseLayer::customSetup();
        this->schedule(schedule_selector(HackHidePause::updateVisibility));
        this->setVisible(!HackState::get().hidePauseMenu);
    }

    void updateVisibility(float dt) {
        bool hide = HackState::get().hidePauseMenu;
        if (hide == this->isVisible())
            this->setVisible(!hide);
    }
};

// ═══════════════════════════════════════════════════════
//  ZERO PERCENT PRACTICE  —       : ZeroPercentPractice.cpp
// ═══════════════════════════════════════════════════════

class $modify(HackZeroPctPL, PlayLayer) {
    struct Fields { bool fromZero = true; };

    void resetLevel() {
        m_fields.self()->fromZero = (m_gameState.m_currentProgress == 0);
        PlayLayer::resetLevel();
    }

    void levelComplete() {
        if (HackState::get().zeroPercentPractice &&
            m_fields.self()->fromZero && m_isPracticeMode)
            m_isPracticeMode = false;
        PlayLayer::levelComplete();
    }
};

// ═══════════════════════════════════════════════════════
//  SHOW HITBOXES  —       : ShowHitboxes.cpp
//  Simplified: draws solid, hazard, player hitboxes using
//  CCDrawNode on the object layer. RingBuffer trail omitted
//  (      -internal). On-death freeze is supported.
// ═══════════════════════════════════════════════════════

static cocos2d::CCDrawNode* s_hitboxNode  = nullptr;
static bool                 s_hbDead      = false;
static GameObject*          s_hbCollObj   = nullptr;

static void drawHitboxRect(cocos2d::CCDrawNode* node, cocos2d::CCRect r,
                            cocos2d::ccColor4F fill, float bw, cocos2d::ccColor4F border) {
    std::array<cocos2d::CCPoint, 4> v = {{
        {r.getMinX(), r.getMinY()}, {r.getMinX(), r.getMaxY()},
        {r.getMaxX(), r.getMaxY()}, {r.getMaxX(), r.getMinY()}
    }};
    node->drawPolygon(v.data(), 4, fill, bw, border);
}

static void visitAllHitboxes(GJBaseGameLayer* bgl) {
    if (!s_hitboxNode) return;
    s_hitboxNode->clear();

    auto& st = HackState::get();
    bool show = st.showHitboxes || (s_hbDead && st.showHitboxesOnDeath);
    if (!show) return;

    cocos2d::ccColor4F solid   = {0.0f, 0.25f, 1.0f, 0.3f};
    cocos2d::ccColor4F danger  = {1.0f, 0.0f,  0.0f, 0.3f};
    cocos2d::ccColor4F other   = {0.0f, 1.0f,  0.0f, 0.3f};
    cocos2d::ccColor4F player  = {1.0f, 1.0f,  0.0f, 0.3f};
    cocos2d::ccColor4F playerI = {0.0f, 1.0f,  0.2f, 0.3f};
    float bw = 0.25f;

    // objects
    int count = bgl->m_sections.empty() ? -1 : (int)bgl->m_sections.size();
    for (int i = bgl->m_leftSectionIndex; i <= bgl->m_rightSectionIndex && i < count; ++i) {
        auto* col = bgl->m_sections[i];
        if (!col) continue;
        int rows = (int)col->size();
        for (int j = bgl->m_bottomSectionIndex; j <= bgl->m_topSectionIndex && j < rows; ++j) {
            auto* sec = col->at(j);
            if (!sec) continue;
            int sz = (int)bgl->m_sectionSizes[i]->at(j);
            for (int k = 0; k < sz; ++k) {
                auto* obj = sec->at(k);
                if (!obj || obj->m_objectType == GameObjectType::Decoration ||
                    !obj->m_isActivated || obj->m_isGroupDisabled) continue;

                switch (obj->m_objectType) {
                    case GameObjectType::Solid:
                        drawHitboxRect(s_hitboxNode, obj->getObjectRect(),
                                       solid, bw, {0,0.25f,1,1}); break;
                    case GameObjectType::Hazard:
                    case GameObjectType::AnimatedHazard:
                        if (obj != bgl->m_anticheatSpike)
                            drawHitboxRect(s_hitboxNode, obj->getObjectRect(),
                                           danger, bw, {1,0,0,1}); break;
                    default:
                        drawHitboxRect(s_hitboxNode, obj->getObjectRect(),
                                       other, bw, {0,1,0,1}); break;
                }
            }
        }
    }

    // players
    auto drawPlayer = [&](PlayerObject* p) {
        if (!p) return;
        drawHitboxRect(s_hitboxNode, p->getObjectRect(),      player,  bw, {1,1,0,1});
        drawHitboxRect(s_hitboxNode, p->getObjectRect(0.3f,0.3f), playerI, bw, {0,1,0.2f,1});
    };
    drawPlayer(bgl->m_player1);
    if (bgl->m_gameState.m_isDualMode) drawPlayer(bgl->m_player2);
}

class $modify(HackHitboxesBGL, GJBaseGameLayer) {
    // Trail update hook (Mac path)
    void processQueuedButtons(float dt, bool clear) {
        GJBaseGameLayer::processQueuedButtons(dt, clear);
        // nothing extra needed — visitHitboxes happens in updateVisibility
    }
};

class $modify(HackHitboxesPL, PlayLayer) {
    void createObjectsFromSetupFinished() {
        PlayLayer::createObjectsFromSetupFinished();
        s_hitboxNode = cocos2d::CCDrawNode::create();
        s_hitboxNode->setBlendFunc({GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
        s_hitboxNode->setID("hitboxes"_spr);
        s_hitboxNode->m_bUseArea = false;
        if (m_debugDrawNode && m_debugDrawNode->getParent())
            m_debugDrawNode->getParent()->addChild(s_hitboxNode, 1402);
        s_hbDead = false;
        s_hbCollObj = nullptr;
    }

    void updateVisibility(float dt) override {
        PlayLayer::updateVisibility(dt);
        s_hbDead = m_player1 && m_player1->m_isDead;
        visitAllHitboxes(this);
    }

    void resetLevel() override {
        PlayLayer::resetLevel();
        s_hbDead = false;
        s_hbCollObj = nullptr;
    }

    void destroyPlayer(PlayerObject* player, GameObject* obj) override {
        PlayLayer::destroyPlayer(player, obj);
        if (obj && obj != m_anticheatSpike) s_hbCollObj = obj;
    }
};

class $modify(HackHitboxesPO, PlayerObject) {
    void playerDestroyed(bool p0) {
        if (auto* pl = PlayLayer::get())
            s_hbDead = (this == pl->m_player1 || this == pl->m_player2);
        PlayerObject::playerDestroyed(p0);
    }
};

class $modify(HackHitboxesLEL, LevelEditorLayer) {
    bool init(GJGameLevel* level, bool unk) {
        if (!LevelEditorLayer::init(level, unk)) return false;
        s_hitboxNode = cocos2d::CCDrawNode::create();
        s_hitboxNode->setBlendFunc({GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
        s_hitboxNode->setID("hitboxes-editor"_spr);
        s_hitboxNode->m_bUseArea = false;
        if (m_debugDrawNode && m_debugDrawNode->getParent())
            m_debugDrawNode->getParent()->addChild(s_hitboxNode, 1402);
        return true;
    }

    void updateVisibility(float dt) override {
        LevelEditorLayer::updateVisibility(dt);
        visitAllHitboxes(this);
    }
};

class $modify(HackHitboxesEditorUI, EditorUI) {
    void onPlaytest(CCObject* sender) {
        EditorUI::onPlaytest(sender);
        s_hbDead = false;
        s_hbCollObj = nullptr;
    }
};

// ═══════════════════════════════════════════════════════
//  START POS SWITCHER  —       : StartPosSwitcher.cpp
//  Simplified: keyboard Q/E switching, no on-screen label
//  (label system requires kasdkjhjkdhal  's label framework)
// ═══════════════════════════════════════════════════════

static std::vector<StartPosObject*> s_startPositions;
static int32_t                      s_startPosIndex = -1;
static bool                         s_levelLoaded   = false;

static void applyStartPos(PlayLayer* pl, int32_t index) {
    if (!pl || s_startPositions.empty() || !s_levelLoaded) return;

    int32_t count = (int32_t)s_startPositions.size();
    if      (index >= count) index = -1;
    else if (index < -1)    index = count - 1;

    s_startPosIndex = index;
    pl->m_currentCheckpoint = nullptr;

    StartPosObject* sp = (index >= 0) ? s_startPositions[index] : nullptr;
    pl->setStartPosObject(sp);
    pl->m_isTestMode = (index >= 0);

    if (pl->m_isPracticeMode) pl->resetLevelFromStart();
    else                      pl->resetLevel();
    pl->startMusic();
    pl->updateTestModeLabel();
}

class $modify(HackStartPosPL, PlayLayer) {
    bool init(GJGameLevel* level, bool p1, bool p2) {
        s_startPositions.clear();
        s_startPosIndex = -1;
        s_levelLoaded = false;
        return PlayLayer::init(level, p1, p2);
    }

    void addObject(GameObject* obj) {
        PlayLayer::addObject(obj);
        if (obj->m_objectID == 31)
            s_startPositions.push_back(static_cast<StartPosObject*>(obj));
    }

    void createObjectsFromSetupFinished() {
        PlayLayer::createObjectsFromSetupFinished();
        if (!s_startPositions.empty()) {
            std::ranges::sort(s_startPositions, [](GameObject* a, GameObject* b){
                return a->getPositionX() < b->getPositionX();
            });
            s_startPosIndex = -1;
            if (m_startPosObject) {
                auto it = std::ranges::find(s_startPositions, m_startPosObject);
                if (it != s_startPositions.end())
                    s_startPosIndex = (int32_t)std::distance(s_startPositions.begin(), it);
            }
        }
        s_levelLoaded = true;
    }
};

// ── StartPos Switcher keyboard (Q / E) ───────────────────────────────────────
// Lives here so it can access s_startPositions, s_startPosIndex, applyStartPos

#include <Geode/modify/CCKeyboardDispatcher.hpp>

class $modify(HackStartPosKeys, cocos2d::CCKeyboardDispatcher) {
    bool dispatchKeyboardMSG(cocos2d::enumKeyCodes key, bool isDown, bool isRepeat, double extra) {
        bool ret = CCKeyboardDispatcher::dispatchKeyboardMSG(key, isDown, isRepeat, extra);
        if (isDown && !isRepeat && HackState::get().startPosSwitcher) {
            if (auto* pl = PlayLayer::get()) {
                if (key == cocos2d::enumKeyCodes::KEY_Q)
                    applyStartPos(pl, s_startPosIndex - 1);
                else if (key == cocos2d::enumKeyCodes::KEY_E)
                    applyStartPos(pl, s_startPosIndex + 1);
            }
        }
        return ret;
    }
};
