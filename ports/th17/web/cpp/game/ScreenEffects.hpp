#pragma once
#include "ScreenShake.hpp"
#include "FrameScheduler.hpp"
#include <list>
#include <memory>
#include <string>
namespace th17 {
// Each effect owns a scheduler callback. Registering every effect preserves
// native equal-priority order and the frame in which a new shake first runs.
class ScreenEffects {
    struct Entry {ScreenEffects* owner=nullptr;FrameCallback callback;std::unique_ptr<ScreenShake> shake;std::unique_ptr<ScreenNudge> nudge;bool retired=false;};
    FrameScheduler* scheduler;Rng& game_random;Rng& visual_random;std::list<std::unique_ptr<Entry>> entries;
    bool submit(std::unique_ptr<Entry>);static i32 update_entry(void*);static i32 retire_entry(void*);
public:
    ScreenShakeContext context;Vec2 world_offset{},screen_offset{};std::string error;
    ScreenEffects(FrameScheduler& s,Rng& game,Rng& visual):scheduler(&s),game_random(game),visual_random(visual){}
    ~ScreenEffects();bool attach(FrameScheduler&);void collect();void clear();
    void reset_offsets()noexcept{world_offset={};screen_offset={};}
    bool shake(ScreenShakeSpec);bool nudge(ScreenNudgeSpec);
    u32 count()const noexcept;bool draw_offset(Vec2& world,Vec2& screen)const noexcept{world=world_offset;screen=screen_offset;return true;}
};
}
