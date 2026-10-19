#pragma once
#include "BattleScene.hpp"
#include "ZunGraphics.hpp"
namespace th075 {
struct BattleDrawingState {
    float cameraSpan=0,sceneFade=1;bool stageEnabled=true,recoveryMarkers=false;
    u8 foreground=0,fadeAlpha=0;u32 fadeColor=0;BattlePhase phase=BattlePhase::Intro;
    std::array<u8,2> fighterFade{};std::array<i8,2> portrait{-1,-1};
    bool firstPortraitFront=false,dialogueVisible=false;bool overlay=false,bonus=false,pause=false;
    static BattleDrawingState from(BattleScene&,bool displayStage,bool markers);
};
// Drawing dependencies are explicit scene/character operations. The ordering
// engine controls transforms, tint and depth independently of the GL backend.
struct BattleDrawCommands {
    virtual ~BattleDrawCommands()=default;
    virtual void background_back()=0;virtual void background_middle()=0;virtual void background_front()=0;
    virtual void object_shadows(u8 side)=0;virtual void fighter_shadow(u8 side)=0;
    virtual void owned_objects(u8 side,u8 layer)=0;virtual void trails(u8 side)=0;virtual void shared_effects(u8 layer)=0;
    virtual void fighter(u8 side)=0;virtual void character_special(u8 side)=0;virtual void recovery_markers()=0;
    virtual void status()=0;virtual void fade_portrait(u8 side)=0;virtual void spells()=0;
    virtual void victory()=0;virtual void story_portrait(u8 side)=0;virtual void dialogue()=0;virtual void story_caption()=0;
    virtual void battle_overlay()=0;virtual void bonus_banner()=0;virtual void pause_menu()=0;virtual void fps()=0;
};
void render_battle_layers(ZunGraphics&,const BattleDrawingState&,BattleDrawCommands&);
}
