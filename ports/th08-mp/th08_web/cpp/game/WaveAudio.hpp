#pragma once
#include "Types.hpp"
#include <vector>
namespace th08 {
#pragma pack(push,2)
struct WaveFormat {u16 tag=0,channels=0;u32 rate=0,byte_rate=0;u16 align=0,bits=0,extra=0;};
#pragma pack(pop)
static_assert(sizeof(WaveFormat)==18);
struct BgmFormat {char name[16]{};i32 start=0;u32 preload=0;i32 intro=0,total=0;WaveFormat format{};};
static_assert(sizeof(BgmFormat)==52);
class PcmWave {
public:
    WaveFormat format;
    std::vector<u8> samples;
    bool load(const u8* data,u32 size);
    static bool valid(const WaveFormat&) noexcept;
};
class BgmFormats {
public:
    bool load(const u8*,u32);
    u32 find(const char* name) const noexcept;
    const BgmFormat* get(u32 i) const noexcept {return i<formats.size()?&formats[i]:nullptr;}
    u32 count() const noexcept {return formats.size();}
private:
    std::vector<BgmFormat> formats;
};
struct AudioByteSource {virtual ~AudioByteSource()=default;virtual u32 read(u32 offset,u8* output,u32 size)=0;};
class WaveStream {
public:
    i32 open(AudioByteSource&,const BgmFormat&,u32 seek_offset=0);
    i32 open_memory(const u8*,u32,const BgmFormat&);
    i32 reopen(const BgmFormat&);
    i32 reset(bool loop);
    i32 read(u8* output,u32 request,u32& received);
    void close() noexcept;
    u32 size() const noexcept {return original_size;}
    u32 position() const noexcept {return cursor;}
    u32 left() const noexcept {return memory?memory_size-cursor:remaining;}
private:
    AudioByteSource* source=nullptr;
    const u8* memory=nullptr;
    BgmFormat format{};
    u32 original_size=0,memory_capacity=0,memory_size=0,cursor=0,remaining=0,seek_offset=0;
};
}
