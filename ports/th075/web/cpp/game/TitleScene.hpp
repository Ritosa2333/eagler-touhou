#pragma once
#include "GameMode.hpp"
#include "FighterControls.hpp"
#include "BattleGeometry.hpp"
#include "ZunGraphics.hpp"
#include "QuadTransform.hpp"
namespace th075 {
enum class SceneTransition:u8 {Stay,CharacterSelect,ReplayMenu,MusicRoom,ResultList,Options,Demonstration,Opening,Exit};
struct GameSelection {
    GameMode mode=GameMode::Story;u8 activePlayer=0,titleSelection=0,demoCycle=0,background=0,difficulty=1,storyStage=0,startingStage=0;bool resumeStory=false;
    std::array<u8,2> character{},firstSpell{},secondSpell{},alternatePalette{};
};
struct SceneSignals {
    virtual ~SceneSignals()=default;
    virtual void sound(u16 id)=0;
    virtual void fade_music(i32 volume,i32 step,u32 interval,u32 completion)=0;
};
struct TitleInput {std::array<ControlCounts,2> players{};bool escape=false;};
class TitleScene {
public:
    u32 startedAt=0,elapsed=0;i32 escapeHeld=0,verticalEdge=0;
    u8 selected=0;std::array<u8,4> fade{255,5,5,5};
    std::array<u8,10> highlight{},offset{0,15,30,45,60,45,60,75,90,75};
    void initialize(u32 start,u32 now,GameSelection&);
    void leave(GameSelection& selection)const{selection.titleSelection=selected;}
    // The scene runner renders the current state before advancing selection/fades.
    SceneTransition advance(const TitleInput&,u32 now,GameSelection&,ReplayRandom&,SceneSignals&);
    void render(ZunGraphics&,const std::array<u32,4>& textures)const;
};
}
