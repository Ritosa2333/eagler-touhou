#pragma once
#include "AnmTransferTh14.hpp"
#include "ShtResource.hpp"
#include <cmath>
#if PORT_TITLE == 14
namespace th11 {
namespace sht_transfer_th14 {
using anm_transfer_th14::Reader;
using anm_transfer_th14::Writer;
constexpr u32 maximum_bytes=4u*1024*1024,header_bytes=36,header_words=161,spec_words=27;
constexpr u32 maximum_groups=(0x108u-0xe0u)/4;
inline i8 byte(Reader& r){const auto word=r.word();const auto value=i8(word);if(word!=u32(i32(value)))r.ok=false;return value;}
inline void pair(Writer& w,const Vec2& value){w.scalar(value.x);w.scalar(value.y);}
inline void pair(Reader& r,Vec2& value){value.x=r.scalar();value.y=r.scalar();}
}
// Explicit fields preserve float bits and signed narrow values. No code
// pointers, ABI padding, game state, clocks or GPU objects cross this boundary.
inline bool th14_encode_sht_transfer(const ShtResource& source,const u8* raw,u32 size,std::vector<u8>& output){
    using namespace sht_transfer_th14;
    const auto& h=source.header;
    if(!raw||!size||size>maximum_bytes||h.version!=4||!h.group_count||h.group_count>maximum_groups
        ||source.groups.size()!=h.group_count||h.max_power_level<0||h.power_step<=0)return false;
    u64 count=0;for(const auto& group:source.groups)count+=group.shots.size();
    if(u64(header_bytes)+header_words*4+u64(h.group_count)*8+count*spec_words*4>maximum_bytes)return false;
    Writer w;w.word(0x31544853);w.word(1);w.word(0);w.word(size);w.word(anm_bytes_hash(raw,size));
    w.word(h.group_count);w.word(u32(count));w.word(header_words);w.word(0);
    w.word(h.version);w.word(h.group_count);
    for(float value:{h.hitbox,h.attraction_speed,h.attraction_diameter,h.speed,h.focus_speed,h.diagonal_speed,h.focus_diagonal_speed})w.scalar(value);
    w.word(u32(h.max_power_level));w.word(u32(h.power_step));
    for(const auto& offset:h.option_offsets)pair(w,offset);
    for(auto word:h.native_extra)w.word(word);
    for(const auto& group:source.groups){
        w.word(group.metadata);w.word(u32(group.shots.size()));
        for(const auto& s:group.shots){
            if(s.interval<=0||!std::isfinite(s.angle)||!std::isfinite(s.speed))return false;
            w.word(u32(i32(s.interval)));w.word(u32(i32(s.delay)));w.word(u32(i32(s.damage)));
            pair(w,s.offset);pair(w,s.hitbox);w.scalar(s.angle);w.scalar(s.speed);
            w.word(u32(i32(s.option)));w.word(u32(i32(s.type)));w.word(u32(i32(s.animation)));w.word(u32(i32(s.hit_animation)));w.word(u32(i32(s.sound)));
            w.word(u32(s.spawn));w.word(u32(s.update));w.word(s.hit_callback);w.word(s.extra_callback);w.word(s.native_at_1c);
            for(auto word:s.native_tail)w.word(word);
        }
    }
    if(!w.ok||w.bytes.size()>maximum_bytes)return false;
    w.patch(8,u32(w.bytes.size()));w.patch(32,anm_bytes_hash(w.bytes.data()+header_bytes,u32(w.bytes.size())-header_bytes));
    output=std::move(w.bytes);return true;
}
inline bool th14_decode_sht_transfer(const u8* packet,u32 size,ShtResource& output,u32* source_size=nullptr,u32* source_hash=nullptr){
    using namespace sht_transfer_th14;
    if(!packet||size<header_bytes+header_words*4||size>maximum_bytes)return false;
    Reader r{packet,size};if(r.word()!=0x31544853||r.word()!=1||r.word()!=size)return false;
    const auto raw_size=r.word(),raw_hash=r.word(),groups=r.word(),specs=r.word(),words=r.word(),checksum=r.word();
    if(!r.ok||!raw_size||raw_size>maximum_bytes||!groups||groups>maximum_groups||words!=header_words
        ||u64(header_bytes)+header_words*4+u64(groups)*8+u64(specs)*spec_words*4!=size
        ||checksum!=anm_bytes_hash(packet+header_bytes,size-header_bytes))return false;
    ShtResource next;auto& h=next.header;h.version=r.short_word();h.group_count=r.short_word();
    h.hitbox=r.scalar();h.attraction_speed=r.scalar();h.attraction_diameter=r.scalar();h.speed=r.scalar();
    h.focus_speed=r.scalar();h.diagonal_speed=r.scalar();h.focus_diagonal_speed=r.scalar();
    h.max_power_level=i32(r.word());h.power_step=i32(r.word());
    for(auto& offset:h.option_offsets)pair(r,offset);
    for(auto& word:h.native_extra)word=r.word();
    if(!r.ok||h.version!=4||h.group_count!=groups||h.max_power_level<0||h.power_step<=0)return false;
    next.groups.resize(groups);u32 consumed=0;
    for(auto& group:next.groups){
        group.metadata=r.word();const auto count=r.word();
        if(!r.ok||consumed>specs||count>specs-consumed||u64(count)*spec_words*4>size-r.at)return false;
        consumed+=count;group.shots.resize(count);
        for(auto& s:group.shots){
            s.interval=byte(r);s.delay=byte(r);s.damage=r.signed_short();pair(r,s.offset);pair(r,s.hitbox);s.angle=r.scalar();s.speed=r.scalar();
            s.option=byte(r);s.type=byte(r);s.animation=r.signed_short();s.hit_animation=r.signed_short();s.sound=r.signed_short();
            s.spawn=ShotSpawn(r.word());s.update=ShotUpdate(r.word());s.hit_callback=r.word();s.extra_callback=r.word();s.native_at_1c=r.word();
            for(auto& word:s.native_tail)word=r.word();
            if(!r.ok||s.interval<=0||!std::isfinite(s.angle)||!std::isfinite(s.speed))return false;
        }
    }
    if(!r.ok||r.at!=size||consumed!=specs)return false;
    output=std::move(next);if(source_size)*source_size=raw_size;if(source_hash)*source_hash=raw_hash;return true;
}
}
#endif
