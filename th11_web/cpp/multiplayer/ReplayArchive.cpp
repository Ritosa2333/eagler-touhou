#include "ReplayArchive.hpp"
#include "InputLanes.hpp"
#include <algorithm>
#include <cstring>
namespace th11::multiplayer {
namespace {
std::uint32_t get(const std::uint8_t* p){return std::uint32_t(p[0])|(std::uint32_t(p[1])<<8)|(std::uint32_t(p[2])<<16)|(std::uint32_t(p[3])<<24);}
void put(std::uint8_t* p,std::uint32_t n){for(unsigned i=0;i<4;++i)p[i]=std::uint8_t(n>>(8*i));}
}
std::vector<std::uint8_t> ReplayArchive::Description(const char* name,std::uint32_t score,bool completed,bool cheat)const{
    std::vector<std::uint8_t> d(DescriptionBytes);
    put(d.data(),Magic);put(d.data()+4,Version);put(d.data()+8,SessionSetup::GameplayVersion);put(d.data()+12,setup_.input_delay);
    put(d.data()+16,std::uint32_t(timestamp_));put(d.data()+20,std::uint32_t(timestamp_>>32));put(d.data()+24,score);
    put(d.data()+28,unsigned(completed)|(unsigned(cheat)<<1));if(name)std::memcpy(d.data()+32,name,std::min<std::size_t>(std::strlen(name),8));
    const auto words=EncodeSessionSetup(setup_);for(unsigned i=0;i<words.size();++i)put(d.data()+40+i*4,words[i]);return d;
}
bool ReplayArchive::Metadata(const Netplay::InputReplayInfo& info,SessionSetup& setup,std::uint64_t& timestamp,std::uint32_t& score,bool& completed,std::string& name){
    const auto& c=info.config;const auto& d=c.description;
    if(c.gameId!=11||d.size()!=DescriptionBytes||get(d.data())!=Magic||get(d.data()+4)!=Version||
       get(d.data()+8)!=SessionSetup::GameplayVersion||get(d.data()+12)>9||(get(d.data()+28)&~3u))return false;
    std::array<std::uint32_t,SessionSetup::Words> words{};for(unsigned i=0;i<words.size();++i)words[i]=get(d.data()+40+i*4);
    SessionSetup next;if(!DecodeSessionSetup(next,words.data(),words.size()))return false;next.input_delay=get(d.data()+12);
    if(c.playerCount!=next.player_count||c.recordedPlayer!=next.local_player||c.gameplayAbi!=GameplayAbi(next))return false;
    for(unsigned i=0;i<info.chapterCount;++i)if(info.chapters[i].label<1||info.chapters[i].label>7||
        (i&&info.chapters[i].label<=info.chapters[i-1].label))return false;
    if(info.chapters[0].label!=next.initial_stage())return false;
    setup=next;timestamp=get(d.data()+16)|(std::uint64_t(get(d.data()+20))<<32);score=get(d.data()+24);completed=(get(d.data()+28)&1)!=0;
    name.assign(reinterpret_cast<const char*>(d.data()+32),8);while(!name.empty()&&(name.back()==0||name.back()==' '))name.pop_back();return true;
}
void ReplayArchive::Clear(){replay_.Clear();setup_={};timestamp_=score_=0;completed_=false;name_.clear();}
bool ReplayArchive::Begin(const SessionSetup& setup,std::uint64_t timestamp){
    if(!setup.valid())return false;ReplayArchive next;next.setup_=setup;next.timestamp_=timestamp;
    Netplay::InputReplayConfig config;config.gameId=11;config.gameplayAbi=GameplayAbi(setup);config.playerCount=std::uint8_t(setup.player_count);
    config.recordedPlayer=std::uint8_t(setup.local_player);config.description=next.Description("",0,false,false);
    if(!next.replay_.Begin(config))return false;*this=std::move(next);return true;
}
bool ReplayArchive::Append(std::uint32_t frame,std::uint32_t stage,const Netplay::FrameInput* input,unsigned count){
    std::array<MultiplayerInput,3> checked;if(!InputLanes::Decode(input,count,checked)||stage<1||stage>7)return false;
    return replay_.Append(frame,stage,input,count);
}
bool ReplayArchive::Load(const std::uint8_t* data,std::size_t bytes){
    ReplayArchive next;if(!next.replay_.Decode(data,bytes)||!Metadata(next.replay_.Info(),next.setup_,next.timestamp_,next.score_,next.completed_,next.name_))return false;
    std::array<MultiplayerInput,3> checked;
    for(std::uint32_t f=0;f<next.FrameCount();++f)if(!InputLanes::Decode(next.FrameAt(f)->data(),next.setup_.player_count,checked))return false;
    *this=std::move(next);return true;
}
bool ReplayArchive::Validate(const std::uint8_t* data,std::size_t bytes){ReplayArchive parsed;return parsed.Load(data,bytes);}
bool ReplayArchive::Encode(std::vector<std::uint8_t>& out,const char* name,std::uint32_t score,bool completed,bool cheat)const{
    if(!name||std::strlen(name)>8||!FrameCount())return false;const auto d=Description(name,score,completed&&!AtCapacity(),cheat);return replay_.Encode(&out,&d);
}
}
