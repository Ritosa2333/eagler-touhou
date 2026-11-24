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
    bool step(const GameInput& input, float fps = 60, bool lost_focus = false);
    void set_practice(bool enabled, i32 section, i32 life, i32 bomb, i32 power, i32 trance);
    i32 practice_stage() const;
    const std::string& error() const;
    const i32* projection() const;
};

}
