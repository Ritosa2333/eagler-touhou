#include "PresentationAudit.hpp"
#ifdef TH_PRESENTATION_AUDIT
#include "GameSession.hpp"
#include "GameBattle.hpp"
#include <algorithm>
#include <cstring>
#include <utility>
#include <vector>
namespace th11::presentation_audit {
namespace {
struct Frame {std::vector<Record> records;u32 tick=0,draw_serial=0,dropped=0;float alpha=1;};
bool enabled_flag=false,reference_frame=false,fault_flag=false,fault_phase=false,rebase_flag=false;
u32 session=0,tick=0,draw_serial=0,draw_id=0,current_scope=0;
Frame current,references[2];
std::vector<std::pair<u64,u32>> occurrences;
u32 digests[StateGroupCount]{};
u32 next_occurrence(u64 key){
    for(auto& [seen,count]:occurrences)if(seen==key)return count++;
    occurrences.emplace_back(key,1);return 0;
}
u32 mix(u32 hash,u32 value){hash^=value;hash*=16777619u;return hash;}
template<class T>u32 hash_value(u32 hash,const T& value){return mix(hash,static_cast<u32>(value));}
u32 hash_float(u32 hash,float value){u32 bits;std::memcpy(&bits,&value,4);return mix(hash,bits);}
u32 hash_vec(u32 hash,const Vec3& value){return hash_float(hash_float(hash_float(hash,value.x),value.y),value.z);}
u32 hash_vec2(u32 hash,const Vec2& value){return hash_float(hash_float(hash,value.x),value.y);}
u32 continuous_fields(const AnmVm& vm){
    u32 fields=0;
    if(vm.rotation_interpolation.duration>0||vm.angular_velocity.x!=0||vm.angular_velocity.y!=0||vm.angular_velocity.z!=0)fields|=1;
    if(vm.scale_interpolation.duration>0||vm.scale_velocity.x!=0||vm.scale_velocity.y!=0)fields|=2;
    if(vm.color_interpolation.duration>0)fields|=4;if(vm.alpha_interpolation.duration>0)fields|=8;
    if(vm.color2_interpolation.duration>0)fields|=16;if(vm.alpha2_interpolation.duration>0)fields|=32;
    if(vm.scroll_velocity.x!=0)fields|=64;if(vm.scroll_velocity.y!=0)fields|=128;
    return fields;
}
void vector3(float* output,const Vec3& value){output[0]=value.x;output[1]=value.y;output[2]=value.z;}
}
void enable(bool value){
    enabled_flag=value;current.records.clear();references[0].records.clear();references[1].records.clear();
    tick=draw_serial=draw_id=0;reference_frame=false;fault_flag=false;rebase_flag=false;current_scope=0;
    if(value){++session;current.records.reserve(MaxRecords);references[0].records.reserve(MaxRecords);references[1].records.reserve(MaxRecords);}
    occurrences.clear();
}
bool enabled()noexcept{return enabled_flag;}
u32 session_epoch()noexcept{return session;}
u32 current_tick()noexcept{return tick;}
u32 current_draw_serial()noexcept{return draw_serial;}
void simulation_tick()noexcept{if(enabled_flag)++tick;}
void begin_reference()noexcept{if(!enabled_flag)return;fault_phase=!fault_phase;reference_frame=true;occurrences.clear();current.records.clear();current.tick=tick;current.draw_serial=++draw_serial;current.alpha=1;current.dropped=0;draw_id=0;}
void begin_sample(float alpha)noexcept{if(!enabled_flag)return;fault_phase=!fault_phase;reference_frame=false;occurrences.clear();current.records.clear();current.tick=tick;current.draw_serial=draw_serial;current.alpha=alpha;current.dropped=0;draw_id=0;}
void end_frame()noexcept{
    if(!enabled_flag)return;
    if(reference_frame){std::swap(references[0],references[1]);std::swap(references[1],current);current.records.clear();current.tick=current.draw_serial=current.dropped=0;current.alpha=1;}
    reference_frame=false;
}
void scope(u32 owner)noexcept{current_scope=owner;}
u32 scope()noexcept{return current_scope;}
void authored_rebase(bool value)noexcept{rebase_flag=value;}
bool authored_rebase()noexcept{return rebase_flag;}
void fault(bool value)noexcept{fault_flag=value;}
void capture(const AnmVm& vm,const AnmVertex* vertices,u32 count,u32 part)noexcept{
    if(!enabled_flag||!vertices||!count)return;
    if(current.records.size()>=MaxRecords){++current.dropped;return;}
    // Render-only frames draw a stack copy; the retained VM id is the identity.
    Record record{};
    record.owner_tag=current_scope?current_scope:vm.layer;
    record.object_id=vm.reserved_420[0]?vm.reserved_420[0]:vm.id;
    record.generation=vm.reserved_420[1]?vm.reserved_420[1]:vm.id;
    // One VM may be submitted several times per frame (tiled stage
    // backgrounds). Keep each submission addressable without collapsing the
    // object: the high half is the in-frame occurrence, the low half the
    // geometry part.
    const u32 occurrence=next_occurrence((u64(record.owner_tag)<<48)|(u64(record.object_id)<<16)|(record.generation&0xffffu));
    record.part=(occurrence<<16)|(part&0xffffu);record.draw_id=draw_id++;
    record.script_index=static_cast<u32>(static_cast<u16>(vm.script_index));
    record.sprite_index=static_cast<u32>(static_cast<u16>(vm.sprite_index));
    record.flags=vm.flags;record.vertex_count=std::min(count,MaxVertices);
    record.continuous=continuous_fields(vm)|(rebase_flag?AuthoredRebase:0u);
    record.file_index=vm.resource?vm.file_index:0xffffu;
    record.layer=vm.layer;
    vector3(record.position,vm.position);vector3(record.script_position,vm.script_position);vector3(record.child_position,vm.child_position);
    vector3(record.rotation,vm.rotation);record.scale[0]=vm.scale.x;record.scale[1]=vm.scale.y;
    record.uv_offset[0]=vm.uv_offset.x;record.uv_offset[1]=vm.uv_offset.y;
    record.color=vm.color;record.secondary_color=vm.secondary_color;
    float left=vertices[0].position.x,top=vertices[0].position.y,right=left,bottom=top;
    for(u32 i=0;i<record.vertex_count;++i){
        const auto& vertex=vertices[i];
        left=std::min(left,vertex.position.x);top=std::min(top,vertex.position.y);
        right=std::max(right,vertex.position.x);bottom=std::max(bottom,vertex.position.y);
        record.vertices[i][0]=vertex.position.x;record.vertices[i][1]=vertex.position.y;record.vertices[i][2]=vertex.position.z;
        record.vertices[i][3]=vertex.uv.x;record.vertices[i][4]=vertex.uv.y;
    }
    record.bounds[0]=left;record.bounds[1]=top;record.bounds[2]=right;record.bounds[3]=bottom;
    // Negative control: perturb a normalized, analyzer-visible field on
    // alternating frames so repeated alpha samples must disagree.
    if(fault_flag&&fault_phase)record.position[0]+=2.f;
    current.records.push_back(record);
}
void sample_state(GameSession& target)noexcept{
    if(!enabled_flag)return;
    u32 state_hash=0xcbf29ce4u;
    state_hash=hash_value(state_hash,static_cast<u32>(target.state.phase));
    state_hash=hash_value(state_hash,target.state.frame);
    state_hash=hash_value(state_hash,target.state.stage);
    state_hash=hash_value(state_hash,static_cast<u32>(target.state.character));
    state_hash=hash_value(state_hash,static_cast<u32>(target.state.difficulty));
    state_hash=hash_value(state_hash,static_cast<u32>(target.economy.score_units));
    state_hash=hash_value(state_hash,static_cast<u32>(target.economy.lives));
    state_hash=hash_value(state_hash,static_cast<u32>(target.economy.power));
    state_hash=hash_value(state_hash,static_cast<u32>(target.economy.graze));
    state_hash=hash_value(state_hash,static_cast<u32>(target.economy.point_value));
    digests[0]=state_hash;

    u32 player_hash=0xcbf29ce4u;
    if(target.battle&&target.battle->player){
        const auto& player=*target.battle->player;
        player_hash=hash_vec(player_hash,player.motion.state.position);
        player_hash=hash_value(player_hash,static_cast<u32>(player.state.life_state));
        player_hash=hash_value(player_hash,static_cast<u32>(player.state.invincibility.current));
        player_hash=hash_value(player_hash,static_cast<u32>(player.state.state_timer.current));
        for(const auto& option:player.motion.options){player_hash=hash_value(player_hash,static_cast<u32>(option.x));player_hash=hash_value(player_hash,static_cast<u32>(option.y));player_hash=hash_float(player_hash,option.angle);player_hash=hash_value(player_hash,static_cast<u32>(option.state));}
    }
    digests[1]=player_hash;

    u32 bullet_hash=0xcbf29ce4u;
    if(target.battle){
        auto& bullets=target.battle->bullets;
        for(u32 i=0;i<BulletManager::capacity;++i){
            const auto& bullet=bullets.at(i);if(!bullet.state)continue;
            bullet_hash=hash_value(bullet_hash,i);bullet_hash=hash_value(bullet_hash,static_cast<u32>(bullet.state));
            bullet_hash=hash_vec(bullet_hash,bullet.position);bullet_hash=hash_vec(bullet_hash,bullet.velocity);
            bullet_hash=hash_float(bullet_hash,bullet.angle);bullet_hash=hash_value(bullet_hash,static_cast<u32>(bullet.lifetime.current));
        }
    }
    digests[2]=bullet_hash;

    u32 item_hash=0xcbf29ce4u;
    if(target.battle){
        auto& items=target.battle->items;
        for(u32 i=0;i<ItemManager::capacity;++i){
            const auto& item=items.at(i);if(!item.state)continue;
            item_hash=hash_value(item_hash,i);item_hash=hash_value(item_hash,static_cast<u32>(item.state));
            item_hash=hash_vec(item_hash,item.position);item_hash=hash_vec(item_hash,item.velocity);
        }
    }
    digests[3]=item_hash;

    u32 laser_hash=0xcbf29ce4u;
    if(target.battle){
        auto& lasers=target.battle->lasers;
        for(auto* laser=lasers.first();laser;laser=laser->next){
            laser_hash=hash_value(laser_hash,static_cast<u32>(laser->state));
            laser_hash=hash_vec(laser_hash,laser->position);laser_hash=hash_float(laser_hash,laser->angle);
            laser_hash=hash_float(laser_hash,laser->length);laser_hash=hash_float(laser_hash,laser->width);
        }
    }
    digests[4]=laser_hash;

    u32 enemy_hash=0xcbf29ce4u;
    if(target.battle){
        for(auto* link=target.battle->enemies.first;link;link=link->next){
            const auto& enemy=link->value->state;
            enemy_hash=hash_vec(enemy_hash,enemy.current.position);
            enemy_hash=hash_value(enemy_hash,static_cast<u32>(enemy.health));
            enemy_hash=hash_value(enemy_hash,static_cast<u32>(enemy.flags));
            enemy_hash=hash_value(enemy_hash,static_cast<u32>(enemy.lifetime.current));
        }
    }
    digests[5]=enemy_hash;

    digests[6]=target.animations.fingerprint();
    u32 camera_hash=hash_vec(0xcbf29ce4u,target.compositor.world_camera.position);
    camera_hash=hash_vec(camera_hash,target.compositor.play_camera.position);
    camera_hash=hash_vec(camera_hash,target.compositor.full_camera.position);
    camera_hash=hash_vec2(camera_hash,target.compositor.world_camera.offset);
    digests[7]=camera_hash;
    u32 cursor_hash=hash_value(0xcbf29ce4u,static_cast<u32>(target.animations.script_rng.seed));
    cursor_hash=hash_value(cursor_hash,target.animations.script_rng.calls);
    cursor_hash=hash_value(cursor_hash,static_cast<u32>(target.animations.visual_rng.seed));
    cursor_hash=hash_value(cursor_hash,target.animations.visual_rng.calls);
    cursor_hash=hash_value(cursor_hash,target.playback.frame());
    cursor_hash=hash_value(cursor_hash,target.playback.selected_stage());
    digests[8]=cursor_hash;
}
const u32* state()noexcept{return digests;}
}
extern "C" {
using namespace th11::presentation_audit;
__attribute__((export_name("audit_enable"))) void audit_enable(th11::u32 value){enable(value!=0);}
__attribute__((export_name("audit_session_epoch"))) th11::u32 audit_session_epoch(){return session_epoch();}
__attribute__((export_name("audit_records"))) const Record* audit_records(){return current.records.data();}
__attribute__((export_name("audit_count"))) th11::u32 audit_count(){return static_cast<th11::u32>(current.records.size());}
__attribute__((export_name("audit_tick"))) th11::u32 audit_tick(){return current.tick;}
__attribute__((export_name("audit_draw_serial"))) th11::u32 audit_draw_serial(){return current.draw_serial;}
__attribute__((export_name("audit_dropped"))) th11::u32 audit_dropped(){return current.dropped;}
__attribute__((export_name("audit_reference"))) const Record* audit_reference(th11::u32 index){return references[index&1].records.data();}
__attribute__((export_name("audit_reference_count"))) th11::u32 audit_reference_count(th11::u32 index){return static_cast<th11::u32>(references[index&1].records.size());}
__attribute__((export_name("audit_reference_tick"))) th11::u32 audit_reference_tick(th11::u32 index){return references[index&1].tick;}
__attribute__((export_name("audit_reference_draw_serial"))) th11::u32 audit_reference_draw_serial(th11::u32 index){return references[index&1].draw_serial;}
__attribute__((export_name("audit_reference_dropped"))) th11::u32 audit_reference_dropped(th11::u32 index){return references[index&1].dropped;}
__attribute__((export_name("audit_stride"))) th11::u32 audit_stride(){return sizeof(Record);}
__attribute__((export_name("audit_scope"))) void audit_scope(th11::u32 owner){scope(owner);}
}
#endif
