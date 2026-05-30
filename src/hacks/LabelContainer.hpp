#pragma once
#include <Geode/Geode.hpp>

namespace lunar::hacks::Labels {

    class SmartLabel;

    class LabelsContainer : public cocos2d::CCNode {
    public:
        enum class Alignment {
            TopLeft = 0, TopCenter = 1, TopRight = 2,
            CenterLeft = 3, Center = 4, CenterRight = 5,
            BottomLeft = 6, BottomCenter = 7, BottomRight = 8
        };

        static LabelsContainer* create(Alignment alignment = Alignment::TopLeft) {
            auto* ret = new LabelsContainer();
            if (ret->init(alignment)) { ret->autorelease(); return ret; }
            delete ret; return nullptr;
        }

        bool init(Alignment alignment);
        void addLabel(SmartLabel* label);
        void removeLabel(SmartLabel* label);
        void update();
        void invalidate() { m_dirty = true; }
        void setAlignment(Alignment a) { m_alignment = a; updatePosition(); updateLayout(false); }
        [[nodiscard]] Alignment getAlignment() const { return m_alignment; }
        void updatePosition();

    private:
        void recalculateLayout();
        std::vector<SmartLabel*> m_labels;
        Alignment m_alignment = Alignment::TopLeft;
        bool m_dirty = false;
    };

}
