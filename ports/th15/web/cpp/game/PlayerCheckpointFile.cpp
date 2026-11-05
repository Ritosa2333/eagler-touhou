#include "PlayerCheckpoint.hpp"
#include "AnmManager.hpp"
namespace th15 {
namespace {
constexpr u32 prefix=0x15ccc,origin=0x618;
template<class T>void put(u8* p,u32 at,const T& value){std::memcpy(p+at-origin,&value,sizeof value);}
template<class T>void get(const u8* p,u32 at,T& value){std::memcpy(&value,p+at-origin,sizeof value);}
}
bool PlayerCheckpoint::write_file(std::vector<u8>& bytes,AnmManager& manager){
    error.clear();bytes.clear();if(!available){error="No player checkpoint to serialize";return false;}bytes.resize(prefix,0);auto* b=bytes.data();const auto& s=saved;
    put(b,0x618,s.position);put(b,0x624,s.fixed);put(b,0x62c,s.life_age);put(b,0x640,s.input_age);put(b,0x654,s.ready_age);
    for(u32 i=0;i<8;i++){const u32 at=i*0xe4;const auto& o=s.options[i];put(b,0x668+at,o.active);put(b,0x6bc+at,o.target);put(b,0x6c4+at,o.position);put(b,0x6cc+at,o.normal_offset);put(b,0x6d4+at,o.focus_offset);put(b,0x710+at,s.option_angles[i]);put(b,0x718+at,s.option_handles[i]);put(b,0x738+at,o.index);put(b,0x73c+at,o.snap);}
    put(b,0xd88,s.shots);put(b,0xcd88,s.damage_cursor);put(b,0xcd8c,s.damage);
    put(b,0x16220,s.state);put(b,0x16224,s.focus_handle);put(b,0x16228,s.barrier_handle);put(b,0x1622c,s.barrier_age);put(b,0x16240,s.focus);put(b,0x16244,s.shot_age);put(b,0x16258,s.continuous_age);
    // These native fields overlap: the five saved laser powers intentionally
    // cover option_count. The source capture preserves this exact alias.
    put(b,0x1626c,s.laser_power);put(b,0x16294,s.behavior_flags);
    put(b,0x16298,s.normal_speed);put(b,0x1629c,s.focus_speed);put(b,0x162a0,s.normal_diagonal);put(b,0x162a4,s.focus_diagonal);
    put(b,0x162a8,s.velocity);put(b,0x162b4,s.last_direction);put(b,0x162c0,s.step);put(b,0x162c8,s.follow);put(b,0x162cc,s.collapse);put(b,0x162d0,s.movement_scale);put(b,0x162d4,s.external_velocity);put(b,0x16280,s.invulnerability);put(b,0x162e0,s.power_level);
    auto animation=[&](u32 handle)->bool{if(!handle)return true;if(animations.write_tree(handle,manager,bytes))return true;error=animations.error;return false;};
    for(const auto& shot:s.shots)if(shot.state&&!animation(shot.animation))return false;
    for(u32 i=0;i<8;i++)if(s.options[i].active)for(const u32 h:s.option_handles[i])if(!animation(h))return false;
    return animation(s.focus_handle)&&animation(s.barrier_handle);
}
bool PlayerCheckpoint::read_file(const u8* bytes,u32 size,u32& consumed,AnmManager& manager){
    error.clear();consumed=0;if(!bytes||size<prefix){error="Truncated player checkpoint record";return false;}State s{};
    get(bytes,0x618,s.position);get(bytes,0x624,s.fixed);get(bytes,0x62c,s.life_age);get(bytes,0x640,s.input_age);get(bytes,0x654,s.ready_age);
    for(u32 i=0;i<8;i++){const u32 at=i*0xe4;auto& o=s.options[i];get(bytes,0x668+at,o.active);get(bytes,0x6bc+at,o.target);get(bytes,0x6c4+at,o.position);get(bytes,0x6cc+at,o.normal_offset);get(bytes,0x6d4+at,o.focus_offset);get(bytes,0x710+at,s.option_angles[i]);get(bytes,0x718+at,s.option_handles[i]);get(bytes,0x738+at,o.index);get(bytes,0x73c+at,o.snap);}
    get(bytes,0xd88,s.shots);get(bytes,0xcd88,s.damage_cursor);get(bytes,0xcd8c,s.damage);
    get(bytes,0x16220,s.state);get(bytes,0x16224,s.focus_handle);get(bytes,0x16228,s.barrier_handle);get(bytes,0x1622c,s.barrier_age);get(bytes,0x16240,s.focus);get(bytes,0x16244,s.shot_age);get(bytes,0x16258,s.continuous_age);
    get(bytes,0x1626c,s.laser_power);get(bytes,0x1626c,s.option_count);get(bytes,0x16294,s.behavior_flags);
    get(bytes,0x16298,s.normal_speed);get(bytes,0x1629c,s.focus_speed);get(bytes,0x162a0,s.normal_diagonal);get(bytes,0x162a4,s.focus_diagonal);
    get(bytes,0x162a8,s.velocity);get(bytes,0x162b4,s.last_direction);get(bytes,0x162c0,s.step);get(bytes,0x162c8,s.follow);get(bytes,0x162cc,s.collapse);get(bytes,0x162d0,s.movement_scale);get(bytes,0x162d4,s.external_velocity);get(bytes,0x16280,s.invulnerability);get(bytes,0x162e0,s.power_level);
    struct ImportGuard {AnmCheckpoint& animations;AnmCheckpoint::Mark mark;u32& consumed;bool committed=false;~ImportGuard(){if(!committed){animations.rollback(mark);consumed=0;}}} guard{animations,animations.mark(),consumed};
    consumed=prefix;auto animation=[&](u32& handle)->bool{u32 used=0;const u32 value=animations.read_tree(bytes+consumed,size-consumed,used,manager);if(!value){error=animations.error;return false;}handle=value;consumed+=used;return true;};
    for(auto& shot:s.shots)if(shot.state&&!animation(shot.animation))return false;
    for(u32 i=0;i<8;i++)if(s.options[i].active){if(!animation(s.option_handles[i][0]))return false;if(s.option_handles[i][1]&&!animation(s.option_handles[i][1]))return false;}
    if(s.focus_handle&&!animation(s.focus_handle))return false;if(s.barrier_handle&&!animation(s.barrier_handle))return false;
    saved=std::move(s);available=true;guard.committed=true;return true;
}
}
