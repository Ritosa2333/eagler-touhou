// 换关资产预备（0xc 换关门内分摊）。
//
// 原版 0x42b990 在关首激活帧同步读入下一关的 ECL ANIM 表、bullet.anm、
// astral.anm 与关卡 logo，并逐张上传纹理。端口保留 0x42b990 的 bank 接管
// 与模板预热时点（预热的 RNG 抽取仍发生在激活帧），把其中与时点无关的
// 工作——文件读取、ANM 解析、纹理解码与上传——在关终门（HUD 120 帧 +
// 装载 30 帧）内逐帧推进：每帧只做一个文件或一个 bank 的上传。激活帧
// 到来时，begin_run 直接接管已就绪的 AnmResource。
//
// 门内任何一步失败即整体放弃：激活帧退回同步装载路径，由该路径产出原有
// 的错误消息。门被打断（回标题/Retry/Game Over）时 discard() 释放已上传
// 的纹理。
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

    // 关终门首调用：登记下一关的资产清单。stage 为 1..7 的关号。
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

    // 门内每帧一步。返回 false 表示预备已放弃（激活帧走同步路径）。
    bool step() {
        if (!active()) return false;
        if (!ecl_.empty()) {
            std::vector<u8> bytes;
            EclResource head;
            if (!reader_(ecl_, bytes) || !head.open(bytes.data(), u32(bytes.size())) ||
                head.animations.empty())
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

    // 取走已就绪的资源（未就绪的步骤在此同步补齐）。预备已放弃或无此项
    // 时返回空，调用方走同步装载。
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

    // 释放尚未被接管的纹理并清空清单。
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

}  // namespace th13::sdl
