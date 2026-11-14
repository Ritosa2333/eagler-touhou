#pragma once
#include "AnmResource.hpp"
#include <utility>

// Private worker transport for already parsed ANM values. This carries no GPU
// handles, pointers, animation templates, game state, clocks or RNG. Integers
// and float bit patterns have explicit little-endian fields, never ABI padding.
namespace th11 {
namespace anm_transfer_th14 {
constexpr u32 maximum_bytes=128u*1024*1024,header_bytes=36;
struct Writer {
    std::vector<u8> bytes;bool ok=true;
    void word(u32 v){if(bytes.size()>maximum_bytes-4){ok=false;return;}for(u32 i=0;i<4;++i)bytes.push_back(u8(v>>(8*i)));}
    void scalar(float v){u32 bits;std::memcpy(&bits,&v,4);word(bits);}
    void blob(const u8* p,size_t n){
        if(n>maximum_bytes||bytes.size()>maximum_bytes-4||n>maximum_bytes-bytes.size()-4){ok=false;return;}
        word(u32(n));if(n)bytes.insert(bytes.end(),p,p+n);
    }
    void text(const std::string& v){blob(reinterpret_cast<const u8*>(v.data()),v.size());}
    void patch(u32 at,u32 value){for(u32 i=0;i<4;++i)bytes[at+i]=u8(value>>(8*i));}
};
struct Reader {
    const u8* bytes;u32 size,at=0;bool ok=true;
    u32 word(){if(at>size||size-at<4){ok=false;return 0;}u32 v=0;for(u32 i=0;i<4;++i)v|=u32(bytes[at++])<<(8*i);return v;}
    float scalar(){const auto bits=word();float value;std::memcpy(&value,&bits,4);return value;}
    bool blob(std::vector<u8>& value){const auto n=word();if(!ok||n>size-at){ok=false;return false;}value.assign(bytes+at,bytes+at+n);at+=n;return true;}
    bool text(std::string& value){const auto n=word();if(!ok||n>size-at){ok=false;return false;}value.assign(reinterpret_cast<const char*>(bytes+at),n);at+=n;return true;}
    u16 short_word(){const auto value=word();if(value>65535)ok=false;return u16(value);}
    i16 signed_short(){const auto value=word();const auto result=i16(value);if(value!=u32(i32(result)))ok=false;return result;}
};
}

inline bool th14_encode_anm_transfer(const AnmResource& source,const u8* raw,u32 raw_size,std::vector<u8>& output){
    using namespace anm_transfer_th14;
    if(!raw||!raw_size||raw_size>maximum_bytes||source.last_error||!source.detail.empty()||source.templates)return false;
    Writer w;w.word(0x314d4e41);w.word(1);w.word(0);w.word(raw_size);w.word(anm_bytes_hash(raw,raw_size));
    w.word(u32(source.textures.size()));w.word(u32(source.sprites.size()));w.word(u32(source.scripts.size()));w.word(0);
    for(const auto& t:source.textures){
        w.text(t.name);w.word(u32(t.kind));w.word(t.width);w.word(t.height);w.word(t.format);
        w.word(t.pixel_width);w.word(t.pixel_height);w.word(t.pixel_format);w.word(u32(i32(t.source_x)));w.word(u32(i32(t.source_y)));
        w.word(t.priority);w.word(t.half_resolution?1:0);w.word(t.first_script);w.word(t.script_count);w.blob(t.pixels.data(),t.pixels.size());
    }
    for(const auto& s:source.sprites){
        w.word(s.source_id);w.word(s.texture);w.scalar(s.x);w.scalar(s.y);w.scalar(s.width);w.scalar(s.height);
        w.scalar(s.u0);w.scalar(s.v0);w.scalar(s.u1);w.scalar(s.v1);
    }
    for(const auto& s:source.scripts){w.word(u32(s.source_id));w.word(s.file_offset);w.word(s.load_hash);w.blob(s.bytes.data(),s.bytes.size());}
    if(!w.ok)return false;
    w.patch(8,u32(w.bytes.size()));w.patch(32,anm_bytes_hash(w.bytes.data()+header_bytes,u32(w.bytes.size())-header_bytes));
    output=std::move(w.bytes);return true;
}

inline bool th14_decode_anm_transfer(const u8* packet,u32 size,AnmResource& output,u32* source_size=nullptr,u32* source_hash=nullptr){
    using namespace anm_transfer_th14;
    if(!packet||size<header_bytes||size>maximum_bytes)return false;
    Reader r{packet,size};if(r.word()!=0x314d4e41||r.word()!=1||r.word()!=size)return false;
    const auto raw_size=r.word(),raw_hash=r.word(),textures=r.word(),sprites=r.word(),scripts=r.word(),checksum=r.word();
    if(!raw_size||raw_size>maximum_bytes||!r.ok||checksum!=anm_bytes_hash(packet+header_bytes,size-header_bytes))return false;
    // Minimum serialized sizes bound every allocation before reading lengths.
    if(u64(textures)*60+u64(sprites)*40+u64(scripts)*16>size-header_bytes)return false;
    AnmResource next;next.textures.resize(textures);next.sprites.resize(sprites);next.scripts.resize(scripts);
    for(auto& t:next.textures){
        if(!r.text(t.name))return false;const auto kind=r.word();if(kind>u32(AnmTexture::Kind::RenderTarget))return false;t.kind=AnmTexture::Kind(kind);
        t.width=r.short_word();t.height=r.short_word();t.format=r.short_word();t.pixel_width=r.short_word();t.pixel_height=r.short_word();t.pixel_format=r.short_word();
        t.source_x=r.signed_short();t.source_y=r.signed_short();t.priority=r.word();const auto half=r.word();if(half>1)return false;t.half_resolution=half!=0;
        t.first_script=r.word();t.script_count=r.word();
        if(!r.ok||!t.width||!t.height||t.first_script>scripts||t.script_count>scripts-t.first_script||!r.blob(t.pixels))return false;
    }
    for(auto& s:next.sprites){
        s.source_id=r.word();s.texture=r.word();s.x=r.scalar();s.y=r.scalar();s.width=r.scalar();s.height=r.scalar();
        s.u0=r.scalar();s.v0=r.scalar();s.u1=r.scalar();s.v1=r.scalar();if(!r.ok||s.texture>=textures)return false;
    }
    for(auto& s:next.scripts){
        s.source_id=i32(r.word());s.file_offset=r.word();s.load_hash=r.word();
        if(!r.blob(s.bytes)||s.bytes.size()<8||s.bytes.size()%4||anm_bytes_hash(s.bytes.data(),u32(s.bytes.size()))!=s.load_hash)return false;
    }
    if(!r.ok||r.at!=size)return false;
    output=std::move(next);if(source_size)*source_size=raw_size;if(source_hash)*source_hash=raw_hash;return true;
}
}
