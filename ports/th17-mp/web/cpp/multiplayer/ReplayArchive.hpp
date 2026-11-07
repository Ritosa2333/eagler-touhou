#pragma once
#include "Session.hpp"
#include <eagler/netplay/InputReplay.hpp>
#include <string>

namespace th17::multiplayer {
class ReplayArchive {
public:
    bool begin(const SessionSetup&,std::uint64_t timestamp);
    bool append(std::uint32_t frame,std::uint32_t stage,const Netplay::FrameInput*);
    bool load(const std::uint8_t*,std::size_t);
    bool encode(std::vector<std::uint8_t>&,const char* name,std::uint32_t score,bool completed) const;
    const SessionSetup& setup() const{return setup_;}
    const Netplay::InputReplayInfo& info() const{return inputs_.Info();}
    const Netplay::InputReplay::Frame* frame(std::uint32_t n) const{return inputs_.FrameAt(n);}
    std::uint32_t frames() const{return info().frameCount;}
    const std::string& name() const{return name_;}
    std::uint64_t timestamp() const{return timestamp_;}
    std::uint32_t score() const{return score_;}
    bool completed() const{return completed_;}
private:
    std::vector<std::uint8_t> description(const char*,std::uint32_t,bool) const;
    Netplay::InputReplay inputs_;
    SessionSetup setup_;
    std::string name_;
    std::uint64_t timestamp_=0;
    std::uint32_t score_=0;
    std::uint32_t last_stage_=0,chapter_epoch_=0;
    bool completed_=false;
};
}
