#pragma once
#include "FileSystem.hpp"
#include "MemoryPool.hpp"
#include "../game/ReplayFile.hpp"
#include <vector>
namespace th10::browser {
struct ReplayCalendar {virtual ReplayDate local_date(i32 timestamp)=0;virtual i32 timestamp()=0;};
ReplayCalendar& default_calendar();
struct ReplayDocument final : ReplayFileEnvironment {
    FileSystem& files;MemoryPool memory;Replay value{};
    u32 flags=0,input=0xffffffff;u8 history[8192]{};
    ReplayDocument(FileSystem& files,u32 flags);
    ~ReplayDocument();
    i32 load(const char* name);
    u8* allocate_bytes(u32 bytes) override;
    void release_bytes(void* bytes) override;
    bool exists(const char* path) override;
    i32 open(const char* path) override;
    u8* read(u32 bytes) override;
    void close() override;
    u8* archive(const char* name,u32& length) override;
};
struct ReplayWriter final : ReplaySaveEnvironment {
    FileSystem& files;ReplayCalendar& calendar;MemoryPool memory;
    u32 output=0xffffffff;u8 dictionary[8192]{};LzssSearchNode nodes[8193]{};
    const std::vector<u8>* motion_trailer=nullptr;
    bool write_failed=false;
    ReplayWriter(FileSystem&,ReplayCalendar&,GameEconomy&,const double& active,const double& total,bool chinese);
    ReplayWriter(FileSystem&,ReplayCalendar&,GameEconomy&,const double& active,const double& total,const bool& cheat_movement_used,bool chinese);
    ~ReplayWriter();
    i32 save(Replay& replay,const char* file,const char* name,const std::vector<u8>& trailer={});
    u8* allocate_bytes(u32 bytes) override;
    void release_bytes(void* bytes) override;
    void create_directory(const char*) override {} // Paths are keys in this player's file store.
    void open(const char* name) override;
    bool is_open() override;
    u32 write(const void* bytes,u32 length) override;
    void close_and_unlock() override;
    ReplayDate local_date(i32 timestamp) override;
    i32 format(char* output,const char* format,const u32* arguments,u32 count) override;
};
}
