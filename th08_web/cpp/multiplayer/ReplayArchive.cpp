#include "ReplayArchive.hpp"
#include <algorithm>
#include <cstdio>
#include <cstring>

namespace th08::multiplayer {
namespace {
constexpr u8 signature[]{'T','8','M','P','R','P','Y','1'};
constexpr std::size_t DescriptionBytes=100;
u32 word(const u8* p){return u32(p[0])|(u32(p[1])<<8)|(u32(p[2])<<16)|(u32(p[3])<<24);}
void put(std::vector<u8>& out,u32 v){for(unsigned i=0;i<4;++i)out.push_back(u8(v>>(8*i)));}
u32 checksum(const u8* p,std::size_t size){
    u32 hash=2166136261u;for(std::size_t i=0;i<size;++i)if(i<20||i>=24){hash^=p[i];hash*=16777619u;}return hash;
}
bool valid_config(const GameConfiguration& c){
    return c.version==0x80001&&c.lives<7&&c.bombs<4&&c.color16<2&&c.music<3&&
        c.sounds<2&&c.difficulty<6&&c.windowed<2&&c.frameskip<3&&c.effects<3&&
        c.slow_mode<2&&c.shot_slow<2&&c.music_volume>=0&&c.music_volume<=100&&
        c.sound_volume>=0&&c.sound_volume<=100;
}
std::vector<u8> metadata(const ReplayDescription& d){
    std::vector<u8> out;out.reserve(DescriptionBytes);
    put(out,2);put(out,d.setup.seed);put(out,d.setup.difficulty);
    for(auto character:d.setup.characters)put(out,character);
    for(auto build:d.setup.build)put(out,build);
    put(out,d.score);put(out,d.last_stage);
    out.insert(out.end(),d.name,d.name+8);out.insert(out.end(),d.date,d.date+6);out.push_back(0);out.push_back(0);
    for(auto score:d.stage_scores)put(out,score);return out;
}
bool decode_description(const Netplay::InputReplayInfo& info,ReplayDescription& d){
    const auto& c=info.config;const auto& v=c.description;
    if(c.gameId!=8||v.size()!=DescriptionBytes||word(v.data())!=2||v[62]||v[63])return false;
    const u32 fields[]{3,c.playerCount,c.recordedPlayer,word(v.data()+8),word(v.data()+4),1,0,
        word(v.data()+24),word(v.data()+28),word(v.data()+32),word(v.data()+36),
        word(v.data()+12),0,word(v.data()+16),0,word(v.data()+20),0};
    ReplayDescription next;
    if(!decode_session_setup(next.setup,fields,17)||c.gameplayAbi!=gameplay_contract(next.setup))return false;
    next.score=word(v.data()+40);next.last_stage=word(v.data()+44);
    if(next.score>999999999||next.last_stage>8)return false;
    std::memcpy(next.name,v.data()+48,8);std::memcpy(next.date,v.data()+56,6);
    for(unsigned i=0;i<9;++i){next.stage_scores[i]=word(v.data()+64+4*i);if(next.stage_scores[i]>999999999)return false;}
    u32 previous_generation=0;
    for(u32 i=0;i<info.chapterCount;++i){const auto label=info.chapters[i].label,gen=label>>4;
        if(!(label&15)||(label&15)>9||(i==0&&gen)||gen<previous_generation||gen>previous_generation+1)return false;
        previous_generation=gen;
    }
    d=next;return true;
}
bool envelope(const u8* data,std::size_t size,std::size_t& score_size,std::size_t& tape_offset){
    if(!data||size<ReplayArchive::HeaderBytes+sizeof(GameConfiguration)||size>ReplayArchive::MaxBytes||
       std::memcmp(data,signature,8)||word(data+12)!=sizeof(GameConfiguration)||
       word(data+20)!=checksum(data,size))return false;
    score_size=word(data+16);const auto tape_size=word(data+8);
    if(score_size>ReplayArchive::MaxBootScore||score_size>size-ReplayArchive::HeaderBytes-sizeof(GameConfiguration))return false;
    tape_offset=ReplayArchive::HeaderBytes+sizeof(GameConfiguration)+score_size;
    return tape_size==size-tape_offset;
}
}
bool ReplayArchive::Begin(const ReplayDescription& value,const std::vector<u8>& score){
    if(value.setup.started||!value.setup.configured||!valid_config(value.configuration)||score.size()>MaxBootScore)return false;
    Netplay::InputReplayConfig config;
    config.gameId=8;config.playerCount=u8(value.setup.player_count);config.recordedPlayer=u8(value.setup.local_player);
    config.gameplayAbi=gameplay_contract(value.setup);config.description=metadata(value);
    Netplay::InputReplayInfo check;check.config=config;ReplayDescription decoded;
    if(!decode_description(check,decoded)||!tape.Begin(config))return false;
    description=value;boot_score=score;base=generation=cursor=0;stamps={};return true;
}
bool ReplayArchive::Inspect(const u8* data,std::size_t size,Netplay::InputReplayInfo& info,ReplayDescription& d){
    std::size_t score_size=0,offset=0;if(!envelope(data,size,score_size,offset))return false;
    Netplay::InputReplayInfo next;ReplayDescription description;
    if(!Netplay::InputReplay::Inspect(data+offset,size-offset,&next)||!decode_description(next,description))return false;
    std::memcpy(&description.configuration,data+HeaderBytes,sizeof(GameConfiguration));
    if(!valid_config(description.configuration))return false;
    // Common validates the wire layout; TH08 additionally rejects unsupported
    // analog modes/flags rather than silently discarding their semantics.
    const auto& c=next.config;
    const std::size_t samples=offset+Netplay::InputReplay::HeaderBytes+c.description.size()+std::size_t(next.chapterCount)*8;
    if(samples>size||std::size_t(next.frameCount)*c.playerCount> (size-samples)/Netplay::InputReplay::SampleBytes)return false;
    // Decoding once in Load validates title sample semantics below. Inspect
    // does it without allocating a full 250000-frame tape for menu previews.
    for(u32 frame=0;frame<next.frameCount;++frame)for(u8 seat=0;seat<c.playerCount;++seat){
        const u8* p=data+samples+(std::size_t(frame)*c.playerCount+seat)*12;
        Netplay::FrameInput input;
        input.buttons=u16(p[0])|(u16(p[1])<<8);input.analogMode=Netplay::AnalogMode(p[2]);
        input.unlimited=(p[3]&1)!=0;input.touchUsed=(p[3]&2)!=0;input.touchBomb=(p[3]&4)!=0;
        const u32 x=word(p+4),y=word(p+8);std::memcpy(&input.x,&x,4);std::memcpy(&input.y,&y,4);
        if(!NetplayRuntime::ValidInput(input))return false;
    }
    info=std::move(next);d=description;return true;
}
bool ReplayArchive::Load(const u8* data,std::size_t size){
    Netplay::InputReplayInfo info;ReplayDescription d;
    if(!Inspect(data,size,info,d))return false;
    std::size_t score_size=0,offset=0;if(!envelope(data,size,score_size,offset))return false;
    if(!tape.Decode(data+offset,size-offset))return false;
    boot_score.assign(data+HeaderBytes+sizeof(GameConfiguration),data+offset);
    description=d;base=generation=cursor=0;stamps={};return true;
}
bool ReplayArchive::Preview(const u8* bytes,std::size_t size,ReplayMetadata& out,u32* scores){
    Netplay::InputReplayInfo info;ReplayDescription d;if(!Inspect(bytes,size,info,d))return false;
    ReplayMetadata preview{};preview.shot_type=u8(d.setup.characters[0]);preview.difficulty=u8(d.setup.difficulty);
    preview.spell_number=-1;preview.configuration=d.configuration;preview.spell_score=d.score;
    preview.header.file_size=u32(size);preview.header.payload_size=u32(size);preview.major_version=0x100;
    std::memcpy(preview.player_name,d.name,8);std::memcpy(preview.date,d.date,6);
    for(u32 i=0;i<info.chapterCount;++i){const auto stage=(info.chapters[i].label&15)-1;
        if(!preview.header.stage_offsets[stage])preview.header.stage_offsets[stage]=info.chapters[i].firstFrame+1;}
    if(scores)std::copy(d.stage_scores,d.stage_scores+9,scores);out=preview;return true;
}
bool ReplayArchive::BeginFrame(u32 local){
    if(!Recording()||local==Netplay::INVALID_FRAME||base>Netplay::INVALID_FRAME-local)return false;
    auto& s=stamps[local%stamps.size()];s={};s.frame=local;return true;
}
bool ReplayArchive::Stamp(u32 local,u32 stage,u32 score){
    if(!Recording()||stage>8||stamps[local%stamps.size()].frame!=local)return false;
    auto& s=stamps[local%stamps.size()];s.label=(generation<<4)|(stage+1);s.score=std::min(score,999999999u);return true;
}
bool ReplayArchive::RequestSave(u32 local,i32 slot,const char* name,const char* date){
    if(!Recording()||slot<1||slot>15||!name||!date)return false;
    auto& s=stamps[local%stamps.size()];if(s.frame!=local)return false;
    s.save_slot=slot;std::memset(s.name,0,8);std::memcpy(s.name,name,std::min<std::size_t>(8,std::strlen(name)));
    std::memcpy(s.date,date,6);return true;
}
bool ReplayArchive::Commit(const NetplayRuntime& net,SaveCallback save,void* context){
    if(!Recording())return true;
    const auto confirmed=net.ConfirmedThrough(),last=net.LastFrame();
    if(net.Correcting()||net.RollbackFrame()!=Netplay::INVALID_FRAME||confirmed==Netplay::INVALID_FRAME||last==Netplay::INVALID_FRAME)return true;
    if(net.Generation()!=generation)return false;
    for(u32 local=tape.Info().frameCount-base;local<=std::min(confirmed,last);++local){
        const auto& stamp=stamps[local%stamps.size()];Frame inputs{};
        if(stamp.frame!=local||!net.ConfirmedInputs(local,inputs)||
           !tape.Append(base+local,stamp.label,inputs.data(),description.setup.player_count))return false;
        description.score=stamp.score;description.last_stage=(stamp.label&15)-1;
        description.stage_scores[description.last_stage]=stamp.score;
        if(stamp.save_slot&&save){
            auto d=description;std::memcpy(d.name,stamp.name,8);std::memcpy(d.date,stamp.date,6);
            std::vector<u8> bytes;char path[48];std::snprintf(path,sizeof(path),"replay/th8_%02d.rpyx",stamp.save_slot);
            if(!Encode(bytes,&d)||!save(context,path,bytes.data(),u32(bytes.size())))return false;
        }
    }
    return true;
}
bool ReplayArchive::NextGeneration(u32 next){
    if(next!=generation+1||next>0x0fffffffu)return false;
    base=Cursor();generation=next;stamps={};return true;
}
const ReplayArchive::Frame* ReplayArchive::PlaybackFrame(u32 local)const{
    return Playing()&&base+u64(local)==cursor?tape.FrameAt(cursor):nullptr;
}
bool ReplayArchive::AdvancePlayback(u32 local,u32 stage){
    if(!PlaybackFrame(local)||stage>8)return false;
    const auto& info=tape.Info();u32 chapter=0;
    for(u32 i=0;i<info.chapterCount&&info.chapters[i].firstFrame<=cursor;++i)chapter=info.chapters[i].label;
    if(chapter!=((generation<<4)|(stage+1)))return false;
    ++cursor;return true;
}
bool ReplayArchive::Encode(std::vector<u8>& out,const ReplayDescription* d)const{
    std::vector<u8> input;const auto desc=metadata(d?*d:description);
    if(!tape.Encode(&input,&desc))return false;
    const auto size=HeaderBytes+sizeof(GameConfiguration)+boot_score.size()+input.size();
    if(size>MaxBytes)return false;
    std::vector<u8> bytes;bytes.reserve(size);bytes.insert(bytes.end(),signature,signature+8);
    put(bytes,u32(input.size()));put(bytes,sizeof(GameConfiguration));put(bytes,u32(boot_score.size()));put(bytes,0);
    const auto* config=reinterpret_cast<const u8*>(&description.configuration);
    bytes.insert(bytes.end(),config,config+sizeof(GameConfiguration));
    bytes.insert(bytes.end(),boot_score.begin(),boot_score.end());bytes.insert(bytes.end(),input.begin(),input.end());
    const auto sum=checksum(bytes.data(),bytes.size());for(unsigned i=0;i<4;++i)bytes[20+i]=u8(sum>>(8*i));
    out=std::move(bytes);return true;
}
u32 ReplayArchive::StageFrame(u32 stage)const{
    if(stage>8)return Netplay::INVALID_FRAME;
    for(u32 i=0;i<tape.Info().chapterCount;++i)if((tape.Info().chapters[i].label&15)==stage+1)return tape.Info().chapters[i].firstFrame;
    return Netplay::INVALID_FRAME;
}
}
