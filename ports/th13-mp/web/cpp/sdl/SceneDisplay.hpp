#pragma once

#include "../game/AnmDrawSchedule.hpp"
#include "../game/AsciiFrame.hpp"
#include "../game/ScreenViews.hpp"
#include "GraphicsDevice.hpp"

#include <functional>
#include <memory>

namespace th13::sdl {

class SceneDisplay {
    FrameScheduler& scheduler;
    AnmManager& animations;
    AnmEnvironment& environment;
    GraphicsDevice& graphics;
    AnmRenderer& renderer;
    ScreenViews& views;
    AsciiText& captions;
    i32 ascii_bank;
    std::unique_ptr<AnmDrawSchedule> animation_drawing;
    std::unique_ptr<AsciiFrame> caption_drawing;
    bool fail(const std::string&);

public:
    std::string error;
    bool playfield_active = false;
    SceneDisplay(FrameScheduler&, AnmManager&, AnmEnvironment&, GraphicsDevice&, AnmRenderer&, ScreenViews&, AsciiText&,
                 i32 ascii_bank);
    ~SceneDisplay();
    bool initialize();
    void detach() noexcept;
    bool prepare();
    bool present();
    bool draw();
    std::function<bool()> before_present;
};

}
