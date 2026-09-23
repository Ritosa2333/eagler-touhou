#include "../platform/Application.hpp"
#include <eagler/netplay/NetplayProtocol.hpp>
#include <vector>
#include <cmath>
#include <cstddef>

using namespace th10;
namespace {
u32 hash_bytes(const void* data,std::size_t bytes,u32 hash=2166136261u){
    const auto* p=static_cast<const u8*>(data);
    for(std::size_t i=0;i<bytes;++i){hash^=p[i];hash*=16777619u;}
    return hash;
}
template<class T>u32 hash_value(const T& value,u32 hash=2166136261u){
    return hash_bytes(&value,sizeof(value),hash);
}
template<std::size_t BlockSize,std::size_t Capacity>
u32 hash_pool(const multiplayer::RollbackPool<BlockSize,Capacity>& pool,u32 hash=2166136261u){
    hash=hash_bytes(pool.occupied,sizeof(pool.occupied),hash);
    hash=hash_value(pool.cursor,hash);
    for(std::size_t i=0;i<Capacity;++i)if(pool.active(i)){
        hash=hash_value(u32(i),hash);hash=hash_bytes(pool.at(i),BlockSize,hash);
    }
    return hash;
}
u32 hash_bullets(const EnemyBulletManager* manager){
    if(!manager)return 0;u32 hash=2166136261u;
    hash=hash_bytes(manager,offsetof(EnemyBulletManager,pool),hash);
    for(u32 i=0;i<2000;++i)if(manager->pool[i].state){
        hash=hash_value(i,hash);hash=hash_value(manager->pool[i],hash);
    }
    return hash_value(manager->animation_file,hash);
}
u32 hash_items(const ItemManager* manager){
    if(!manager)return 0;u32 hash=2166136261u;
    hash=hash_bytes(manager,offsetof(ItemManager,regular),hash);
    for(u32 i=0;i<150;++i)if(manager->regular[i].state){hash=hash_value(i,hash);hash=hash_value(manager->regular[i],hash);}
    for(u32 i=0;i<2048;++i)if(manager->faith[i].state){hash=hash_value(i,hash);hash=hash_value(manager->faith[i],hash);}
    hash=hash_value(manager->active_count,hash);hash=hash_value(manager->faith_cursor,hash);hash=hash_value(manager->faith_count,hash);return hash;
}
u32 hash_stage(const Stage* stage){
    if(!stage)return 0;u32 hash=hash_value(*stage);
    if(stage->object_animations&&stage->file&&stage->file->primitive_count>0)
        hash=hash_bytes(stage->object_animations,std::size_t(stage->file->primitive_count)*sizeof(AnmVm),hash);
    return hash;
}
u32 hash_anm_vm_authored(const AnmVm& vm,u32 hash){
    hash=hash_value(vm.id,hash);hash=hash_value(vm.owner_tag,hash);
    hash=hash_value(vm.rotation,hash);hash=hash_value(vm.angular_velocity,hash);
    hash=hash_value(vm.scale,hash);hash=hash_value(vm.scale_velocity,hash);
    hash=hash_value(vm.sprite_size,hash);hash=hash_value(vm.uv_offset,hash);
    hash=hash_value(vm.script_timer,hash);hash=hash_value(vm.script_timer_flags,hash);
    hash=hash_value(vm.position_interpolation,hash);hash=hash_value(vm.color_interpolation,hash);
    hash=hash_value(vm.alpha_interpolation,hash);hash=hash_value(vm.rotation_interpolation,hash);
    hash=hash_value(vm.scale_interpolation,hash);hash=hash_value(vm.color2_interpolation,hash);
    hash=hash_value(vm.alpha2_interpolation,hash);hash=hash_value(vm.uv_velocity,hash);
    hash=hash_value(vm.color,hash);hash=hash_value(vm.secondary_color,hash);
    hash=hash_value(vm.pending_interrupt,hash);hash=hash_value(vm.animation_file,hash);
    hash=hash_bytes(vm.integer_variables,sizeof(vm.integer_variables),hash);
    hash=hash_bytes(vm.float_variables,sizeof(vm.float_variables),hash);
    hash=hash_bytes(vm.extra_integer_variables,sizeof(vm.extra_integer_variables),hash);
    hash=hash_value(vm.script_position,hash);hash=hash_value(vm.position,hash);hash=hash_value(vm.child_position,hash);
    const u32 authoredFlags=vm.flags&~12u;hash=hash_value(authoredFlags,hash);
    hash=hash_value(vm.saved_timer,hash);hash=hash_value(vm.saved_timer_flags,hash);
    hash=hash_value(vm.saved_instruction,hash);hash=hash_value(vm.sprite_frame,hash);
    hash=hash_value(vm.sprite_index,hash);hash=hash_value(vm.file_index,hash);
    hash=hash_value(vm.script_index,hash);hash=hash_value(vm.script_begin,hash);
    hash=hash_value(vm.instruction,hash);hash=hash_value(vm.sprite,hash);return hash;
}
struct AnmHashes {u32 metadata=2166136261u,pooled=2166136261u,overflow=2166136261u;};
AnmHashes hash_anm_parts(const browser::AnimationEngine& engine){
    const auto& manager=engine.manager;AnmHashes result;
    result.metadata=hash_value(manager.started_scripts,result.metadata);
    result.metadata=hash_value(manager.processed_count,result.metadata);
    result.metadata=hash_bytes(manager.occupied,sizeof(manager.occupied),result.metadata);
    result.metadata=hash_value(manager.cursor,result.metadata);
    result.metadata=hash_value(manager.registry,result.metadata);
    result.metadata=hash_value(manager.last_id,result.metadata);
    for(u32 i=0;i<4096;++i)if(manager.occupied[i]){
        result.pooled=hash_value(i,result.pooled);
        result.pooled=hash_anm_vm_authored(manager.pool[i],result.pooled);
    }
    for(u32 i=0;i<2048;++i)if(engine.rollback_animation_overflow.active(i)){
        result.overflow=hash_value(i,result.overflow);
        result.overflow=hash_anm_vm_authored(*static_cast<const AnmVm*>(engine.rollback_animation_overflow.at(i)),result.overflow);
    }
    return result;
}
u32 hash_anm(const AnmHashes& parts){
    u32 hash=hash_value(parts.metadata);hash=hash_value(parts.pooled,hash);return hash_value(parts.overflow,hash);
}
bool decode_frame_input(const u32* row,Netplay::FrameInput& input){
    if(!row||row[0]>65535||row[1]>4||row[4]>7)return false;
    input.buttons=u16(row[0]);input.analogMode=Netplay::AnalogMode(row[1]);
    std::memcpy(&input.x,row+2,4);std::memcpy(&input.y,row+3,4);
    input.unlimited=(row[4]&1)!=0;input.touchUsed=(row[4]&2)!=0;input.touchBomb=(row[4]&4)!=0;
    return std::isfinite(input.x)&&std::isfinite(input.y);
}
}
extern "C" __attribute__((export_name("multiplayer_commit_inputs")))
u32 multiplayer_commit_inputs(browser::Application* app,const u32* words,u32 count){
    if(!app||app->stopped||!app->state.multiplayer_session.configured||!words||
       count!=app->state.multiplayer_session.playerCount*5)return 0;
    if(app->state.multiplayer_session.sessionId&&app->multiplayer_active())return 0;
    Netplay::FrameInput inputs[3]{};
    const auto seats=app->state.multiplayer_session.playerCount;
    for(u32 seat=0;seat<seats;++seat){
        const auto* row=words+seat*5;
        if(!decode_frame_input(row,inputs[seat]))return 0;
    }
    return multiplayer::InputLanes::Commit(app->state.input_lanes,inputs,seats)?1:0;
}
extern "C" __attribute__((export_name("multiplayer_configure")))
u32 multiplayer_configure(browser::Application* app,const u32* words,u32 count){
    if(!app||app->stopped||app->world||app->value.screen!=-2)return 0;
    multiplayer::SessionSetup next=app->state.multiplayer_session;
    if(!multiplayer::DecodeSessionSetup(next,words,count))return 0;
    if(next.sessionId&&!app->state.netplay_runtime.Reset(next,next.sessionId))return 0;
    app->state.multiplayer_session=next;return 1;
}
extern "C" __attribute__((export_name("multiplayer_contract")))
u32 multiplayer_contract(browser::Application* app){
    if(!app||!app->state.multiplayer_session.configured)return 0;
    return multiplayer::GameplayContract(app->state.multiplayer_session);
}
extern "C" __attribute__((export_name("multiplayer_session_build")))
u32 multiplayer_session_build(browser::Application* app,u32 phase,u8* out,u32 capacity){
    if(!app||!app->state.netplay_runtime.Configured()||(phase!=1&&phase!=2))return 0;
    std::vector<u8> wire;
    const auto packet=phase==1?app->state.netplay_runtime.Hello():app->state.netplay_runtime.Ready();
    if(!Netplay::EncodeSessionPacket(packet,&wire)||wire.size()>capacity||(!out&&!wire.empty()))return 0;
    if(!wire.empty())std::memcpy(out,wire.data(),wire.size());return u32(wire.size());
}
extern "C" __attribute__((export_name("multiplayer_session_apply")))
u32 multiplayer_session_apply(browser::Application* app,const u8* bytes,u32 size){
    if(!app||!app->state.netplay_runtime.Configured()||!bytes||!size)return 0;
    Netplay::SessionPacket packet{};if(!Netplay::DecodeSessionPacket(bytes,size,&packet))return 0;
    const auto result=app->state.netplay_runtime.ApplySession(packet);
    return result==Netplay::SessionPacketResult::Accepted||result==Netplay::SessionPacketResult::Duplicate?1:0;
}
extern "C" __attribute__((export_name("multiplayer_session_mark_ready")))
u32 multiplayer_session_mark_ready(browser::Application* app){
    if(!app||!app->state.netplay_runtime.Configured()||!app->state.netplay_runtime.CanSendReady())return 0;
    app->state.netplay_runtime.MarkLocalReady();return 1;
}
extern "C" __attribute__((export_name("multiplayer_session_can_start")))
u32 multiplayer_session_can_start(browser::Application* app){
    return app&&app->state.netplay_runtime.CanStart()?1:0;
}
extern "C" __attribute__((export_name("multiplayer_submit_remote")))
u32 multiplayer_submit_remote(browser::Application* app,u32 player,u32 frame,
                              const u32* words,u32 count){
    if(!app||!app->state.netplay_runtime.Configured()||count!=5||
       player>=app->state.multiplayer_session.playerCount||
       player==app->state.multiplayer_session.localPlayer)return 0;
    Netplay::FrameInput input{};if(!decode_frame_input(words,input))return 0;
    const auto result=app->state.netplay_runtime.SubmitRemote(
        static_cast<std::uint8_t>(player),frame,input);
    return static_cast<u32>(result)+1;
}
extern "C" __attribute__((export_name("multiplayer_netplay_status")))
const i32* multiplayer_netplay_status(browser::Application* app){
    static i32 words[11]{};std::fill(words,words+11,0);if(!app)return words;
    auto& runtime=app->state.netplay_runtime;
    words[0]=runtime.Configured();words[1]=runtime.CanStart();
    words[2]=i32(runtime.NextFrame());words[3]=i32(runtime.LastSimulatedFrame());
    words[4]=i32(runtime.ConfirmedThroughAllRemotes());
    words[5]=runtime.HasRollbackRequest()?i32(runtime.RollbackFrame()):-1;
    words[6]=app->world?i32(app->world->rollback.CapturedBytes(
        runtime.LastSimulatedFrame()==Netplay::INVALID_FRAME?0:runtime.LastSimulatedFrame())):0;
    const auto sessionId=app->state.multiplayer_session.sessionId;
    words[7]=i32(u32(sessionId));words[8]=i32(u32(sessionId>>32));
    words[9]=app->multiplayer_active()?1:0;
    words[10]=app->world&&app->world->actors.session?1:0;
    return words;
}
extern "C" __attribute__((export_name("multiplayer_canonical_hashes")))
const u32* multiplayer_canonical_hashes(browser::Application* app){
    static u32 words[28]{};std::fill(words,words+28,0);if(!app||!app->world)return words;
    auto& w=*app->world;u32 economy=hash_value(app->state.team_economy);
    economy=hash_bytes(app->state.pilot_economies,sizeof(app->state.pilot_economies),economy);
    economy=hash_value(w.cooperation,economy);economy=hash_value(app->state.input_lanes,economy);
    u32 rng=hash_value(app->engine.script_random);rng=hash_value(app->engine.visual_random,rng);
    u32 players=2166136261u;
    for(u32 seat=0;seat<w.player_count;++seat){players=hash_value(w.pilots[seat].input_keys,players);if(w.pilots[seat].player)players=hash_value(*w.pilots[seat].player,players);if(w.pilots[seat].bomb)players=hash_value(*w.pilots[seat].bomb,players);}
    u32 enemies=w.actors.enemies?hash_value(*w.actors.enemies):0;enemies=hash_pool(w.rollback_enemies,enemies);enemies=hash_pool(w.rollback_ecl,enemies);
    u32 lasers=w.actors.lasers?hash_value(*w.actors.lasers):0;lasers=hash_pool(w.rollback_lasers,lasers);
    u32 items=hash_items(w.actors.items);items=hash_bytes(w.regular_item_owners,sizeof(w.regular_item_owners),items);items=hash_bytes(w.faith_item_owners,sizeof(w.faith_item_owners),items);
    u32 scene=2166136261u;if(w.actors.session)scene=hash_value(*w.actors.session,scene);if(w.actors.spell)scene=hash_value(*w.actors.spell,scene);if(w.actors.gui)scene=hash_value(*w.actors.gui,scene);if(w.actors.results)scene=hash_value(*w.actors.results,scene);if(w.actors.popups)scene=hash_value(*w.actors.popups,scene);if(w.actors.hints)scene=hash_value(*w.actors.hints,scene);if(w.actors.effects)scene=hash_value(*w.actors.effects,scene);scene=hash_value(hash_stage(w.backgrounds.current),scene);scene=hash_value(hash_stage(w.backgrounds.previous),scene);
    u32 chain=hash_value(app->engine.chain_value);chain=hash_pool(app->engine.callback_environment.rollback_entries,chain);chain=hash_pool(app->effects.rollback_effects,chain);
    const auto anm=hash_anm_parts(app->engine);
    words[0]=1;words[2]=economy;words[3]=rng;words[4]=players;words[5]=enemies;words[6]=hash_bullets(w.actors.bullets);words[7]=lasers;words[8]=items;words[9]=hash_anm(anm);words[10]=scene;words[11]=chain;
    u32 composite=2166136261u;for(u32 i=2;i<12;++i)composite=hash_value(words[i],composite);words[1]=composite;words[12]=app->state.multiplayer_session.started;
    words[13]=app->engine.script_random.seed;words[14]=app->engine.script_random.calls;
    words[15]=app->engine.visual_random.seed;words[16]=app->engine.visual_random.calls;
    words[17]=anm.metadata;words[18]=anm.pooled;words[19]=anm.overflow;
    auto& manager=app->engine.manager;u32 occupied=0,worldCount=0,uiCount=0,overflowCount=0;
    for(const auto value:manager.occupied)occupied+=value?1u:0u;
    for(auto* node=manager.registry.world_head;node;node=node->next)++worldCount;
    for(auto* node=manager.registry.ui_head;node;node=node->next)++uiCount;
    for(u32 i=0;i<2048;++i)overflowCount+=app->engine.rollback_animation_overflow.active(i)?1u:0u;
    words[20]=manager.started_scripts;words[21]=manager.processed_count;words[22]=u32(manager.cursor);words[23]=manager.last_id;
    words[24]=occupied;words[25]=worldCount;words[26]=uiCount;words[27]=overflowCount;
    return words;
}
extern "C" __attribute__((export_name("multiplayer_status")))
const i32* multiplayer_status(browser::Application* app){
    static i32 words[44];std::memset(words,0,sizeof(words));
    if(!app)return words;
    const auto& setup=app->state.multiplayer_session;
    words[0]=1;words[1]=i32(setup.playerCount);words[2]=i32(setup.localPlayer);
    words[3]=setup.started;words[4]=app->world&&app->world->loading;
    words[5]=app->error;words[6]=app->state.game.stage;
    words[7]=i32(app->state.game.stage_frames);
    if(auto* world=app->world)for(u32 seat=0;seat<world->player_count;++seat){
        const auto& pilot=world->pilots[seat];auto* out=words+8+seat*12;
        out[0]=pilot.game.character;out[1]=pilot.game.shot_type;
        out[2]=pilot.game.lives;out[3]=pilot.game.power;
        if(pilot.player){
            out[4]=pilot.player->state;out[5]=pilot.player->fixed_position.x;
            out[6]=pilot.player->fixed_position.y;out[7]=pilot.player->focused;
        }
        out[8]=pilot.bomb?pilot.bomb->active:0;
        out[9]=i32(world->cooperation.seats[seat].lifeState);
        out[10]=world->cooperation.seats[seat].rescueTicks;
        out[11]=i32(pilot.input_keys);
    }
    return words;
}
