#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/EffectGameObject.hpp>
#include <Geode/modify/FMODAudioEngine.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include <Geode/modify/GameObject.hpp>
#include "../HackState.hpp"

using namespace geode::prelude;

// ═══════════════════════════════════════════════════════
//  NOCLIP  —       : Noclip.cpp
// ═══════════════════════════════════════════════════════

class $modify(HackPlayLayer, PlayLayer) {

    struct Fields {
        int deadFrames = 0;
    };

    void destroyPlayer(PlayerObject* player, GameObject* object) {
        if (object == m_anticheatSpike)
            return PlayLayer::destroyPlayer(player, object);

        auto& st = HackState::get();

        // Path Tracer needs to intercept destroyPlayer too
        if (st.pathTracer && st.pathTracerSimDead) {
            st.pathTracerSimDead = false;
            return;
        }

        if (st.noclip) {
            st.noclipDeaths++;
            auto frame = m_gameState.m_currentProgress;
            if (frame > 0) {
                m_fields.self()->deadFrames++;
                st.noclipAccuracy = (float)(frame - m_fields.self()->deadFrames)
                                  / (float)frame * 100.f;
            }
            return;
        }

        // Respawn Delay — after calling destroyPlayer, replace the respawn sequence
        PlayLayer::destroyPlayer(player, object);

        if (st.respawnDelay) {
            if (auto* seq = this->getActionByTag(0x10)) {
                this->stopAction(seq);
                auto* newSeq = cocos2d::CCSequence::create(
                    cocos2d::CCDelayTime::create(st.respawnDelayTime),
                    cocos2d::CCCallFunc::create(this, callfunc_selector(PlayLayer::delayedResetLevel)),
                    nullptr
                );
                newSeq->setTag(0x10);
                this->runAction(newSeq);
            }
        }
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        m_fields.self()->deadFrames    = 0;
        HackState::get().noclipDeaths  = 0;
        HackState::get().noclipAccuracy = 100.f;
    }

    // Path Tracer init
    bool init(GJGameLevel* level, bool p1, bool p2) {
        if (!PlayLayer::init(level, p1, p2)) return false;
        if (HackState::get().pathTracer)
            HackState::get().pathTracerNeedsInit = true;
        return true;
    }

    void onQuit() {
        PlayLayer::onQuit();
        HackState::get().pathTracerPlayer1 = nullptr;
        HackState::get().pathTracerPlayer2 = nullptr;
    }
};

// ═══════════════════════════════════════════════════════
//  AUTO CLICKER  —       : Autoclicker.cpp
//  JUMP HACK     —       : JumpHack.cpp
//  AUTO KILL     —       : AutoKill.cpp
//  Mac: processQueuedButtons + handleButton()
// ═══════════════════════════════════════════════════════

static bool s_jumpHackFlag = false;

class $modify(HackBGL, GJBaseGameLayer) {

    struct Fields {
        int  acTimer    = 0;
        bool acClicking = false;
    };

    void processQueuedButtons(float dt, bool clearInputQueue) {
        GJBaseGameLayer::processQueuedButtons(dt, clearInputQueue);

        auto& st = HackState::get();

        // Auto Clicker
        if (st.autoClicker) {
            auto* f = m_fields.self();
            int interval = std::max(1, (int)(240.f / st.autoClickerCPS));
            f->acTimer++;
            if ((f->acTimer >= interval && !f->acClicking) ||
                (f->acTimer >= interval &&  f->acClicking)) {
                f->acClicking = !f->acClicking;
                this->handleButton(f->acClicking, 1, true);
                f->acTimer = 0;
            }
            st.currentCPS = (int)(240.f / (float)interval);
        } else {
            st.currentCPS = 0;
        }
    }

    void update(float dt) {
        auto& st = HackState::get();

        // Jump Hack — set isOnGround before and after update
        if (st.jumpHack && s_jumpHackFlag) {
            if (m_player1) m_player1->m_isOnGround = true;
            if (m_player2) m_player2->m_isOnGround = true;
        }

        GJBaseGameLayer::update(dt);

        if (st.jumpHack && s_jumpHackFlag) {
            if (m_player1) m_player1->m_isOnGround = true;
            if (m_player2) m_player2->m_isOnGround = true;
            s_jumpHackFlag = false;
        }

        // Auto Kill
        if (!st.autoKill) return;
        auto* pl = PlayLayer::get();
        if (!pl) return;
        bool shouldKill = false;
        if (st.autoKillByPercent)
            shouldKill |= pl->getCurrentPercent() >= st.autoKillPercent;
        if (st.autoKillByTime)
            shouldKill |= m_gameState.m_levelTime >= st.autoKillTime;
        if (shouldKill && m_player1 && !m_player1->m_isDead) {
            bool prevNoclip = st.noclip;
            st.noclip = false;
            pl->PlayLayer::destroyPlayer(m_player1, m_player1);
            st.noclip = prevNoclip;
        }
    }
};

// ═══════════════════════════════════════════════════════
//  NO DEATH EFFECT        —       : NoDeathEffect.cpp
//  NO ROBOT FIRE          —       : NoRobotFire.cpp
//  NO SPIDER DASH         —       : NoSpiderDash.cpp
//  NO PLATFORMER SQUISH   —       : NoPlatformerJumpSquish.cpp
//  NO RESPAWN FLASH       —       : NoRespawnFlash.cpp
//  JUMP HACK              —       : JumpHack.cpp
//  SHIPCOPTER             —       : Shipcopter.cpp
//  WAVE TRAIL ON DEATH    —       : WaveTrailOnDeath.cpp
//  HIDE/SHOW PLAYER       —       : HideOrShowPlayer.cpp
//  GHOST TRAIL SETTINGS   —       : GhostTrailSettings.cpp
//  SOLID WAVE TRAIL       —       : SolidWaveTrail.cpp
// ═══════════════════════════════════════════════════════

static bool s_waveTrailDead = false;

class $modify(HackPlayerObject, PlayerObject) {

    struct Fields {
        bool           doReleaseFlip       = true;
        GhostType      curGhostType        = GhostType::Disabled;
        bool           isPlayerInvis       = false;
        std::optional<bool> wasSolidOriginally = std::nullopt;
    };

    // ── No Death Effect ──────────────────────────────────
    void playDeathEffect() {
        if (!HackState::get().noDeathEffect)
            PlayerObject::playDeathEffect();
    }

    // ── No Respawn Flash ─────────────────────────────────
    void playSpawnEffect() {
        if (!HackState::get().noRespawnFlash)
            PlayerObject::playSpawnEffect();
    }

    // ── No Platformer Jump Squish ────────────────────────
    void animatePlatformerJump(float p0) {
        if (!HackState::get().noPlatformerSquish)
            PlayerObject::animatePlatformerJump(p0);
    }

    // ── No Spider Dash ───────────────────────────────────
    void playSpiderDashEffect(cocos2d::CCPoint from, cocos2d::CCPoint to) {
        if (HackState::get().noSpiderDash) {
            bool prev = m_playEffects;
            m_playEffects = false;
            PlayerObject::playSpiderDashEffect(from, to);
            m_playEffects = prev;
            return;
        }
        PlayerObject::playSpiderDashEffect(from, to);
    }

    // ── No Robot Fire ────────────────────────────────────
    void update(float dt) {
        PlayerObject::update(dt);
        auto& st = HackState::get();
        if (st.noRobotFire) {
            if (m_robotFire)           m_robotFire->setVisible(false);
            if (m_robotBurstParticles) m_robotBurstParticles->setVisible(false);
        }
    }

    // ── Jump Hack ────────────────────────────────────────
    bool pushButton(PlayerButton p0) {
        auto& st = HackState::get();
        if (st.jumpHack) s_jumpHackFlag = true;

        // Shipcopter
        if (st.shipcopter && m_gameLayer) {
            bool ret = PlayerObject::pushButton(p0);
            if (ret && m_isSwing) {
                if (m_touchedRing && !m_isDashing)
                    m_fields.self()->doReleaseFlip = false;
                else
                    this->flipGravity(m_isUpsideDown, true);
            }
            return ret;
        }
        return PlayerObject::pushButton(p0);
    }

    bool releaseButton(PlayerButton p0) {
        // Shipcopter
        auto& st = HackState::get();
        if (st.shipcopter && m_gameLayer) {
            bool ret = PlayerObject::releaseButton(p0);
            if (ret && m_isSwing) {
                if (m_fields.self()->doReleaseFlip)
                    this->flipGravity(!m_isUpsideDown, true);
                else
                    m_fields.self()->doReleaseFlip = true;
            }
            return ret;
        }
        return PlayerObject::releaseButton(p0);
    }

    // ── Wave Trail On Death ──────────────────────────────
    void playerDestroyed(bool p0) {
        if (auto* pl = PlayLayer::get())
            s_waveTrailDead = HackState::get().waveTrailOnDeath &&
                              (this == pl->m_player1 || this == pl->m_player2);
        PlayerObject::playerDestroyed(p0);
        s_waveTrailDead = false;
    }

    void fadeOutStreak2(float duration) {
        if (s_waveTrailDead) return;
        PlayerObject::fadeOutStreak2(duration);
    }

    void createFadeOutDartStreak() {
        if (s_waveTrailDead) return;
        PlayerObject::createFadeOutDartStreak();
    }

    // ── Hide / Show Player ───────────────────────────────
    void toggleVisibility(bool p0) {
        m_fields.self()->isPlayerInvis = p0;
        auto& st = HackState::get();
        if (st.hidePlayer)     { PlayerObject::toggleVisibility(false); return; }
        if (st.noHideTrigger)  { PlayerObject::toggleVisibility(true);  return; }
        PlayerObject::toggleVisibility(p0);
    }

    // ── Ghost Trail ──────────────────────────────────────
    void toggleGhostEffect(GhostType p0) {
        m_fields.self()->curGhostType = p0;
        auto& st = HackState::get();
        if (st.forceGhostTrail && !m_isDead && PlayLayer::get())
            p0 = GhostType::Enabled;
        else if (st.noGhostTrail)
            p0 = GhostType::Disabled;
        PlayerObject::toggleGhostEffect(p0);
    }

    // ── Solid Wave Trail ─────────────────────────────────
    void setupStreak() {
        PlayerObject::setupStreak();
        if (HackState::get().solidWaveTrail) {
            auto* f = m_fields.self();
            if (!f->wasSolidOriginally.has_value())
                f->wasSolidOriginally = m_waveTrail->m_isSolid;
            m_waveTrail->m_isSolid = true;
            m_waveTrail->setBlendFunc({GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA});
        }
    }
};

// ═══════════════════════════════════════════════════════
//  NO WAVE TRAIL  —       : NoTrail.cpp
//  resumeStroke/stopStroke are on HardStreak, not CCMotionStreak
// ═══════════════════════════════════════════════════════

#include <Geode/modify/HardStreak.hpp>

class $modify(HackHardStreak, HardStreak) {
    void resumeStroke() {
        if (!HackState::get().noWave)
            HardStreak::resumeStroke();
    }
};

// ═══════════════════════════════════════════════════════
//  NO VEHICLE PARTICLES  —       : NoVehicleParticles.cpp
// ═══════════════════════════════════════════════════════

static void applyParticleVisibility(PlayerObject* player, bool hide) {
    if (!player) return;
    bool show = !hide;
    player->m_playerGroundParticles->setVisible(show);
    player->m_ufoClickParticles->setVisible(show);
    player->m_dashParticles->setVisible(show);
    player->m_robotBurstParticles->setVisible(show);
    player->m_trailingParticles->setVisible(show);
    player->m_shipClickParticles->setVisible(show);
    player->m_vehicleGroundParticles->setVisible(show);
    player->m_landParticles0->setVisible(show);
    player->m_landParticles1->setVisible(show);
    player->m_swingBurstParticles1->setVisible(show);
    player->m_swingBurstParticles2->setVisible(show);
}

class $modify(HackBGLParticles, GJBaseGameLayer) {
    void createPlayer() {
        GJBaseGameLayer::createPlayer();
        if (HackState::get().noVehicleParticles) {
            applyParticleVisibility(m_player1, true);
            applyParticleVisibility(m_player2, true);
        }
    }
};

// ═══════════════════════════════════════════════════════
//  MUTE LEVEL AUDIO ON DEATH  —       : MuteLevelAudioOnDeath.cpp
// ═══════════════════════════════════════════════════════

class $modify(HackMutePO, PlayerObject) {
    void playerDestroyed(bool p0) {
        auto* pl = PlayLayer::get();
        if (!pl || !HackState::get().muteLevelAudioOnDeath)
            return PlayerObject::playerDestroyed(p0);

        if (this != pl->m_player1 && this != pl->m_player2)
            return PlayerObject::playerDestroyed(p0);

        if (pl->m_isPracticeMode && !pl->m_practiceMusicSync)
            return PlayerObject::playerDestroyed(p0);

        auto* fmod = FMODAudioEngine::get();
        if (pl->m_isPlatformer) fmod->pauseAllMusic(true);
        else                    fmod->stopAllMusic(true);

        if (this == pl->m_player2 && pl->m_level->m_twoPlayerMode)
            return PlayerObject::playerDestroyed(p0);

        fmod->stopAllEffects();
        PlayerObject::playerDestroyed(p0);
    }
};

class $modify(HackMuteEGO, EffectGameObject) {
    void triggerObject(GJBaseGameLayer* p0, int p1, gd::vector<int> const* p2) {
        auto* pl = PlayLayer::get();
        if (!pl || !HackState::get().muteLevelAudioOnDeath)
            return EffectGameObject::triggerObject(p0, p1, p2);

        auto* player = pl->m_player1;
        if (!player || !player->m_isDead)
            return EffectGameObject::triggerObject(p0, p1, p2);

        int id = m_objectID;
        if (id == 3602 || id == 1934)
            EffectGameObject::triggerObject(p0, p1, p2);
    }
};

// ═══════════════════════════════════════════════════════
//  MUTE REWARDS SFX  —       : MuteRewardsSFX.cpp
// ═══════════════════════════════════════════════════════

static constexpr std::array<std::string_view, 4> s_badSFX = {
    "achievement_01.ogg", "magicExplosion.ogg", "gold02.ogg", "secretKey.ogg"
};

class $modify(HackFMOD, FMODAudioEngine) {
    int playEffect(gd::string path, float speed, float p2, float volume) {
        if (!HackState::get().muteRewardsSFX)
            return FMODAudioEngine::playEffect(path, speed, p2, volume);

        auto* pl = PlayLayer::get();
        if (!pl || !pl->m_player1->m_isDead || pl->m_isPaused)
            return FMODAudioEngine::playEffect(path, speed, p2, volume);

        if (std::ranges::find(s_badSFX, std::string_view(path)) == s_badSFX.end())
            return FMODAudioEngine::playEffect(path, speed, p2, volume);

        return 0;
    }
};

// ═══════════════════════════════════════════════════════
//  SHOW TRAJECTORY  —       : ShowTrajectory.cpp
//  checkCollisions is not in 2.2081 bindings.
//  Instead: sample player position every frame and draw
//  segments using a persistent CCDrawNode on the object layer.
//  HardStreak::addPoint is blocked during simulation to stop
//  the wave trail from rendering ghost points.
// ═══════════════════════════════════════════════════════

static cocos2d::CCDrawNode* s_pathTracerNode  = nullptr;
static bool                 s_pathTracerSimulating = false;

class $modify(HackPathTracerPL, PlayLayer) {

    bool init(GJGameLevel* level, bool p1, bool p2) {
        if (!PlayLayer::init(level, p1, p2)) return false;

        // Create a persistent draw node on the object layer
        s_pathTracerNode = cocos2d::CCDrawNode::create();
        s_pathTracerNode->setID("path-tracer-draw-node"_spr);
        s_pathTracerNode->setZOrder(999);
        if (m_objectLayer)
            m_objectLayer->addChild(s_pathTracerNode);

        return true;
    }

    void onQuit() {
        s_pathTracerNode = nullptr;
        s_pathTracerSimulating = false;
        PlayLayer::onQuit();
    }

    void postUpdate(float dt) {
        PlayLayer::postUpdate(dt);

        auto& st = HackState::get();
        if (!st.pathTracer || !s_pathTracerNode || !m_player1) return;
        if (m_isPaused || m_hasCompletedLevel) return;

        // Sample current player position and draw a dot
        // (builds up over the run to show the trajectory taken)
        auto pos = m_player1->getPosition();
        s_pathTracerNode->drawDot(pos, 1.5f, {0.f, 1.f, 0.2f, 0.7f});
    }

    void resetLevel() {
        PlayLayer::resetLevel();
        if (s_pathTracerNode) s_pathTracerNode->clear();
    }
};

class $modify(HackPathTracerBGL, GJBaseGameLayer) {

    void handleButton(bool down, int button, bool isPlayer1) {
        if (button == 1 && HackState::get().pathTracer) {
            if (isPlayer1) HackState::get().pathTracerP1 = down;
            else           HackState::get().pathTracerP2 = down;
        }
        GJBaseGameLayer::handleButton(down, button, isPlayer1);
    }
};

// Block HardStreak::addPoint during simulation so the wave
// trail doesn't draw phantom points while trajectory is shown
class $modify(HackTrajHardStreak, HardStreak) {
    void addPoint(cocos2d::CCPoint p0) {
        if (s_pathTracerSimulating) return;
        HardStreak::addPoint(p0);
    }
};
