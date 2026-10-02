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
                RollbackPool<BlockSize,Capacity>& pool,
                RollbackPoolCapture<Capacity>& capture){
    return capture.Capture(pool,[&](void* p,std::size_t size){return journal.Touch(p,size);});
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

bool touch_animation_manager(Netplay::RollbackJournal& journal,browser::AnimationEngine& engine,
                             Netplay::SparsePoolCapture<4096>& capture){
    auto& manager=engine.manager;
    if(!journal.Touch(&manager.started_scripts,sizeof(manager.started_scripts))||
       !journal.Touch(&manager.processed_count,sizeof(manager.processed_count))||
       !journal.Touch(manager.occupied,sizeof(manager.occupied))||
       !journal.Touch(&manager.cursor,sizeof(manager.cursor))||
       !touch(journal,manager.registry)||
       !journal.Touch(manager.draw_layers,sizeof(manager.draw_layers))||
       !journal.Touch(&manager.last_id,sizeof(manager.last_id)))return false;
    if(!capture.Capture(manager.pool,[&](const AnmVm& vm){return manager.occupied[&vm-manager.pool]!=0;},
        [&](void* p,std::size_t size){return journal.Touch(p,size);}))return false;
    return true;
}

bool touch_bullets(Netplay::RollbackJournal& journal,EnemyBulletManager* manager,
                   Netplay::SparsePoolCapture<2000>& capture){
    if(!manager)return true;
    if(!journal.Touch(manager,offsetof(EnemyBulletManager,pool))||
       !journal.Touch(&manager->animation_file,sizeof(manager->animation_file)))return false;
    return capture.Capture(manager->pool,[](const EnemyBullet& bullet){return bullet.state!=0;},
        [&](void* p,std::size_t size){return journal.Touch(p,size);});
}

bool touch_items(Netplay::RollbackJournal& journal,ItemManager* manager,
                 Netplay::SparsePoolCapture<150>& regular,Netplay::SparsePoolCapture<2048>& faith){
    if(!manager)return true;
    if(!journal.Touch(manager,offsetof(ItemManager,regular))||
       !journal.Touch(&manager->active_count,sizeof(manager->active_count)+
                                             sizeof(manager->faith_cursor)+
                                             sizeof(manager->faith_count)))return false;
    const auto live=[](const Item& item){return item.state!=0;};
    const auto save=[&](void* p,std::size_t size){return journal.Touch(p,size);};
    return regular.Capture(manager->regular,live,save)&&faith.Capture(manager->faith,live,save);
}

template<class T,std::size_t Capacity>
bool pool_slot(T* pool,void* address,std::size_t bytes){
    if(!pool||bytes!=sizeof(T))return false;
    const auto begin=reinterpret_cast<std::uintptr_t>(pool);
    const auto value=reinterpret_cast<std::uintptr_t>(address);
    return value>=begin&&value-begin<sizeof(T)*Capacity&&(value-begin)%sizeof(T)==0;
}

} // namespace

bool RollbackState::Reset(){
    Clear();
    configured_=journal_.Reset(Netplay::RollbackJournalConfig{
        14,20u*1024u*1024u,24000,true,true});
    return configured_;
}

void RollbackState::Clear(){
    journal_.Clear();configured_=false;
    frame_open_=false;open_frame_=elided_frames_=0;
    animation_pool_=nullptr;bullet_pool_=nullptr;regular_pool_=faith_pool_=nullptr;
    ClearPoolCaptures();
    last_bytes_=peak_bytes_=last_blocks_=0;total_bytes_=0;snapshots_=0;
}

void RollbackState::ClearPoolCaptures(){
    overflow_capture_.Clear();geometry_capture_.Clear();callback_capture_.Clear();
    enemy_capture_.Clear();laser_capture_.Clear();ecl_capture_.Clear();
    hint_capture_.Clear();effect_capture_.Clear();dialogue_capture_.Clear();
}

bool RollbackState::Touch(void* address,std::size_t bytes){
    if(!configured_||!journal_.IsFrameOpen())return true;
    const auto save=[&](void* p,std::size_t size){return journal_.Touch(p,size);};
    if(pool_slot<AnmVm,4096>(animation_pool_,address,bytes))
        return animation_capture_.TouchSlot(animation_pool_,static_cast<AnmVm*>(address),save);
    if(pool_slot<EnemyBullet,2000>(bullet_pool_,address,bytes))
        return bullet_capture_.TouchSlot(bullet_pool_,static_cast<EnemyBullet*>(address),save);
    if(pool_slot<Item,150>(regular_pool_,address,bytes))
        return regular_capture_.TouchSlot(regular_pool_,static_cast<Item*>(address),save);
    if(pool_slot<Item,2048>(faith_pool_,address,bytes))
        return faith_capture_.TouchSlot(faith_pool_,static_cast<Item*>(address),save);
    if(overflow_capture_.OwnsSlot(address,bytes))return overflow_capture_.TouchSlot(address,bytes,save);
    if(geometry_capture_.OwnsSlot(address,bytes))return geometry_capture_.TouchSlot(address,bytes,save);
    if(callback_capture_.OwnsSlot(address,bytes))return callback_capture_.TouchSlot(address,bytes,save);
    if(enemy_capture_.OwnsSlot(address,bytes))return enemy_capture_.TouchSlot(address,bytes,save);
    if(laser_capture_.OwnsSlot(address,bytes))return laser_capture_.TouchSlot(address,bytes,save);
    if(ecl_capture_.OwnsSlot(address,bytes))return ecl_capture_.TouchSlot(address,bytes,save);
    if(hint_capture_.OwnsSlot(address,bytes))return hint_capture_.TouchSlot(address,bytes,save);
    if(effect_capture_.OwnsSlot(address,bytes))return effect_capture_.TouchSlot(address,bytes,save);
    if(dialogue_capture_.OwnsSlot(address,bytes))return dialogue_capture_.TouchSlot(address,bytes,save);
    return address&&bytes&&journal_.Touch(address,bytes);
}

bool RollbackState::BeginFrame(browser::World& world,std::uint32_t frame,bool capture){
    if(!configured_&&!Reset())return false;
    if(frame_open_||journal_.Failed())return false;
    frame_open_=true;open_frame_=frame;
    if(!capture)return true;
    if(!journal_.BeginFrame(frame))return false;
    ClearPoolCaptures();

    auto& state=world.state;
    auto& engine=world.engine;
    animation_pool_=engine.manager.pool;
    bullet_pool_=world.actors.bullets?world.actors.bullets->pool:nullptr;
    regular_pool_=world.actors.items?world.actors.items->regular:nullptr;
    faith_pool_=world.actors.items?world.actors.items->faith:nullptr;
    if(!touch(journal_,state.team_economy)||
       !journal_.Touch(state.pilot_economies,sizeof(state.pilot_economies))||
       !touch(journal_,state.input_lanes)||
       !touch(journal_,state.multiplayer_cheat_movement_used)||
       !touch(journal_,state.multiplayer_replay.save)||
       !touch(journal_,world.replay_checkpoint_pending)||
       !touch(journal_,world.replay_checkpoint_pending_valid)||
       !touch(journal_,world.replay_checkpoint_commit_pending)||
       !touch(journal_,world.replay_checkpoint_commit_frame)||
       !touch(journal_,world.replay_checkpoint_commit_valid)||
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
       !touch_animation_manager(journal_,engine,animation_capture_)||
       !touch_pool(journal_,engine.rollback_animation_overflow,overflow_capture_)||
       !touch_pool(journal_,engine.rollback_geometry,geometry_capture_)||
       !touch_pool(journal_,engine.callback_environment.rollback_entries,callback_capture_)||
       !touch_pool(journal_,world.rollback_enemies,enemy_capture_)||
       !touch_pool(journal_,world.rollback_lasers,laser_capture_)||
       !touch_pool(journal_,world.rollback_ecl,ecl_capture_)||
       !touch_pool(journal_,world.rollback_hints,hint_capture_)||
       !touch_pool(journal_,world.effects.rollback_effects,effect_capture_))return false;

    // These owners survive the title screen. Their loading/introduction ANM
    // handles and fixed-tick counters are read by GameSession and authored
    // Draw; restoring only the ANM registry leaves handles from the future.
    if(state.application.startup&&!touch(journal_,*state.application.startup))return false;
    if(world.common.value&&!touch(journal_,*world.common.value))return false;
    if(world.hud&&!touch(journal_,world.hud->last_multiplayer_hud_frame))return false;
    if(world.hud&&!touch_pool(journal_,world.hud->rollback_dialogues,dialogue_capture_))return false;
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
    if(!touch_bullets(journal_,world.actors.bullets,bullet_capture_)||
       !touch_items(journal_,world.actors.items,regular_capture_,faith_capture_))return false;
    if(world.actors.spell&&!touch(journal_,*world.actors.spell))return false;
    if(world.actors.gui){
        if(!touch(journal_,*world.actors.gui))return false;
        // The HUD pool may already cover this dialogue inside a multi-slot
        // run. Use the slot coverage path; retain a journal fallback for an
        // independently owned dialogue.
        if(world.actors.gui->dialogue&&
           !Touch(world.actors.gui->dialogue,sizeof(*world.actors.gui->dialogue)))return false;
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
    if(!configured_||!frame_open_||journal_.Failed())return false;
    frame_open_=false;
    if(!journal_.IsFrameOpen()){
        last_bytes_=last_blocks_=0;++elided_frames_;return true;
    }
    const auto bytes=journal_.BytesForFrame(journal_.OpenFrame());
    last_blocks_=journal_.BlocksForFrame(journal_.OpenFrame());
    if(!journal_.EndFrame())return false;
    last_bytes_=bytes;peak_bytes_=std::max(peak_bytes_,bytes);
    total_bytes_+=bytes;++snapshots_;return true;
}

bool RollbackState::RestoreTo(std::uint32_t frame,std::uint32_t* replayFrom){
    return configured_&&!frame_open_&&journal_.UndoTo(frame,replayFrom);
}

} // namespace th10::multiplayer
