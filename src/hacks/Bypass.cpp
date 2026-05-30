#include <Geode/Geode.hpp>
#include <Geode/modify/CCTextInputNode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GameStatsManager.hpp>
#include <Geode/modify/GameManager.hpp>
#include <Geode/modify/GameLevelManager.hpp>
#include <Geode/modify/CreatorLayer.hpp>
#include <Geode/modify/OptionsLayer.hpp>
#include <Geode/modify/SecretLayer2.hpp>
#include <Geode/modify/LevelPage.hpp>
#include "../HackState.hpp"

using namespace geode::prelude;

class $modify(HackCCTextInputFilter, CCTextInputNode) {
    void updateLabel(gd::string str) {
        if (HackState::get().charFilter)
            this->setAllowedChars(
                "abcdefghijklmnopqrstuvwxyz"
                "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                "0123456789!@#$%^&*()-=_+"
                "`~[]{}/?.>,<\\|;:'\""
                " "
            );
        CCTextInputNode::updateLabel(std::move(str));
    }
};


class $modify(HackCCTextInputLimit, CCTextInputNode) {
    void updateLabel(gd::string str) {
        if (HackState::get().charLimit)
            this->setMaxLabelLength(99999);
        CCTextInputNode::updateLabel(str);
    }
};

class $modify(HackInstantComplete, PlayLayer) {
    void onEnterTransitionDidFinish() {
        PlayLayer::onEnterTransitionDidFinish();
        if (!HackState::get().instantComplete) return;
        if (m_isPlatformer)
            this->playPlatformerEndAnimationToPos({0.f, 105.f}, true);
        else
            this->playEndAnimationToPos({2.f, 2.f});
    }

    void levelComplete() {
        if (HackState::get().instantComplete) {
            if (m_isPlatformer) m_timePlayed  = 10.0;
            else                m_attemptTime = 10.0;
        }
        PlayLayer::levelComplete();
    }
};


class $modify(HackGameStatsManager, GameStatsManager) {
    bool isItemUnlocked(UnlockType type, int key) {
        if (GameStatsManager::isItemUnlocked(type, key)) return true;
        auto& st = HackState::get();

        if (type == UnlockType::GJItem) {
            if (key == 17 && st.practiceMusic)     return true; // practice music sync
            if (key == 16 && st.musicCustomizer)   return true; // music customizer
            if (key >= 6 && key <= 15 && st.unlockPaths) return true; // paths
            if (key >= 18 && key <= 20 && st.unlockIcons) return true; // icon items
            // vault keys 1-4
            if (key >= 1 && key <= 4 && st.unlockVaults) return true;
        }

        return false;
    }

    int getStat(const char* key) {
        int value = GameStatsManager::getStat(key);
        if (!s_bypassGameStat) return value;
        s_bypassGameStat = false;
        return std::max(value, s_bypassGameStatValue);
    }

    static bool s_bypassGameStat;
    static int  s_bypassGameStatValue;
};

bool HackGameStatsManager::s_bypassGameStat      = false;
int  HackGameStatsManager::s_bypassGameStatValue = 0;


static bool s_bypassUGV     = false;
static bool s_bypassUGV2    = false;
static bool s_bypassUGVSkip = false;

// Used during save to force all icon checks to return true
static bool s_savingIcons = false;

class $modify(HackGameManager, GameManager) {
    bool isColorUnlocked(int key, UnlockType type) {
        GameManager::isColorUnlocked(key, type);  // call base to keep internal state consistent
        if (HackState::get().unlockIcons) return true;
        return GameManager::isColorUnlocked(key, type);
    }

    bool isIconUnlocked(int key, IconType type) {
        GameManager::isIconUnlocked(key, type);   // call base to keep internal state consistent
        if (HackState::get().unlockIcons) return true;
        return GameManager::isIconUnlocked(key, type);
    }

    // Force all icon unlock checks to pass during save so chosen icons persist
    void save() {
        if (HackState::get().unlockIcons) {
            s_savingIcons = true;
            GameManager::save();
            s_savingIcons = false;
        } else {
            GameManager::save();
        }
    }

    bool getUGV(const char* key) {
        bool result = GameManager::getUGV(key);
        auto& st = HackState::get();

        if (st.unlockVaults) {
            if (s_bypassUGVSkip) {
                s_bypassUGVSkip = false;
                s_bypassUGV = true;
                return result;
            } else if (s_bypassUGV2) {
                s_bypassUGV2 = false;
                s_bypassUGV = true;
                return true;
            } else if (s_bypassUGV) {
                s_bypassUGV = false;
                return true;
            }
        }


        if (st.unlockShops) {
            if (strcmp(key, "11") == 0 || strcmp(key, "20") == 0 ||
                strcmp(key, "34") == 0 || strcmp(key, "35") == 0)
                return true;
        }

        return result;
    }
};


class $modify(HackGameLevelManager, GameLevelManager) {
    GJGameLevel* getMainLevel(int levelID, bool dontGetLevelString) {
        auto level = GameLevelManager::getMainLevel(levelID, dontGetLevelString);
        if (level && level->m_requiredCoins > 0 && HackState::get().unlockMainLevels)
            level->m_requiredCoins = 0;
        return level;
    }
};


class $modify(HackCreatorLayer, CreatorLayer) {
    bool init() override {
        if (HackState::get().unlockVaults) {
            s_bypassUGV = true;
            HackGameStatsManager::s_bypassGameStat      = true;
            HackGameStatsManager::s_bypassGameStatValue = 51;
        }
        return CreatorLayer::init();
    }

    void onSecretVault(cocos2d::CCObject* sender) {
        if (HackState::get().unlockVaults) {
            HackGameStatsManager::s_bypassGameStat      = true;
            HackGameStatsManager::s_bypassGameStatValue = 51;
        }
        CreatorLayer::onSecretVault(sender);
    }

    void onTreasureRoom(cocos2d::CCObject* sender) {
        if (HackState::get().unlockVaults)
            s_bypassUGV = true;
        CreatorLayer::onTreasureRoom(sender);
    }
};

class $modify(HackOptionsLayer, OptionsLayer) {
    void onSecretVault(cocos2d::CCObject* sender) {
        if (HackState::get().unlockVaults) {
            HackGameStatsManager::s_bypassGameStat      = true;
            HackGameStatsManager::s_bypassGameStatValue = 11;
        }
        OptionsLayer::onSecretVault(sender);
    }
};

class $modify(HackSecretLayer2, SecretLayer2) {
    void onDoor(cocos2d::CCObject* sender) {
        if (HackState::get().unlockVaults)
            s_bypassUGV = true;
        SecretLayer2::onDoor(sender);
    }
};

class $modify(HackLevelPageVault, LevelPage) {
    void addSecretDoor() {
        if (HackState::get().unlockVaults)
            s_bypassUGV = true;
        LevelPage::addSecretDoor();
    }
};
