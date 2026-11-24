#pragma once

#include "../game/AnmResource.hpp"
#include "../game/EclResource.hpp"
#include "GraphicsDevice.hpp"

#include <cstdio>
#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace th13::sdl {

class StageAssetStager {
public:
    using Reader = std::function<bool(const std::string&, std::vector<u8>&)>;

    static constexpr const char* kStageKey = "stage";

    void begin(i32 stage, GraphicsDevice* graphics, Reader reader) {
        discard();
        stage_ = stage;
        graphics_ = graphics;
        reader_ = std::move(reader);
        char name[20];
        std::snprintf(name, sizeof name, "st%02d.ecl", stage);
        ecl_ = name;
        entries_.push_back(Entry{kStageKey, {}});
        entries_.push_back(Entry{"bullet.anm", {"bullet.anm"}});
        entries_.push_back(Entry{"astral.anm", {"astral.anm"}});
        std::snprintf(name, sizeof name, "st%02dlogo.anm", stage);
        entries_.push_back(Entry{name, {name}});
    }

    bool active() const { return stage_ > 0 && !failed_; }
    bool staged_for(i32 stage) const { return active() && stage_ == stage; }
    bool done() const {
        if (!active()) return true;
        if (!ecl_.empty()) return false;
        for (const auto& entry : entries_)
            if (!complete(entry)) return false;
        return true;
    }

    bool step() {
        if (!active()) return false;
        if (!ecl_.empty()) {
            std::vector<u8> bytes;
            EclResource head;
            if (!reader_(ecl_, bytes) || !head.open(bytes.data(), u32(bytes.size())) || head.animations.empty())
                return abandon();
            entries_.front().files = head.animations;
            ecl_.clear();
            return true;
        }
        for (auto& entry : entries_) {
            if (complete(entry)) continue;
            if (!entry.resource) entry.resource = std::make_unique<AnmResource>();
            if (entry.appended < entry.files.size()) {
                std::vector<u8> bytes;
                if (!reader_(entry.files[entry.appended], bytes) ||
                    !entry.resource->append(bytes.data(), u32(bytes.size())))
                    return abandon();
                ++entry.appended;
                return true;
            }
            if (!graphics_->preload(*entry.resource)) return abandon();
            entry.preloaded = true;
            return true;
        }
        return true;
    }

    std::unique_ptr<AnmResource> take(const std::string& key) {
        if (!active()) return nullptr;
        for (auto& entry : entries_) {
            if (entry.key != key) continue;
            while (!complete(entry))
                if (!step()) return nullptr;
            return std::move(entry.resource);
        }
        return nullptr;
    }

    void discard() {
        if (graphics_) {
            for (auto& entry : entries_)
                if (entry.resource) graphics_->unload(*entry.resource);
        }
        entries_.clear();
        ecl_.clear();
        stage_ = -1;
        failed_ = false;
        graphics_ = nullptr;
        reader_ = nullptr;
    }

private:
    struct Entry {
        std::string key;
        std::vector<std::string> files;
        std::unique_ptr<AnmResource> resource;
        size_t appended = 0;
        bool preloaded = false;
        Entry(std::string entry_key, std::vector<std::string> entry_files)
            : key(std::move(entry_key)), files(std::move(entry_files)) {}
    };

    static bool complete(const Entry& entry) { return entry.preloaded; }

    bool abandon() {
        for (auto& entry : entries_)
            if (entry.resource && graphics_) graphics_->unload(*entry.resource);
        entries_.clear();
        failed_ = true;
        return false;
    }

    i32 stage_ = -1;
    bool failed_ = false;
    std::string ecl_;
    GraphicsDevice* graphics_ = nullptr;
    Reader reader_;
    std::vector<Entry> entries_;
};

}
