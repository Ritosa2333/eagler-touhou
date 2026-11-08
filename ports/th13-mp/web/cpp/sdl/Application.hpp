// SDL 宿主层 PIMPL 外壳（th15 同构：initialize/step/error/projection）。
#pragma once

#include "../game/GameInput.hpp"
#include "../game/Types.hpp"

#include <memory>
#include <string>

namespace th13::sdl {

struct ApplicationState;

class Application {
    std::unique_ptr<ApplicationState> state;

public:
    Application();
    ~Application();
    bool initialize(bool audio_device = true);
    // 唯一 GameInput 实例（0x4e49f0 等价）：宿主每游戏帧 update 后传入，
    // 同一实例的 held/pressed/released/repeated 经 RunScene 直达 StageBattle。
    bool step(const GameInput& input, float fps = 60, bool lost_focus = false);
    const std::string& error() const;
    const i32* projection() const;
};

}  // namespace th13::sdl
