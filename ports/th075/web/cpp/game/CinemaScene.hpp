#pragma once
#include "TitleScene.hpp"
#include "BattleProgress.hpp"
#include "FighterActor.hpp"
#include "ImageRenderer.hpp"
namespace th075 {
struct CinemaSignals:SceneSignals {
 virtual void render_cinema()=0;virtual void present_cinema()=0;virtual void select_music(i16)=0;
};
class CinemaScene {
public:
 u32 elapsed=0,startedAt=0;bool credits=false;
 void initialize(bool staffRoll,u32 start,u32 now){credits=staffRoll;startedAt=start;elapsed=now-start;}
 u16 advance(const std::array<i32,2>& confirm,u32 now,const FighterActor* player,u32 scoreToBeat,u8 route,i8& replayDisposition,CinemaSignals&);
 void render(ImageRenderer& images)const;
 void render_opening(ImageRenderer& images)const;
 void render_credits(ImageRenderer& images)const;
};
}
