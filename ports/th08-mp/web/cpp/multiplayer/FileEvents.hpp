#pragma once
#ifndef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#error Confirmed file writes are multiplayer-only
#endif
#include "../game/Types.hpp"
#include <deque>
#include <string>
#include <vector>

namespace th08::multiplayer {
struct FileEventOutput {
    virtual ~FileEventOutput()=default;
    virtual bool apply_file_event(const char*,const u8*,u32)=0;
};
class FileEvents {
public:
    static constexpr u32 Invalid=~u32(0),Capacity=16,MaxBytes=16*1024*1024;
    void Reset(u32 first=0);
    bool BeginFrame(u32);
    bool Write(const std::string&,const u8*,u32);
    bool EndFrame();
    bool DiscardFrom(u32);
    bool CommitThrough(u32 confirmed,u32 simulated,FileEventOutput&);
    const std::vector<u8>* Pending(const std::string&)const;
    bool IsOpen()const{return open!=Invalid;}
    bool Failed()const{return failed;}
    u32 NextCommit()const{return next_commit;}
    u32 CommittedWrites()const{return committed;}
    u32 Digest()const{return digest;}
private:
    struct WriteRecord {std::string path;std::vector<u8> bytes;};
    struct Frame {u32 number;std::vector<WriteRecord> writes;};
    std::deque<Frame> frames;
    u32 open=Invalid,next=0,next_commit=0,committed=0,digest=2166136261u;
    std::size_t pending_bytes=0;bool failed=false;
    bool Fail(){failed=true;return false;}
};
}
