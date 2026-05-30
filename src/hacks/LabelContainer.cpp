#include "LabelContainer.hpp"
#include "Label.hpp"
#include <Geode/Geode.hpp>

using namespace geode::prelude;

namespace lunar::hacks::Labels {

    bool LabelsContainer::init(Alignment alignment) {
        if (!CCNode::init()) return false;
        m_alignment = alignment;
        updatePosition();

        auto* layout = geode::AxisLayout::create(geode::Axis::Column)
            ->setAxisReverse(true)
            ->setAutoScale(false)
            ->setGrowCrossAxis(false)
            ->setCrossAxisOverflow(true)
            ->setGap(0.f);
        layout->ignoreInvisibleChildren(true);

#define SET_ALIGN(ax, cx, cxl) \
    layout->setAxisAlignment(geode::AxisAlignment::ax); \
    layout->setCrossAxisAlignment(geode::AxisAlignment::cx); \
    layout->setCrossAxisLineAlignment(geode::AxisAlignment::cxl);

        switch (m_alignment) {
            case Alignment::TopLeft:     SET_ALIGN(End,    Start,  Start);  break;
            case Alignment::TopCenter:   SET_ALIGN(End,    Center, Center); break;
            case Alignment::TopRight:    SET_ALIGN(End,    End,    End);    break;
            case Alignment::CenterLeft:  SET_ALIGN(Center, Start,  Start);  break;
            case Alignment::Center:      SET_ALIGN(Center, Center, Center); break;
            case Alignment::CenterRight: SET_ALIGN(Center, End,    End);    break;
            case Alignment::BottomLeft:
                layout->setAxisReverse(false);
                SET_ALIGN(Start, Start, Start); break;
            case Alignment::BottomCenter:
                layout->setAxisReverse(false);
                SET_ALIGN(Start, Center, Center); break;
            case Alignment::BottomRight:
                layout->setAxisReverse(false);
                SET_ALIGN(Start, End, End); break;
        }
#undef SET_ALIGN

        this->setLayout(layout, false);
        recalculateLayout();
        return true;
    }

    void LabelsContainer::updatePosition() {
        auto ws      = CCDirector::get()->getWinSize();
        float pad    = 3.f;
        setContentSize({ws.width - pad * 2, ws.height - pad * 2});

        switch (m_alignment) {
            case Alignment::TopLeft:     setPosition(pad,              ws.height - pad); setAnchorPoint({0,   1}); break;
            case Alignment::TopCenter:   setPosition(ws.width / 2,     ws.height - pad); setAnchorPoint({0.5, 1}); break;
            case Alignment::TopRight:    setPosition(ws.width - pad,   ws.height - pad); setAnchorPoint({1,   1}); break;
            case Alignment::CenterLeft:  setPosition(pad,              ws.height / 2);   setAnchorPoint({0,   0.5}); break;
            case Alignment::Center:      setPosition(ws.width / 2,     ws.height / 2);   setAnchorPoint({0.5, 0.5}); break;
            case Alignment::CenterRight: setPosition(ws.width - pad,   ws.height / 2);   setAnchorPoint({1,   0.5}); break;
            case Alignment::BottomLeft:  setPosition(pad,              pad);             setAnchorPoint({0,   0}); break;
            case Alignment::BottomCenter:setPosition(ws.width / 2,     pad);             setAnchorPoint({0.5, 0}); break;
            case Alignment::BottomRight: setPosition(ws.width - pad,   pad);             setAnchorPoint({1,   0}); break;
        }
        recalculateLayout();
    }

    void LabelsContainer::recalculateLayout() {
        updateLayout(false);
    }

    void LabelsContainer::addLabel(SmartLabel* label) {
        label->setParentContainer(this);
        m_labels.push_back(label);

        switch (m_alignment) {
            case Alignment::TopLeft:     label->setAnchorPoint({0,   1}); break;
            case Alignment::TopCenter:   label->setAnchorPoint({0.5, 1}); break;
            case Alignment::TopRight:    label->setAnchorPoint({1,   1}); break;
            case Alignment::CenterLeft:  label->setAnchorPoint({0,   0.5}); break;
            case Alignment::Center:      label->setAnchorPoint({0.5, 0.5}); break;
            case Alignment::CenterRight: label->setAnchorPoint({1,   0.5}); break;
            case Alignment::BottomLeft:  label->setAnchorPoint({0,   0}); break;
            case Alignment::BottomCenter:label->setAnchorPoint({0.5, 0}); break;
            case Alignment::BottomRight: label->setAnchorPoint({1,   0}); break;
        }
        addChild(label);
    }

    void LabelsContainer::removeLabel(SmartLabel* label) {
        auto it = std::ranges::find(m_labels, label);
        if (it != m_labels.end()) { m_labels.erase(it); removeChild(label); }
    }

    void LabelsContainer::update() {
        if (!isVisible()) return;
        for (auto* lbl : m_labels) lbl->update();
        if (m_dirty) { recalculateLayout(); m_dirty = false; }
    }

}
