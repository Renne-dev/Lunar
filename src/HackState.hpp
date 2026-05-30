#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

class HackState {
public:
    static HackState& get() {
        static HackState instance;
        return instance;
    }

    // ── Player ────────────────────────────────────────────
    bool  noclip               = false;
    bool  autoClicker          = false;   // Autoclicker.cpp
    float autoClickerCPS       = 15.0f;
    bool  noDeathEffect        = false;   // NoDeathEffect.cpp
    bool  noWave               = false;   // NoTrail.cpp
    bool  noRobotFire          = false;   // NoRobotFire.cpp
    bool  noSpiderDash         = false;   // NoSpiderDash.cpp
    bool  jumpHack             = false;   // JumpHack.cpp
    bool  shipcopter           = false;   // Shipcopter.cpp
    bool  noVehicleParticles   = false;   // NoVehicleParticles.cpp
    bool  noRespawnFlash       = false;   // NoRespawnFlash.cpp
    bool  noPlatformerSquish   = false;   // NoPlatformerJumpSquish.cpp
    bool  muteLevelAudioOnDeath = false;  // MuteLevelAudioOnDeath.cpp
    bool  muteRewardsSFX       = false;   // MuteRewardsSFX.cpp
    bool  waveTrailOnDeath     = false;   // WaveTrailOnDeath.cpp
    bool  solidWaveTrail       = false;   // SolidWaveTrail.cpp
    bool  hidePlayer           = false;   // HideOrShowPlayer.cpp
    bool  noHideTrigger        = false;   // HideOrShowPlayer.cpp
    bool  forceGhostTrail      = false;   // GhostTrailSettings.cpp
    bool  noGhostTrail         = false;   // GhostTrailSettings.cpp
    bool  pathTracer           = false;   // path tracer (draw-node based)

    // Custom Wave Trail — CustomWaveTrail.cpp
    bool  customWaveTrail       = false;
    bool  waveTrailPulse        = true;
    float waveTrailScale        = 1.f;
    bool  waveTrailRainbow      = false;
    float waveTrailRainbowSpeed = 0.5f;
    bool  waveTrailCustomColor  = false;
    float waveTrailColor[3]     = {1.f, 1.f, 1.f};
    bool  waveTrailOutline      = false;
    float waveTrailOutlineStroke= 2.f;
    float waveTrailOutlineColor[3] = {0.f, 0.f, 0.f};

    // Show Trajectory — ShowTrajectory.cpp
    bool  showTrajectory        = false;
    int   trajectoryIterations  = 300;
    bool  autoKill             = false;   // AutoKill.cpp
    bool  autoKillByPercent    = true;
    float autoKillPercent      = 50.0f;
    bool  autoKillByTime       = false;
    float autoKillTime         = 90.0f;
    bool  respawnDelay         = false;   // RespawnDelay.cpp
    float respawnDelayTime     = 1.0f;

    // ── Global ────────────────────────────────────────────
    bool  speedHackEnabled     = false;   // Speedhack.cpp
    float speedHackValue       = 1.0f;
    bool  audioSpeedEnabled    = false;   // AudioSpeed.cpp
    float audioSpeedValue      = 1.0f;
    bool  audioSpeedSync       = false;
    bool  pitchShiftEnabled    = false;   // PitchShift.cpp — renamed to Mute Sound
    float pitchShiftValue      = 1.0f;
    bool  transparentLists     = false;   // TransparentLists.cpp

    // ── Cosmetic ──────────────────────────────────────────
    bool  noMirror             = false;   // NoMirror.cpp
    bool  instantMirror        = false;   // InstantMirror.cpp
    bool  noLevelParticles     = false;   // NoParticles.cpp
    bool  noMiscParticles      = true;
    bool  noCustomParticles    = false;
    bool  noPortalLightning    = false;   // NoPortalLightning.cpp
    bool  noShader             = false;   // NoShader.cpp
    bool  layoutMode           = false;   // LayoutMode.cpp

    // ── Global ─────────────────────────────────────────────
    bool  accuratePercentage     = false; // AccuratePercentage.cpp
    int   accuratePercentageDigits = 4;
    bool  autoSongDownload       = false; // AutoSongDownload.cpp
    bool  forcePlatformer        = false; // ForcePlatformer.cpp
    bool  hidePauseMenu          = false; // HidePauseMenu.cpp
    bool  uncompleteLevel        = false; // uncomplete current level (button action)
    bool  zeroPercentPractice    = false; // ZeroPercentPractice.cpp
    bool  showHitboxes           = false; // ShowHitboxes.cpp
    bool  showHitboxesOnDeath    = false;
    bool  startPosSwitcher       = false; // StartPosSwitcher.cpp

    // ── Labels ─────────────────────────────────────────────
    // 0=NoclipDeaths  1=NoclipAccuracy  2=FPS  3=CPS  4=IsPractice  5=Clock
    static constexpr int kLabelCount = 6;

    struct LabelSetting {
        bool  enabled   = false;
        float r         = 1.f;
        float g         = 1.f;
        float b         = 1.f;
        float opacity   = 1.f;
        int   alignment = 0;   // 0-8 matching LabelsContainer::Alignment
    };

    bool         labelsVisible              = true;
    bool         labelEnabled[kLabelCount]  = {};
    LabelSetting labelSettings[kLabelCount] = {};

    // ── Bypass ────────────────────────────────────────────
    bool  charFilter           = false;   // CharacterFilter.cpp
    bool  charLimit            = false;   // CharacterLimit.cpp
    bool  instantComplete      = false;   // InstantComplete.cpp
    bool  practiceMusic        = false;   // MusicUnlocker.cpp
    bool  musicCustomizer      = false;   // MusicUnlocker.cpp
    bool  unlockPaths          = false;   // PathsUnlocker.cpp
    bool  unlockIcons          = false;   // UnlockIcons.cpp
    bool  unlockMainLevels     = false;   // UnlockMainLevels.cpp
    bool  unlockShops          = false;   // UnlockShops.cpp
    bool  unlockVaults         = false;   // UnlockVaults.cpp

    // ── Editor ────────────────────────────────────────────
    bool  copyBypass           = false;
    bool  verifyBypass         = false;
    bool  noCopyMark           = false;
    bool  freeScroll           = false;
    bool  customObjectBypass   = false;
    bool  hideUI               = false;
    bool  resetPercentOnSave   = false;
    bool  levelEdit            = false;
    bool  hideTriggers         = false;
    bool  smoothTrail          = false;
    bool  sliderLimit          = false;
    bool  defaultSongBypass    = false;

    // ── UI style ──────────────────────────────────────────
    bool  androidUI             = false;
    bool  hideFloatBtn          = false;

    // ── Runtime only (not saved) ──────────────────────────
    int   noclipDeaths         = 0;
    float noclipAccuracy       = 100.0f;
    int   currentCPS           = 0;
    // Trajectory runtime state
    bool  pathTracerNeedsInit  = false;
    bool  pathTracerSimDead    = false;
    bool  pathTracerP1        = false;
    bool  pathTracerP2        = false;
    PlayerObject* pathTracerPlayer1 = nullptr;
    PlayerObject* pathTracerPlayer2 = nullptr;

    void save() {
        auto* m = Mod::get();
        m->setSavedValue("androidUI",            androidUI);
        m->setSavedValue("hideFloatBtn",         hideFloatBtn);
        // Player
        m->setSavedValue("noclip",               noclip);
        m->setSavedValue("autoClicker",          autoClicker);
        m->setSavedValue("autoClickerCPS",       autoClickerCPS);
        m->setSavedValue("noDeathEffect",        noDeathEffect);
        m->setSavedValue("noWave",               noWave);
        m->setSavedValue("noRobotFire",          noRobotFire);
        m->setSavedValue("noSpiderDash",         noSpiderDash);
        m->setSavedValue("jumpHack",             jumpHack);
        m->setSavedValue("shipcopter",           shipcopter);
        m->setSavedValue("noVehicleParticles",   noVehicleParticles);
        m->setSavedValue("noRespawnFlash",       noRespawnFlash);
        m->setSavedValue("noPlatformerSquish",   noPlatformerSquish);
        m->setSavedValue("muteLevelAudioOnDeath",muteLevelAudioOnDeath);
        m->setSavedValue("muteRewardsSFX",       muteRewardsSFX);
        m->setSavedValue("waveTrailOnDeath",     waveTrailOnDeath);
        m->setSavedValue("solidWaveTrail",       solidWaveTrail);
        m->setSavedValue("hidePlayer",           hidePlayer);
        m->setSavedValue("noHideTrigger",        noHideTrigger);
        m->setSavedValue("forceGhostTrail",      forceGhostTrail);
        m->setSavedValue("noGhostTrail",         noGhostTrail);
        m->setSavedValue("pathTracer",             pathTracer);
        m->setSavedValue("customWaveTrail",        customWaveTrail);
        m->setSavedValue("waveTrailPulse",         waveTrailPulse);
        m->setSavedValue("waveTrailScale",         waveTrailScale);
        m->setSavedValue("waveTrailRainbow",       waveTrailRainbow);
        m->setSavedValue("waveTrailRainbowSpeed",  waveTrailRainbowSpeed);
        m->setSavedValue("waveTrailCustomColor",   waveTrailCustomColor);
        m->setSavedValue("waveTrailColor_r",       waveTrailColor[0]);
        m->setSavedValue("waveTrailColor_g",       waveTrailColor[1]);
        m->setSavedValue("waveTrailColor_b",       waveTrailColor[2]);
        m->setSavedValue("waveTrailOutline",       waveTrailOutline);
        m->setSavedValue("waveTrailOutlineStroke", waveTrailOutlineStroke);
        m->setSavedValue("waveTrailOutlineColor_r",waveTrailOutlineColor[0]);
        m->setSavedValue("waveTrailOutlineColor_g",waveTrailOutlineColor[1]);
        m->setSavedValue("waveTrailOutlineColor_b",waveTrailOutlineColor[2]);
        m->setSavedValue("showTrajectory",         showTrajectory);
        m->setSavedValue("trajectoryIterations",   trajectoryIterations);
        m->setSavedValue("autoKill",             autoKill);
        m->setSavedValue("autoKillByPercent",    autoKillByPercent);
        m->setSavedValue("autoKillPercent",      autoKillPercent);
        m->setSavedValue("autoKillByTime",       autoKillByTime);
        m->setSavedValue("autoKillTime",         autoKillTime);
        m->setSavedValue("respawnDelay",         respawnDelay);
        m->setSavedValue("respawnDelayTime",     respawnDelayTime);
        // Global
        m->setSavedValue("speedHackEnabled",   speedHackEnabled);
        m->setSavedValue("speedHackValue",     speedHackValue);
        m->setSavedValue("audioSpeedEnabled",  audioSpeedEnabled);
        m->setSavedValue("audioSpeedValue",    audioSpeedValue);
        m->setSavedValue("audioSpeedSync",     audioSpeedSync);
        m->setSavedValue("pitchShiftEnabled",  pitchShiftEnabled);
        m->setSavedValue("pitchShiftValue",    pitchShiftValue);
        m->setSavedValue("transparentLists",   transparentLists);
        // Cosmetic
        m->setSavedValue("noMirror",           noMirror);
        m->setSavedValue("instantMirror",      instantMirror);
        m->setSavedValue("noLevelParticles",   noLevelParticles);
        m->setSavedValue("noMiscParticles",    noMiscParticles);
        m->setSavedValue("noCustomParticles",  noCustomParticles);
        m->setSavedValue("noPortalLightning",  noPortalLightning);
        m->setSavedValue("noShader",           noShader);
        m->setSavedValue("layoutMode",         layoutMode);
        // Level
        m->setSavedValue("accuratePercentage",      accuratePercentage);
        m->setSavedValue("accuratePercentageDigits",accuratePercentageDigits);
        m->setSavedValue("autoSongDownload",        autoSongDownload);
        m->setSavedValue("forcePlatformer",         forcePlatformer);
        m->setSavedValue("hidePauseMenu",           hidePauseMenu);
        m->setSavedValue("zeroPercentPractice",     zeroPercentPractice);
        m->setSavedValue("showHitboxes",            showHitboxes);
        m->setSavedValue("showHitboxesOnDeath",     showHitboxesOnDeath);
        m->setSavedValue("startPosSwitcher",        startPosSwitcher);
        m->setSavedValue("labelsVisible",           labelsVisible);
        for (int i = 0; i < kLabelCount; i++) {
            auto& s = labelSettings[i];
            m->setSavedValue(fmt::format("label_{}_en",        i), s.enabled);
            m->setSavedValue(fmt::format("label_{}_r",         i), s.r);
            m->setSavedValue(fmt::format("label_{}_g",         i), s.g);
            m->setSavedValue(fmt::format("label_{}_b",         i), s.b);
            m->setSavedValue(fmt::format("label_{}_opacity",   i), s.opacity);
            m->setSavedValue(fmt::format("label_{}_alignment", i), s.alignment);
            labelEnabled[i] = s.enabled;
        }
        // Bypass
        m->setSavedValue("charFilter",         charFilter);
        m->setSavedValue("charLimit",          charLimit);
        m->setSavedValue("instantComplete",    instantComplete);
        m->setSavedValue("practiceMusic",      practiceMusic);
        m->setSavedValue("musicCustomizer",    musicCustomizer);
        m->setSavedValue("unlockPaths",        unlockPaths);
        m->setSavedValue("unlockIcons",        unlockIcons);
        m->setSavedValue("unlockMainLevels",   unlockMainLevels);
        m->setSavedValue("unlockShops",        unlockShops);
        m->setSavedValue("unlockVaults",       unlockVaults);
        // Editor
        m->setSavedValue("copyBypass",           copyBypass);
        m->setSavedValue("verifyBypass",         verifyBypass);
        m->setSavedValue("noCopyMark",           noCopyMark);
        m->setSavedValue("freeScroll",           freeScroll);
        m->setSavedValue("customObjectBypass",   customObjectBypass);
        m->setSavedValue("hideUI",               hideUI);
        m->setSavedValue("resetPercentOnSave",   resetPercentOnSave);
        m->setSavedValue("levelEdit",            levelEdit);
        m->setSavedValue("hideTriggers",         hideTriggers);
        m->setSavedValue("smoothTrail",          smoothTrail);
        m->setSavedValue("sliderLimit",          sliderLimit);
        m->setSavedValue("defaultSongBypass",    defaultSongBypass);
    }

    void load() {
        auto* m = Mod::get();
        androidUI            = m->getSavedValue<bool> ("androidUI",            false);
        hideFloatBtn         = m->getSavedValue<bool> ("hideFloatBtn",         false);
#if defined(GEODE_IS_ANDROID) || defined(GEODE_IS_IOS)
        androidUI = true; // always use mobile layout on mobile platforms
#endif
        noclip               = m->getSavedValue<bool> ("noclip",               false);
        autoClicker          = m->getSavedValue<bool> ("autoClicker",          false);
        autoClickerCPS       = m->getSavedValue<float>("autoClickerCPS",       15.0f);
        noDeathEffect        = m->getSavedValue<bool> ("noDeathEffect",        false);
        noWave               = m->getSavedValue<bool> ("noWave",               false);
        noRobotFire          = m->getSavedValue<bool> ("noRobotFire",          false);
        noSpiderDash         = m->getSavedValue<bool> ("noSpiderDash",         false);
        jumpHack             = m->getSavedValue<bool> ("jumpHack",             false);
        shipcopter           = m->getSavedValue<bool> ("shipcopter",           false);
        noVehicleParticles   = m->getSavedValue<bool> ("noVehicleParticles",   false);
        noRespawnFlash       = m->getSavedValue<bool> ("noRespawnFlash",       false);
        noPlatformerSquish   = m->getSavedValue<bool> ("noPlatformerSquish",   false);
        muteLevelAudioOnDeath= m->getSavedValue<bool> ("muteLevelAudioOnDeath",false);
        muteRewardsSFX       = m->getSavedValue<bool> ("muteRewardsSFX",       false);
        waveTrailOnDeath     = m->getSavedValue<bool> ("waveTrailOnDeath",     false);
        solidWaveTrail       = m->getSavedValue<bool> ("solidWaveTrail",       false);
        hidePlayer           = m->getSavedValue<bool> ("hidePlayer",           false);
        noHideTrigger        = m->getSavedValue<bool> ("noHideTrigger",        false);
        forceGhostTrail      = m->getSavedValue<bool> ("forceGhostTrail",      false);
        noGhostTrail         = m->getSavedValue<bool> ("noGhostTrail",         false);
        pathTracer             = m->getSavedValue<bool> ("pathTracer",             false);
        customWaveTrail        = m->getSavedValue<bool> ("customWaveTrail",        false);
        waveTrailPulse         = m->getSavedValue<bool> ("waveTrailPulse",         true);
        waveTrailScale         = m->getSavedValue<float>("waveTrailScale",         1.f);
        waveTrailRainbow       = m->getSavedValue<bool> ("waveTrailRainbow",       false);
        waveTrailRainbowSpeed  = m->getSavedValue<float>("waveTrailRainbowSpeed",  0.5f);
        waveTrailCustomColor   = m->getSavedValue<bool> ("waveTrailCustomColor",   false);
        waveTrailColor[0]      = m->getSavedValue<float>("waveTrailColor_r",       1.f);
        waveTrailColor[1]      = m->getSavedValue<float>("waveTrailColor_g",       1.f);
        waveTrailColor[2]      = m->getSavedValue<float>("waveTrailColor_b",       1.f);
        waveTrailOutline       = m->getSavedValue<bool> ("waveTrailOutline",       false);
        waveTrailOutlineStroke = m->getSavedValue<float>("waveTrailOutlineStroke", 2.f);
        waveTrailOutlineColor[0]= m->getSavedValue<float>("waveTrailOutlineColor_r",0.f);
        waveTrailOutlineColor[1]= m->getSavedValue<float>("waveTrailOutlineColor_g",0.f);
        waveTrailOutlineColor[2]= m->getSavedValue<float>("waveTrailOutlineColor_b",0.f);
        showTrajectory         = m->getSavedValue<bool> ("showTrajectory",         false);
        trajectoryIterations   = m->getSavedValue<int>  ("trajectoryIterations",   300);
        autoKill             = m->getSavedValue<bool> ("autoKill",             false);
        autoKillByPercent    = m->getSavedValue<bool> ("autoKillByPercent",    true);
        autoKillPercent      = m->getSavedValue<float>("autoKillPercent",      50.0f);
        autoKillByTime       = m->getSavedValue<bool> ("autoKillByTime",       false);
        autoKillTime         = m->getSavedValue<float>("autoKillTime",         90.0f);
        respawnDelay         = m->getSavedValue<bool> ("respawnDelay",         false);
        respawnDelayTime     = m->getSavedValue<float>("respawnDelayTime",     1.0f);
        speedHackEnabled     = m->getSavedValue<bool> ("speedHackEnabled",     false);
        speedHackValue       = m->getSavedValue<float>("speedHackValue",       1.0f);
        audioSpeedEnabled    = m->getSavedValue<bool> ("audioSpeedEnabled",    false);
        audioSpeedValue      = m->getSavedValue<float>("audioSpeedValue",      1.0f);
        audioSpeedSync       = m->getSavedValue<bool> ("audioSpeedSync",       false);
        pitchShiftEnabled    = m->getSavedValue<bool> ("pitchShiftEnabled",    false);
        pitchShiftValue      = m->getSavedValue<float>("pitchShiftValue",      1.0f);
        transparentLists     = m->getSavedValue<bool> ("transparentLists",     false);
        noMirror             = m->getSavedValue<bool> ("noMirror",             false);
        instantMirror        = m->getSavedValue<bool> ("instantMirror",        false);
        noLevelParticles     = m->getSavedValue<bool> ("noLevelParticles",     false);
        noMiscParticles      = m->getSavedValue<bool> ("noMiscParticles",      true);
        noCustomParticles    = m->getSavedValue<bool> ("noCustomParticles",    false);
        noPortalLightning    = m->getSavedValue<bool> ("noPortalLightning",    false);
        noShader             = m->getSavedValue<bool> ("noShader",             false);
        layoutMode           = m->getSavedValue<bool> ("layoutMode",           false);
        accuratePercentage     = m->getSavedValue<bool>("accuratePercentage",     false);
        accuratePercentageDigits=m->getSavedValue<int> ("accuratePercentageDigits",4);
        autoSongDownload       = m->getSavedValue<bool>("autoSongDownload",       false);
        forcePlatformer        = m->getSavedValue<bool>("forcePlatformer",        false);
        hidePauseMenu          = m->getSavedValue<bool>("hidePauseMenu",          false);
        zeroPercentPractice    = m->getSavedValue<bool>("zeroPercentPractice",    false);
        showHitboxes           = m->getSavedValue<bool>("showHitboxes",           false);
        showHitboxesOnDeath    = m->getSavedValue<bool>("showHitboxesOnDeath",    false);
        startPosSwitcher       = m->getSavedValue<bool>("startPosSwitcher",       false);
        labelsVisible          = m->getSavedValue<bool>("labelsVisible",           true);
        for (int i = 0; i < kLabelCount; i++) {
            auto& s    = labelSettings[i];
            s.enabled   = m->getSavedValue<bool> (fmt::format("label_{}_en",        i), false);
            s.r         = m->getSavedValue<float>(fmt::format("label_{}_r",         i), 1.f);
            s.g         = m->getSavedValue<float>(fmt::format("label_{}_g",         i), 1.f);
            s.b         = m->getSavedValue<float>(fmt::format("label_{}_b",         i), 1.f);
            s.opacity   = m->getSavedValue<float>(fmt::format("label_{}_opacity",   i), 1.f);
            s.alignment = m->getSavedValue<int>  (fmt::format("label_{}_alignment", i), 0);
            labelEnabled[i] = s.enabled;
        }
        charFilter           = m->getSavedValue<bool> ("charFilter",           false);
        charLimit            = m->getSavedValue<bool> ("charLimit",            false);
        instantComplete      = m->getSavedValue<bool> ("instantComplete",      false);
        practiceMusic        = m->getSavedValue<bool> ("practiceMusic",        false);
        musicCustomizer      = m->getSavedValue<bool> ("musicCustomizer",      false);
        unlockPaths          = m->getSavedValue<bool> ("unlockPaths",          false);
        unlockIcons          = m->getSavedValue<bool> ("unlockIcons",          false);
        unlockMainLevels     = m->getSavedValue<bool> ("unlockMainLevels",     false);
        unlockShops          = m->getSavedValue<bool> ("unlockShops",          false);
        unlockVaults         = m->getSavedValue<bool> ("unlockVaults",         false);
        copyBypass           = m->getSavedValue<bool> ("copyBypass",           false);
        verifyBypass         = m->getSavedValue<bool> ("verifyBypass",         false);
        noCopyMark           = m->getSavedValue<bool> ("noCopyMark",           false);
        freeScroll           = m->getSavedValue<bool> ("freeScroll",           false);
        customObjectBypass   = m->getSavedValue<bool> ("customObjectBypass",   false);
        hideUI               = m->getSavedValue<bool> ("hideUI",               false);
        resetPercentOnSave   = m->getSavedValue<bool> ("resetPercentOnSave",   false);
        levelEdit            = m->getSavedValue<bool> ("levelEdit",            false);
        hideTriggers         = m->getSavedValue<bool> ("hideTriggers",         false);
        smoothTrail          = m->getSavedValue<bool> ("smoothTrail",          false);
        sliderLimit          = m->getSavedValue<bool> ("sliderLimit",          false);
        defaultSongBypass    = m->getSavedValue<bool> ("defaultSongBypass",    false);
    }

    // ── Panel layout (position, size, collapsed) ──────────
    static constexpr int kPanelCount = 9;
    struct PanelLayout {
        float x         = 0.f;
        float y         = 0.f;
        float w         = 180.f;
        float h         = 400.f;
        bool  collapsed = false;
    };
    PanelLayout panels[kPanelCount] = {};

    void savePanels() {
        auto* m = Mod::get();
        for (int i = 0; i < kPanelCount; i++) {
            auto& p = panels[i];
            m->setSavedValue(fmt::format("panel_{}_x", i), p.x);
            m->setSavedValue(fmt::format("panel_{}_y", i), p.y);
            m->setSavedValue(fmt::format("panel_{}_w", i), p.w);
            m->setSavedValue(fmt::format("panel_{}_h", i), p.h);
            m->setSavedValue(fmt::format("panel_{}_c", i), p.collapsed);
        }
    }

    void loadPanels(float screenW, float screenH) {
        auto* m = Mod::get();
        const float W   = screenW * 0.14f;  // ~14% of screen width
        const float PAD = screenW * 0.006f;
        const float defY = screenH * 0.12f;
        // Default heights per panel (rough estimate based on item count)
        static const float kDefH[] = {
            screenH * 0.75f, // Player  — lots of items
            screenH * 0.55f, // Global
            screenH * 0.45f, // Cosmetic
            screenH * 0.35f, // General
            screenH * 0.45f, // Bypass
            screenH * 0.45f, // Editor
            screenH * 0.50f, // Labels
            screenH * 0.20f, // Settings
            screenH * 0.18f, // Info
        };
        for (int i = 0; i < kPanelCount; i++) {
            float defX = PAD + i * (W + PAD);
            auto& p = panels[i];
            p.x         = m->getSavedValue<float>(fmt::format("panel_{}_x", i), defX);
            p.y         = m->getSavedValue<float>(fmt::format("panel_{}_y", i), defY);
            p.w         = m->getSavedValue<float>(fmt::format("panel_{}_w", i), W);
            p.h         = m->getSavedValue<float>(fmt::format("panel_{}_h", i), kDefH[i]);
            p.collapsed = m->getSavedValue<bool> (fmt::format("panel_{}_c", i), false);
        }
    }

private:
    HackState() = default;
};
