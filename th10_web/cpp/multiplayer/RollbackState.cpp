#include "RollbackState.hpp"
#include "../platform/World.hpp"
#include "../game/Dialogue.hpp"
#include "../game/StartupScreen.hpp"

#include <cstddef>
#include <algorithm>

namespace th10::multiplayer {
namespace {

template<class T>
bool touch(Netplay::RollbackJournal& journal,T& value){
    return journal.Touch(&value,sizeof(value));
}

template<std::size_t BlockSize,std::size_t Capacity>
bool touch_pool(Netplay::RollbackJournal& journal,
                RollbackPool<BlockSize,Capacity>& pool){
    if(!journal.Touch(pool.occupied,sizeof(pool.occupied))||
       !journal.Touch(&pool.cursor,sizeof(pool.cursor)))return false;
    for(std::size_t i=0;i<Capacity;++i)
        if(pool.active(i)&&!journal.Touch(pool.at(i),BlockSize))return false;
    return true;
}

bool touch_stage(Netplay::RollbackJournal& journal,Stage* stage){
    if(!stage)return true;
    if(!touch(journal,*stage))return false;
    if(stage->object_animations&&stage->file&&stage->file->primitive_count>0){
        const auto count=static_cast<std::size_t>(stage->file->primitive_count);
        if(!journal.Touch(stage->object_animations,count*sizeof(AnmVm)))return false;
    }
    if(stage->objects&&stage->file)for(i32 i=0;i<stage->file->object_count;++i)
        if(stage->objects[i]&&!touch(journal,stage->objects[i]->flags))return false;
    return true;
}

bool touch_animation_manager(Netplay::RollbackJournal& journal,browser::AnimationEngine& engine){
    auto& manager=engine.manager;
    if(!journal.Touch(&manager.started_scripts,sizeof(manager.started_scripts))||
       !journal.Touch(&manager.processed_count,sizeof(manager.processed_count))||
       !journal.Touch(manager.occupied,sizeof(manager.occupied))||
       !journal.Touch(&manager.cursor,sizeof(manager.cursor))||
       !touch(journal,manager.registry)||
       !journal.Touch(manager.draw_layers,sizeof(manager.draw_layers))||
       !journal.Touch(&manager.last_id,sizeof(manager.last_id)))return false;
    for(std::size_t i=0;i<4096;++i)
        if(manager.occupied[i]&&!journal.Touch(&manager.pool[i],sizeof(AnmVm)))return false;
    return touch_pool(journal,engine.rollback_animation_overflow)&&
           touch_pool(journal,engine.rollback_geometry)&&
           touch_pool(journal,engine.callback_environment.rollback_entries);
}

bool touch_bullets(Netplay::RollbackJournal& journal,EnemyBulletManager* manager){
    if(!manager)return true;
    if(!journal.Touch(manager,offsetof(EnemyBulletManager,pool))||
       !journal.Touch(&manager->animation_file,sizeof(manager->animation_file)))return false;
    for(std::size_t i=0;i<2000;++i){
        auto& bullet=manager->pool[i];
        if(bullet.state){
            if(!journal.Touch(&bullet,sizeof(bullet)))return false;
        }
    }
    return true;
}

bool touch_items(Netplay::RollbackJournal& journal,ItemManager* manager){
    if(!manager)return true;
    if(!journal.Touch(manager,offsetof(ItemManager,regular))||
       !journal.Touch(&manager->active_count,sizeof(manager->active_count)+
                                             sizeof(manager->faith_cursor)+
                                             sizeof(manager->faith_count)))return false;
    for(auto& item:manager->regular){
        if(item.state){
            if(!journal.Touch(&item,sizeof(item)))return false;
        }
    }
    for(auto& item:manager->faith){
        if(item.state){
            if(!journal.Touch(&item,sizeof(item)))return false;
        }
    }
    return true;
}

} // namespace

bool RollbackState::Reset(){
    Clear();
    configured_=journal_.Reset(Netplay::RollbackJournalConfig{
        14,20u*1024u*1024u,24000,false,true});
    return configured_;
}

void RollbackState::Clear(){
    journal_.Clear();configured_=false;
    last_bytes_=peak_bytes_=0;total_bytes_=0;snapshots_=0;
}

bool RollbackState::Touch(void* address,std::size_t bytes){
    if(!configured_||!journal_.IsFrameOpen())return true;
    return address&&bytes&&journal_.Touch(address,bytes);
}

bool RollbackState::BeginFrame(browser::World& world,std::uint32_t frame){
    if(!configured_&&!Reset())return false;
    if(!journal_.BeginFrame(frame))return false;

    auto& state=world.state;
    auto& engine=world.engine;
    if(!touch(journal_,state.team_economy)||
       !journal_.Touch(state.pilot_economies,sizeof(state.pilot_economies))||
       !touch(journal_,state.input_lanes)||
       !journal_.Touch(&state.application.pending_screen,sizeof(state.application.pending_screen))||
       !journal_.Touch(&state.application.background_color,sizeof(state.application.background_color))||
       !journal_.Touch(&state.current_stage,sizeof(state.current_stage))||
       !touch(journal_,engine.script_random)||
       !touch(journal_,engine.visual_random)||
       !touch(journal_,engine.world)||!touch(journal_,engine.ui)||
       !touch(journal_,engine.tangent)||
       !touch(journal_,state.application.engine_flags)||!touch(journal_,state.quitting)||
       !journal_.Touch(&engine.speed,sizeof(engine.speed))||
       !touch(journal_,world.cooperation)||
       !touch(journal_,world.backgrounds.current)||!touch(journal_,world.backgrounds.previous)||
       !journal_.Touch(world.regular_item_owners,sizeof(world.regular_item_owners))||
       !journal_.Touch(world.faith_item_owners,sizeof(world.faith_item_owners))||
       !touch(journal_,engine.chain_value)||
       !touch_animation_manager(journal_,engine)||
       !touch_pool(journal_,world.rollback_enemies)||
       !touch_pool(journal_,world.rollback_lasers)||
       !touch_pool(journal_,world.rollback_ecl)||
       !touch_pool(journal_,world.effects.rollback_effects))return false;

    // These owners survive the title screen. Their loading/introduction ANM
    // handles and fixed-tick counters are read by GameSession and authored
    // Draw; restoring only the ANM registry leaves handles from the future.
    if(state.application.startup&&!touch(journal_,*state.application.startup))return false;
    if(world.common.value&&!touch(journal_,*world.common.value))return false;
    if(world.hud&&!touch(journal_,world.hud->last_multiplayer_hud_frame))return false;
    if(world.hud&&!touch_pool(journal_,world.hud->rollback_dialogues))return false;
    // Records changed by native score/spell/statistics code are deterministic
    // values. Codec buffers and file handles are NOT part of the snapshot.
    if(world.scores.data&&(!touch(journal_,world.scores.data->characters)||
                          !touch(journal_,world.scores.data->settings)))return false;

    if(world.actors.session&&!touch(journal_,*world.actors.session))return false;
    if(state.replay){
        auto& replay=*state.replay;
        // Capture native gameplay metadata, not emitted file buffers, malloc
        // ownership or committed output cursors. Those live outside rollback.
        if(!touch(journal_,replay.flags)||!touch(journal_,replay.manager_state)||
           !touch(journal_,replay.elapsed)||!touch(journal_,replay.active_stage))return false;
        for(auto* stage:replay.stages)if(stage&&!touch(journal_,*stage))return false;
        if(replay.info&&!touch(journal_,*replay.info))return false;
    }
    for(std::uint32_t seat=0;seat<world.player_count;++seat){
        auto& pilot=world.pilots[seat];
        if(!journal_.Touch(&pilot.input_keys,sizeof(pilot.input_keys)))return false;
        if(pilot.player&&!touch(journal_,*pilot.player))return false;
        if(pilot.bomb&&!touch(journal_,*pilot.bomb))return false;
    }

    if(world.actors.enemies&&!touch(journal_,*world.actors.enemies))return false;
    if(world.actors.lasers&&!touch(journal_,*world.actors.lasers))return false;
    if(!touch_bullets(journal_,world.actors.bullets)||
       !touch_items(journal_,world.actors.items))return false;
    if(world.actors.spell&&!touch(journal_,*world.actors.spell))return false;
    if(world.actors.gui){
        if(!touch(journal_,*world.actors.gui))return false;
        if(world.actors.gui->dialogue&&!touch(journal_,*world.actors.gui->dialogue))return false;
    }
    if(world.actors.results&&!touch(journal_,*world.actors.results))return false;
    if(world.actors.popups&&!touch(journal_,*world.actors.popups))return false;
    if(world.actors.hints&&!touch(journal_,*world.actors.hints))return false;
    if(world.actors.effects&&!touch(journal_,*world.actors.effects))return false;
    if(!touch_stage(journal_,world.backgrounds.current)||
       !touch_stage(journal_,world.backgrounds.previous))return false;
    return true;
}

bool RollbackState::EndFrame(){
    if(!configured_||!journal_.IsFrameOpen())return false;
    const auto bytes=journal_.BytesForFrame(journal_.OpenFrame());
    if(!journal_.EndFrame())return false;
    last_bytes_=bytes;peak_bytes_=std::max(peak_bytes_,bytes);
    total_bytes_+=bytes;++snapshots_;return true;
}

bool RollbackState::RestoreTo(std::uint32_t frame,std::uint32_t* replayFrom){
    return configured_&&journal_.UndoTo(frame,replayFrom);
}

} // namespace th10::multiplayer
