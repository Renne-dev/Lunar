#include "config.hpp"
#include <fstream>
#include <Geode/loader/Log.hpp>
#include <Geode/loader/Mod.hpp>
#include <fmt/format.h>

namespace lunar::config {

    // ── Callback storage ──────────────────────────────────────────────────────

    using CallbackMap = std::unordered_map<std::string, std::vector<std::function<void()>>>;

    static CallbackMap& getCallbacks() {
        static CallbackMap m;
        return m;
    }

    static CallbackMap& getTempCallbacks() {
        static CallbackMap m;
        return m;
    }

    // ── JSON storage ──────────────────────────────────────────────────────────

    matjson::Value& getStorage() {
        static matjson::Value storage = [] {
            auto path = geode::Mod::get()->getSaveDir() / "config.json";
            auto res  = geode::utils::file::readJson(path);
            if (!res) {
                geode::log::warn("[config] Failed to read config: {}", res.unwrapErr());
                return matjson::Value::object();
            }
            geode::log::debug("[config] Loaded config.json");
            return std::move(res).unwrap();
        }();
        return storage;
    }

    matjson::Value& getTempStorage() {
        static matjson::Value tmp = matjson::Value::object();
        return tmp;
    }

    // ── Callbacks ─────────────────────────────────────────────────────────────

    void executeCallbacks(std::string_view key) {
        auto it = getCallbacks().find(std::string(key));
        if (it == getCallbacks().end()) return;
        for (auto& cb : it->second) cb();
    }

    void executeTempCallbacks(std::string_view key) {
        auto it = getTempCallbacks().find(std::string(key));
        if (it == getTempCallbacks().end()) return;
        for (auto& cb : it->second) cb();
    }

    void addDelegate(std::string_view key, std::function<void()> cb, bool first) {
        auto& vec = getCallbacks()[std::string(key)];
        if (first) vec.insert(vec.begin(), std::move(cb));
        else        vec.push_back(std::move(cb));
    }

    void addTempDelegate(std::string_view key, std::function<void()> cb, bool first) {
        auto& vec = getTempCallbacks()[std::string(key)];
        if (first) vec.insert(vec.begin(), std::move(cb));
        else        vec.push_back(std::move(cb));
    }

    // ── Lifecycle ─────────────────────────────────────────────────────────────

    void load() {
        // Storage initialises itself lazily on first getStorage() call.
        (void)getStorage();
    }

    void save() {
        auto path = geode::Mod::get()->getSaveDir() / "config.json";
        auto data = getStorage().dump();
        auto res  = geode::utils::file::writeStringSafe(path, data);
        if (res.isErr())
            geode::log::error("[config] Failed to save: {}", res.unwrapErr());
    }

    // ── Profiles ──────────────────────────────────────────────────────────────

    static std::filesystem::path profilesDir() {
        return geode::Mod::get()->getSaveDir() / "profiles";
    }

    static bool loadFile(std::filesystem::path const& path) {
        auto res = geode::utils::file::readJson(path);
        if (!res) {
            geode::log::warn("[config] Failed to read {}: {}", path.string(), res.unwrapErr());
            getStorage() = matjson::Value::object();
            return false;
        }
        getStorage() = std::move(res).unwrap();
        return true;
    }

    static void saveFile(std::filesystem::path const& path) {
        auto res = geode::utils::file::writeStringSafe(path, getStorage().dump());
        if (res.isErr())
            geode::log::error("[config] Failed to write {}: {}", path.string(), res.unwrapErr());
    }

    void saveProfile(std::string_view profile) {
        std::error_code ec;
        std::filesystem::create_directories(profilesDir(), ec);
        if (ec) { geode::log::error("[config] mkdir failed: {}", ec.message()); return; }
        saveFile(profilesDir() / fmt::format("{}.json", profile));
    }

    void loadProfile(std::string_view profile) {
        if (!loadFile(profilesDir() / fmt::format("{}.json", profile))) {
            geode::log::warn("[config] Profile '{}' not found, using defaults.", profile);
            load();
        }
    }

    void deleteProfile(std::string_view profile) {
        std::error_code ec;
        auto path = profilesDir() / fmt::format("{}.json", profile);
        if (std::filesystem::exists(path, ec))
            std::filesystem::remove(path, ec);
        if (ec) geode::log::error("[config] delete profile failed: {}", ec.message());
    }

    std::vector<std::string> getProfiles() {
        std::vector<std::string> out;
        std::error_code ec;
        if (!std::filesystem::exists(profilesDir(), ec)) return out;
        for (auto& e : std::filesystem::directory_iterator(profilesDir(), ec))
            if (e.is_regular_file() && e.path().extension() == ".json")
                out.push_back(geode::utils::string::pathToString(e.path().stem()));
        return out;
    }

    // ── Misc ──────────────────────────────────────────────────────────────────

    bool has(std::string_view key)     { return getStorage().contains(key);     }
    bool hasTemp(std::string_view key) { return getTempStorage().contains(key); }
    void erase(std::string_view key)   { getStorage().erase(key);               }

    matjson::Type getType(std::string_view key) {
        if (!has(key)) return matjson::Type::Null;
        return getStorage()[key].type();
    }

}
