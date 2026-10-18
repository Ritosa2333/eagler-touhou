// Read-only, exact-word snapshots at the completed update-chain boundary.
// This export neither advances the simulation nor alters its input/RNG.
#include "../../../source_reconstruction/replay_system/replay.hpp"
#include "../../../source_reconstruction/input/input.hpp"
#include "../../../source_reconstruction/runtime_state/state.hpp"
#include "../../../source_reconstruction/player_entity/owner.hpp"
#include "../../../source_reconstruction/gameplay/gameplay.hpp"
#include "../../../source_reconstruction/hud_system/hud.hpp"
#include <cstring>
#include "../../../source_reconstruction/program_entry/program_entry.hpp"
#include "../../../source_reconstruction/item_system/item.hpp"
#include "../../../source_reconstruction/special_state/special.hpp"
#include "../../../source_reconstruction/damage_regions/regions.hpp"
#include "../../../source_reconstruction/gameplay/player_state.hpp"
namespace {
#ifdef TH20_REPLAY_CLOCK_EXPERIMENTS
int charge_stage=-1,charge_offset=0;
int shot_stage=-1,shot_offset=0;
#endif
unsigned activation_state[12]{};
}
extern "C" __attribute__((export_name("replay_verifier_activation"))) const unsigned* replay_verifier_activation(){return activation_state;}
#ifdef TH20_REPLAY_CLOCK_EXPERIMENTS
// Explicit diagnostic experiment, disabled by default. This is not an
// inferred repair and its captures must not be accepted as parity results.
extern "C" __attribute__((export_name("replay_verifier_charge_offset"))) void replay_verifier_charge_offset(int stage,int offset){
 if(stage<1||stage>7||offset<-300||offset>300)return;
 charge_stage=stage;charge_offset=offset;
}
extern "C" __attribute__((export_name("replay_verifier_shot_offset"))) void replay_verifier_shot_offset(int stage,int offset){
 if(stage<1||stage>7||offset<-300||offset>300)return;
 shot_stage=stage;shot_offset=offset;
}
#endif
namespace th20::source::replay {
void verifier_stage_activation(){
 auto* r=controller();if(!r||r->mode!=1)return;
 auto* p=static_cast<player_entity::Player*>(game_session::context(0).objects_04[0]);
 auto* summon=special_state::controller;if(!p||!summon)return;
 const int stage=gameplay::player_state::stage(game_session::session.player_table);
#ifdef TH20_REPLAY_CLOCK_EXPERIMENTS
 if(stage==charge_stage)recovered::timer_add(summon->charge_age,charge_offset,state::timer_rate);
 if(stage==shot_stage)recovered::timer_add(p->shots.timer_12580,shot_offset,state::timer_rate);
#endif
 activation_state[0]=stage;activation_state[1]=p->shots.timer_12580.current;activation_state[2]=summon->charge_age.current;
 for(unsigned i=0;i<4;++i){activation_state[3+i]=p->options[i].field_d4;activation_state[7+i]=p->options[i].timer_e4.current;}
 activation_state[11]=0;if(auto* regions=damage::controller())for(auto* l=regions->active.sentinel.next;l;l=l->next)++activation_state[11];
}
}
extern "C" __attribute__((export_name("replay_verifier_items"))) const unsigned* replay_verifier_items(){
 using namespace th20::source;
 static unsigned out[1+1536*13];out[0]=0;
 if(auto* owner=item::controller())for(unsigned i=0;i<1536;i++){
  const auto& item=owner->pool[i];if(!item.state||item.link.owner!=&owner->active)continue;
  auto* row=out+1+13*out[0]++;row[0]=i;row[1]=item.state;row[2]=item.type;
  std::memcpy(row+3,&item.position,12);std::memcpy(row+6,&item.velocity,12);
  std::memcpy(row+9,&item.attraction_speed,4);row[10]=item.delay;row[11]=item.timer.current;row[12]=item.generation;
 }
 return out;
}
// Logical firing state only: omit ANM handles, callbacks and allocation addresses.
extern "C" __attribute__((export_name("replay_verifier_firing"))) const unsigned* replay_verifier_firing(){
 using namespace th20::source;
 static unsigned out[4096];unsigned n=1;
 auto* p=static_cast<player_entity::Player*>(game_session::context(0).objects_04[0]);
 if(!p){out[0]=0;return out;}
 auto copy=[&](const void* data,unsigned bytes){std::memcpy(out+n,data,bytes);n+=bytes/4;};
 copy(p->fields_674,16);copy(p->fields_20e4,12);
 copy(&p->shots.timer_12400,48);copy(p->shots.counters_12468,120);
 out[n++]=p->shots.field_1255c;
 for(const auto& o:p->options){copy(&o,0xdc);copy(o.fields_f4,40);out[n++]=o.fields_f4[12];out[n++]=0;}
 unsigned count=0;const auto count_word=n++;
 for(auto* l=p->shots.active.sentinel.next;l&&n+14<4096;l=l->next){
  const auto& s=*reinterpret_cast<player_entity::Shot*>(l->value);++count;
  out[n++]=s.fields_b8[8];out[n++]=s.fields_98[1];out[n++]=s.fields_98[5];
  copy(&s.motion.position,12);copy(&s.motion.vector_38,12);
  out[n++]=s.timer_1c.current;out[n++]=s.fields_3c[0];out[n++]=s.byte_114;
 }
 out[count_word]=count;out[0]=n-1;return out;
}
extern "C" __attribute__((export_name("replay_verifier_finished"))) unsigned replay_verifier_finished() {
    const auto* r=th20::source::replay::controller();
    return r&&r->mode==1&&(r->flags&2u)?1u:0u;
}
extern "C" __attribute__((export_name("replay_verifier_trace"))) const unsigned* replay_verifier_trace() {
    using namespace th20::source;
    static unsigned out[36];std::memset(out,0,sizeof out);
    auto* r=replay::controller();auto* g=gameplay::controller;
    if(!r||r->mode!=1||!g||r->active_stage<0||r->active_stage>=8)return out;
    const auto& c=r->playback[r->active_stage];
    auto* p=static_cast<player_entity::Player*>(game_session::context(0).objects_04[0]);
    if(!p||c.frame<=0)return out;
    out[0]=1;out[1]=r->active_stage;out[2]=c.frame;
    out[3]=g->frame_timer.current;out[4]=g->secondary_timer.current;out[5]=g->game_flags;
    const auto& b=input::shared_state().slots[0];
    out[6]=b.retained_298[1];out[7]=b.retained_298[4];out[8]=b.retained_298[5];
    const auto& rng=state::random_streams[0];out[9]=rng.last;out[10]=rng.state;
    std::memcpy(out+11,&p->position_614,8);out[13]=p->state;
    out[14]=p->fixed_position.x;out[15]=p->fixed_position.y;out[16]=p->focused_204c;
    const auto& table=game_session::session.player_table;
    std::memcpy(out+17,&table.players[0],8); // exact 64-bit score
    std::memcpy(out+19,reinterpret_cast<const unsigned char*>(&table.players[0])+0x30,4);
    std::memcpy(out+20,reinterpret_cast<const unsigned char*>(&table.players[0])+0xb8,4);
    std::memcpy(out+21,reinterpret_cast<const unsigned char*>(&table.players[0])+0xc0,4);
    std::memcpy(out+22,reinterpret_cast<const unsigned char*>(&table.players[0])+0xd0,4);
    std::memcpy(out+23,reinterpret_cast<const unsigned char*>(&table.players[0])+0xcc,4);
    out[24]=game_session::session.flags;out[25]=hud::controller&&hud::controller->collecting?1:0;
    std::memcpy(out+26,p->speeds_20b4,sizeof(p->speeds_20b4));
    std::memcpy(out+30,reinterpret_cast<const unsigned char*>(&table.players[0])+0xe4,4);
    std::memcpy(out+31,&program_entry::window_state.scale,4);
    std::memcpy(out+32,reinterpret_cast<const unsigned char*>(&table.players[0])+0x40,16);
    return out;
}
