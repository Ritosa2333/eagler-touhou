#pragma once
#include "../cpp/game/GameWorld.hpp"
namespace th09::audit {
inline const i32* snapshot(GameWorld* w,EclWorldState& state){
 static i32 v[48];std::memset(v,0,sizeof(v));v[0]=state.random.seed;v[1]=state.random.calls;if(!w)return v;
 v[2]=w->battle->state.flags;v[3]=w->dialogue->id;v[4]=w->scene->phase;v[5]=w->rules.progress.round;
 for(i32 side=0;side<2;++side){auto& f=w->battle->fields[side];auto& p=*f.player;auto* q=v+6+side*20;
 const auto bits=[](float x){i32 b;std::memcpy(&b,&x,4);return b;};
 q[0]=bits(p.motion.position.x);q[1]=bits(p.motion.position.y);q[2]=p.motion.health;q[3]=p.control.player_state;q[4]=bits(p.control.charge);q[5]=bits(p.control.available);q[6]=w->rules.scores[side].points;q[7]=f.cpu_level;q[8]=p.cpu.survival.current;q[9]=p.combo_state.best_hits;q[10]=p.input.held;q[11]=p.input.pressed;q[12]=p.cpu.direction;q[13]=p.shots.areas.count;q[14]=bits(w->rules.scores[side].lives);
 for(i32 d=0;d<5;++d)q[15+d]=w->huds[side]->animations[45+d].activeSpriteIndex;
 }return v;
}
}
