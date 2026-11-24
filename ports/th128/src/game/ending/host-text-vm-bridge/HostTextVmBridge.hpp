#pragma once

#include "../EndingSceneHost.hpp"
#include "animation/AnmLogic.hpp"
#include "game/NativeOrdinaryTerminalOwner.hpp"

#include <array>
#include <cstdint>
#include <string>
#include <vector>

namespace th128::ending::hosttext {

struct TextVmReceipt {
    int slot = 0;
    int script = 0;
    int sprite = -1;
    float x = 0;
    float y = 0;
    int layer = 0;
    float scaleX = 1;
    float scaleY = 1;
    bool visible = false;
    bool held = false;
    bool ended = false;
    bool alphaAvailable = false;
    float alpha = 0;
    int clock = 0;
};

struct BridgeState {
    SceneState scene;
    std::array<TextVmReceipt, 5> texts;
    std::size_t consumedOp3 = 0;
    int irqQueued = 0;
    bool normalModeClear = false;
    bool failed = false;
    std::string error;
};

class HostTextVmBridge {
public:
    bool load(int index, std::span<const std::uint8_t> message,
              std::span<const std::uint8_t> anim, std::span<const std::uint8_t> baseAnim,
              std::span<const std::uint8_t> textBank, std::string& error);
    bool loadOrdinary(const th128::record::NativeOrdinaryEndingPermit& permit,
              std::span<const std::uint8_t> message,std::span<const std::uint8_t> anim,
              std::span<const std::uint8_t> baseAnim,std::span<const std::uint8_t> textBank,std::string& error);
    bool configureServices(MediaHost media,FadeHost fade,std::string& error);
    bool configureOriginalInput(std::uint32_t sceneFlags20, std::function<bool(std::string&)> confirmSound, std::string& error);
    bool tick(message::Input input, std::string& error);
    bool tick(bool confirm, std::string& error);
    bool reset(std::string& error);
    const BridgeState& state() const { return state_; }
    int childSpawns() const { return childSpawns_; }

private:
    bool loadOwned(int index,std::span<const std::uint8_t> message,std::span<const std::uint8_t> anim,
                   std::span<const std::uint8_t> baseAnim,std::span<const std::uint8_t> textBank,
                   bool debugTainted,std::string& error);
    void collectNewOp3();
    void refresh();
    void dispose();

    SceneHost scene_;
    th128::anm::Registry registry_;
    std::array<th128::anm::VM, 5> texts_;
    std::array<th128::anm::Runtime, 5> runtimes_;
    BridgeState state_;
    bool loaded_ = false;
    bool cleaned_ = true;
    int childSpawns_ = 0;
};

}
