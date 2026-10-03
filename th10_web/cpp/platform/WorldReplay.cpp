#include "../game/CallbackNames.hpp"
#include "World.hpp"
#include "../game/ReplayResources.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "../multiplayer/ReplayFiles.hpp"
#include <algorithm>
#endif
#ifdef TH_ENABLE_THPRAC
#include "../game/PracticeConfig.hpp"
#endif
#include <new>
namespace th10::browser {
namespace {
struct Gameplay final:ReplayEnvironment {
    World& w;explicit Gameplay(World& world):w(world){game=&w.state.game;input=&w.input.player_profiles[0].input;random=&w.engine.script_random;rate=&w.engine.speed;display_flags=&w.state.configuration.display_flags;recording_mode=reinterpret_cast<const u32*>(&w.new_game);controller_flags=w.actors.session?&w.actors.session->session_flags:nullptr;measured_fps=&w.measured_fps;player=&w.actors.player;}
    void* allocate(u32 bytes) override{
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
        // Output storage is not a rewindable allocation graph. Bootstrap it
        // before frame zero; future MP record output commits outside journals.
        if(w.engine.rollback_state){w.fail();return nullptr;}
#endif
        return w.replay_memory.allocate(bytes);
    }
    void release(void* bytes) override{if(!bytes)return;if(w.replay_memory.owns(bytes))w.replay_memory.release(bytes);else w.replay_files.memory.release(bytes);}
    void configure_options(Player&) override{w.configure_player();}
    void activate_player(Player&) override{w.activate_player();}
    void draw_rate(const Vec3& position,u32 color,u8 fps) override{w.common.value->color=color;const u32 argument=fps;w.queue_text(position,"%3d",&argument,1);w.common.value->color=0xffffffff;}
    void timestamp(i32& destination) override{destination=w.calendar.timestamp();}
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    void restart_recording_buffer(Replay& replay,i32 stage) override{
        auto* buffer=replay.buffers[stage].next;
        if(!buffer||!buffer->value){w.fail();return;}
        // Native stage metadata is still captured by activate_stage. The
        // record buffer/cursor is an external owner, not recreated on rewind.
        replay.active_buffer=buffer;
    }
#endif
};
struct Resources final:ReplayResourceEnvironment {
    World& w;Gameplay services;explicit Resources(World& world):w(world),services(world){gameplay=&services;current=&w.state.replay;configuration=w.actors.session?w.actors.session->configuration:nullptr;chain=&w.chain;callbacks=&w.engine.callback_environment;input_callback=callback_id::ReplayUpdate;end_frame_callback=callback_id::ReplayFrame;draw_callback=callback_id::ReplayDraw;}
    i32 load(Replay& replay,const char* name) override{
        w.replay_files.flags=w.state.game.flags;const auto result=load_replay(replay,name,w.replay_files);if(result)return result;
        char path[264];const bool demo=(w.state.game.flags&0x20)!=0;const auto length=std::strlen(name);if(length>255)return -1;const auto prefix=demo?0:7;if(prefix)std::memcpy(path,"replay/",prefix);std::memcpy(path+prefix,name,length+1);const auto handle=w.scores.files.host.open(path,false);
        if(handle!=0xffffffff){const auto size=w.scores.files.host.size(handle);if(size>16*1024*1024){w.scores.files.host.close(handle);return -1;}std::vector<u8> bytes(size);const bool complete=w.scores.files.host.read(handle,bytes.data(),size)==size;w.scores.files.host.close(handle);if(!complete||!w.motion.load(bytes.data(),size,10))return -1;
#ifdef TH_ENABLE_THPRAC
        // THGuiRep::State(2)/(3) restore: the raw file carries the 'USER'/'PRAC'
        // block after the packed replay (practice_replay_read, upstream
        // ReplayLoadParam). Playback is authoritative for the live run, so a
        // valid block is copied into both the menu candidate and the run.
        if(w.state.practice.replay){
            w.state.practice.replay_candidate.reset();w.state.practice.replay_candidate_valid=false;w.state.practice.run.reset();w.state.practice.active=false;
            PracticeConfig config;
            if(practice_replay_read(bytes.data(),u32(size),config)){
                w.state.practice.replay_candidate=config;w.state.practice.replay_candidate_valid=true;
                w.state.practice.run=config;w.state.practice.active=true;
            }
        }
#endif
        }else w.motion.clear();return 0;
    }
};
}
bool World::create_replay(i32 mode,const char* name){
    motion.clear();Resources env(*this);
    if(!ReplayResources::create(mode,name,env))return false;
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(mode==0){
        if(state.multiplayer_replay.Recording())begin_replay_checkpoint();
        else if(state.multiplayer_replay.SelectedCheckpoint()&&!restore_replay_checkpoint_bootstrap())return false;
    }
#endif
    return true;
}
void World::destroy_replay(Replay* replay){if(!replay)return;Resources env(*this);ReplayResources{*replay,env}.shutdown();env.services.release(replay);replay_files.close();replay_files.memory.clear();}
void World::prepare_replay(){Gameplay env(*this);state.replay->prepare_stage(env);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    auto& replay=*state.replay;
    if(replay.mode==0&&!replay.buffers[state.game.stage].next)
        if(!replay.add_buffer(state.game.stage,env))fail();
    if(replay.mode==0&&state.multiplayer_replay.Recording())begin_replay_checkpoint();
#endif
}
void World::activate_replay(){Gameplay env(*this);state.replay->activate_stage(env);
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    if(state.multiplayer_replay.Recording()){
        if(!finalize_replay_checkpoint())fail();
    }else if(const auto* checkpoint=state.multiplayer_replay.SelectedCheckpoint()){
        if((checkpoint->label&255u)!=u32(state.game.stage)||checkpoint->firstFrame!=state.multiplayer_replay.Base()){
            fail();return;
        }
        if(!checkpoint->retainedStateValid||checkpoint->faithCursor>=2048||
           !actors.items||!actors.lasers){
            fail();return;
        }
        actors.items->faith_cursor=static_cast<i32>(checkpoint->faithCursor);
        actors.lasers->last_id=checkpoint->laserLastId;
        state.game.reserved_040=checkpoint->reservedStage;
        for(u32 seat=0;seat<player_count;++seat){
            auto& pilot=pilots[seat];auto* player=pilot.player;const auto& snap=checkpoint->pilots[seat];
            if(!player){fail();return;}
            player->set_position(snap.position);
            std::memcpy(player->position_history,snap.history,sizeof(snap.history));
            player->focused=snap.focused;configure_player(pilot);player=pilot.player;
            for(int i=0;i<4;++i){
                player->options[i].target=snap.option_targets[i];
                player->options[i].position=snap.option_positions[i];
                player->options[i].offset=snap.option_offsets[i];
                player->options[i].focused_offset=
                    pilot.game.character*3+pilot.game.shot_type==5?
                    snap.option_targets[i]:snap.option_focused_offsets[i];
                player->options[i].snap_next=0;
            }
        }
        activate_player();
        if(checkpoint->activationRandomValid){
            engine.script_random=checkpoint->activationScriptRandom;
            engine.visual_random=checkpoint->activationVisualRandom;
        }
        state.multiplayer_replay.DeselectCheckpoint();
    }
#else
    motion.begin(state.game.stage,false,state.replay->mode!=0,state.replay->mode==0);
#endif
}
i32 World::update_replay(){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    for(u32 seat=0;seat<player_count;++seat){
        pilots[seat].input_keys=state.input_lanes.seats[seat].current;
        const auto& input=state.input_lanes.inputs[seat];
        if(input.unlimited&&(input.x!=0||input.y!=0))state.multiplayer_cheat_movement_used=true;
    }
    state.replay->elapsed=wrapping_add(state.replay->elapsed,1);return 1;
#else
    Gameplay env(*this);return state.replay->update_input(env);
#endif
}
i32 World::replay_frame_action(){Gameplay env(*this);return state.replay->frame_action(env,true);}
i32 World::draw_replay(){
    Gameplay env(*this);const i32 result=state.replay->draw(env,true);
    if(motion.playing&&common.value){
        touhou::input::MotionTrack::TouchPoint points[10];const int count=motion.replay_points(points,10);
        auto& text=*common.value;const u32 color=text.color;const Vec2 scale=text.scale;const i32 shadow=text.shadow;
        text.color=0xffffffff;text.scale={.7f,.7f};text.shadow=0;
        for(int i=0;i<count;++i)text.queue("+",{points[i].x*640.f-5.f,points[i].y*480.f-7.f,0},false);
        text.color=color;text.scale=scale;text.shadow=shadow;
    }
    return result;
}
void World::finish_replay(i32 clear){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // Result classification is deterministic; a wall-clock timestamp belongs
    // to a confirmed save operation and must not run again during resimulation.
    state.replay->info->last_stage=clear?wrapping_add(clear,7):state.game.stage;
#else
    Gameplay env(*this);state.replay->finish_recording(clear,env);
#endif
}
Replay* World::preview(const char* name){auto* entry=new(std::malloc(sizeof(Preview))) Preview(scores.files,state.game.flags,previews);if(entry->document.load(name)){entry->~Preview();std::free(entry);return nullptr;}entry->document.value.mode=2;previews=entry;return &entry->document.value;}
void World::release_replay(Replay* replay){if(!replay)return;for(auto** next=&previews;*next;next=&(*next)->next){auto* entry=*next;if(&entry->document.value==replay){*next=entry->next;entry->~Preview();std::free(entry);return;}}if(replay==state.replay){destroy_replay(replay);return;}__builtin_trap();}
void World::save_replay(const char* file,const char* name){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // A spectator replays the players' menu inputs but owns no external
    // recording or save decision.
    if(state.netplay_runtime.Spectator())return;
    auto& archive=state.multiplayer_replay;
    // Replaying the recorded save-menu choice is gameplay input, not consent
    // to repeat the original user's external file write.
    if(archive.Playing())return;
    if(!file||std::strlen(file)>240){fail();return;}
    char path[256];std::memcpy(path,"replay/",7);std::strcpy(path+7,file);
    const i32 stage=state.replay&&state.replay->info?
        std::clamp(state.replay->info->last_stage,1,8):std::clamp(state.game.stage,1,8);
    if(engine.rollback_state){
        if(!archive.RequestSave(path,name,state.netplay_runtime.NextFrame(),
                                state.game.score,state.game.score_units,stage))fail();
    }else if(!commit_replay()||!multiplayer::SaveReplayFile(scores.files,calendar,state,
             path,name,state.game.score,state.game.score_units,stage))fail();
    return;
#endif
#ifdef TH_ENABLE_THPRAC
    const auto motion_tail=motion.playing?std::vector<u8>{}:motion.trailer(10);
    std::vector<u8> tail;
    // Advanced-practice runs append their 'USER'/'PRAC' block immediately after
    // the vanilla replay, ahead of the motion trailer (upstream THSaveReplay +
    // ReplaySaveParam). Assisted/cheated runs are deliberately not savable.
    const bool practice_save=state.practice.active&&!state.practice.replay&&!state.practice.assisted&&state.practice.run.mode==1;
    if(practice_save)tail=practice_replay_block(state.practice.run);
    tail.insert(tail.end(),motion_tail.begin(),motion_tail.end());
    // th10_rep_power_fix runs inside save_replay while the payload is plain.
    replay_writer.practice_mode=practice_save;
    replay_writer.practice_power=state.practice.run.power;
    if((motion.used()&&!motion.playing&&motion_tail.empty())||replay_writer.save(*state.replay,file,name,tail))fail();
#else
    const auto tail=motion.playing?std::vector<u8>{}:motion.trailer(10);if((motion.used()&&!motion.playing&&tail.empty())||replay_writer.save(*state.replay,file,name,tail))fail();
#endif
}
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
void World::begin_replay_checkpoint(){
    replay_checkpoint_pending_valid=false;
    auto& archive=state.multiplayer_replay;
    if(!archive.Recording()||!state.replay||state.game.stage<1||state.game.stage>7)return;
    auto* native=state.replay->stages[state.game.stage];
    if(!native){fail();return;}
    auto& cp=replay_checkpoint_pending;cp={};
    cp.label=(archive.Generation()<<8)|u32(state.game.stage);
    cp.firstFrame=archive.Cursor();
    cp.scriptRandom=engine.script_random;cp.visualRandom=engine.visual_random;
    const auto& input_lanes=replay_checkpoint_precommit_valid?
        replay_checkpoint_precommit_lanes:state.input_lanes;
    for(u32 seat=0;seat<Netplay::MAX_PLAYERS;++seat)cp.inputSeats[seat]=input_lanes.seats[seat];
    cp.cheatMovementUsed=replay_checkpoint_precommit_valid?
        replay_checkpoint_precommit_cheat_used:state.multiplayer_cheat_movement_used;
    for(u32 seat=0;seat<player_count;++seat){
        auto& snap=cp.pilots[seat];snap.initialize();
        snap.stage=static_cast<std::int16_t>(state.game.stage);
        snap.seed=native->seed;snap.flags=native->flags;
        snap.capture_game(pilots[seat].game);
        snap.extend_index=pilots[seat].game.extend_index;
        cp.reservedPower[seat]=pilots[seat].game.reserved_power;
    }
    replay_checkpoint_pending_valid=true;
}

void World::capture_replay_checkpoint_precommit(u32 archiveFrame){
    replay_checkpoint_precommit_lanes=state.input_lanes;
    replay_checkpoint_precommit_frame=archiveFrame;
    replay_checkpoint_precommit_cheat_used=state.multiplayer_cheat_movement_used;
    replay_checkpoint_precommit_valid=true;
    if(replay_checkpoint_pending_valid){
        for(u32 seat=0;seat<Netplay::MAX_PLAYERS;++seat)
            replay_checkpoint_pending.inputSeats[seat]=state.input_lanes.seats[seat];
        replay_checkpoint_pending.cheatMovementUsed=state.multiplayer_cheat_movement_used;
    }
}

void World::clear_replay_checkpoint_precommit(){
    replay_checkpoint_precommit_valid=false;
    replay_checkpoint_precommit_frame=Netplay::INVALID_FRAME;
}

bool World::restore_replay_checkpoint_bootstrap(){
    const auto* checkpoint=state.multiplayer_replay.SelectedCheckpoint();
    if(!checkpoint||checkpoint->firstFrame!=state.multiplayer_replay.Base()||
       (checkpoint->label&255u)!=u32(state.game.stage)||
       checkpoint->cooperation.seatCount!=player_count)return false;
    for(u32 seat=0;seat<player_count;++seat){
        checkpoint->pilots[seat].restore_game(pilots[seat].game,engine.script_random,&engine.speed);
        pilots[seat].game.reserved_power=checkpoint->reservedPower[seat];
    }
    engine.script_random=checkpoint->scriptRandom;
    engine.visual_random=checkpoint->visualRandom;
    state.input_lanes={};
    for(u32 seat=0;seat<Netplay::MAX_PLAYERS;++seat)
        state.input_lanes.seats[seat]=checkpoint->inputSeats[seat];
    state.input_lanes.host=state.input_lanes.seats[0];
    state.multiplayer_cheat_movement_used=checkpoint->cheatMovementUsed;
    return true;
}

bool World::finalize_replay_checkpoint(){
    if(!replay_checkpoint_pending_valid)return false;
    if(!actors.items||!actors.lasers||actors.items->faith_cursor<0||
       actors.items->faith_cursor>=2048)return false;
    auto& cp=replay_checkpoint_pending;
    // Activation happens inside the current logical tick. Use that tick's
    // pre-Commit global frame; the archive checkpoint itself is queued until
    // this frame becomes confirmed and immutable.
    if(!replay_checkpoint_precommit_valid||
       replay_checkpoint_precommit_frame==Netplay::INVALID_FRAME)return false;
    cp.firstFrame=replay_checkpoint_precommit_frame;
    cp.label=(state.multiplayer_replay.Generation()<<8)|u32(state.game.stage);
    cp.cooperation.seatCount=cooperation.seatCount;
    cp.cooperation.retryPending=cooperation.retryPending?1:0;
    cp.cooperation.wipeTicks=cooperation.wipeTicks;
    for(u32 seat=0;seat<Netplay::MAX_PLAYERS;++seat){
        auto& out=cp.cooperation.seats[seat];const auto& in=cooperation.seats[seat];
        out.lives=in.lives;out.power=in.power;out.character=in.character;out.shot=in.shot;
        out.lifeState=u8(in.lifeState);out.rescueTicks=in.rescueTicks;out.rescueTarget=in.rescueTarget;
        out.waitingForFocusRelease=in.waitingForFocusRelease?1:0;
    }
    for(u32 seat=0;seat<player_count;++seat){
        auto& pilot=pilots[seat];auto* player=pilot.player;auto& snap=cp.pilots[seat];
        if(!player)return false;
        snap.capture_game(pilot.game);snap.extend_index=pilot.game.extend_index;
        cp.reservedPower[seat]=pilot.game.reserved_power;
        snap.position=player->fixed_position;
        std::memcpy(snap.history,player->position_history,sizeof(snap.history));
        for(int i=0;i<4;++i){
            snap.option_targets[i]=player->options[i].target;
            snap.option_positions[i]=player->options[i].position;
            snap.option_offsets[i]=player->options[i].offset;
            snap.option_focused_offsets[i]=player->options[i].focused_offset;
        }
        snap.focused=player->focused;
    }
    cp.activationScriptRandom=engine.script_random;
    cp.activationVisualRandom=engine.visual_random;
    cp.activationRandomValid=true;
    cp.faithCursor=static_cast<u32>(actors.items->faith_cursor);
    cp.laserLastId=actors.lasers->last_id;
    cp.reservedStage=static_cast<u32>(state.game.reserved_040);
    cp.retainedStateValid=true;
    if(replay_checkpoint_commit_valid)return false;
    replay_checkpoint_commit_pending=cp;
    replay_checkpoint_commit_frame=cp.firstFrame;
    replay_checkpoint_commit_valid=true;
    replay_checkpoint_pending_valid=false;
    return true;
}

bool World::commit_replay(){
    auto& archive=state.multiplayer_replay;
    if(!archive.Recording())return true;
    if(!archive.Commit(state.netplay_runtime))return false;
    if(replay_checkpoint_commit_valid&&
       archive.Frames()>replay_checkpoint_commit_frame){
        if(replay_checkpoint_commit_pending.firstFrame!=replay_checkpoint_commit_frame||
           !archive.CaptureCheckpoint(replay_checkpoint_commit_pending))return false;
        replay_checkpoint_commit_valid=false;
        replay_checkpoint_commit_frame=Netplay::INVALID_FRAME;
    }
    auto& request=archive.save;
    if(!request.pending||request.frame==Netplay::INVALID_FRAME||
       archive.Frames()<=archive.Base()+request.frame)return true;
    if(!archive.SaveCommitted(request.frame)){
        if(!multiplayer::SaveReplayFile(scores.files,calendar,state,request.file,request.name,
             request.score,request.scoreUnits,request.lastStage,archive.Base()+request.frame+1))return false;
        archive.MarkSaveCommitted(request.frame);
    }
    request.pending=false;return true;
}
#endif
}
