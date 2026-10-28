// 跨场景显示拥有者（th15 SceneDisplay 裁剪：动画层调度 + 字幕通道；
// 离屏捕获/背景保留过渡（ScreenTargets/SceneCapture）延后，ui-base.md §三）。
#pragma once

#include "GraphicsDevice.hpp"
#include "../game/AnmDrawSchedule.hpp"
#include "../game/AsciiFrame.hpp"
#include "../game/ScreenViews.hpp"

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
    SceneDisplay(FrameScheduler&, AnmManager&, AnmEnvironment&, GraphicsDevice&, AnmRenderer&,
                 ScreenViews&, AsciiText&, i32 ascii_bank);
    ~SceneDisplay();
    bool initialize();
    void detach() noexcept;
    bool draw();
};

}  // namespace th13::sdl
