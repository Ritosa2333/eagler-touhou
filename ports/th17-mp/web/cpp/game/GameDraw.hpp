#pragma once
#include "StageGameplay.hpp"
#include "StageDrawFrame.hpp"
#include "SpellDraw.hpp"
#include "AsciiDrawSchedule.hpp"
#include "PauseDraw.hpp"
namespace th17 {
// Concrete game owners and transparent ANM layers share one stable scheduler.
class GameDraw final:private PopupDrawServices {
    struct Callback {FrameCallback frame;GameDraw* owner=nullptr;u32 index=0;};std::array<Callback,9> callbacks;
    StageGameplay& scene;GameRun& run;GameRecords& records;SpellRecords& spells;AnmManager& animations;AnmRenderer& renderer;ScreenViews& views;AsciiText& text;FrameScheduler& scheduler;
    bool fail(const std::string&);bool popup_glyph(AnmVm&)override;bool popup_text(const HudTextDraw&)override;bool pass(u32);
public:
    static constexpr std::array<i32,9> priorities{{12,29,33,36,38,41,48,49,75}};
    PauseDraw* pause=nullptr;ReplayCalendarServices* dates=nullptr;AnmVm* captured=nullptr;std::array<u8,0xa0>* replay_header=nullptr;
    StageDrawFrame background;std::string error;
    GameDraw(StageGameplay&,GameRun&,GameRecords&,SpellRecords&,AnmManager&,AnmRenderer&,ZunGraphics&,ScreenViews&,AsciiText&,StageDrawServices&);~GameDraw();
    bool draw();void enabled(bool active)noexcept{for(auto& c:callbacks)c.frame.enabled=active;}
};
}
