#pragma once

#include "animation/AnmScene.hpp"
#include "game/MessageHost.hpp"
#include <functional>

#include <cstdint>
#include <span>
#include <string>
#include <vector>

namespace th128::ending {

struct SceneDraw {
    std::uint32_t id = 0;
    std::uint32_t owner = 0;
    int bank = -1;
    int script = -1;
    float x = 0, y = 0, z = 0;
    float alpha = 0;
    float width = 0, height = 0, sx = 1, sy = 1;
    float u0 = 0, v0 = 0, u1 = 1, v1 = 1;
    int anchorX = 0, anchorY = 0, textureEntry = -1;
    int layer = 0;
    int sprite = -1;
    bool visible = false;
    bool ended = true;
};

struct Op3Context {
    int time = 0;
    int ordinal = -1;
    int slot = -1;
    std::vector<std::uint8_t> source;
};

struct MediaRequest {std::uint32_t serial=0;int operation=0;std::string path;int durationFrames=0;};
using MediaHost=std::function<bool(const MediaRequest&,std::string&)>;
using FadeHost=std::function<bool(int,std::string&)>;
struct SceneState {
    bool loaded = false;
    bool finished = false;
    bool failed = false;
    bool waitingConfirm = false;
    bool debugTainted = false;
    bool normalModeClear = false;
    bool cleaned = false;
    bool fontBound = false;
    int index = -1;
    int clock = 0;
    int finishTime = -1;
    int effect14 = 0;
    int waitRemaining = 0, waitPrevious = -1;
    float waitAccumulator = 0;
    std::uint32_t heldShoot = 0, heldRapid = 0;
    std::uint32_t originalSceneFlags20 = 1;
    bool pressSeen = false;
    int confirmationSoundRequests = 0;
    std::uint32_t mediaSerial = 0;
    int textColor = 0xffffff;
    std::size_t executedOp3 = 0;
    std::string glyphText;
    std::string error;
    std::vector<std::string> events;
    std::vector<SceneDraw> draws;
    std::vector<Op3Context> op3;
};

class SceneHost {
public:
    bool load(int index, std::span<const std::uint8_t> message,
              std::span<const std::uint8_t> anm, std::span<const std::uint8_t> baseAnm,
              bool debugTainted, std::string& error);
    bool configureServices(MediaHost media, FadeHost fade, std::string& error);
    bool configureOriginalInput(std::uint32_t sceneFlags20, std::function<bool(std::string&)> confirmSound, std::string& error);
    bool tick(message::Input input, std::string& error);
    bool tick(bool confirm, std::string& error);
    bool switchTo(int index, std::span<const std::uint8_t> message,
                  std::span<const std::uint8_t> anm, std::span<const std::uint8_t> baseAnm,
                  std::string& error);
    void cleanup();
    const SceneState& state() const { return state_; }

private:
    struct Command {
        int time = 0;
        int opcode = 0;
        std::vector<std::uint8_t> bytes;
        std::string path;
    };
    bool parse(std::span<const std::uint8_t> message, std::span<const std::uint8_t> anm,
               std::span<const std::uint8_t> baseAnm, std::string& error);
    bool execute(const Command& command, message::Input input, std::string& error);
    bool fail(const std::string& reason, std::string& error);
    void snapshot();

    anm::Scene scene_;
    SceneState state_;
    std::vector<Command> commands_;
    std::size_t cursor_ = 0;
    bool boundBanks_ = false;
    std::uint32_t inputSceneFlags20_ = 1;
    std::function<bool(std::string&)> confirmSound_;
    MediaHost media_;
    FadeHost fade_;
};

}
