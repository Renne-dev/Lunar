#include <Geode/Geode.hpp>
#include <Geode/modify/CCKeyboardDispatcher.hpp>
#include <Geode/modify/MenuLayer.hpp>
#include <chrono>
#include "HackMenu.hpp"
#include "HackState.hpp"
#include "FloatingButton.hpp"

using namespace geode::prelude;

void toggleMenu();

// h
static int s_backslashCount = 0;

static void showEasterEgg() {
    auto* scene = CCDirector::sharedDirector()->getRunningScene();
    if (!scene) return;

    auto winSize = CCDirector::sharedDirector()->getWinSize();

    auto* lbl = CCNode::create();
    lbl->setID("lunar-easter-egg");
    lbl->setZOrder(9999);
    lbl->setPosition(winSize / 2);

    const char* lines[] = {
        "hi",
        "im renne",
        "what are yall doing today",
        "idk why i added this ;)",
        "this mod menu took 6 months",
        "love you ",
        "- Renne"
    };
    const int NUM_LINES = 7;
    float lineH  = 36.f;
    float totalH = lineH * NUM_LINES;

    for (int i = 0; i < NUM_LINES; i++) {
        auto* line = CCLabelBMFont::create(lines[i], "bigFont.fnt");
        line->setColor({180, 100, 255});
        line->setScale(0.7f);
        line->setOpacity(0);
        line->setPosition({0, totalH * 0.5f - i * lineH - lineH * 0.5f});
        lbl->addChild(line);
    }
    scene->addChild(lbl);

    // Fade all lines in/out, remove parent on last line's completion
    for (int i = 0; i < NUM_LINES; i++) {
        auto* child = static_cast<CCNode*>(lbl->getChildren()->objectAtIndex(i));
        child->runAction(CCSequence::create(
            CCFadeIn::create(0.3f),
            CCDelayTime::create(3.5f),
            CCFadeOut::create(0.5f),
            i == NUM_LINES - 1
                ? (CCFiniteTimeAction*)CCCallFunc::create(lbl, callfunc_selector(CCNode::removeFromParent))
                : (CCFiniteTimeAction*)CCDelayTime::create(0.f),
            nullptr
        ));
    }
}

// ── Keyboard shortcut (TAB) — all platforms ───────────
class $modify(HackKeyboard, CCKeyboardDispatcher) {
    bool dispatchKeyboardMSG(enumKeyCodes key, bool isDown, bool isRepeat, double extra) {
        if (isDown && !isRepeat && key == enumKeyCodes::KEY_Tab) {
            toggleMenu();
            return true;
        }

        // Easter egg: press H 5 times within 1.5 seconds
        if (isDown && !isRepeat && key == enumKeyCodes::KEY_H) {
            using namespace std::chrono;
            static auto s_lastTime = steady_clock::now();
            auto now = steady_clock::now();
            float elapsed = duration<float>(now - s_lastTime).count();
            if (elapsed > 1.5f) s_backslashCount = 0;
            s_lastTime = now;
            s_backslashCount++;
            if (s_backslashCount >= 5) {
                s_backslashCount = 0;
                showEasterEgg();
            }
        }

        return CCKeyboardDispatcher::dispatchKeyboardMSG(key, isDown, isRepeat, extra);
    }
};

// ── More Games button → open mod menu (Android, iOS & macOS) ─
#if defined(GEODE_IS_ANDROID) || defined(GEODE_IS_IOS) || defined(GEODE_IS_MACOS)
class $modify(HackMenuLayer, MenuLayer) {
    void moreGamesCallback(CCObject* sender) {
        toggleMenu();
    }
};
#endif

// ── Mod entry ─────────────────────────────────────────
$on_mod(Loaded) {
    HackState::get().load();
    setupImGui();
    LunarFloatBtn::create();
#if defined(GEODE_IS_ANDROID) || defined(GEODE_IS_IOS)
    log::info("HackMod loaded! Tap the floating button or More Games to open.");
#else
    log::info("HackMod loaded! Press TAB or click the floating button to open.");
#endif
}
