#pragma once

#include "AnmManager.hpp"
#include "FrameScheduler.hpp"
#include "ScreenViews.hpp"

namespace th13 {

// th13's two @R surfaces and four embedded compositor VMs (0043cd40).
// Game sprites remain in the original +320/+16 intermediate coordinate domain.
// text.anm script84 performs the final source128 -> destination32 composition;
// neither gameplay coordinates nor the HUD are translated to approximate it.
class PlayfieldTargets final {
    FrameScheduler& scheduler;
    AnmManager& animations;
    AnmRenderer& renderer;
    ZunGraphics& graphics;
    ScreenViews& views;
    AnmResource* resource = nullptr;
    std::array<AnmVm, 4> compositors;
    std::array<u32, 2> targets{};
    struct Callback {
        FrameCallback frame;
        PlayfieldTargets* owner = nullptr;
        u32 index = 0;
    };
    std::array<Callback, 9> drawing;
    FrameCallback updating;
    bool fail(const std::string&);
    bool select(u32 target, bool clear);
    bool compose(u32 index);
    bool draw(u32 index);
    bool update();

public:
    std::string error;
    PlayfieldTargets(FrameScheduler&, AnmManager&, AnmRenderer&, ZunGraphics&, ScreenViews&);
    ~PlayfieldTargets();
    bool initialize(i32 text_bank);
    // 监督者四 VM 的层标（29/30/30/20；battle_oracle replay 窗全帧实测）——
    // 供 draw-v1 层字段对拍的测试只读访问，不参与绘制决策。
    i32 compositor_layer(u32 index) const { return compositors[index].visual.layer; }
};

}  // namespace th13
