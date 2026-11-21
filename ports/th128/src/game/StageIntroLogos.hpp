#pragma once
#include "animation/AnmScene.hpp"
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
namespace th128::stageintro {
struct DrawRecord {
    std::uint32_t id = 0, parent = 0;
    std::int32_t bank = -1, sprite = -1, entry = 0, layer = 0, mode = 0, anchorX = 0, anchorY = 0, blend = 0;
    std::uint32_t flags = 0, flags2 = 0, primary = 0, secondary = 0;
    float width = 0, height = 0, x = 0, y = 0, z = 0, rotation = 0, sx = 1, sy = 1;
    float u = 0, v = 0, uvScaleX = 1, uvScaleY = 1, u0 = 0, v0 = 0, u1 = 1, v1 = 1;
    float rx = 0, ry = 0, radius = 0, geometryWidth = 0;
    std::int32_t geometryCount = 0, script = -1, clock = 0, order = 0;
    std::uint32_t reserved[2]{};
};
static_assert(sizeof(DrawRecord) == 160);
struct CaptionInfo {
    bool active = false;
    int trackFile = -1, role = -1, atlasRow = -1, script = -1, sprite = -1, entry = -1, clock = 0;
    float x = 0, y = 0;
};
class Host {
  public:
    static constexpr int LogoBank = 30;
    Host();
    ~Host();
    Host(const Host&) = delete;
    Host& operator=(const Host&) = delete;
    bool load(const std::string& stage, const std::string& logo, const std::uint8_t* bytes,
              std::size_t length, std::string& error);
    void tick();
    void restart();
    void cleanup();
    bool active() const { return active_; }
    bool loaded() const { return loaded_; }
    bool introActive() const { return active_; }
    bool finished() const { return !active_; }
    bool firstStage() const { return firstStage_; }
    bool worldPause() const { return false; }
    int statusCounter() const { return frame_; }
    int frame() const { return frame_; }
    int rootClock();
    int nodes() const { return int(scene_.nodes().size()); }
    int scripts() const;
    int sprites() const;
    bool beginTrackCaption(int trackFile, int role, std::string& error);
    bool captionActive();
    CaptionInfo captionInfo();
    const std::string& error() const { return error_; }
    const std::string& stage() const { return stage_; }
    const std::string& logo() const { return logo_; }
    const std::vector<DrawRecord>& draw();
  private:
    anm::Scene scene_;
    std::string stage_, logo_, error_;
    bool loaded_ = false;
    bool active_ = false;
    bool firstStage_ = false;
    int stageId_ = 0;
    int frame_ = 0;
    int captionTrackFile_ = -1, captionRole_ = -1, captionRow_ = -1, captionScript_ = -1;
    bool captionActive_ = false;
    bool bindRoot();
    void capture();
    std::vector<DrawRecord> draws_;
};
}
