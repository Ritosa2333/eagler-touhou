#pragma once
#include "../game/ReplayStageSnapshot.hpp"
#include <eagler/netplay/SpectatorStageScalars.hpp>
namespace th15::multiplayer {
// Native t15r stage headers, one per seat. Each header is 0x238 file bytes.
struct SpectatorStageState {
    static constexpr u32 Schema=2;
    u32 frame=0,stage=0,seats=0;
    std::array<std::array<u8,0x238>,3> headers{};
};
inline bool EncodeSpectatorStageState(const SpectatorStageState& s,std::vector<u8>& out){
    if(s.stage<1||s.stage>7||s.seats<2||s.seats>3)return false;
    Netplay::SpectatorScalars::Writer writer;
    if(!writer.Values(SpectatorStageState::Schema,s.frame,s.stage,s.seats))return false;
    for(unsigned i=0;i<s.seats;++i){ReplayStageSnapshot header;
        if(u32(s.headers[i][0])+(u32(s.headers[i][1])<<8)!=s.stage||
           !header.read(s.headers[i].data(),u32(s.headers[i].size()))||!writer.Array(s.headers[i]))return false;
    }
    out=writer.Bytes();return writer.Good();
}
inline bool DecodeSpectatorStageState(const std::vector<u8>& bytes,SpectatorStageState& out){
    SpectatorStageState s;u32 schema=0;Netplay::SpectatorScalars::Reader reader(bytes);
    if(!reader.Values(schema,s.frame,s.stage,s.seats)||schema!=SpectatorStageState::Schema||
       s.stage<1||s.stage>7||s.seats<2||s.seats>3)return false;
    for(unsigned i=0;i<s.seats;++i){ReplayStageSnapshot header;
        if(!reader.Array(s.headers[i])||!header.read(s.headers[i].data(),u32(s.headers[i].size())))return false;
        if(u32(s.headers[i][0])+(u32(s.headers[i][1])<<8)!=s.stage)return false;
    }
    if(!reader.Done())return false;out=s;return true;
}
}
