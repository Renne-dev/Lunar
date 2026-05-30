#pragma once
#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace lunar::hacks::Labels {

    class LabelsContainer;

    // ── Shared variable bag — populated by Labels.cpp, read by Label.cpp ─────
    struct LabelVars {
        float       fps            = 0;
        int         attempt        = 0;
        float       progress       = 0;
        float       noclipAccuracy = 100.f;
        int         noclipDeaths   = 0;
        int         cps            = 0;
        int         clicks         = 0;
        int         maxCps         = 0;
        bool        isPracticeMode = false;
        bool        isTestMode     = false;
        std::string time;   // elapsed mm:ss.ms
        std::string clock;  // wall-clock HH:MM:SS
    };

    // Accessor — storage lives in Label.cpp
    LabelVars& getLabelVars();

    // ── SmartLabel ────────────────────────────────────────────────────────────
    class SmartLabel : public CCLabelBMFont {
    public:
        static SmartLabel* create(std::string_view fmt, std::string const& font) {
            auto* r = new SmartLabel();
            if (r->initLabel(fmt, font)) { r->autorelease(); return r; }
            delete r; return nullptr;
        }

        bool initLabel(std::string_view fmt, std::string const& font);
        void setParentContainer(LabelsContainer* c) { m_container = c; }
        void setHeightMultiplier(float m)           { m_heightMul = m; }
        void update();

    private:
        std::string      m_format;
        LabelsContainer* m_container  = nullptr;
        float            m_heightMul  = 1.f;
        float            m_lastHeight = 0.f;
        bool             m_wasVisible = false;

        std::string evaluate() const;
    };

}
