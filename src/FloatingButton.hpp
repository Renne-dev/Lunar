#pragma once
#include <Geode/Geode.hpp>
using namespace geode::prelude;

void toggleMenu();

class LunarFloatBtn : public CCMenu {
    static constexpr float MIN_MOVE_DIST = 5.f;
    static constexpr float MOVE_SPEED    = 10.75f;
    static constexpr float SNAP_MARGIN   = 0.1f;
    static constexpr float PRESS_SCALE   = 0.9f;

    CCSprite* m_sprite       = nullptr;
    CCPoint   m_holdPos      = {};
    float     m_baseScale    = 0.12f;
    float     m_minOpacity   = 0.5f;
    float     m_maxOpacity   = 1.0f;
    float     m_postClick    = 0.f;
    bool      m_shouldMove   = false;
    bool      m_haveMoved    = false;
    bool      m_haveReleased = false;

public:
    static LunarFloatBtn* create() {
        auto* r = new LunarFloatBtn();
        if (r->init()) { r->autorelease(); return r; }
        CC_SAFE_DELETE(r); return nullptr;
    }

    bool init() override {
        if (!CCMenu::init()) return false;

        this->setPosition({0, 0});
        this->setZOrder(256);
        this->scheduleUpdate();

        auto winSize = CCDirector::sharedDirector()->getWinSize();

        // Load button sprite from mod resources
        auto path = Mod::get()->getResourcesDir() / "Lunarbutton.png";
        m_sprite = CCSprite::create(path.string().c_str());
        if (!m_sprite) {
            // hi
            m_sprite = CCSprite::createWithSpriteFrameName("GJ_thumbsUp_001.png");
        }
        m_sprite->setScale(m_baseScale);
        m_sprite->setOpacity(m_minOpacity * 255);
// this doesnt work if i poop
        
        // Restore saved position
        auto* mod = Mod::get();
        float bx = mod->getSavedValue<float>("floatbtn_x", 60.f);
        float by = mod->getSavedValue<float>("floatbtn_y", 60.f);
        m_sprite->setPosition({bx, by});

        this->addChild(m_sprite);

        // Add to overlay so it persists across scenes
        OverlayManager::get()->addChild(this);

        return true;
    }

    void update(float dt) override {
        // Hide on PC if setting enabled
#if !defined(GEODE_IS_ANDROID) && !defined(GEODE_IS_IOS)
        this->setVisible(!HackState::get().hideFloatBtn);
        if (HackState::get().hideFloatBtn) return;
#endif

        auto winSize = CCDirector::sharedDirector()->getWinSize();
        auto curPos  = m_sprite->getPosition();
        float r      = m_sprite->getScaledContentSize().width * 0.5f;

        // Clamp to screen
        CCPoint clamped = {
            std::clamp(curPos.x, r, winSize.width  - r),
            std::clamp(curPos.y, r, winSize.height - r)
        };
        if (clamped.x != curPos.x || clamped.y != curPos.y) {
            m_sprite->setPosition(clamped);
            curPos = clamped;
            savePos(clamped);
        }

        // Post-click fade out timer
        if (m_haveReleased) {
            m_postClick -= dt;
            if (m_postClick <= 0.f) {
                m_postClick    = 0.f;
                m_haveReleased = false;
                fadeOut();
            }
        }

        if (!m_shouldMove) return;

        // Lerp toward hold position
        if (ccpDistance(curPos, m_holdPos) < SNAP_MARGIN * 10.f) dt *= 2.f;
        auto newPos = ccpLerp(curPos, m_holdPos, MOVE_SPEED * dt);
        if (ccpDistance(newPos, m_holdPos) < SNAP_MARGIN) {
            newPos = m_holdPos;
            m_shouldMove = false;
        }
        newPos.x = std::clamp(newPos.x, r, winSize.width  - r);
        newPos.y = std::clamp(newPos.y, r, winSize.height - r);
        m_sprite->setPosition(newPos);
        savePos(newPos);
    }

    // ── Touch ─────────────────────────────────────────────────────────────────
    bool ccTouchBegan(CCTouch* touch, CCEvent*) override {
        auto loc = this->convertToNodeSpace(touch->getLocation());
        if (ccpDistance(loc, m_sprite->getPosition()) > getRadius()) return false;
        m_haveMoved    = false;
        m_haveReleased = false;
        m_holdPos      = loc;
        fadeIn();
        scaleDown();
        return true;
    }

    void ccTouchMoved(CCTouch* touch, CCEvent*) override {
        auto loc = this->convertToNodeSpace(touch->getLocation());
        if (!m_shouldMove && ccpDistance(loc, m_holdPos) < MIN_MOVE_DIST) return;
        m_shouldMove = true;
        m_haveMoved  = true;
        m_holdPos    = loc;
    }

    void ccTouchEnded(CCTouch*, CCEvent*) override {
        scaleUp();
        if (m_haveMoved) { fadeOut(); return; }
        m_haveReleased = true;
        m_postClick    = 1.f;
        toggleMenu();
    }

    void ccTouchCancelled(CCTouch* t, CCEvent* e) override { ccTouchEnded(t, e); }

    void registerWithTouchDispatcher() override {
        CCDirector::sharedDirector()->getTouchDispatcher()
            ->addTargetedDelegate(this, -1000, true);
    }

    // ── Helpers ───────────────────────────────────────────────────────────────
    float getRadius() const {
        return m_sprite->getScaledContentSize().width * 0.5f;
    }

    void savePos(CCPoint p) {
        auto* mod = Mod::get();
        mod->setSavedValue("floatbtn_x", p.x);
        mod->setSavedValue("floatbtn_y", p.y);
    }

    void fadeIn() {
        m_sprite->stopActionByTag(2);
        auto* a = CCFadeTo::create(0.2f, m_maxOpacity * 255);
        a->setTag(2); m_sprite->runAction(a);
    }

    void fadeOut() {
        m_sprite->stopActionByTag(2);
        auto* a = CCSequence::create(
            CCDelayTime::create(1.f),
            CCFadeTo::create(0.2f, m_minOpacity * 255),
            nullptr
        );
        a->setTag(2); m_sprite->runAction(a);
    }

    void scaleDown() {
        m_sprite->stopActionByTag(1);
        auto* a = CCScaleTo::create(0.1f, m_baseScale * PRESS_SCALE);
        a->setTag(1); m_sprite->runAction(a);
    }

    void scaleUp() {
        m_sprite->stopActionByTag(1);
        auto* a = CCEaseBackOut::create(CCScaleTo::create(0.15f, m_baseScale));
        a->setTag(1); m_sprite->runAction(a);
    }
};
