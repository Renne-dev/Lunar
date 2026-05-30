#include "HackMenu.hpp"
#include "HackState.hpp"
#include <imgui-cocos.hpp>
#include <Geode/Geode.hpp>
#include <Geode/modify/OptionsLayer.hpp>
#include <Geode/binding/LevelInfoLayer.hpp>
#include <Geode/binding/GameStatsManager.hpp>
#include <Geode/binding/GameLevelManager.hpp>

using namespace geode::prelude;

static bool s_open = false;
static bool s_collapseInit[9] = {};

void toggleMenu() {
    s_open = !s_open;
    if (s_open) {
        for (int i = 0; i < 9; i++) s_collapseInit[i] = false;
    }
}

static void applyStyle() {
    ImGuiStyle& style = ImGui::GetStyle();
    ImGuiIO& io       = ImGui::GetIO();

    const float scale = std::max(0.5f, (io.DisplaySize.x / 1920.f + io.DisplaySize.y / 1080.f) * 0.5f);

    style.WindowRounding        = 0.f;
    style.ChildRounding         = 0.f;
    style.FrameRounding         = 0.f;
    style.GrabRounding          = 0.f;
    style.TabRounding           = 0.f;
    style.ScrollbarRounding     = 0.f;
    style.PopupRounding         = 0.f;
    style.WindowBorderSize      = 1.f;
    style.FrameBorderSize       = 0.f;
    style.TabBorderSize         = 0.f;
    style.WindowTitleAlign      = {0.5f, 0.5f};
    style.WindowPadding         = {std::round(6.f  * scale), std::round(6.f  * scale)};
    style.FramePadding          = {std::round(4.f  * scale), std::round(2.f  * scale)};
    style.ItemSpacing           = {std::round(4.f  * scale), std::round(3.f  * scale)};
    style.ItemInnerSpacing      = {std::round(4.f  * scale), std::round(4.f  * scale)};
    style.IndentSpacing         = std::round(12.f * scale);
    style.ScrollbarSize         = std::round(8.f  * scale);
    style.GrabMinSize           = std::round(6.f  * scale);


    ImVec4* c = style.Colors;

    c[ImGuiCol_WindowBg]            = {0.10f, 0.10f, 0.10f, 0.97f};  // #1a1a1a
    c[ImGuiCol_ChildBg]             = {0.10f, 0.10f, 0.10f, 1.00f};
    c[ImGuiCol_PopupBg]             = {0.12f, 0.12f, 0.12f, 0.97f};
    c[ImGuiCol_Border]              = {0.20f, 0.20f, 0.20f, 1.00f};

    c[ImGuiCol_TitleBg]             = {0.10f, 0.10f, 0.10f, 1.00f};
    c[ImGuiCol_TitleBgActive]       = {0.10f, 0.10f, 0.10f, 1.00f};
    c[ImGuiCol_TitleBgCollapsed]    = {0.10f, 0.10f, 0.10f, 1.00f};


    c[ImGuiCol_Tab]                 = {0.14f, 0.14f, 0.14f, 1.00f};
    c[ImGuiCol_TabHovered]          = {0.10f, 0.45f, 0.22f, 1.00f};  //  green
    c[ImGuiCol_TabActive]           = {0.00f, 0.55f, 0.27f, 1.00f};  // #00c853
    c[ImGuiCol_TabUnfocused]        = {0.14f, 0.14f, 0.14f, 1.00f};
    c[ImGuiCol_TabUnfocusedActive]  = {0.00f, 0.45f, 0.22f, 1.00f};


    c[ImGuiCol_FrameBg]             = {0.17f, 0.17f, 0.17f, 1.00f};
    c[ImGuiCol_FrameBgHovered]      = {0.22f, 0.22f, 0.22f, 1.00f};
    c[ImGuiCol_FrameBgActive]       = {0.25f, 0.25f, 0.25f, 1.00f};

 
    c[ImGuiCol_CheckMark]           = {0.00f, 0.90f, 0.45f, 1.00f};  // #00e676
    c[ImGuiCol_SliderGrab]          = {0.00f, 0.75f, 0.38f, 1.00f};
    c[ImGuiCol_SliderGrabActive]    = {0.00f, 0.90f, 0.45f, 1.00f};


    c[ImGuiCol_Button]              = {0.18f, 0.18f, 0.18f, 1.00f};
    c[ImGuiCol_ButtonHovered]       = {0.10f, 0.40f, 0.20f, 1.00f};
    c[ImGuiCol_ButtonActive]        = {0.00f, 0.55f, 0.27f, 1.00f};


    c[ImGuiCol_Header]              = {0.10f, 0.35f, 0.18f, 1.00f};
    c[ImGuiCol_HeaderHovered]       = {0.10f, 0.45f, 0.22f, 1.00f};
    c[ImGuiCol_HeaderActive]        = {0.00f, 0.55f, 0.27f, 1.00f};


    c[ImGuiCol_Separator]           = {0.15f, 0.30f, 0.20f, 1.00f};
    c[ImGuiCol_SeparatorHovered]    = {0.00f, 0.65f, 0.32f, 1.00f};
    c[ImGuiCol_SeparatorActive]     = {0.00f, 0.75f, 0.38f, 1.00f};

    // Scrollbar
    c[ImGuiCol_ScrollbarBg]         = {0.10f, 0.10f, 0.10f, 1.00f};
    c[ImGuiCol_ScrollbarGrab]       = {0.25f, 0.25f, 0.25f, 1.00f};
    c[ImGuiCol_ScrollbarGrabHovered]= {0.35f, 0.35f, 0.35f, 1.00f};
    c[ImGuiCol_ScrollbarGrabActive] = {0.00f, 0.65f, 0.32f, 1.00f};

    // Text
    c[ImGuiCol_Text]                = {1.00f, 1.00f, 1.00f, 1.00f};
    c[ImGuiCol_TextDisabled]        = {0.50f, 0.50f, 0.50f, 1.00f};

    // Resize grip
    c[ImGuiCol_ResizeGrip]          = {0.00f, 0.55f, 0.27f, 0.40f};
    c[ImGuiCol_ResizeGripHovered]   = {0.00f, 0.65f, 0.32f, 0.70f};
    c[ImGuiCol_ResizeGripActive]    = {0.00f, 0.75f, 0.38f, 1.00f};
}

// ── Macros ───────────────────────────────────────────────────────────────────

#define HACK(label, field) \
    if (ImGui::Checkbox(label, &st.field)) st.save();

// Slider with an inline text input box to its right for precise entry
#define HACK_SLIDER(label, field, mn, mx, fmt) { \
    float _avail = ImGui::GetContentRegionAvail().x - 68.f; \
    ImGui::SetNextItemWidth(_avail); \
    if (ImGui::SliderFloat("##sl_" #field, &st.field, mn, mx, fmt)) st.save(); \
    ImGui::SameLine(0.f, 4.f); \
    ImGui::SetNextItemWidth(60.f); \
    if (ImGui::InputFloat("##in_" #field, &st.field, 0.f, 0.f, fmt)) { \
        st.field = std::clamp(st.field, (float)(mn), (float)(mx)); \
        st.save(); \
    } \
    ImGui::SameLine(0.f, 4.f); \
    ImGui::TextUnformatted(label); \
}


// Forward declarations needed by drawPanelContent
static bool s_layoutLoaded = false;
static bool s_layoutDirty  = false;

// ── Shared content renderer (used by both desktop & android layouts) ──────────
static void drawPanelContent(int idx, HackState& st) {
    switch (idx) {
    case 0: // PLAYER
        ImGui::SeparatorText("Noclip");
        HACK("Noclip", noclip)
        ImGui::SeparatorText("Auto Clicker");
        HACK("Auto Clicker", autoClicker)
        HACK_SLIDER("CPS", autoClickerCPS, 1.f, 60.f, "%.0f")
        if (st.autoClicker) ImGui::TextDisabled("  CPS: %d", st.currentCPS);
        ImGui::SeparatorText("Auto Kill");
        HACK("Auto Kill", autoKill)
        if (st.autoKill) {
            HACK("  By Percent", autoKillByPercent)
            if (st.autoKillByPercent) HACK_SLIDER("Kill %", autoKillPercent, 0.f, 100.f, "%.1f")
            HACK("  By Time", autoKillByTime)
            if (st.autoKillByTime) HACK_SLIDER("Kill Time", autoKillTime, 0.f, 300.f, "%.1f")
        }
        ImGui::SeparatorText("Movement");
        HACK("Jump Hack", jumpHack)
        HACK("Shipcopter", shipcopter)
        ImGui::SeparatorText("Respawn");
        HACK("Respawn Delay", respawnDelay)
        if (st.respawnDelay) HACK_SLIDER("Delay (s)", respawnDelayTime, 0.f, 10.f, "%.2f")
        HACK("No Respawn Flash", noRespawnFlash)
        ImGui::SeparatorText("Effects");
        HACK("No Death Effect",      noDeathEffect)
        HACK("No Wave Trail",        noWave)
        HACK("Wave Trail On Death",  waveTrailOnDeath)
        HACK("Solid Wave Trail",     solidWaveTrail)
        HACK("No Platformer Squish", noPlatformerSquish)
        ImGui::SeparatorText("Visibility");
        HACK("Hide Player",       hidePlayer)
        HACK("No Hide Trigger",   noHideTrigger)
        HACK("Force Ghost Trail", forceGhostTrail)
        HACK("No Ghost Trail",    noGhostTrail)
        ImGui::SeparatorText("Audio");
        HACK("Mute Audio On Death", muteLevelAudioOnDeath)
        HACK("Mute Rewards SFX",   muteRewardsSFX)
        ImGui::SeparatorText("Misc");
        HACK("Path Tracer",     pathTracer)
        HACK("Show Trajectory", showTrajectory)
        if (st.showTrajectory) {
            ImGui::SetNextItemWidth(120.f);
            if (ImGui::InputInt("Iterations", &st.trajectoryIterations)) {
                st.trajectoryIterations = std::clamp(st.trajectoryIterations, 1, 2000); st.save();
            }
        }
        ImGui::SeparatorText("Wave Trail");
        HACK("Custom Wave Trail", customWaveTrail)
        if (st.customWaveTrail) {
            HACK("  Pulse", waveTrailPulse)
            HACK_SLIDER("Scale", waveTrailScale, 0.f, 25.f, "%.2f")
            HACK("  Rainbow", waveTrailRainbow)
            if (st.waveTrailRainbow) HACK_SLIDER("Speed", waveTrailRainbowSpeed, 0.f, 5.f, "%.2f")
            HACK("  Custom Color", waveTrailCustomColor)
            if (st.waveTrailCustomColor) if (ImGui::ColorEdit3("Trail##wt", st.waveTrailColor)) st.save();
            HACK("  Outline", waveTrailOutline)
            if (st.waveTrailOutline) {
                HACK_SLIDER("Stroke", waveTrailOutlineStroke, 0.f, 10.f, "%.2f")
                if (ImGui::ColorEdit3("Outline##wt", st.waveTrailOutlineColor)) st.save();
            }
        }
        break;

    case 1: // GLOBAL
        ImGui::SeparatorText("Display");
        HACK("Accurate %", accuratePercentage)
        if (st.accuratePercentage) {
            ImGui::SetNextItemWidth(80.f);
            if (ImGui::InputInt("Digits", &st.accuratePercentageDigits)) {
                st.accuratePercentageDigits = std::clamp(st.accuratePercentageDigits, 0, 15); st.save();
            }
        }
        ImGui::SeparatorText("Gameplay");
        HACK("Force Platformer",  forcePlatformer)
        HACK("0% Practice",       zeroPercentPractice)
        HACK("StartPos Switcher", startPosSwitcher)
        if (st.startPosSwitcher) ImGui::TextDisabled("Q=prev  E=next");
        ImGui::SeparatorText("Hitboxes");
        HACK("Show Hitboxes",     showHitboxes)
        HACK("Hitboxes On Death", showHitboxesOnDeath)
        ImGui::SeparatorText("Misc");
        HACK("Auto Song Download", autoSongDownload)
        HACK("Hide Pause Menu",    hidePauseMenu)
        ImGui::Spacing();
        if (ImGui::Button("Uncomplete Level", {-1, 0})) {
            auto* scene = CCDirector::get()->getRunningScene();
            GJGameLevel* level = nullptr;
            if (auto* pl = PlayLayer::get()) level = pl->m_level;
            else if (scene) level = scene->getChildByType<LevelInfoLayer>(0)
                ? scene->getChildByType<LevelInfoLayer>(0)->m_level : nullptr;
            if (level && level->m_levelType == GJLevelType::Saved) {
                level->m_practicePercent = 0; level->m_normalPercent = 0;
                level->m_newNormalPercent2 = 0; level->m_orbCompletion = 0;
                level->m_platformerSeed = 0; level->m_bestPoints = 0; level->m_bestTime = 0;
                auto* gsm = GameStatsManager::sharedState();
                auto* glm = GameLevelManager::sharedState();
                if (level->m_normalPercent >= 100 && gsm->hasCompletedLevel(level)) {
                    int id = level->m_levelID.value();
                    gsm->setStat("4", gsm->getStat("4") - 1);
                    std::string lk;
                    if (level->m_dailyID > 200000) lk = fmt::format("e_{}", id);
                    else if (level->m_gauntletLevel) lk = fmt::format("g_{}", id);
                    else if (level->m_dailyID > 0) lk = fmt::format("d_{}", id);
                    else if (level->m_levelType != GJLevelType::Main) lk = fmt::format("c_{}", id);
                    else lk = fmt::format("n_{}", id);
                    gsm->m_completedLevels->removeObjectForKey(lk);
                    if (level->m_stars > 0) {
                        gsm->m_completedLevels->removeObjectForKey(fmt::format("{}", gsm->getStarLevelKey(level)));
                        gsm->m_completedLevels->removeObjectForKey(fmt::format("demon_{}", id));
                        if (level->isPlatformer()) gsm->setStat("28", gsm->getStat("28") - level->m_stars);
                        else gsm->setStat("6", gsm->getStat("6") - level->m_stars);
                        if (level->m_demon > 0) gsm->setStat("5", gsm->getStat("5") - 1);
                    }
                    for (int i = 0; i < level->m_coins; i++) {
                        auto key = level->getCoinKey(i + 1);
                        if (level->m_coinsVerified.value() > 0 && gsm->hasUserCoin(key))
                            gsm->m_verifiedUserCoins->removeObjectForKey(key);
                        else if (gsm->hasPendingUserCoin(key))
                            gsm->m_pendingUserCoins->removeObjectForKey(key);
                    }
                }
                glm->saveLevel(level);
            }
        }
        break;

    case 2: // COSMETIC
        ImGui::SeparatorText("Player");
        HACK("No Robot Fire",        noRobotFire)
        HACK("No Spider Dash",       noSpiderDash)
        HACK("No Vehicle Particles", noVehicleParticles)
        ImGui::SeparatorText("Level");
        HACK("No Mirror",           noMirror)
        HACK("Instant Mirror",      instantMirror)
        HACK("No Portal Lightning", noPortalLightning)
        HACK("No Shaders",          noShader)
        HACK("Layout Mode",         layoutMode)
        ImGui::SeparatorText("Particles");
        HACK("No Level Particles", noLevelParticles)
        if (st.noLevelParticles) {
            HACK("  No Misc",   noMiscParticles)
            HACK("  No Custom", noCustomParticles)
        }
        break;

    case 3: // GENERAL
        ImGui::SeparatorText("Speed");
        HACK("Speed Hack", speedHackEnabled)
        HACK_SLIDER("Speed", speedHackValue, 0.01f, 10.f, "%.3f")
        ImGui::SeparatorText("Audio");
        HACK("Audio Speed", audioSpeedEnabled)
        if (st.audioSpeedEnabled) HACK_SLIDER("Audio Speed", audioSpeedValue, 0.001f, 10.f, "%.3f")
        HACK("  Sync With Speed", audioSpeedSync)
        HACK("Mute Sound", pitchShiftEnabled)
        ImGui::SeparatorText("Visual");
        HACK("Transparent Lists", transparentLists)
        break;

    case 4: // BYPASS
        ImGui::SeparatorText("Input");
        HACK("Char Filter Bypass", charFilter)
        HACK("Char Limit Bypass",  charLimit)
        ImGui::SeparatorText("Level");
        HACK("Instant Complete",   instantComplete)
        HACK("Unlock Main Levels", unlockMainLevels)
        ImGui::SeparatorText("Unlock");
        HACK("Unlock Icons",  unlockIcons)
        HACK("Unlock Shops",  unlockShops)
        HACK("Unlock Vaults", unlockVaults)
        HACK("Unlock Paths",  unlockPaths)
        ImGui::SeparatorText("Music");
        HACK("Practice Music Sync", practiceMusic)
        HACK("Music Customizer",    musicCustomizer)
        break;

    case 5: // EDITOR
        ImGui::SeparatorText("Bypass");
        HACK("Copy Bypass",          copyBypass)
        HACK("Verify Bypass",        verifyBypass)
        HACK("No (C) Mark",          noCopyMark)
        HACK("Default Song Bypass",  defaultSongBypass)
        HACK("Custom Object Bypass", customObjectBypass)
        ImGui::SeparatorText("Tools");
        HACK("Free Scroll",      freeScroll)
        HACK("Hide UI",          hideUI)
        HACK("Hide Triggers",    hideTriggers)
        HACK("Smooth Trail",     smoothTrail)
        HACK("Slider Limit",     sliderLimit)
        HACK("Reset % on Save",  resetPercentOnSave)
        HACK("Level Edit",       levelEdit)
        break;

    case 6: { // LABELS
        if (ImGui::Checkbox("Show Labels", &st.labelsVisible)) st.save();
        ImGui::Spacing();
        static const char* kLN[] = {"Noclip Deaths","Noclip Accuracy","FPS","CPS","Practice Mode","Clock"};
        static const char* kLP[] = {"Deaths: {noclipDeaths}","Accuracy: {noclipAccuracy}%","{round(fps)} FPS","{cps}/{clicks}","{isPracticeMode}","{clock}"};
        static const char* kLA[] = {"Top Left","Top Center","Top Right","Center Left","Center","Center Right","Bottom Left","Bottom Center","Bottom Right"};
        for (int i = 0; i < HackState::kLabelCount; i++) {
            auto& s = st.labelSettings[i];
            ImGui::PushID(i);
            if (ImGui::Checkbox(kLN[i], &s.enabled)) { st.labelEnabled[i] = s.enabled; st.save(); }
            if (s.enabled) {
                ImGui::TextDisabled("  %s", kLP[i]);
                ImGui::SetNextItemWidth(-1.f);
                if (ImGui::Combo("Pos##l", &s.alignment, kLA, 9)) st.save();
                float col3[3] = {s.r, s.g, s.b};
                if (ImGui::ColorEdit3("Color##l", col3, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
                    { s.r = col3[0]; s.g = col3[1]; s.b = col3[2]; st.save(); }
                ImGui::SameLine(0, 8);
                ImGui::SetNextItemWidth(-1.f);
                if (ImGui::SliderFloat("Opacity##l", &s.opacity, 0.f, 1.f, "%.2f")) st.save();
            }
            ImGui::PopID();
        }
        break;
    }

    case 7: // SETTINGS
        ImGui::SeparatorText("Interface");
#if !defined(GEODE_IS_ANDROID) && !defined(GEODE_IS_IOS)
        if (ImGui::Checkbox("Android UI", &st.androidUI)) {
            st.save();
            s_layoutLoaded = false;
            for (int i = 0; i < 9; i++) s_collapseInit[i] = false;
        }
        ImGui::TextDisabled("  Mobile-style sidebar layout");
        if (ImGui::Checkbox("Hide Floating Button", &st.hideFloatBtn)) st.save();
        ImGui::TextDisabled("  Hides the floating button");
#else
        ImGui::TextDisabled("Mobile layout (always on)");
#endif
        ImGui::SeparatorText("Game");
        if (ImGui::Button("Open Options Menu", {-1, 0})) {
            s_open = false;
            if (auto* options = OptionsLayer::create()) {
                auto* scene = CCDirector::get()->getRunningScene();
                if (scene) { scene->addChild(options, scene->getHighestChildZ() + 1); options->showLayer(false); }
            }
        }
        break;

    case 8: // INFO
        ImGui::SeparatorText("Lunar");
        ImGui::TextDisabled("Press TAB to toggle");
        break;
    }
}

// ── Android-style fullscreen layout ──────────────────────────────────────────
static void drawAndroidMenu() {
    static int   s_tab     = 0;
    static float s_scrollY = 0.f;

    auto& st = HackState::get();
    ImGuiIO& io = ImGui::GetIO();

    static const char* kPanelNames[9] = {
        "Player","Global","Cosmetic","General",
        "Bypass","Editor","Labels","Settings","Info"
    };
    static const char* kIcons[9] = {
        "[P]","[G]","[C]","[~]","[B]","[E]","[L]","[S]","[i]"
    };

    const float WIN_W = io.DisplaySize.x;
    const float WIN_H = io.DisplaySize.y;
    const float posX  = 0.f;
    const float posY  = 0.f;

    // Scale all sizing relative to display resolution (baseline 1280x720)
    const float scaleX = WIN_W / 1280.f;
    const float scaleY = WIN_H / 720.f;
    const float scale  = (scaleX + scaleY) * 0.5f;

    // Content font scale — mobile gets a boost for readability, tabs stay at scale
#if defined(GEODE_IS_ANDROID) || defined(GEODE_IS_IOS)
    const float contentFontScale = scale * 2.0f;
#else
    const float contentFontScale = scale;
#endif

    ImGui::SetNextWindowPos({posX, posY}, ImGuiCond_Always);
    ImGui::SetNextWindowSize({WIN_W, WIN_H}, ImGuiCond_Always);
    ImGui::Begin("##android_root", nullptr,
        ImGuiWindowFlags_NoTitleBar   | ImGuiWindowFlags_NoResize  |
        ImGuiWindowFlags_NoMove       | ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus
    );

    const float sidebarW = std::round(200.f * scaleX);
    const float fullH    = WIN_H;
    const float tabH     = std::round(60.f  * scaleY);

    // ── LEFT SIDEBAR ─────────────────────────────────────────────────────────
    ImGui::BeginChild("##sidebar", {sidebarW, fullH}, false,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    // Scale font size proportionally to resolution
    ImGui::SetWindowFontScale(scale);

    const float hdrH    = std::round(50.f * scaleY);
    const float accentW = std::round(4.f  * scaleX);
    const float padX    = std::round(16.f * scaleX);
    const float padY    = std::round(10.f * scaleY);

    // Sidebar header
    {
        ImVec2 p0 = ImGui::GetWindowPos();
        ImGui::GetWindowDrawList()->AddRectFilled(
            p0, {p0.x + sidebarW, p0.y + hdrH},
            IM_COL32(0, 130, 65, 255)
        );
        ImGui::Dummy({0, padY});
        float tw = ImGui::CalcTextSize("Lunar").x;
        ImGui::SetCursorPosX((sidebarW - tw) * 0.5f);
        ImGui::TextUnformatted("Lunar");

        // X close button — top right of header
        const float btnSz = hdrH * 0.6f;
        ImGui::SetCursorPos({sidebarW - btnSz - 4.f, (hdrH - btnSz) * 0.5f});
        ImGui::PushStyleColor(ImGuiCol_Button,        {0.6f, 0.1f, 0.1f, 1.f});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.8f, 0.2f, 0.2f, 1.f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  {1.0f, 0.3f, 0.3f, 1.f});
        if (ImGui::Button("X##close", {btnSz, btnSz}))
            s_open = false;
        ImGui::PopStyleColor(3);

        ImGui::Dummy({0, std::round(6.f * scaleY)});
    }

    // Tab buttons
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, {0.f, 1.f});
    for (int i = 0; i < 9; i++) {
        bool active = (s_tab == i);
        ImVec2 btnPos = ImGui::GetCursorScreenPos();

        // Active highlight
        if (active) {
            ImGui::GetWindowDrawList()->AddRectFilled(
                btnPos, {btnPos.x + sidebarW, btnPos.y + tabH},
                IM_COL32(0, 160, 80, 255)
            );
            // Green left accent bar
            ImGui::GetWindowDrawList()->AddRectFilled(
                btnPos, {btnPos.x + accentW, btnPos.y + tabH},
                IM_COL32(0, 230, 115, 255)
            );
        }

        ImGui::PushStyleColor(ImGuiCol_Button,        {0,0,0,0});
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.f, 0.55f, 0.27f, 0.5f});
        ImGui::PushStyleColor(ImGuiCol_ButtonActive,  {0.f, 0.70f, 0.35f, 1.f});
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 0.f);

        char btn[64];
        snprintf(btn, sizeof(btn), "  %s  %s##tab%d", kIcons[i], kPanelNames[i], i);
        if (ImGui::Button(btn, {sidebarW, tabH})) {
            s_tab = i;
            s_scrollY = 0.f;
        }

        ImGui::PopStyleVar();
        ImGui::PopStyleColor(3);

        // Separator line between tabs
        if (!active) {
            ImVec2 after = ImGui::GetCursorScreenPos();
            ImGui::GetWindowDrawList()->AddLine(
                {btnPos.x + padX, after.y},
                {btnPos.x + sidebarW - padX, after.y},
                IM_COL32(50, 50, 50, 255), 1.f
            );
        }
    }
    ImGui::PopStyleVar();

    ImGui::EndChild();

    // ── CONTENT AREA ─────────────────────────────────────────────────────────
    ImGui::SameLine(0, 0);

    // Vertical green separator
    {
        ImVec2 p = ImGui::GetCursorScreenPos();
        ImGui::GetWindowDrawList()->AddLine(
            p, {p.x, p.y + fullH},
            IM_COL32(0, 180, 90, 200), std::max(1.f, std::round(2.f * scale))
        );
    }

    ImGui::BeginChild("##content", {0, fullH}, false,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::SetWindowFontScale(contentFontScale);

    // Content header bar (fixed height at top)
    {
        ImVec2 p0 = ImGui::GetWindowPos();
        float cw = ImGui::GetWindowWidth();
        ImGui::GetWindowDrawList()->AddRectFilled(
            p0, {p0.x + cw, p0.y + hdrH},
            IM_COL32(18, 18, 18, 255)
        );
        ImGui::GetWindowDrawList()->AddLine(
            {p0.x, p0.y + hdrH - 1.f}, {p0.x + cw, p0.y + hdrH - 1.f},
            IM_COL32(0, 200, 100, 180), 1.f
        );
        // Advance cursor past the header rect
        ImGui::SetCursorPosY(hdrH + 4.f);
        ImGui::SetCursorPosX(padX);
        ImGui::Text("%s  %s", kIcons[s_tab], kPanelNames[s_tab]);
        ImGui::SetCursorPosY(hdrH + ImGui::GetTextLineHeight() + 8.f);
    }

    const float scrollH    = fullH - hdrH - ImGui::GetTextLineHeight() - 12.f;
    const float scrollBtnW = std::round(60.f * scaleX);


    // ── Left scroll buttons ───────────────────────────────────────────────────
    ImGui::BeginChild("##scrollbtns", {scrollBtnW, scrollH}, false,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::SetWindowFontScale(contentFontScale);

    const float btnH = scrollH * 0.5f - 2.f;
    ImGui::PushStyleColor(ImGuiCol_Button,        {0.f, 0.5f, 0.25f, 1.f});
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {0.f, 0.65f, 0.32f, 1.f});
    ImGui::PushStyleColor(ImGuiCol_ButtonActive,  {0.f, 0.8f,  0.4f,  1.f});
    if (ImGui::Button("^", {scrollBtnW, btnH}))
        s_scrollY = std::max(0.f, s_scrollY - 80.f * scale);
    ImGui::Dummy({0, 4.f});
    if (ImGui::Button("v", {scrollBtnW, btnH}))
        s_scrollY += 80.f * scale;
    ImGui::PopStyleColor(3);
    ImGui::EndChild();

    ImGui::SameLine(0, 2.f);

    // ── Scrollable content ────────────────────────────────────────────────────
    ImGui::BeginChild("##content_scroll", {0, scrollH}, false,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
    ImGui::SetWindowFontScale(contentFontScale);
    ImGui::SetScrollY(s_scrollY);
    // Clamp after content is rendered
    float maxScroll = ImGui::GetScrollMaxY();
    s_scrollY = std::clamp(s_scrollY, 0.f, maxScroll > 0.f ? maxScroll : s_scrollY);

#if defined(GEODE_IS_ANDROID) || defined(GEODE_IS_IOS)
    const float ws = scale * 0.85f;
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding,     {5.f * ws, 3.f * ws});
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing,      {4.f * ws, 5.f * ws});
    ImGui::PushStyleVar(ImGuiStyleVar_GrabMinSize,      7.f * ws);
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding,    3.f  * ws);
    ImGui::PushStyleVar(ImGuiStyleVar_ItemInnerSpacing, {4.f * ws, 4.f * ws});
#endif
    ImGui::SetCursorPosX(ImGui::GetCursorPosX() + 8.f * scale);
    ImGui::BeginGroup();
    drawPanelContent(s_tab, st);
    ImGui::EndGroup();
#if defined(GEODE_IS_ANDROID) || defined(GEODE_IS_IOS)
    ImGui::PopStyleVar(5);
#endif
    ImGui::EndChild();

    ImGui::EndChild();
    ImGui::End();
}


// ── Menu ─────────────────────────────────────────────────────────────────────

// Column layout: 9 panels arranged in columns.
static const char* kPanelNames[9] = {
    "Player","Global","Cosmetic","General",
    "Bypass","Editor","Labels","Settings","Info"
};

static void drawMenu() {
    if (!s_open) return;
    auto& st  = HackState::get();

    // On Android/iOS always use the mobile layout — no choice
#if defined(GEODE_IS_ANDROID) || defined(GEODE_IS_IOS)
    drawAndroidMenu(); return;
#else
    // Dispatch to android layout if enabled
    if (st.androidUI) { drawAndroidMenu(); return; }
#endif

    ImGuiIO& io = ImGui::GetIO();

    if (!s_layoutLoaded) {
        st.loadPanels(io.DisplaySize.x, io.DisplaySize.y);
        s_layoutLoaded = true;
    }

    for (int idx = 0; idx < 9; idx++) {
            auto& p = st.panels[idx];

            ImGui::SetNextWindowPos({p.x, p.y}, ImGuiCond_Once);
            ImGui::SetNextWindowSize({p.w, p.h}, ImGuiCond_Once);

            if (!s_collapseInit[idx]) {
                ImGui::SetNextWindowCollapsed(p.collapsed, ImGuiCond_Always);
                s_collapseInit[idx] = true;
            }

            ImGuiWindowFlags flags =
                ImGuiWindowFlags_NoScrollbar       |
                ImGuiWindowFlags_NoScrollWithMouse |
                ImGuiWindowFlags_NoSavedSettings;

            char title[64];
            snprintf(title, sizeof(title), "%s###hm_%d", kPanelNames[idx], idx);
            ImGui::Begin(title, nullptr, flags);

            bool nowCollapsed = ImGui::IsWindowCollapsed();
            if (nowCollapsed != p.collapsed) { p.collapsed = nowCollapsed; s_layoutDirty = true; }

            ImVec2 pos  = ImGui::GetWindowPos();
            ImVec2 size = ImGui::GetWindowSize();
            if (pos.x != p.x || pos.y != p.y) { p.x = pos.x; p.y = pos.y; s_layoutDirty = true; }
            if (!p.collapsed && (size.x != p.w || size.y != p.h)) { p.w = size.x; p.h = size.y; s_layoutDirty = true; }

            if (p.collapsed) { ImGui::End(); continue; }

            // Green divider
            {
                ImVec2 c = ImGui::GetCursorScreenPos();
                ImGui::GetWindowDrawList()->AddLine(
                    c, {c.x + ImGui::GetContentRegionAvail().x, c.y},
                    IM_COL32(0, 200, 100, 150), 1.f
                );
                ImGui::Dummy({0.f, 3.f});
            }

            // ── Panel content ─────────────────────────────────────────────────
            ImGui::BeginChild(
                (std::string("##sc") + std::to_string(idx)).c_str(),
                {0, 0}, false, ImGuiWindowFlags_HorizontalScrollbar
            );

            switch (idx) {

            case 0: // PLAYER
                ImGui::SeparatorText("Noclip");
                HACK("Noclip", noclip)
                ImGui::SeparatorText("Auto Clicker");
                HACK("Auto Clicker", autoClicker)
                HACK_SLIDER("CPS", autoClickerCPS, 1.f, 60.f, "%.0f")
                if (st.autoClicker) ImGui::TextDisabled("  CPS: %d", st.currentCPS);
                ImGui::SeparatorText("Auto Kill");
                HACK("Auto Kill", autoKill)
                if (st.autoKill) {
                    HACK("  By Percent", autoKillByPercent)
                    if (st.autoKillByPercent) HACK_SLIDER("Kill %", autoKillPercent, 0.f, 100.f, "%.1f")
                    HACK("  By Time", autoKillByTime)
                    if (st.autoKillByTime) HACK_SLIDER("Kill Time", autoKillTime, 0.f, 300.f, "%.1f")
                }
                ImGui::SeparatorText("Movement");
                HACK("Jump Hack", jumpHack)
                HACK("Shipcopter", shipcopter)
                ImGui::SeparatorText("Respawn");
                HACK("Respawn Delay", respawnDelay)
                if (st.respawnDelay) HACK_SLIDER("Delay (s)", respawnDelayTime, 0.f, 10.f, "%.2f")
                HACK("No Respawn Flash", noRespawnFlash)
                ImGui::SeparatorText("Effects");
                HACK("No Death Effect",      noDeathEffect)
                HACK("No Wave Trail",        noWave)
                HACK("Wave Trail On Death",  waveTrailOnDeath)
                HACK("Solid Wave Trail",     solidWaveTrail)
                HACK("No Platformer Squish", noPlatformerSquish)
                ImGui::SeparatorText("Visibility");
                HACK("Hide Player",       hidePlayer)
                HACK("No Hide Trigger",   noHideTrigger)
                HACK("Force Ghost Trail", forceGhostTrail)
                HACK("No Ghost Trail",    noGhostTrail)
                ImGui::SeparatorText("Audio");
                HACK("Mute Audio On Death", muteLevelAudioOnDeath)
                HACK("Mute Rewards SFX",   muteRewardsSFX)
                ImGui::SeparatorText("Misc");
                HACK("Path Tracer",     pathTracer)
                HACK("Show Trajectory", showTrajectory)
                if (st.showTrajectory) {
                    ImGui::SetNextItemWidth(100.f);
                    if (ImGui::InputInt("Iterations", &st.trajectoryIterations)) {
                        st.trajectoryIterations = std::clamp(st.trajectoryIterations, 1, 2000);
                        st.save();
                    }
                }
                ImGui::SeparatorText("Wave Trail");
                HACK("Custom Wave Trail", customWaveTrail)
                if (st.customWaveTrail) {
                    HACK("  Pulse", waveTrailPulse)
                    HACK_SLIDER("Scale", waveTrailScale, 0.f, 25.f, "%.2f")
                    HACK("  Rainbow", waveTrailRainbow)
                    if (st.waveTrailRainbow) HACK_SLIDER("Speed", waveTrailRainbowSpeed, 0.f, 5.f, "%.2f")
                    HACK("  Custom Color", waveTrailCustomColor)
                    if (st.waveTrailCustomColor)
                        if (ImGui::ColorEdit3("Trail##wt", st.waveTrailColor)) st.save();
                    HACK("  Outline", waveTrailOutline)
                    if (st.waveTrailOutline) {
                        HACK_SLIDER("Stroke", waveTrailOutlineStroke, 0.f, 10.f, "%.2f")
                        if (ImGui::ColorEdit3("Outline##wt", st.waveTrailOutlineColor)) st.save();
                    }
                }
                break;

            case 1: // GLOBAL
                ImGui::SeparatorText("Display");
                HACK("Accurate %", accuratePercentage)
                if (st.accuratePercentage) {
                    ImGui::SetNextItemWidth(80.f);
                    if (ImGui::InputInt("Digits", &st.accuratePercentageDigits)) {
                        st.accuratePercentageDigits = std::clamp(st.accuratePercentageDigits, 0, 15);
                        st.save();
                    }
                }
                ImGui::SeparatorText("Gameplay");
                HACK("Force Platformer",  forcePlatformer)
                HACK("0% Practice",       zeroPercentPractice)
                HACK("StartPos Switcher", startPosSwitcher)
                if (st.startPosSwitcher) ImGui::TextDisabled("Q=prev  E=next");
                ImGui::SeparatorText("Hitboxes");
                HACK("Show Hitboxes",     showHitboxes)
                HACK("Hitboxes On Death", showHitboxesOnDeath)
                ImGui::SeparatorText("Misc");
                HACK("Auto Song Download", autoSongDownload)
                HACK("Hide Pause Menu",    hidePauseMenu)
                ImGui::Spacing();
                if (ImGui::Button("Uncomplete Level", {-1, 0})) {
                    auto* scene = CCDirector::get()->getRunningScene();
                    GJGameLevel* level = nullptr;
                    if (auto* pl = PlayLayer::get()) level = pl->m_level;
                    else if (scene) level = scene->getChildByType<LevelInfoLayer>(0)
                                        ? scene->getChildByType<LevelInfoLayer>(0)->m_level : nullptr;
                    if (level && level->m_levelType == GJLevelType::Saved) {
                        level->m_practicePercent = 0; level->m_normalPercent = 0;
                        level->m_newNormalPercent2 = 0; level->m_orbCompletion = 0;
                        level->m_platformerSeed = 0; level->m_bestPoints = 0; level->m_bestTime = 0;
                        auto* gsm = GameStatsManager::sharedState();
                        auto* glm = GameLevelManager::sharedState();
                        if (level->m_normalPercent >= 100 && gsm->hasCompletedLevel(level)) {
                            int id = level->m_levelID.value();
                            gsm->setStat("4", gsm->getStat("4") - 1);
                            std::string lk;
                            if (level->m_dailyID > 200000) lk = fmt::format("e_{}", id);
                            else if (level->m_gauntletLevel) lk = fmt::format("g_{}", id);
                            else if (level->m_dailyID > 0) lk = fmt::format("d_{}", id);
                            else if (level->m_levelType != GJLevelType::Main) lk = fmt::format("c_{}", id);
                            else lk = fmt::format("n_{}", id);
                            gsm->m_completedLevels->removeObjectForKey(lk);
                            if (level->m_stars > 0) {
                                gsm->m_completedLevels->removeObjectForKey(fmt::format("{}", gsm->getStarLevelKey(level)));
                                gsm->m_completedLevels->removeObjectForKey(fmt::format("demon_{}", id));
                                if (level->isPlatformer()) gsm->setStat("28", gsm->getStat("28") - level->m_stars);
                                else gsm->setStat("6", gsm->getStat("6") - level->m_stars);
                                if (level->m_demon > 0) gsm->setStat("5", gsm->getStat("5") - 1);
                            }
                            for (int i = 0; i < level->m_coins; i++) {
                                auto key = level->getCoinKey(i + 1);
                                if (level->m_coinsVerified.value() > 0 && gsm->hasUserCoin(key))
                                    gsm->m_verifiedUserCoins->removeObjectForKey(key);
                                else if (gsm->hasPendingUserCoin(key))
                                    gsm->m_pendingUserCoins->removeObjectForKey(key);
                            }
                        }
                        glm->saveLevel(level);
                    }
                }
                break;

            case 2: // COSMETIC
                ImGui::SeparatorText("Player");
                HACK("No Robot Fire",        noRobotFire)
                HACK("No Spider Dash",       noSpiderDash)
                HACK("No Vehicle Particles", noVehicleParticles)
                ImGui::SeparatorText("Level");
                HACK("No Mirror",           noMirror)
                HACK("Instant Mirror",      instantMirror)
                HACK("No Portal Lightning", noPortalLightning)
                HACK("No Shaders",          noShader)
                HACK("Layout Mode",         layoutMode)
                ImGui::SeparatorText("Particles");
                HACK("No Level Particles", noLevelParticles)
                if (st.noLevelParticles) {
                    HACK("  No Misc",   noMiscParticles)
                    HACK("  No Custom", noCustomParticles)
                }
                break;

            case 3: // GENERAL
                ImGui::SeparatorText("Speed");
                HACK("Speed Hack", speedHackEnabled)
                HACK_SLIDER("Speed", speedHackValue, 0.01f, 10.f, "%.3f")
                ImGui::SeparatorText("Audio");
                HACK("Audio Speed", audioSpeedEnabled)
                if (st.audioSpeedEnabled) HACK_SLIDER("Audio Speed", audioSpeedValue, 0.001f, 10.f, "%.3f")
                HACK("  Sync With Speed", audioSpeedSync)
                HACK("Mute Sound", pitchShiftEnabled)
                ImGui::SeparatorText("Visual");
                HACK("Transparent Lists", transparentLists)
                break;

            case 4: // BYPASS
                ImGui::SeparatorText("Input");
                HACK("Char Filter Bypass", charFilter)
                HACK("Char Limit Bypass",  charLimit)
                ImGui::SeparatorText("Level");
                HACK("Instant Complete",   instantComplete)
                HACK("Unlock Main Levels", unlockMainLevels)
                ImGui::SeparatorText("Unlock");
                HACK("Unlock Icons",  unlockIcons)
                HACK("Unlock Shops",  unlockShops)
                HACK("Unlock Vaults", unlockVaults)
                HACK("Unlock Paths",  unlockPaths)
                ImGui::SeparatorText("Music");
                HACK("Practice Music Sync", practiceMusic)
                HACK("Music Customizer",    musicCustomizer)
                break;

            case 5: // EDITOR
                ImGui::SeparatorText("Bypass");
                HACK("Copy Bypass",          copyBypass)
                HACK("Verify Bypass",        verifyBypass)
                HACK("No (C) Mark",          noCopyMark)
                HACK("Default Song Bypass",  defaultSongBypass)
                HACK("Custom Object Bypass", customObjectBypass)
                ImGui::SeparatorText("Tools");
                HACK("Free Scroll",      freeScroll)
                HACK("Hide UI",          hideUI)
                HACK("Hide Triggers",    hideTriggers)
                HACK("Smooth Trail",     smoothTrail)
                HACK("Slider Limit",     sliderLimit)
                HACK("Reset % on Save",  resetPercentOnSave)
                HACK("Level Edit",       levelEdit)
                break;

            case 6: { // LABELS
                if (ImGui::Checkbox("Show Labels", &st.labelsVisible)) st.save();
                ImGui::Spacing();
                static const char* kLN[] = {"Noclip Deaths","Noclip Accuracy","FPS","CPS","Practice Mode","Clock"};
                static const char* kLP[] = {"Deaths: {noclipDeaths}","Accuracy: {noclipAccuracy}%","{round(fps)} FPS","{cps}/{clicks}","{isPracticeMode}","{clock}"};
                static const char* kLA[] = {"Top Left","Top Center","Top Right","Center Left","Center","Center Right","Bottom Left","Bottom Center","Bottom Right"};
                for (int i = 0; i < HackState::kLabelCount; i++) {
                    auto& s = st.labelSettings[i];
                    ImGui::PushID(i);
                    if (ImGui::Checkbox(kLN[i], &s.enabled)) { st.labelEnabled[i] = s.enabled; st.save(); }
                    if (s.enabled) {
                        ImGui::TextDisabled("  %s", kLP[i]);
                        ImGui::SetNextItemWidth(-1.f);
                        if (ImGui::Combo("Pos##l", &s.alignment, kLA, 9)) st.save();
                        float col[3] = {s.r, s.g, s.b};
                        if (ImGui::ColorEdit3("Color##l", col, ImGuiColorEditFlags_NoInputs | ImGuiColorEditFlags_NoLabel))
                            { s.r = col[0]; s.g = col[1]; s.b = col[2]; st.save(); }
                        ImGui::SameLine(0, 8);
                        ImGui::SetNextItemWidth(-1.f);
                        if (ImGui::SliderFloat("Opacity##l", &s.opacity, 0.f, 1.f, "%.2f")) st.save();
                    }
                    ImGui::PopID();
                }
                break;
            }

            case 7: // SETTINGS
                ImGui::SeparatorText("Interface");
#if !defined(GEODE_IS_ANDROID) && !defined(GEODE_IS_IOS)
                if (ImGui::Checkbox("Android UI", &st.androidUI)) {
                    st.save();
                    s_layoutLoaded = false;
                    for (int i = 0; i < 9; i++) s_collapseInit[i] = false;
                }
                ImGui::TextDisabled("  Mobile-style sidebar layout");
                if (ImGui::Checkbox("Hide Floating Button", &st.hideFloatBtn)) st.save();
                ImGui::TextDisabled("  Hides the floating button");
#else
                ImGui::TextDisabled("Mobile layout (always on)");
#endif
                ImGui::SeparatorText("Game");
                if (ImGui::Button("Open Options Menu", {-1, 0})) {
                    s_open = false;
                    if (auto* options = OptionsLayer::create()) {
                        auto* scene = CCDirector::get()->getRunningScene();
                        if (scene) { scene->addChild(options, scene->getHighestChildZ() + 1); options->showLayer(false); }
                    }
                }
                break;

            case 8: // INFO
                ImGui::SeparatorText("Lunar");
                ImGui::TextDisabled("Press TAB to toggle");
                break;

            } // end switch

            ImGui::EndChild();
            ImGui::End();
    } // end panel loop

    if (s_layoutDirty) {
        st.savePanels();
        s_layoutDirty = false;
    }
}

void setupImGui() {
    ImGuiCocos::get()
        .setup([] { applyStyle(); })
        .draw([]   { drawMenu();  });
}
