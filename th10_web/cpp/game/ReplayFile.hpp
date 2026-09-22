#pragma once
#include "Replay.hpp"
#include "ResourceCodec.hpp"
namespace th10 {
struct ReplayFileEnvironment : CodecMemory {
    const u32* game_flags;
    u8* dictionary;
    virtual bool exists(const char* path)=0;
    virtual i32 open(const char* path)=0;
    virtual u8* read(u32 bytes)=0;
    virtual void close()=0;
    virtual u8* archive(const char* name,u32& length)=0;
};
i32 load_replay(Replay& replay,const char* name,ReplayFileEnvironment& environment);
struct ReplayDate {i32 second,minute,hour,day,month,year,weekday,yearday,dst;};
struct ReplaySaveEnvironment : CodecMemory {
    GameEconomy* game;
    const double* active_time;
    const double* total_time;
    const bool* cheat_movement_used;
    LzssSearch search;
#ifdef TH_ENABLE_THPRAC
    // th10_rep_power_fix: set for advanced-practice saves so save_replay can
    // rewrite the recorded option layout before compression.
    bool practice_mode=false;
    i32 practice_power=0;
#endif
    // Title, version, character and difficulty strings use the game's encoding.
    const char* title;
    const char* version;
    const char* comment;
    const char* stage_range;
    const char* const* characters;
    const char* const* difficulties;
    virtual void create_directory(const char* name)=0;
    virtual void open(const char* name)=0;
    virtual bool is_open()=0;
    virtual u32 write(const void* bytes,u32 length)=0;
    virtual void close_and_unlock()=0;
    virtual ReplayDate local_date(i32 timestamp)=0;
    // Standard C string/number formatting is a platform service, with bounded
    // argument lists rather than exposing game state or an instruction stream.
    virtual i32 format(char* output,const char* format,const u32* arguments,u32 count)=0;
};
i32 save_replay(Replay& replay,const char* file_name,const char* player_name,ReplaySaveEnvironment& environment);
}
