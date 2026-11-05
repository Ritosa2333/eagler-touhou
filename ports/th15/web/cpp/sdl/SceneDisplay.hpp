#pragma once
#include "GraphicsDevice.hpp"
#include "SceneCaptureDevice.hpp"
#include "../game/GameDraw.hpp"
#include "../game/AsciiFrame.hpp"
#include "../game/ScreenTargets.hpp"
namespace th15::sdl {
// The application keeps this display across title, gameplay and ending owners.
// Only the stage object drawing callbacks are replaced at a stage boundary.
class SceneDisplay {
 FrameScheduler& scheduler;AnmManager& animations;AnmEnvironment& environment;GraphicsDevice& graphics;AnmRenderer& renderer;ScreenViews& views;AsciiText& captions;i32 text_bank,ascii_bank;
 std::unique_ptr<AnmDrawSchedule> animation_drawing;std::unique_ptr<AsciiFrame> caption_drawing;std::unique_ptr<ScreenCompositor> compositor;std::unique_ptr<ScreenTargets> targets;
 SceneCaptureDevice capture_device;SceneCapture screenshots;bool fail(const std::string&);
 FrameCallback previous_update;StageScene* previous_scene=nullptr;
public:
 std::unique_ptr<GameDraw> game;std::unique_ptr<StageDrawFrame> previous_background;std::string error;
 SceneDisplay(FrameScheduler&,AnmManager&,AnmEnvironment&,GraphicsDevice&,AnmRenderer&,ScreenViews&,AsciiText&,i32 text_bank,i32 ascii_bank);
 ~SceneDisplay();
 bool initialize();bool attach(StageGameplay&,SessionState&,RecordStore&,StageDrawServices&,ReplayCalendarServices&,RunPause*,std::array<u8,0xa4>*);
 void detach();bool capture(u32& handle,bool results);bool draw();
 bool retain_background(StageGameplay&,StageDrawServices&);void release_previous()noexcept;
 ScreenCompositor* screen()const noexcept{return compositor.get();}
 ScreenTargets* screen_targets()const noexcept{return targets.get();}
};
}
