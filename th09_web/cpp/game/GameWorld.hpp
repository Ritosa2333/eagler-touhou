#pragma once
#include "GameBattle.hpp"
#include "HeadsUpDisplay.hpp"
#include "MatchScene.hpp"
#include "Background.hpp"
#include "ScreenEffects.hpp"
namespace th09 {
// The platform owns pictures, sound and transitions; the session owns all
// simulation objects and their deterministic update/draw order.
struct WorldPresentation {
    virtual ~WorldPresentation()=default;
    virtual void begin_field(i32 side)=0;
    virtual void animation(AnmVm&,BattleSprite)=0;
    virtual void colored(const AnmVm*,const AttackColorVertex*,u32,BattleGeometry,bool additive)=0;
    virtual void textured(const AnmVm&,const AttackTextureVertex*,u32,BattleGeometry)=0;
    virtual void text(AnmVm&,const char* shift_jis,u32 color,u32 shadow)=0;
    virtual void sound(i32 id,i32 pan)=0;
    virtual void positioned_sound(i32 side,i32 id,float x)=0;
    virtual void music(i32 track)=0;
    virtual void fade_music()=0;
    virtual void background(Background&,const Background& first,i32 side,bool overlay)=0;
    virtual void score_popup(i32 side,const Vec3&,i32 value,u32 color)=0;
    virtual void draw_score_popups(){}
    virtual void draw_overlay(){}
    virtual void number(const Vec3&,i32 width,i32 value)=0;
    virtual void clock(const Vec3&,i32 stage,i32 minutes,i32 seconds)=0;
    virtual void shake(i32 side,float x,float y)=0;
    virtual void rectangle(float left,float top,float right,float bottom,u32 color,bool full_view)=0;
    virtual void encountered(i32 character)=0;
    virtual void defeated(i32 character)=0;
};
struct WorldConfiguration {
    StageSelectionState selection;
    i32 controllers[2]{0,1},health[2]{10,10},starting_extra_lives=0;
    bool automatic_focus[2]{},alternate[2]{};
    u32 game_flags=4;
};
class GameWorld {
    struct Services;std::unique_ptr<Services> services;
    EclWorldState& world;GameResources& resources;AnmExecutor& animations;WorldPresentation& output;
    bool fail(const char*);void sync();void reset_round();
public:
    MatchRules rules;ScreenEffects screen_effects;WorldConfiguration configuration;
    std::unique_ptr<GameBattle> battle;
    std::array<std::unique_ptr<Background>,2> backgrounds;
    std::array<std::unique_ptr<HeadsUpDisplay>,2> huds;
    MessageResource messages[2];std::unique_ptr<Dialogue> dialogue;std::unique_ptr<MatchScene> scene;
    GameInput combined_input;FrameTiming timing;
    struct BossMarker {Vec3 position;i32 state=0;};std::array<BossMarker,4> markers;
    i32 music_track=-1;bool ready=false,paused=false,transition_pending=false;
    SceneTransition transition=SceneTransition::title;
    std::string error;
    GameWorld(EclWorldState&,GameResources&,AnmExecutor&,WorldPresentation&);
    ~GameWorld();
    bool initialize(const WorldConfiguration&,bool choose_stage=true,const std::array<ScoreCounter,2>* scores=nullptr);
    bool update(u16 left,u16 right,u16 menu);
    bool update_prepared(const GameInput (&inputs)[3]);
    bool simulate();
    void continue_game();
    void copy_inputs(GameInput (&inputs)[3])const;
    void draw();
    MatchSceneServices& scene_services();
};
}
