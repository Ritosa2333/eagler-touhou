#include "Replay.hpp"
namespace th10 {
// 0x42aae0 / 0x42a8a0 / 0x42a900. A block contains 3600 input frames.
void ReplayBuffer::initialize() noexcept {std::memset(this,0,sizeof(*this));input_cursor=inputs;rate_cursor=frame_rates;node.value=this;}
bool ReplayBuffer::append(ReplayKeys keys) noexcept {*input_cursor=keys;++input_cursor;return input_cursor-inputs>=3600;}
bool ReplayBuffer::append_rate(u8 rate) noexcept {*rate_cursor++=rate;return rate_cursor-frame_rates>=120;}
ListNode<ReplayBuffer>* Replay::add_buffer(i32 stage,ReplayEnvironment& env){auto* buffer=static_cast<ReplayBuffer*>(env.allocate(sizeof(ReplayBuffer)));if(!buffer)return nullptr;buffer->initialize();auto* tail=&buffers[stage];while(tail->next)tail=tail->next;tail->next=&buffer->node;buffer->node.previous=tail;return &buffer->node;}
void Replay::clear_buffers(i32 stage,ReplayEnvironment& env){auto* node=buffers[stage].next;while(node){auto* next=node->next;if(auto* buffer=node->value){auto& link=buffer->node;if(link.next)link.next->previous=link.previous;if(link.previous)link.previous->next=link.next;link.next=link.previous=nullptr;env.release(buffer);}node=next;}}
// 0x4297d0. Playback keeps the previous held keys even after a stream ends.
i32 Replay::update_input(ReplayEnvironment& env){
    if(!env.controller_flags)return 1;
    auto& keys=*env.input;
    if(mode==0){
        keys.previous=keys.current;keys.current=keys.raw&0x1f7;
        if(*env.display_flags&0x200){if(keys.current&1){keys.focus_hold=static_cast<u16>(keys.focus_hold+1);if(keys.focus_hold>=8){keys.current|=4;keys.focus_hold=8;}}else keys.focus_hold=0;}
        keys.update_edges();
        if(elapsed%30==0){const auto fps=number(*env.measured_fps)+number(.5f);*active_buffer->value->rate_cursor++=(fps.is_nan()||fps<number(256))?static_cast<u8>(fps.truncate_int()):255;}
        if(active_buffer->value->append({keys.current,keys.pressed,keys.released}))active_buffer=add_buffer(active_stage,env);
    }else if(active_stage>=0){
        keys.previous=keys.current;
        auto& reader=readers[active_stage];
        if(reader.frame<reader.stage->frames){keys.current=reader.input_cursor->held;keys.pressed=reader.input_cursor->pressed;keys.released=reader.input_cursor->released;recorded_fps=*reader.rate_cursor;++reader.input_cursor;if(elapsed%30==0)++reader.rate_cursor;}
        else keys.current=keys.pressed=keys.released=0;
        reader.frame=wrapping_add(reader.frame,1);
    }else keys.current=keys.pressed=keys.released=0;
    elapsed=wrapping_add(elapsed,1);return 1;
}
// 0x429a30 and update-chain wrapper 0x42a3e0. Return 6 runs another logic
// iteration before drawing, through UpdateChain's restart rule.
i32 Replay::frame_action(const ReplayEnvironment& env,bool check_pause) const noexcept {return env.controller_flags&&(!check_pause||!(*env.controller_flags&4))&&mode==1&&(env.input->raw&0x100)&&elapsed%4!=0?6:1;}
i32 Replay::draw(ReplayEnvironment& env,bool check_pause) const {if(env.controller_flags&&mode==1&&(!check_pause||!(*env.controller_flags&4))){const auto color=recorded_fps<30?0xff5050ffu:recorded_fps<50?0xffa0a0ffu:0xffffffffu;env.draw_rate({383,450,0},color,recorded_fps);}return 1;}
// 0x42a930. Restoring item value resets the faith timer first (0x418b80).
void restore_faith(GameEconomy& game,i32 frames,float* rate) noexcept {if(!(game.faith_timer_flags&1)){game.faith_timer.rate=rate;game.faith_timer_flags|=1;}game.faith_timer.previous=wrapping_add(frames,-1);game.faith_timer.current=frames;game.faith_timer.fractional=Extended::from_int(frames).to_float();}
void ReplayStage::capture_game(const GameEconomy& game) noexcept {score=game.score;power=game.power;item_value=game.item_value;faith=game.faith_timer.current;lives=game.lives;rank=game.rank;score_units=game.score_units;}
void ReplayStage::restore_game(GameEconomy& game,Rng& random,float* rate) const noexcept {random.seed=seed;random.calls=0;game.score=score;game.power=power;const u32 bits=static_cast<u32>(item_value)*10u;i32 points;std::memcpy(&points,&bits,4);game.item_value=points/10;restore_faith(game,faith,rate);game.lives=lives;game.rank=rank;game.score_units=score_units;game.extend_index=extend_index;}
// 0x42a6a0. The initial recording snapshot is filled later by activate_stage.
void Replay::prepare_stage(ReplayEnvironment& env){auto& game=*env.game;if(mode==0){auto* snapshot=static_cast<ReplayStage*>(env.allocate(sizeof(ReplayStage)));stages[game.stage]=snapshot;if(!snapshot)return;snapshot->initialize();snapshot->seed=env.random->seed;env.random->calls=0;snapshot->stage=static_cast<std::int16_t>(game.stage);snapshot->flags=(snapshot->flags&~1u)|(*env.recording_mode&1);}else if(mode==1){auto& reader=readers[game.stage];reader.rewind();reader.stage->restore_game(game,*env.random,env.rate);}}
// 0x42a450. Marisa C restores focused offsets from targets, a profile-specific
// original adjustment which must happen after configuring the four options.
void Replay::activate_stage(ReplayEnvironment& env){
    if(update_entry)update_entry->flags|=2;if(end_frame_entry)end_frame_entry->flags|=2;if(draw_entry)draw_entry->flags|=2;
    auto& game=*env.game;
    if(mode==0){auto* snapshot=stages[game.stage];clear_buffers(game.stage,env);active_buffer=add_buffer(game.stage,env);if(!*env.recording_mode)snapshot->capture_game(game);auto& p=**env.player;snapshot->position=p.fixed_position;std::memcpy(stages[game.stage]->history,p.position_history,sizeof(p.position_history));for(int i=0;i<4;i++){snapshot->option_targets[i]=p.options[i].target;snapshot->option_positions[i]=p.options[i].position;snapshot->option_offsets[i]=p.options[i].offset;snapshot->option_focused_offsets[i]=p.options[i].focused_offset;}snapshot->extend_index=game.extend_index;active_stage=game.stage;stages[game.stage]->focused=p.focused;}
    else if(mode==1){const auto& snapshot=*readers[game.stage].stage;auto* p=*env.player;active_stage=game.stage;p->set_position(snapshot.position);std::memcpy(p->position_history,snapshot.history,sizeof(snapshot.history));(*env.player)->focused=snapshot.focused;env.configure_options(**env.player);p=*env.player;for(int i=0;i<4;i++){p->options[i].target=snapshot.option_targets[i];p->options[i].position=snapshot.option_positions[i];p->options[i].offset=snapshot.option_offsets[i];p->options[i].focused_offset=game.character*3+game.shot_type==5?snapshot.option_targets[i]:snapshot.option_focused_offsets[i];}for(auto& option:p->options)option.snap_next=0;env.activate_player(*p);}
    elapsed=0;
}
i32 Replay::finish_recording(i32 clear,ReplayEnvironment& env){env.timestamp(info->timestamp);info->last_stage=clear?wrapping_add(clear,7):env.game->stage;return 0;}
}
