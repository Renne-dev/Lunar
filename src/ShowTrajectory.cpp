//100% is chatgpt
#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/GJBaseGameLayer.hpp>
#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <Geode/modify/GameObject.hpp>
#include <Geode/modify/EffectGameObject.hpp>
#include <Geode/modify/LevelEditorLayer.hpp>
#include "../HackState.hpp"
#include "../modules/hack.hpp"

using namespace geode::prelude;

static cocos2d::CCDrawNode* getTrajectoryNode() {
    static cocos2d::CCDrawNode* s_node = nullptr;
    if (!s_node) {
        s_node = cocos2d::CCDrawNode::create();
        s_node->retain();
        s_node->m_bUseArea = false;
        s_node->setID("show-trajectory-draw-node"_spr);
    }
    return s_node;
}

// ── Simulation state ──────────────────────────────────────
struct TrajectoryState {
    PlayerObject* simPlayer1 = nullptr;
    PlayerObject* simPlayer2 = nullptr;
    bool simulating   = false;
    bool simDead      = false;
    bool p1Pressed    = false;
    bool p2Pressed    = false;
    float frameDt     = 1.f / 240.f;
};

static TrajectoryState s_traj;

static void resetCollisionLog(PlayerObject* p) {
    p->m_collisionLogTop->removeAllObjects();
    p->m_collisionLogBottom->removeAllObjects();
    p->m_collisionLogLeft->removeAllObjects();
    p->m_collisionLogRight->removeAllObjects();
}

static void drawPlayerHitbox(PlayerObject* p) {
    auto* node = getTrajectoryNode();
    auto r1 = p->getObjectRect();
    auto r2 = p->getObjectRect(0.25f, 0.25f);

    auto drawRect = [&](cocos2d::CCRect const& r, cocos2d::ccColor4F c) {
        std::array<cocos2d::CCPoint,4> v = {{
            {r.getMinX(),r.getMinY()},{r.getMinX(),r.getMaxY()},
            {r.getMaxX(),r.getMaxY()},{r.getMaxX(),r.getMinY()}
        }};
        node->drawPolygon(v.data(), 4, {c.r,c.g,c.b,0}, 0.25f, c);
    };
    drawRect(r1, {1,1,0,1});
    drawRect(r2, {0,1,0.2f,1});
}

static void iteratePlayer(PlayLayer* pl, PlayerObject* sim,
                           PlayerObject* base, bool isP2) {
    sim->copyAttributes(base);
    sim->m_gravityMod  = base->m_gravityMod;
    sim->m_isOnGround  = base->m_isOnGround;
    sim->setVisible(false);
    s_traj.simDead = false;

    bool& pressed = isP2 ? s_traj.p2Pressed : s_traj.p1Pressed;
    if (pressed)
        sim->pushButton(PlayerButton::Jump);

    int iterations = HackState::get().trajectoryIterations;
    bool actionDone = false;

    for (int i = 0; i < iterations; i++) {
        cocos2d::CCPoint prevPos = sim->getPosition();
        resetCollisionLog(sim);

        pl->checkCollisions(sim, s_traj.frameDt, false);
        if (s_traj.simDead) break;

        if (!actionDone && !pressed) {
            actionDone = true;
            sim->pushButton(PlayerButton::Jump);
        }

        sim->update(s_traj.frameDt);

        getTrajectoryNode()->drawSegment(
            prevPos, sim->getPosition(), 0.65f,
            pressed ? cocos2d::ccColor4F{0,1,0.1f,1} : cocos2d::ccColor4F{1,0,0.1f,1}
        );
    }

    drawPlayerHitbox(sim);
}

static void runSimulation() {
    auto* pl = PlayLayer::get();
    if (!pl || !s_traj.simPlayer1 || !s_traj.simPlayer2) return;

    s_traj.simulating = true;
    getTrajectoryNode()->clear();

    iteratePlayer(pl, s_traj.simPlayer1, pl->m_player1, false);
    if (pl->m_gameState.m_isDualMode)
        iteratePlayer(pl, s_traj.simPlayer2, pl->m_player2, true);

    s_traj.simulating = false;
}

static PlayerObject* makeSimPlayer(PlayLayer* pl) {
    auto* p = PlayerObject::create(1, 1, pl, pl, true);
    p->setPosition({0, 105});
    p->setVisible(false);
    p->setID("trajectory-sim-player"_spr);
    pl->m_objectLayer->addChild(p);
    return p;
}

// ── PlayLayer hooks ───────────────────────────────────────
class $modify(HackTrajPL, PlayLayer) {
    bool init(GJGameLevel* level, bool p1, bool p2) {
        if (!PlayLayer::init(level, p1, p2)) return false;

        s_traj.simPlayer1 = makeSimPlayer(this);
        s_traj.simPlayer2 = makeSimPlayer(this);

        if (m_debugDrawNode && m_debugDrawNode->getParent()) {
            auto* node = getTrajectoryNode();
            m_debugDrawNode->getParent()->addChild(node);
            node->setZOrder(m_debugDrawNode->getZOrder());
        }
        return true;
    }

    void onQuit() {
        PlayLayer::onQuit();
        s_traj.simPlayer1 = nullptr;
        s_traj.simPlayer2 = nullptr;
        getTrajectoryNode()->clear();
    }

    void destroyPlayer(PlayerObject* player, GameObject* obj) override {
        // Block sim-player deaths from affecting real gameplay
        if (s_traj.simulating && (player == s_traj.simPlayer1 || player == s_traj.simPlayer2)) {
            s_traj.simDead = true;
            return;
        }
        PlayLayer::destroyPlayer(player, obj);
    }

    void playEndAnimationToPos(cocos2d::CCPoint p) {
        if (s_traj.simulating) return;
        PlayLayer::playEndAnimationToPos(p);
    }
};

// ── LevelEditorLayer: clean up on editor load ─────────────
class $modify(HackTrajLEL, LevelEditorLayer) {
    bool init(GJGameLevel* level, bool unk) {
        bool r = LevelEditorLayer::init(level, unk);
        s_traj.simPlayer1 = nullptr;
        s_traj.simPlayer2 = nullptr;
        return r;
    }
};

// ── GJBaseGameLayer: run simulation each camera update ────
class $modify(HackTrajBGL, GJBaseGameLayer) {
    void handleButton(bool down, int button, bool isP1) {
        if (button == 1) {
            if (isP1) s_traj.p1Pressed = down;
            else      s_traj.p2Pressed = down;
        }
        GJBaseGameLayer::handleButton(down, button, isP1);
    }

    void updateCamera(float dt) {
        if (HackState::get().showTrajectory && PlayLayer::get())
            runSimulation();
        GJBaseGameLayer::updateCamera(dt);
    }

    void flipGravity(PlayerObject* p, bool f, bool b) {
        if (s_traj.simulating) return;
        GJBaseGameLayer::flipGravity(p, f, b);
    }

    bool canBeActivatedByPlayer(PlayerObject* p, EffectGameObject* obj) {
        if (s_traj.simulating) return false;
        return GJBaseGameLayer::canBeActivatedByPlayer(p, obj);
    }

    void playerTouchedRing(PlayerObject* p, RingObject* r) {
        if (s_traj.simulating) return;
        GJBaseGameLayer::playerTouchedRing(p, r);
    }

    void collisionCheckObjects(PlayerObject* player,
                                gd::vector<GameObject*>* vec,
                                int count, float dt)
    {
        if (s_traj.simulating) {
            // Only check solid/hazard/slope objects during simulation
            gd::vector<GameObject*> filtered;
            for (int i = 0; i < count; i++) {
                auto* obj = vec->at(i);
                auto t = obj->m_objectType;
                if (t == GameObjectType::Solid   || t == GameObjectType::Hazard ||
                    t == GameObjectType::AnimatedHazard || t == GameObjectType::Slope)
                    filtered.push_back(obj);
            }
            GJBaseGameLayer::collisionCheckObjects(player, &filtered, (int)filtered.size(), dt);
            return;
        }
        GJBaseGameLayer::collisionCheckObjects(player, vec, count, dt);
    }
};

// ── HardStreak: suppress trail during simulation ──────────
class $modify(HackTrajHS, HardStreak) {
    void addPoint(cocos2d::CCPoint p) {
        if (s_traj.simulating) return;
        HardStreak::addPoint(p);
    }
};

// ── PlayerObject: frame delta + suppress side effects ─────
class $modify(HackTrajPO, PlayerObject) {
    void update(float dt) override {
        PlayerObject::update(dt);
        if (PlayLayer::get() && !s_traj.simulating)
            s_traj.frameDt = dt / PlayLayer::get()->m_gameState.m_timeWarp;
    }

    void incrementJumps() {
        if (s_traj.simulating) return;
        PlayerObject::incrementJumps();
    }

    void playSpiderDashEffect(cocos2d::CCPoint from, cocos2d::CCPoint to) {
        if (s_traj.simulating) return;
        PlayerObject::playSpiderDashEffect(from, to);
    }

    void ringJump(RingObject* ring, bool b) {
        if (s_traj.simulating) return;
        PlayerObject::ringJump(ring, b);
    }
};

// ── EffectGameObject: suppress triggers during simulation ──
class $modify(HackTrajEGO, EffectGameObject) {
    void triggerObject(GJBaseGameLayer* bgl, int p1, const gd::vector<int>* p2) override {
        if (s_traj.simulating) return;
        EffectGameObject::triggerObject(bgl, p1, p2);
    }
};

// ── GameObject: suppress shine during simulation ───────────
class $modify(HackTrajGO, GameObject) {
    void playShineEffect() {
        if (s_traj.simulating) return;
        GameObject::playShineEffect();
    }
};
// its a bit werid sometimess
