#pragma once
#include "../../cpp/game/ReplayState.hpp"
struct ReplayStateFixture {ReplayRunState state;ReplayStageState entry;PlayerMotion motion;Rng game,visual;};
API(replay_state_create) ReplayStateFixture* replay_state_create(){return new ReplayStateFixture;}
API(replay_state_delete) void replay_state_delete(ReplayStateFixture* p){delete p;}
API(replay_state_values) ReplayRunState* replay_state_values(ReplayStateFixture* p){return &p->state;}
API(replay_state_entry) ReplayStageState* replay_state_entry(ReplayStateFixture* p){return &p->entry;}
API(replay_state_assign) void replay_state_assign(ReplayStateFixture* p,const ReplayRunState* source){p->state.assign(*source);}
API(replay_state_restore_player) void replay_state_restore_player(ReplayStateFixture* p){p->entry.restore_player(p->motion);}
API(replay_state_seed) void replay_state_seed(ReplayStateFixture* p){p->entry.seed_random(p->game,p->visual);}
API(replay_state_rng) Rng* replay_state_rng(ReplayStateFixture* p,u32 visual){return visual?&p->visual:&p->game;}
API(replay_state_motion) void* replay_state_motion(ReplayStateFixture* p,u32 field){switch(field){case 0:return &p->motion.position_fixed;case 1:return &p->motion.position;case 2:return &p->motion.focus;case 3:return p->motion.history.data();default:return &p->motion.options[field-4].snap;}}
API(replay_initial_state) u32 replay_initial_state(Replay* replay,u32 stage,ReplayStageState* state){return replay->initial_state(stage,*state);}
