#include "Label.hpp"
#include "LabelContainer.hpp"
#include "../HackState.hpp"
#include <Geode/Geode.hpp>
#include <ctime>

using namespace geode::prelude;

namespace lunar::hacks::Labels {

    // Storage for the shared variable bag
    static LabelVars s_vars;
    LabelVars& getLabelVars() { return s_vars; }

    // ── Simple {key} substitution ─────────────────────────────────────────────
    std::string SmartLabel::evaluate() const {
        auto& v = s_vars;
        auto& st = HackState::get();

        // Build substitution map — plain {key} tokens
        std::unordered_map<std::string, std::string> subs;
        subs["fps"]             = fmt::format("{:.0f}", v.fps);
        subs["round(fps)"]      = fmt::format("{}", (int)std::round(v.fps));
        subs["attempt"]         = fmt::format("{}", v.attempt);
        subs["progress"]        = fmt::format("{:.1f}", v.progress);
        subs["noclipAccuracy"]  = fmt::format("{:.1f}", v.noclipAccuracy);
        subs["noclipDeaths"]    = fmt::format("{}", v.noclipDeaths);
        subs["cps"]             = fmt::format("{}", v.cps);
        subs["clicks"]          = fmt::format("{}", v.clicks);
        subs["maxCps"]          = fmt::format("{}", v.maxCps);
        subs["isPracticeMode"]  = v.isPracticeMode ? "Practice" : "";
        subs["isTestMode"]      = v.isTestMode     ? "Testmode" : "";
        subs["time"]            = v.time;
        subs["clock"]           = v.clock;

        // Replace {key} tokens
        std::string result = m_format;
        size_t pos = 0;
        while ((pos = result.find('{', pos)) != std::string::npos) {
            size_t end = result.find('}', pos);
            if (end == std::string::npos) break;
            std::string key = result.substr(pos + 1, end - pos - 1);
            auto it = subs.find(key);
            if (it != subs.end()) {
                result.replace(pos, end - pos + 1, it->second);
                pos += it->second.size();
            } else {
                pos = end + 1;
            }
        }
        return result;
    }

    bool SmartLabel::initLabel(std::string_view fmt, std::string const& font) {
        if (!CCLabelBMFont::initWithString("", font.c_str())) return false;
        m_format = std::string(fmt);
        return true;
    }

    void SmartLabel::update() {
        bool vis = isVisible();

        // Notify container if visibility changed
        if (vis != m_wasVisible) {
            m_wasVisible = vis;
            if (m_container) m_container->invalidate();
        }

        if (!vis) return;

        setString(evaluate().c_str());

        // Notify container if height changed (triggers layout recalc)
        float h = getContentSize().height * m_fScaleY * m_heightMul;
        if (h != m_lastHeight) {
            m_lastHeight = h;
            if (m_container) m_container->invalidate();
        }
    }

}
