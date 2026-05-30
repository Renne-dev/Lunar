#include "Label.hpp"       // pulls in LabelVars + getLabelVars()
#include "LabelContainer.hpp"
#include "../HackState.hpp"
#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/UILayer.hpp>
#include <Geode/modify/PlayerObject.hpp>
#include <ctime>

using namespace geode::prelude;

namespace lunar::hacks::Labels {

    // ── CPS / click tracking ──────────────────────────────
    using ClickDeque = std::deque<int64_t>;

    struct ClickTracker {
        ClickDeque times;
        int        total   = 0;
        int        maxCps  = 0;

        void addClick() {
            auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            times.push_back(now);
            total++;
        }

        void cleanup() {
            auto now = std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            while (!times.empty() && now - times.front() > 1000)
                times.pop_front();
            maxCps = std::max((int)times.size(), maxCps);
        }

        void reset() { times.clear(); total = 0; maxCps = 0; }
        int cps()  const { return (int)times.size(); }
    };

    static ClickTracker s_p1, s_p2;

    class $modify(LabelsPO, PlayerObject) {
        bool pushButton(PlayerButton btn) {
            bool r = PlayerObject::pushButton(btn);
            if (btn == PlayerButton::Jump) {
                auto* bgl = GJBaseGameLayer::get();
                if (bgl) {
                    if (this == bgl->m_player1) s_p1.addClick();
                    else if (this == bgl->m_player2) s_p2.addClick();
                }
            }
            return r;
        }
    };

    // ── PlayLayer hooks ───────────────────────────────────
    class $modify(LabelsPL, PlayLayer) {
        void resetLevel() {
            PlayLayer::resetLevel();
            s_p1.reset();
            s_p2.reset();
        }
    };

    // ── Per-label definition ──────────────────────────────
    struct LabelDef {
        std::string name;
        std::string format;
        LabelsContainer::Alignment alignment = LabelsContainer::Alignment::TopLeft;
        bool visible = true;
    };

    // ── Label definitions — index matches HackState::kLabelCount ─────────────
    // 0=NoclipDeaths  1=NoclipAccuracy  2=FPS  3=CPS  4=IsPractice  5=Clock
    static const std::vector<LabelDef> DEFAULT_LABELS = {
        {"Noclip Deaths",    "Deaths: {noclipDeaths}",    LabelsContainer::Alignment::TopLeft},
        {"Noclip Accuracy",  "Accuracy: {noclipAccuracy}%", LabelsContainer::Alignment::TopLeft},
        {"FPS",              "{round(fps)} FPS",           LabelsContainer::Alignment::TopRight},
        {"CPS",              "{cps}/{clicks}",             LabelsContainer::Alignment::TopLeft},
        {"Practice Mode",    "{isPracticeMode}",           LabelsContainer::Alignment::TopCenter},
        {"Clock",            "{clock}",                    LabelsContainer::Alignment::TopRight},
    };

    // ── Active label list (user can toggle each) ──────────
    // Persisted via HackState::labelConfig (a bitfield of enabled indices
    // so we don't need complex serialisation beyond what Geode save gives us).
    // For simplicity each label has its own bool in HackState.

    // ── UILayer hook — builds the label overlay ───────────
    class $modify(LabelsUIL, UILayer) {
        struct Fields {
            CCNode*                           root       = nullptr;
            std::array<LabelsContainer*, 9>   containers = {};
            std::vector<SmartLabel*>          labels;
            bool                              inEditor   = false;
        };

        void buildLabels() {
            auto& st  = HackState::get();
            auto* f   = m_fields.self();

            f->root->removeAllChildren();
            f->labels.clear();

            // Create 9 alignment containers
            for (int i = 0; i < 9; i++) {
                auto align = static_cast<LabelsContainer::Alignment>(i);
                f->containers[i] = LabelsContainer::create(align);
                f->containers[i]->setID(fmt::format("label-container-{}", i).c_str());
                f->root->addChild(f->containers[i]);
            }

            // Add whichever labels the user has enabled
            for (int i = 0; i < (int)DEFAULT_LABELS.size(); i++) {
                if (!st.labelSettings[i].enabled) continue;
                auto& def = DEFAULT_LABELS[i];
                auto& cfg = st.labelSettings[i];

                auto* lbl = SmartLabel::create(def.format, "bigFont.fnt");
                if (!lbl) continue;
                lbl->setScale(0.35f);
                lbl->setID(fmt::format("smart-label-{}", i).c_str());
                lbl->setColor({(GLubyte)(cfg.r*255), (GLubyte)(cfg.g*255), (GLubyte)(cfg.b*255)});
                lbl->setOpacity((GLubyte)(cfg.opacity * 255));

                // Use user-chosen alignment, falling back to the default
                int ci = cfg.alignment;
                f->containers[ci]->addLabel(lbl);
                f->labels.push_back(lbl);
            }
        }

        void tickLabels(float) {
            auto& st = HackState::get();
            if (!st.labelsVisible) return;

            // ── Update variable bag ───────────────────────
            auto& v = getLabelVars();

            // FPS — CCDirector has no getFrameRate(); derive from delta time
            float dt = CCDirector::get()->getDeltaTime();
            v.fps = (dt > 0.f) ? (1.f / dt) : 0.f;

            // Gameplay vars (only meaningful inside PlayLayer)
            if (auto* pl = PlayLayer::get()) {
                v.attempt        = pl->m_attempts;
                v.progress       = pl->getCurrentPercent();
                v.isPracticeMode = pl->m_isPracticeMode;
                v.isTestMode     = pl->m_isTestMode;
                v.noclipAccuracy = st.noclipAccuracy;
                v.noclipDeaths   = st.noclipDeaths;

                // Format elapsed time
                float t = pl->m_timePlayed;
                int min = (int)(t / 60);
                float sec = t - min * 60;
                v.time = fmt::format("{:02}:{:05.2f}", min, sec);
            } else {
                v.progress = 0; v.attempt = 0;
                v.isPracticeMode = v.isTestMode = false;
                v.time = "";
            }

            // Wall clock
            auto now = std::time(nullptr);
            auto* tm = std::localtime(&now);
            char buf[16];
            std::strftime(buf, sizeof(buf), "%H:%M:%S", tm);
            v.clock = buf;

            // CPS tracking
            s_p1.cleanup(); s_p2.cleanup();
            v.cps     = s_p1.cps();
            v.clicks  = s_p1.total;
            v.maxCps  = s_p1.maxCps;

            // Tick all containers
            for (auto* c : m_fields.self()->containers)
                if (c) c->update();
        }

        bool init(GJBaseGameLayer* bgl) {
            if (!UILayer::init(bgl)) return false;

            auto* f        = m_fields.self();
            f->inEditor    = typeinfo_cast<LevelEditorLayer*>(bgl) != nullptr;
            f->root        = CCNode::create();
            f->root->setID("hack-labels-root"_spr);
            this->addChild(f->root, 1000);

            buildLabels();

            this->schedule(schedule_selector(LabelsUIL::tickLabels));
            return true;
        }
    };

} 
