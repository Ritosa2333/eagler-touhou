#include "ReplayStageSnapshot.hpp"
#include <algorithm>
namespace th15 {
namespace {
template<class T> void save(u8* out,u32 at,const T& value){std::memcpy(out+at,&value,sizeof value);}
template<class T> T load(const u8* in,u32 at){T value;std::memcpy(&value,in+at,sizeof value);return value;}
}
void ReplayStageSnapshot::capture_progress(const ReplayRunState& r){
 auto* out=data.data()+0x14;const auto& s=r.progress;const auto& p=r.player;const auto& v=r.score;
 save(out,0x00,s.stage);save(out,0x04,s.starting_stage);save(out,0x08,s.chapter);save(out,0x0c,s.stage_frame);save(out,0x10,s.run_clock);save(out,0x14,s.character);save(out,0x18,s.subcharacter);save(out,0x1c,v.score);save(out,0x20,s.difficulty);save(out,0x24,s.continues);save(out,0x28,r.enemies.rank);save(out,0x2c,v.graze_total);save(out,0x30,v.graze_chapter);save(out,0x34,s.spell_id);save(out,0x3c,p.deaths);save(out,0x40,v.point_items);save(out,0x44,v.point_value);save(out,0x48,s.initial_point_value);save(out,0x4c,v.max_point_value);save(out,0x50,p.power);save(out,0x54,v.max_power);save(out,0x58,p.power_step);save(out,0x5c,s.checkpoint_power);save(out,0x60,p.extra_lives);save(out,0x64,p.life_pieces);save(out,0x68,v.life_piece_tier);save(out,0x6c,p.bombs);save(out,0x70,p.bomb_pieces);save(out,0x74,v.chapter_value);save(out,0x7c,v.chapter_count);save(out,0x80,v.collection_timer);save(out,0x84,v.collection_position);save(out,0x90,s.alternating_pieces);save(out,0x94,r.enemies.chapter_total);save(out,0x98,r.enemies.chapter_defeated);
 const u32 n=std::min(u32(r.music.size()),u32(63));std::memcpy(out+0x9c,r.music.data(),n);out[0x9c+n]=0;
 for(u32 i=0;i<9;i++)save(out,0x1a4+i*4,s.stage_deaths[i]);save(out,0x1c8,s.chapter_deaths);
}
bool ReplayStageSnapshot::create(const ReplayRunState& state,const Rng& game,bool initial_recording){
 error.clear();available=false;if(state.progress.stage<1||state.progress.stage>7){error="Recording stage outside original range";return false;}data.fill(0);save(data.data(),0,u16(state.progress.stage));save(data.data(),2,game.seed);save(data.data(),0x234,u32(state.progress.new_run));if(initial_recording){capture_progress(state);for(u32 i=0;i<20;i++)save(data.data(),0x1e4+i*4,u32(0)-i*u32(0x21522153));}available=true;return true;
}
bool ReplayStageSnapshot::read(const u8* in,u32 size){error.clear();available=false;if(!in||size!=data.size()){error="Invalid replay stage snapshot size";return false;}const u16 stage=load<u16>(in,0);if(stage<1||stage>7){error="Replay snapshot stage outside original range";return false;}std::copy(in,in+size,data.begin());available=true;return true;}
bool ReplayStageSnapshot::refresh(const ReplayRunState& state){if(!available){error="Replay snapshot has not been created";return false;}if(load<u16>(data.data(),0)!=state.progress.stage){error="Replay snapshot belongs to another stage";return false;}if(!state.progress.new_run)capture_progress(state);save(data.data(),0xc,state.motion.position_fixed);save(data.data(),0x1e0,state.motion.focus);return true;}
bool ReplayStageSnapshot::restore_progress(ReplayRunState& r){
 if(!available){error="Replay snapshot unavailable";return false;}const auto* in=data.data()+0x14;auto& s=r.progress;auto& p=r.player;auto& v=r.score;
 s.stage=load<i32>(in,0x00);s.starting_stage=load<i32>(in,0x04);s.chapter=load<i32>(in,0x08);s.stage_frame=load<i32>(in,0x0c);s.run_clock=load<i32>(in,0x10);s.character=load<i32>(in,0x14);s.subcharacter=load<i32>(in,0x18);v.score=load<i32>(in,0x1c);s.difficulty=load<i32>(in,0x20);s.continues=load<i32>(in,0x24);r.enemies.rank=load<i32>(in,0x28);v.graze_total=load<i32>(in,0x2c);v.graze_chapter=load<i32>(in,0x30);s.spell_id=load<i32>(in,0x34);p.deaths=load<i32>(in,0x3c);v.point_items=load<i32>(in,0x40);v.point_value=load<i32>(in,0x44);s.initial_point_value=load<i32>(in,0x48);v.max_point_value=load<i32>(in,0x4c);p.power=load<i32>(in,0x50);v.max_power=load<i32>(in,0x54);p.power_step=load<i32>(in,0x58);s.checkpoint_power=load<i32>(in,0x5c);p.extra_lives=load<i32>(in,0x60);p.life_pieces=load<i32>(in,0x64);v.life_piece_tier=load<i32>(in,0x68);p.bombs=load<i32>(in,0x6c);p.bomb_pieces=load<i32>(in,0x70);v.chapter_value=load<i32>(in,0x74);v.chapter_count=load<i32>(in,0x7c);v.collection_timer=load<i32>(in,0x80);v.collection_position=load<Vec3>(in,0x84);s.alternating_pieces=load<i32>(in,0x90);r.enemies.chapter_total=load<i32>(in,0x94);r.enemies.chapter_defeated=load<i32>(in,0x98);
 u32 n=0;while(n<64&&in[0x9c+n])++n;r.music.assign(reinterpret_cast<const char*>(in+0x9c),n);for(u32 i=0;i<9;i++)s.stage_deaths[i]=load<i32>(in,0x1a4+i*4);s.chapter_deaths=load<i32>(in,0x1c8);v.difficulty=s.difficulty;r.enemies.difficulty=s.difficulty;r.enemies.current_chapter=s.chapter;return true;
}
bool ReplayStageSnapshot::restore_seed(Rng& game,Rng* visual)const noexcept{if(!available)return false;game.seed=load<u16>(data.data(),2);game.calls=0;if(visual)visual->seed=game.seed;return true;}
bool ReplayStageSnapshot::restore_player(PlayerMotion& motion)const{if(!available)return false;motion.position_fixed=load<PlayerFixedPosition>(data.data(),0xc);motion.position.x=float(float(motion.position_fixed.x)*0.0078125f);motion.position.y=float(float(motion.position_fixed.y)*0.0078125f);motion.focus=load<i32>(data.data(),0x1e0);motion.snap_options();return true;}
bool ReplayStageSnapshot::spell_timing(u32 sequence,i32& value,bool write){
 if(!available||sequence>=20){error="Replay spell timing outside stage snapshot";return false;}
 if(write)save(data.data(),0x1e4+sequence*4,value);else value=load<i32>(data.data(),0x1e4+sequence*4);return true;
}

}
