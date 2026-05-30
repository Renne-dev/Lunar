#pragma once
#include <memory>
#include <string_view>
#include <vector>
#include <Geode/Geode.hpp>

constexpr int32_t SAFE_HOOK_PRIORITY  =  0x500000;
constexpr int32_t FIRST_HOOK_PRIORITY = -0x500000;

namespace lunar::hack {

    void safeHooksAll(geode::utils::StringMap<std::shared_ptr<geode::Hook>>& hooks);
    void safeHooks(geode::utils::StringMap<std::shared_ptr<geode::Hook>>& hooks,
                   std::string_view className,
                   std::initializer_list<std::string_view> funcs);

    void firstHooksAll(geode::utils::StringMap<std::shared_ptr<geode::Hook>>& hooks);
    void firstHooks(geode::utils::StringMap<std::shared_ptr<geode::Hook>>& hooks,
                    std::string_view className,
                    std::initializer_list<std::string_view> funcs);

    void setupTogglesAll(std::string_view id,
                         geode::utils::StringMap<std::shared_ptr<geode::Hook>>& hooks);
    void setupToggles(std::string_view id,
                      geode::utils::StringMap<std::shared_ptr<geode::Hook>>& hooks,
                      std::string_view className,
                      std::initializer_list<std::string_view> funcs);

}

// ── Priority helpers ──────────────────────────────────────────────────────────
#define SAFE_SET_PRIO(name, prio)  (void) self.setHookPriority(name, prio)
#define SAFE_PRIORITY(name)        SAFE_SET_PRIO(name, SAFE_HOOK_PRIORITY)
#define FIRST_PRIORITY(name)       SAFE_SET_PRIO(name, FIRST_HOOK_PRIORITY)

#define SAFE_HOOKS(cls, ...)  lunar::hack::safeHooks(self.m_hooks,  #cls, { __VA_ARGS__ })
#define SAFE_HOOKS_ALL()      lunar::hack::safeHooksAll(self.m_hooks)
#define FIRST_HOOKS(cls, ...) lunar::hack::firstHooks(self.m_hooks, #cls, { __VA_ARGS__ })
#define FIRST_HOOKS_ALL()     lunar::hack::firstHooksAll(self.m_hooks)

// ── Toggle-delegate helpers ───────────────────────────────────────────────────
#define HOOKS_TOGGLE(id, cls, ...) \
    lunar::hack::setupToggles(id, self.m_hooks, #cls, { __VA_ARGS__ })
#define HOOKS_TOGGLE_ALL(id) \
    lunar::hack::setupTogglesAll(id, self.m_hooks)

// ── onModify shorthands ───────────────────────────────────────────────────────
#define ENABLE_SAFE_HOOKS_ALL() \
    static void onModify(auto& self) { SAFE_HOOKS_ALL(); }
#define ENABLE_FIRST_HOOKS_ALL() \
    static void onModify(auto& self) { FIRST_HOOKS_ALL(); }
#define ENABLE_SAFE_HOOKS(cls, ...) \
    static void onModify(auto& self) { SAFE_HOOKS(cls, __VA_ARGS__); }
#define ENABLE_FIRST_HOOKS(cls, ...) \
    static void onModify(auto& self) { FIRST_HOOKS(cls, __VA_ARGS__); }

// ADD_HOOKS_DELEGATE: all hooks in a $modify toggle with the given config key
#define ADD_HOOKS_DELEGATE(id) \
    static void onModify(auto& self) { HOOKS_TOGGLE_ALL(id); }

// ALL_DELEGATES_AND_SAFE_PRIO: toggle all + set all to safe priority
#define ALL_DELEGATES_AND_SAFE_PRIO(id) \
    static void onModify(auto& self) { SAFE_HOOKS_ALL(); HOOKS_TOGGLE_ALL(id); }

// DELEGATES_AND_SAFE_PRIO: toggle all + set specific to safe priority
#define DELEGATES_AND_SAFE_PRIO(id, cls, ...) \
    static void onModify(auto& self) { SAFE_HOOKS(cls, __VA_ARGS__); HOOKS_TOGGLE_ALL(id); }
