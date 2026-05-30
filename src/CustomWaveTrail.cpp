
#include <cmath>
#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>
#include <Geode/modify/HardStreak.hpp>
#include <Geode/modify/CCDrawNode.hpp>
#include "../HackState.hpp"
#include "../modules/hack.hpp"

using namespace geode::prelude;

// ═══════════════════════════════════════════════════════
//  CUSTOM WAVE TRAIL  — Eclipse: CustomWaveTrail.cpp
//  Controls wave trail size/color. Outline drawn via
//  CCDrawNode::drawPolygon hook.
// ═══════════════════════════════════════════════════════

static cocos2d::CCDrawNode* s_streak1 = nullptr;
static cocos2d::CCDrawNode* s_streak2 = nullptr;

// ── Reset streak pointers on level reset ─────────────────
class $modify(HackCWTPL, PlayLayer) {
    void resetLevel() {
        s_streak1 = nullptr;
        s_streak2 = nullptr;
        PlayLayer::resetLevel();
    }
};

// ── HardStreak: size + color ──────────────────────────────
class $modify(HackCWTStreak, HardStreak) {
    struct Fields { float lastPulseSize = 0.f; };

    void updateStroke(float dt) {
        auto& st = HackState::get();
        if (!st.customWaveTrail) return HardStreak::updateStroke(dt);

        // Color
        if (st.waveTrailRainbow) {
            // Simple HSV rainbow: cycle hue over time
            static float hue = 0.f;
            hue += dt * st.waveTrailRainbowSpeed * 60.f;
            if (hue >= 360.f) hue -= 360.f;
            // Convert HSV→RGB
            float h = hue / 60.f;
            int   i = (int)h;
            float f = h - i;
            float q = 1.f - f;
            float r=0,g=0,b=0;
            switch (i % 6) {
                case 0: r=1,g=f,b=0; break;
                case 1: r=q,g=1,b=0; break;
                case 2: r=0,g=1,b=f; break;
                case 3: r=0,g=q,b=1; break;
                case 4: r=f,g=0,b=1; break;
                case 5: r=1,g=0,b=q; break;
            }
            this->setColor({(GLubyte)(r*255),(GLubyte)(g*255),(GLubyte)(b*255)});
        } else if (st.waveTrailCustomColor) {
            this->setColor({
                (GLubyte)(st.waveTrailColor[0]*255),
                (GLubyte)(st.waveTrailColor[1]*255),
                (GLubyte)(st.waveTrailColor[2]*255)
            });
        }

        // Size
        if (st.waveTrailPulse) {
            auto& last = m_fields.self()->lastPulseSize;
            if (last != m_pulseSize) {
                m_pulseSize *= st.waveTrailScale;
                last = m_pulseSize;
            }
        } else {
            m_pulseSize = st.waveTrailScale;
        }

        // Track which draw node belongs to which player
        if (auto* pl = PlayLayer::get()) {
            if (pl->m_player1 && !s_streak1 && this == pl->m_player1->m_waveTrail) {
                s_streak1 = this;
            }
            if (pl->m_player2 && !s_streak2 && this == pl->m_player2->m_waveTrail) {
                s_streak2 = this;
            }
        }

        HardStreak::updateStroke(dt);
    }
};

// ── CCDrawNode: outline injection ─────────────────────────
class $modify(HackCWTDraw, cocos2d::CCDrawNode) {
    bool drawPolygon(cocos2d::CCPoint* verts, unsigned int count,
                     const cocos2d::ccColor4F& fill, float borderWidth,
                     const cocos2d::ccColor4F& border,
                     cocos2d::BorderAlignment alignment)
    {
        auto& st = HackState::get();
        bool isStreak = (this == s_streak1 || this == s_streak2);

        if (!st.customWaveTrail || !st.waveTrailOutline || !isStreak
            || (fill.r==1.f && fill.g==1.f && fill.b==1.f && fill.a!=1.f))
            return CCDrawNode::drawPolygon(verts, count, fill, borderWidth, border, alignment);

        float w = st.waveTrailOutlineStroke;
        cocos2d::ccColor4F outlineCol = {
            st.waveTrailOutlineColor[0],
            st.waveTrailOutlineColor[1],
            st.waveTrailOutlineColor[2],
            1.f
        };

        this->setBlendFunc(cocos2d::CCSprite::create()->getBlendFunc());
        this->setZOrder(-1);

        // Copy verts, offset top/bottom for outline
        std::vector<cocos2d::CCPoint> v(verts, verts + count);
        float offset = w + (count > 0 ? w / count : 0.f);
        if (count >= 4) {
            v[0].y -= offset; v[3].y -= offset;
            v[1].y += offset; v[2].y += offset;
            this->drawSegment(v[0], v[3], w, outlineCol);
            this->drawSegment(v[1], v[2], w, outlineCol);
        }

        return CCDrawNode::drawPolygon(verts, count, fill, borderWidth, border, alignment);
    }
};
