#pragma once
#include "NetplayRuntime.hpp"
#include "../game/GameConfiguration.hpp"
#include "../game/ReplayFile.hpp"
#include <eagler/netplay/InputReplay.hpp>
#include <array>

namespace th08::multiplayer {
struct ReplayDescription {
    SessionSetup setup{};
    GameConfiguration configuration{};
    u32 score=0,last_stage=0;
    u32 stage_scores[9]{};
    char name[8]{},date[6]{};
};

// TH08's envelope owns the immutable boot files; common InputReplay owns every
// input row, checksum and chapter. No pointer/native world dump is serialized.
// The boot score is a bounded ordinary ScoreFile blob consumed by the same
// native decoder. It is not installed into the viewer's persistent namespace.
class ReplayArchive {
public:
    static constexpr std::size_t MaxBytes=Netplay::InputReplay::MaxBytes;
    static constexpr std::size_t MaxBootScore=4*1024*1024,HeaderBytes=24;
    using Frame=Netplay::InputReplay::Frame;
    bool Begin(const ReplayDescription&,const std::vector<u8>& boot_score);
    bool Load(const u8*,std::size_t);
    static bool Inspect(const u8*,std::size_t,Netplay::InputReplayInfo&,ReplayDescription&);
    static bool Preview(const u8*,std::size_t,ReplayMetadata&,u32* stage_scores=nullptr);
    bool BeginFrame(u32 local);
    bool Stamp(u32 local,u32 stage,u32 score);
    bool RequestSave(u32 local,i32 slot,const char* name,const char* date);
    using SaveCallback=bool(*)(void*,const char*,const u8*,u32);
    bool Commit(const NetplayRuntime&,SaveCallback=nullptr,void* context=nullptr);
    bool NextGeneration(u32 generation);
    const Frame* PlaybackFrame(u32 local)const;
    bool AdvancePlayback(u32 local,u32 stage);
    bool Encode(std::vector<u8>&,const ReplayDescription* override=nullptr)const;
    bool Recording()const{return tape.Recording();}
    bool Playing()const{return tape.Loaded();}
    bool Complete()const{return Playing()&&cursor==tape.Info().frameCount;}
    u32 Cursor()const{return Playing()?cursor:tape.Info().frameCount;}
    u32 Base()const{return base;}
    u32 Generation()const{return generation;}
    u32 StageFrame(u32 stage)const;
    const Netplay::InputReplayInfo& Info()const{return tape.Info();}
    const ReplayDescription& Description()const{return description;}
    const std::vector<u8>& BootScore()const{return boot_score;}
private:
    struct StampEntry {
        u32 frame=Netplay::INVALID_FRAME,label=0,score=0;
        i32 save_slot=0;char name[8]{},date[6]{};
    };
    Netplay::InputReplay tape;
    ReplayDescription description{};
    std::vector<u8> boot_score;
    std::array<StampEntry,Netplay::INPUT_HISTORY_SIZE> stamps{};
    u32 base=0,generation=0,cursor=0;
};
}
