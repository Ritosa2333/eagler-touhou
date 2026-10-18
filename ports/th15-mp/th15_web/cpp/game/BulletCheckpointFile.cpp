#include "BulletCheckpoint.hpp"
#include "AnmFile.hpp"
namespace th15 {
namespace {
constexpr u32 stride=0x1494,extension_magic=0x31585442;
template<class T>void put(u8* p,u32 at,const T& v){std::memcpy(p+at,&v,sizeof v);}
template<class T>void get(const u8* p,u32 at,T& v){std::memcpy(&v,p+at,sizeof v);}
u32 word(const u8* p,u32 at){u32 v;get(p,at,v);return v;}
template<class S,class Copy>void fields(S& s,Copy copy){
    copy(0xc38,s.motion);copy(0xc80,s.transform_flags);copy(0xfbc,s.boost);
    copy(0x1004,s.acceleration.timer);copy(0x1018,s.acceleration.speed);copy(0x101c,s.acceleration.angle);copy(0x102c,s.acceleration.vector);copy(0x1038,s.acceleration.duration);
    copy(0x128c,s.approach.timer);copy(0x12a0,s.approach.speed);copy(0x12a4,s.approach.angle);copy(0x12b4,s.approach.vector);copy(0x12c0,s.approach.duration);
    copy(0x104c,s.angular.timer);copy(0x1060,s.angular.speed);copy(0x1064,s.angular.angle);copy(0x1080,s.angular.duration);
    copy(0x10f0,s.reflection.speed);copy(0x10f8,s.reflection.bounds);copy(0x1110,s.reflection.count);copy(0x1114,s.reflection.limit);copy(0x111c,s.reflection.sides);
    copy(0x11a0,s.wrapping.count);copy(0x11a4,s.wrapping.limit);copy(0x11a8,s.wrapping.sides);
    copy(0x1244,s.drift.timer);copy(0x126c,s.drift.velocity);copy(0x1278,s.drift.duration);copy(0x1258,s.drift.speed);copy(0x125c,s.drift.angle);
    copy(0x11fc,s.position_curve.timer);copy(0x1210,s.position_curve.speed);copy(0x1224,s.position_curve.target);copy(0x1230,s.position_curve.duration);copy(0x1234,s.position_curve.mode);copy(0x13b0,s.position_curve.interpolation);
    copy(0xc98,s.transform_sound);copy(0x1094,s.turn.timer);copy(0x10a8,s.turn.speed);copy(0x10ac,s.turn.angle);copy(0x10c8,s.turn.duration);copy(0x10cc,s.turn.limit);copy(0x10d0,s.turn.count);copy(0x10d4,s.turn.mode);copy(0x10d8,s.turn.extra);
    copy(0x1338,s.saved.position);copy(0x1334,s.saved.angle);copy(0x1330,s.saved.speed);copy(0xc9c,s.transform_index);
    copy(0x12d4,s.wait);copy(0x1308,s.wait_outside);copy(0x1364,s.freeze);copy(0x1124,s.invulnerability);copy(0xfec,s.boost_stage);copy(0xc64,s.step_limit);copy(0x24,s.collision_delay);
    copy(0xc58,s.hitbox);copy(0xc78,s.offscreen_grace);copy(0xc7c,s.cancellation_script);copy(0xc84,s.initial_transform_flags);copy(0x20,s.flags);copy(0x40,s.visual_flags);copy(0x60,s.visual_jitter);copy(0x4c4,s.hit_interrupt);copy(0x564,s.graze_color);
    copy(0x1408,s.scale_curve);copy(0x1438,s.scale);copy(0x143c,s.graze_flash);copy(0x1450,s.graze_duration);copy(0x1464,s.graze_interval);copy(0x1468,s.spawn);copy(0x147c,s.lifetime);copy(0x1490,s.sprite_type);copy(0x1492,s.color);
    copy(0xca0,s.cancel_item);
    for(u32 i=0;i<18;i++){const u32 at=0xca4+i*44;auto& t=s.transforms[i];copy(at,t.floats);copy(at+16,t.integers);copy(at+32,t.type);copy(at+36,t.active);}
}
BulletState empty_state(){BulletState s;const Timer zero{0,0,0,0,0};s.boost=s.acceleration.timer=s.approach.timer=s.angular.timer=s.drift.timer=s.position_curve.timer=s.turn.timer=s.wait=s.invulnerability=s.freeze=s.lifetime=s.spawn=s.graze_flash=s.graze_duration=zero;s.reflection.bounds={};s.transform_sound=0;s.phase=0;s.scale=0;s.cancellation_script=0;s.graze_interval=0;return s;}
}
bool BulletCheckpoint::write_file(std::vector<u8>& output){
    error.clear();output.clear();if(!available){error="No bullet checkpoint to serialize";return false;}output.resize(4);AnmFile codec;u32 count=0;std::vector<const Slot*> packed;
    for(const auto& slot:slots)if(slot.state.flags&1){const u32 at=output.size();output.resize(at+stride,0);const auto& s=slot.state;
        for(u32 i=0;i<2;i++){const auto& vm=i?slot.overlay:slot.body;AnmFile::Block block;if(!codec.write(vm,animations,block,true,true)){error=codec.error;output.clear();return false;}std::memcpy(output.data()+at+(i?0x630:0x28),block.data(),block.size());}
        fields(s,[&](u32 offset,const auto& value){put(output.data()+at,offset,value);});put(output.data()+at,0xc8a,i16(s.phase));
        for(const auto& vm:{&slot.body,&slot.overlay})if(vm->geometry.allocation_bytes){AnmFile::Block record;if(!codec.write(*vm,animations,record,true)){error=codec.error;output.clear();return false;}output.insert(output.end(),record.begin(),record.end());if(!codec.write_geometry(*vm,animations,output,{})){error=codec.error;output.clear();return false;}}
        packed.push_back(&slot);count++;
    }put(output.data(),0,count);const u32 trailer=output.size();output.resize(trailer+16,0);put(output.data(),trailer,reflection_bounds);put(output.data(),trailer+8,reward_count);u32 effect_count=0;
    for(const u32 handle:effects)if(handle){if(!pool.write_tree(handle,animations,output)){error=pool.error;output.clear();return false;}effect_count++;}put(output.data(),trailer+12,effect_count);
    // Native transform records carry borrowed routine-string addresses. An
    // optional bounded tail stores their actual bytes for portable restarts.
    // Original module readers stop after their counted animation records.
    u32 payloads=0;for(const auto* slot:packed)for(const auto& t:slot->state.transforms)if(!t.payload.empty())payloads++;
    if(payloads){const u32 at=output.size();output.resize(at+8);put(output.data(),at,extension_magic);put(output.data(),at+4,payloads);for(u32 i=0;i<packed.size();i++)for(u32 j=0;j<18;j++){const auto& p=packed[i]->state.transforms[j].payload;if(p.empty())continue;if(p.size()>65536){error="Bullet checkpoint transform payload exceeds limit";output.clear();return false;}const u32 at=output.size();output.resize(at+12);put(output.data(),at,i);put(output.data(),at+4,j);put(output.data(),at+8,u32(p.size()));output.insert(output.end(),p.begin(),p.end());}}
    return true;
}
bool BulletCheckpoint::read_file(const u8* data,u32 size,u32& consumed){
    error.clear();consumed=0;if(!data||size<20){error="Truncated bullet checkpoint counts";return false;}const u32 count=word(data,0);if(count>BulletManager::capacity){error="Bullet checkpoint count exceeds pool capacity";return false;}std::vector<Slot> next(BulletManager::capacity);for(auto& slot:next)slot.state=empty_state();AnmFile codec;u32 offset=4;
    auto animation=[&](const u8* fixed,AnmVm& vm,bool unbound)->bool{if(word(fixed,0x18)==0&&word(fixed,0x28)==0&&word(fixed,0x30)==0&&word(fixed,0x34)==0&&word(fixed,0x5c8)==0)return true;const bool geometry=word(fixed,0x5c8)!=0;const auto* record=geometry?data+offset:fixed;const u32 bytes=geometry?size-offset:0x608;if(!codec.read(record,bytes,vm,animations,true,unbound)){error=codec.error;return false;}if(geometry){u32 used=0;if(!codec.read_geometry(record,record+0x608,bytes-0x608,vm,animations,used,{})){error=codec.error;return false;}offset+=0x608+used;}return true;};
    for(u32 i=0;i<count;i++){if(size-offset<stride){error="Truncated bullet checkpoint slot";return false;}const auto* record=data+offset;offset+=stride;auto& slot=next[i];auto& s=slot.state;fields(s,[&](u32 at,auto& value){get(record,at,value);});i16 phase;get(record,0xc8a,phase);s.phase=phase;if(!(s.flags&1)){error="Inactive record in packed bullet checkpoint";return false;}
        if(!animation(record+0x28,slot.body,false)||!animation(record+0x630,slot.overlay,!bullet_appearance(s.sprite_type)||bullet_appearance(s.sprite_type)->overlay==0))return false;s.spawn_animation_finished=slot.body.variables.integers[0]!=0;const auto* appearance=bullet_appearance(s.sprite_type);s.overlay_active=appearance&&appearance->overlay!=0;
    }
    if(size-offset<16){error="Missing bullet checkpoint trailer";return false;}Vec2 bounds;get(data,offset,bounds);i32 rewards;get(data,offset+8,rewards);const u32 effect_count=word(data,offset+12);offset+=16;if(effect_count>effects.size()){error="Bullet checkpoint effect count exceeds pool capacity";return false;}
    struct ImportGuard{AnmCheckpoint& pool;AnmCheckpoint::Mark mark;bool committed=false;~ImportGuard(){if(!committed)pool.rollback(mark);}} guard{pool,pool.mark()};std::array<u32,BulletManager::capacity> next_effects{};
    for(u32 i=0;i<effect_count;i++){u32 used=0;next_effects[i]=pool.read_tree(data+offset,size-offset,used,animations);if(!next_effects[i]){error=pool.error;return false;}offset+=used;}
    if(size-offset>=8&&word(data,offset)==extension_magic){const u32 records=word(data,offset+4);offset+=8;if(records>count*18){error="Bullet checkpoint payload count exceeds transform capacity";return false;}for(u32 i=0;i<records;i++){if(size-offset<12){error="Truncated bullet checkpoint payload header";return false;}const u32 slot=word(data,offset),transform=word(data,offset+4),length=word(data,offset+8);offset+=12;if(slot>=count||transform>=18||length>65536||length>size-offset||!next[slot].state.transforms[transform].payload.empty()){error="Invalid bullet checkpoint transform payload";return false;}auto& p=next[slot].state.transforms[transform].payload;p.assign(data+offset,data+offset+length);offset+=length;}}
    for(u32 i=0;i<count;i++)for(const auto& t:next[i].state.transforms)if(t.type==16777216&&(t.payload.empty()||t.payload.back()!=0)){error="Enemy routine in original bullet checkpoint lacks portable string data";return false;}
    slots=std::move(next);effects=next_effects;reflection_bounds=bounds;reward_count=rewards;available=true;consumed=offset;guard.committed=true;return true;
}
}
