#pragma once
#include "AnmTransferTh14.hpp"
#include "StageResource.hpp"
#include "EclResource.hpp"

// CPU worker packets contain parsed values and owned bytes only. Program
// definition pointers, animation templates and GPU objects are built by their
// existing owners after delivery. Floating-point values retain their bits.
namespace th11 {
namespace stage_ecl_transfer_th14 {
using anm_transfer_th14::Reader;
using anm_transfer_th14::Writer;
using anm_transfer_th14::header_bytes;
using anm_transfer_th14::maximum_bytes;
inline void start(Writer& w,u32 magic,const u8* raw,u32 size,u32 a,u32 b,u32 c){
    w.word(magic);w.word(1);w.word(0);w.word(size);w.word(anm_bytes_hash(raw,size));w.word(a);w.word(b);w.word(c);w.word(0);
}
inline bool finish(Writer& w,std::vector<u8>& output){
    if(!w.ok)return false;w.patch(8,u32(w.bytes.size()));w.patch(32,anm_bytes_hash(w.bytes.data()+header_bytes,u32(w.bytes.size())-header_bytes));
    output=std::move(w.bytes);return true;
}
inline bool begin(Reader& r,u32 magic,u32& source_size,u32& source_hash,u32& a,u32& b,u32& c){
    if(!r.bytes||r.size<header_bytes||r.size>maximum_bytes)return false;
    if(r.word()!=magic||r.word()!=1||r.word()!=r.size)return false;
    source_size=r.word();source_hash=r.word();a=r.word();b=r.word();c=r.word();const auto checksum=r.word();
    return r.ok&&source_size&&source_size<=maximum_bytes&&checksum==anm_bytes_hash(r.bytes+header_bytes,r.size-header_bytes);
}
inline void vec3(Writer& w,const Vec3& v){w.scalar(v.x);w.scalar(v.y);w.scalar(v.z);}
inline void vec3(Reader& r,Vec3& v){v.x=r.scalar();v.y=r.scalar();v.z=r.scalar();}
}

inline bool th14_encode_std_transfer(const StageResource& source,const u8* raw,u32 size,std::vector<u8>& output){
    using namespace stage_ecl_transfer_th14;
    if(!raw||!size||size>maximum_bytes||!source.error.empty())return false;
    Writer w;start(w,0x31445453,raw,size,u32(source.objects.size()),u32(source.instances.size()),u32(source.instructions.size()));
    w.text(source.animation_name);w.word(source.animation_count);
    for(const auto& o:source.objects){
        w.word(u32(i32(o.id)));w.word(o.layer);w.word(o.flags);vec3(w,o.position);vec3(w,o.size);w.word(u32(o.primitives.size()));
        for(const auto& p:o.primitives){w.word(u32(i32(p.type)));w.word(u32(i32(p.script)));w.word(p.animation);vec3(w,p.position);w.scalar(p.size.x);w.scalar(p.size.y);}
    }
    for(const auto& i:source.instances){w.word(u32(i32(i.object)));w.word(u32(i32(i.flags)));vec3(w,i.position);}
    for(const auto& i:source.instructions){w.word(u32(i.time));w.word(u32(i32(i.opcode)));w.word(u32(i32(i.length)));w.word(i.offset);w.word(u32(i.arguments.size()));for(auto a:i.arguments)w.word(a);}
    return finish(w,output);
}

inline bool th14_decode_std_transfer(const u8* packet,u32 size,StageResource& output,u32* source_size=nullptr,u32* source_hash=nullptr){
    using namespace stage_ecl_transfer_th14;
    Reader r{packet,size};u32 raw_size,raw_hash,objects,instances,instructions;
    if(!begin(r,0x31445453,raw_size,raw_hash,objects,instances,instructions)||u64(objects)*40+u64(instances)*20+u64(instructions)*20+8>size-header_bytes)return false;
    StageResource next;if(!r.text(next.animation_name))return false;next.animation_count=r.word();
    next.objects.resize(objects);next.instances.resize(instances);next.instructions.resize(instructions);u32 primitives=0;
    for(auto& o:next.objects){
        o.id=r.signed_short();const auto layer=r.word(),flags=r.word();if(layer>255||flags>255)return false;o.layer=u8(layer);o.flags=u8(flags);
        vec3(r,o.position);vec3(r,o.size);const auto count=r.word();if(!r.ok||u64(count)*32>size-r.at||count>next.animation_count-primitives)return false;
        o.primitives.resize(count);
        for(auto& p:o.primitives){p.type=r.signed_short();p.script=r.signed_short();p.animation=r.word();vec3(r,p.position);p.size.x=r.scalar();p.size.y=r.scalar();if(!r.ok||p.animation>=next.animation_count)return false;}
        primitives+=count;
    }
    if(primitives!=next.animation_count)return false;
    for(auto& i:next.instances){i.object=r.signed_short();i.flags=r.signed_short();vec3(r,i.position);if(!r.ok||i.object<0||u32(i.object)>=objects)return false;}
    for(auto& i:next.instructions){
        i.time=i32(r.word());i.opcode=r.signed_short();i.length=r.signed_short();i.offset=r.word();const auto count=r.word();
        if(!r.ok||u64(count)*4>size-r.at)return false;i.arguments.resize(count);for(auto& a:i.arguments)a=r.word();
    }
    if(!r.ok||r.at!=size)return false;
    output=std::move(next);if(source_size)*source_size=raw_size;if(source_hash)*source_hash=raw_hash;return true;
}

inline bool th14_encode_ecl_transfer(const EclResource& source,const u8* raw,u32 size,std::vector<u8>& output){
    using namespace stage_ecl_transfer_th14;
    if(!raw||!size||size>maximum_bytes)return false;
    Writer w;start(w,0x314c4345,raw,size,u32(source.subroutines.size()),u32(source.animations.size()),u32(source.includes.size()));
    w.blob(source.bytes.data(),source.bytes.size());
    for(const auto& sub:source.subroutines){w.text(sub.name);w.word(sub.offset);w.word(sub.size);}
    for(const auto& name:source.animations)w.text(name);for(const auto& name:source.includes)w.text(name);
    return finish(w,output);
}

inline bool th14_decode_ecl_transfer(const u8* packet,u32 size,EclResource& output,u32* source_size=nullptr,u32* source_hash=nullptr){
    using namespace stage_ecl_transfer_th14;
    Reader r{packet,size};u32 raw_size,raw_hash,subroutines,animations,includes;
    if(!begin(r,0x314c4345,raw_size,raw_hash,subroutines,animations,includes)||u64(subroutines)*12+u64(animations)*4+u64(includes)*4+4>size-header_bytes)return false;
    EclResource next;if(!r.blob(next.bytes)||next.bytes.size()!=raw_size||raw_size<36||anm_bytes_hash(next.bytes.data(),raw_size)!=raw_hash)return false;
    next.subroutines.resize(subroutines);next.animations.resize(animations);next.includes.resize(includes);
    for(auto& sub:next.subroutines){
        if(!r.text(sub.name))return false;sub.offset=r.word();sub.size=r.word();
        if(!r.ok||sub.offset>raw_size||sub.size<16||sub.size>raw_size-sub.offset)return false;
    }
    for(auto& name:next.animations)if(!r.text(name))return false;for(auto& name:next.includes)if(!r.text(name))return false;
    if(!r.ok||r.at!=size)return false;
    output=std::move(next);if(source_size)*source_size=raw_size;if(source_hash)*source_hash=raw_hash;return true;
}
}
