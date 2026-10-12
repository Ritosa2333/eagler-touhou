#pragma once
#include "StageGameplay.hpp"
#include "StageDrawFrame.hpp"
#include "SpellDraw.hpp"
#include "AsciiText.hpp"
#include "RunPause.hpp"
namespace th15 {
#if TH15_MULTIPLAYER
class MultiplayerHud;
#endif
// Scene owners draw in the same stable scheduler as registered ANM layers and
// the shared compositor. Their individual gameplay/update ownership is retained.
class GameDraw final:private PopupDrawServices {
 struct Callback {FrameCallback frame;GameDraw* owner=nullptr;u32 index=0;};
#if TH15_MULTIPLAYER
 std::array<Callback,8> callbacks;std::unique_ptr<MultiplayerHud> multiplayer_hud;
 bool prepare_multiplayer_hud();
#else
 std::array<Callback,8> callbacks;
#endif
 StageGameplay& scene;SessionState& progress;RecordStore& records;AnmManager& animations;AnmRenderer& renderer;AsciiText& text;ReplayCalendarServices& dates;FrameScheduler& scheduler;
 bool fail(const std::string&);bool draw_glyph(AnmVm&)override;bool draw_text(const Vec3&,u32,const std::string&)override;bool run(u32);
public:
 static constexpr std::array<i32,8> priorities{12,28,32,34,36,45,46,69};
 StageDrawFrame background;RunPause* pause=nullptr;AnmVm* captured=nullptr;std::array<u8,0xa4>* replay_header=nullptr;
 SpellDrawContext spell_context;std::string error;
 GameDraw(StageGameplay&,SessionState&,RecordStore&,AnmManager&,AnmRenderer&,ZunGraphics&,ScreenViews&,AsciiText&,StageDrawServices&,ReplayCalendarServices&);~GameDraw();
 bool pass(u32 index){return index<callbacks.size()&&run(index);}void enable(bool value)noexcept{for(auto& c:callbacks)c.frame.enabled=value;
 }
 bool draw(){prepare_frame();if(scheduler.draw()<0)return fail(background.error.empty()?error:background.error);return background.error.empty()&&error.empty();}
 void prepare_frame()noexcept;
};
}
