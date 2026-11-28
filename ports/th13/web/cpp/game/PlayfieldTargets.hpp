#pragma once

#include "AnmManager.hpp"
#include "FrameScheduler.hpp"
#include "ScreenViews.hpp"

namespace th13 {

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
    i32 compositor_layer(u32 index) const { return compositors[index].visual.layer; }
};

}
