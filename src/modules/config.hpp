#pragma once
#include <string>
#include <string_view>
#include <vector>
#include <functional>
#include <optional>
#include <Geode/Geode.hpp>
#include <matjson.hpp>

namespace lunar::config {

    // ── Storage access ────────────────────────────────────────────────────────
    matjson::Value& getStorage();
    matjson::Value& getTempStorage();

    // ── Lifecycle ─────────────────────────────────────────────────────────────
    void load();
    void save();

    // ── Profiles ──────────────────────────────────────────────────────────────
    void saveProfile(std::string_view profile);
    void loadProfile(std::string_view profile);
    void deleteProfile(std::string_view profile);
    std::vector<std::string> getProfiles();

    // ── Existence ─────────────────────────────────────────────────────────────
    bool has(std::string_view key);
    bool hasTemp(std::string_view key);
    void erase(std::string_view key);
    matjson::Type getType(std::string_view key);

    // ── Callbacks ─────────────────────────────────────────────────────────────
    void executeCallbacks(std::string_view key);
    void executeTempCallbacks(std::string_view key);
    void addDelegate(std::string_view key, std::function<void()> cb, bool first = false);
    void addTempDelegate(std::string_view key, std::function<void()> cb, bool first = false);

    // ── Generic get ───────────────────────────────────────────────────────────
    template<typename T>
    T get(std::string_view key, T const& def = T{}) {
        auto res = getStorage().get(key);
        if (!res) return def;
        auto v = res.unwrap().template as<T>();
        if (!v) return def;
        return std::move(v).unwrap();
    }

    template<typename T>
    T getTemp(std::string_view key, T const& def = T{}) {
        auto res = getTempStorage().get(key);
        if (!res) return def;
        auto v = res.unwrap().template as<T>();
        if (!v) return def;
        return std::move(v).unwrap();
    }

    // ── Generic set ───────────────────────────────────────────────────────────
    template<typename T>
    void set(std::string_view key, T value) {
        getStorage().set(key, std::move(value));
        executeCallbacks(key);
    }

    template<typename T>
    void setTemp(std::string_view key, T value) {
        getTempStorage().set(key, std::move(value));
        executeTempCallbacks(key);
    }

    template<typename T>
    void setIfEmpty(std::string_view key, T value) {
        if (!has(key)) set<T>(key, std::move(value));
    }


    namespace __impl {
        template<size_t N>
        struct Key {
            char data[N];
            explicit(false) constexpr Key(char const* s) { std::copy_n(s, N, data); }
            constexpr operator std::string_view() const { return {data, N}; }
        };
        template<size_t N> Key(char const (&)[N]) -> Key<N - 1>;
    }

    /// Cached get — avoids repeated JSON lookups for hot paths.
    template<__impl::Key key, typename T>
    T get(T const& def = T{}) {
        static T value = (addDelegate(key, [] { value = get<T>(key, T{}); }, true),
                          get<T>(key, def));
        return value;
    }

}
