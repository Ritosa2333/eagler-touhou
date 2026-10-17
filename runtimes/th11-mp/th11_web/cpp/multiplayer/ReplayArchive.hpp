#pragma once
#include "SessionSetup.hpp"
#include <eagler/netplay/InputReplay.hpp>
#include <string>
namespace th11::multiplayer {
class ReplayArchive {
public:
    static constexpr std::uint32_t DescriptionBytes=128,Magic=0x4d313154u,Version=1;
    static constexpr std::uint32_t MaxFrames=Netplay::InputReplay::MaxFrames;
    bool Begin(const SessionSetup&,std::uint64_t timestamp);
    bool Append(std::uint32_t frame,std::uint32_t stage,const Netplay::FrameInput*,unsigned);
    bool Load(const std::uint8_t*,std::size_t);
    static bool Validate(const std::uint8_t*,std::size_t);
    bool Encode(std::vector<std::uint8_t>&,const char* name,std::uint32_t score,bool completed,bool cheat=false)const;
    void Clear();
    const SessionSetup& Setup()const{return setup_;}
    const Netplay::InputReplay::Frame* FrameAt(std::uint32_t f)const{return replay_.FrameAt(f);}
    const Netplay::InputReplayInfo& Info()const{return replay_.Info();}
    std::uint32_t FrameCount()const{return replay_.Info().frameCount;}
    bool AtCapacity()const{return FrameCount()>=MaxFrames;}
    bool Recording()const{return replay_.Recording();}
    bool Loaded()const{return replay_.Loaded();}
    std::uint64_t Timestamp()const{return timestamp_;}
    std::uint32_t Score()const{return score_;}
    bool Completed()const{return completed_;}
    const std::string& Name()const{return name_;}
private:
    static bool Metadata(const Netplay::InputReplayInfo&,SessionSetup&,std::uint64_t&,std::uint32_t&,bool&,std::string&);
    std::vector<std::uint8_t> Description(const char*,std::uint32_t,bool,bool)const;
    Netplay::InputReplay replay_;
    SessionSetup setup_;
    std::uint64_t timestamp_=0;
    std::uint32_t score_=0;
    bool completed_=false;
    std::string name_;
};
}
