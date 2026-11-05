#pragma once
#include "BattleHud.hpp"
namespace th075 {
void render_story_portrait(ImageRenderer&,const FighterActor&);
void render_fade_portrait(ImageRenderer&,const FighterActor&);
void render_victory_portrait(ImageRenderer&,const FighterActor&,i16 image,float slide,float y,u32 color);
void render_recovery_markers(ImageRenderer&,const BattleHudView&,const CameraMotion&);
}
