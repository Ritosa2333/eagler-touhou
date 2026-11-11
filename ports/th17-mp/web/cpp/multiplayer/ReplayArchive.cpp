#include "ReplayArchive.hpp"
#include <eagler/netplay/SpectatorStageScalars.hpp>
#include <cstring>

namespace th17::multiplayer {
namespace {
constexpr std::uint32_t magic=0x4d373154,legacy_version=1,challenge_version=2;
template<class Codec> bool visit(Codec& codec,SessionSetup& s){
    if(!codec.Values(s.room,s.seed,s.seats,s.local,s.difficulty,s.delay,s.rollback))return false;
    for(auto& loadout:s.loadouts)if(!codec.Values(loadout.character,loadout.animal))return false;
    return codec.Array(s.build);
}
}
std::vector<std::uint8_t> ReplayArchive::description(const char* name,std::uint32_t score,bool completed) const{
    Netplay::SpectatorScalars::Writer writer;
    auto setup=setup_;
    std::array<std::uint8_t,8> label{};
    if(name)std::memcpy(label.data(),name,std::strlen(name));
    writer.Values(magic,setup.challenge?challenge_version:legacy_version,timestamp_,score,completed);
    writer.Array(label);
    visit(writer,setup);
    if(setup.challenge)writer.Value(setup.challenge);
    return writer.Bytes();
}
bool ReplayArchive::begin(const SessionSetup& setup,std::uint64_t timestamp){
    if(!setup.valid())return false;
    ReplayArchive next;next.setup_=setup;next.timestamp_=timestamp;
    Netplay::InputReplayConfig config;
    config.gameId=17;config.gameplayAbi=setup.gameplay_abi();
    config.playerCount=setup.seats;config.recordedPlayer=setup.local;
    config.description=next.description("",0,false);
    if(!next.inputs_.Begin(config))return false;
    *this=std::move(next);return true;
}
bool ReplayArchive::append(std::uint32_t frame,std::uint32_t stage,const Netplay::FrameInput* inputs){
    if(stage<1||stage>7)return false;
    const auto epoch=chapter_epoch_+(last_stage_&&stage<last_stage_?8u:0u);
    if(!inputs_.Append(frame,epoch+stage,inputs,setup_.seats))return false;
    last_stage_=stage;chapter_epoch_=epoch;return true;
}
bool ReplayArchive::load(const std::uint8_t* data,std::size_t size){
    ReplayArchive next;
    if(!next.inputs_.Decode(data,size))return false;
    const auto& info=next.info();
    Netplay::SpectatorScalars::Reader reader(info.config.description);
    std::uint32_t signature=0,format=0;
    std::array<std::uint8_t,8> name{};
    if(!reader.Values(signature,format,next.timestamp_,next.score_,next.completed_)||
       signature!=magic||(format!=legacy_version&&format!=challenge_version)||!reader.Array(name)||!visit(reader,next.setup_)||
       (format==challenge_version&&(!reader.Value(next.setup_.challenge)||!next.setup_.challenge))||
       !reader.Done()||!next.setup_.valid()||info.config.gameId!=17||
       info.config.gameplayAbi!=next.setup_.gameplay_abi()||
       info.config.playerCount!=next.setup_.seats||info.config.recordedPlayer!=next.setup_.local||
       !info.chapterCount)return false;
    for(unsigned n=0;n<info.chapterCount;++n)
        if(!(info.chapters[n].label%8)||info.chapters[n].label>511||
           (n&&info.chapters[n].label<=info.chapters[n-1].label))return false;
    if(info.chapters[0].label!=(next.setup_.difficulty==4?7u:1u))return false;
    next.name_.assign(reinterpret_cast<const char*>(name.data()),name.size());
    while(!next.name_.empty()&&(next.name_.back()==0||next.name_.back()==' '))next.name_.pop_back();
    *this=std::move(next);return true;
}
bool ReplayArchive::encode(std::vector<std::uint8_t>& out,const char* name,std::uint32_t score,bool completed) const{
    if(!name||std::strlen(name)>8||!frames())return false;
    const auto metadata=description(name,score,completed&&frames()<Netplay::InputReplay::MaxFrames);
    return inputs_.Encode(&out,&metadata);
}
}
