#pragma once
#include "GraphicsDevice.hpp"
#include "../game/GameDraw.hpp"
#include "../game/AsciiDrawSchedule.hpp"
#include "../game/ScreenTargets.hpp"
#include "SceneCaptureDevice.hpp"
namespace th17::sdl {
// All game drawings use the same semantic renderer and the original TH17
// callback ordering. Offscreen captures remain game-owned ANM resources.
class SceneDisplay {
    StageGameplay& scene;AnmManager& animations;AnmEnvironment& environment;GraphicsDevice& graphics;AnmRenderer& renderer;
    ScreenViews views;AsciiText captions;
    SceneCaptureDevice capture_device;std::unique_ptr<SceneCapture> screenshots;
    std::unique_ptr<AnmDrawSchedule> layers;std::unique_ptr<AsciiDrawSchedule> letters;
    std::unique_ptr<ScreenCompositor> compositor;std::unique_ptr<GameDraw> game;std::unique_ptr<ScreenTargets> targets;
    std::unique_ptr<StageDrawFrame> outgoing_background;
    FrameCallback fps_callback;
    bool fail(const std::string&);
public:
    std::string error;float presentation_fps=60;
    SceneDisplay(StageGameplay&,AnmManager&,AnmEnvironment&,GraphicsDevice&,AnmRenderer&);
    ~SceneDisplay();
    bool initialize(GameRun&,GameRecords&,SpellRecords&,StageDrawServices&,i32 text_bank,i32 ascii_bank,StageGameplay* previous=nullptr);
    void release_previous(){outgoing_background.reset();}
    bool capture(u32& handle,bool results);
    bool hud_text(const HudTextDraw& value){return captions.hud_text(value);}
    void pause(PauseDraw* p,ReplayCalendarServices* dates,std::array<u8,0xa0>* header=nullptr){if(game){game->pause=p;game->dates=dates;game->replay_header=header;}}
    bool draw();
};
}
