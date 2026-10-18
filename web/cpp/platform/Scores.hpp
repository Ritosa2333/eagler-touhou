#pragma once
#include "FileSystem.hpp"
#include "../game/ScoreData.hpp"
#include "MemoryPool.hpp"
namespace th10::browser {
// The same score records and codec as the original game, owned by the
// browser application rather than by addresses in an executable image.
struct Scores final : ScoreFileEnvironment {
    FileSystem& files;
    ScoreData* data=nullptr;
    char path[32]{};
    u8 dictionary[8192]{};
    LzssSearchNode nodes[8193]{};
    u32 output=0xffffffff;
    MemoryPool memory;
    Scores(FileSystem& files,Rng& random,bool chinese);
    ~Scores();
    void reload();
    i32 save();
    void release_scratch();
    ScoreData* allocate_score() override;
    void delete_score(ScoreData* score) override;
    u8* allocate_bytes(u32 bytes) override;
    void release_bytes(void* bytes) override;
    u8* read_file(const char* name,u32& length) override;
    i32 open(const char* name) override;
    bool is_open() override;
    u32 write(const void* bytes,u32 length) override;
    void close_and_unlock() override;
};
}
