#include "hack.hpp"
#include "config.hpp"
#include <ranges>
#include <fmt/format.h>

namespace lunar::hack {

    // ── Priority helpers ──────────────────────────────────────────────────────

    void safeHooksAll(geode::utils::StringMap<std::shared_ptr<geode::Hook>>& hooks) {
        for (auto& h : hooks | std::views::values)
            h->setPriority(SAFE_HOOK_PRIORITY);
    }

    void safeHooks(geode::utils::StringMap<std::shared_ptr<geode::Hook>>& hooks,
                   std::string_view className,
                   std::initializer_list<std::string_view> funcs)
    {
        for (auto fn : funcs) {
            auto name = fmt::format("{}::{}", className, fn);
            if (auto it = hooks.find(name); it != hooks.end())
                it->second->setPriority(SAFE_HOOK_PRIORITY);
            else
                geode::log::warn("[hack] safe hook not found: {}", name);
        }
    }

    void firstHooksAll(geode::utils::StringMap<std::shared_ptr<geode::Hook>>& hooks) {
        for (auto& h : hooks | std::views::values)
            h->setPriority(FIRST_HOOK_PRIORITY);
    }

    void firstHooks(geode::utils::StringMap<std::shared_ptr<geode::Hook>>& hooks,
                    std::string_view className,
                    std::initializer_list<std::string_view> funcs)
    {
        for (auto fn : funcs) {
            auto name = fmt::format("{}::{}", className, fn);
            if (auto it = hooks.find(name); it != hooks.end())
                it->second->setPriority(FIRST_HOOK_PRIORITY);
            else
                geode::log::warn("[hack] first hook not found: {}", name);
        }
    }

    // ── Toggle-delegate setup ─────────────────────────────────────────────────

    void setupTogglesAll(std::string_view id,
                         geode::utils::StringMap<std::shared_ptr<geode::Hook>>& hooks)
    {
        bool initial = config::get<bool>(std::string(id), false);
        std::vector<geode::Hook*> ptrs;
        ptrs.reserve(hooks.size());
        for (auto& h : hooks | std::views::values) {
            h->setAutoEnable(initial);
            ptrs.push_back(h.get());
        }
        config::addDelegate(id, [ptrs, id = std::string(id)] {
            bool val = config::get<bool>(id, false);
            for (auto* h : ptrs) (void)h->toggle(val);
        });
    }

    void setupToggles(std::string_view id,
                      geode::utils::StringMap<std::shared_ptr<geode::Hook>>& hooks,
                      std::string_view className,
                      std::initializer_list<std::string_view> funcs)
    {
        bool initial = config::get<bool>(std::string(id), false);
        std::vector<geode::Hook*> ptrs;
        for (auto fn : funcs) {
            auto name = fmt::format("{}::{}", className, fn);
            if (auto it = hooks.find(name); it != hooks.end()) {
                it->second->setAutoEnable(initial);
                ptrs.push_back(it->second.get());
            } else {
                geode::log::warn("[hack] toggle hook not found: {}", name);
            }
        }
        config::addDelegate(id, [ptrs, id = std::string(id)] {
            bool val = config::get<bool>(id, false);
            for (auto* h : ptrs) (void)h->toggle(val);
        });
    }

}
