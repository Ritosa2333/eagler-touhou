#include "ReplayArchive.hpp"
#include "Binary.hpp"
#include <algorithm>
#include <cstdio>
namespace th09 {
namespace {
#include "ReplayData.inc"
}
void ReplayArchive::begin(const ReplayMetadata& settings){metadata=settings;error.clear();active=-1;for(auto& s:stages){s.present=false;s.headers={};s.recording=ReplayRecording{};}}
bool ReplayArchive::begin_stage(u32 index,const ReplayRoundSettings& settings){
    if(index>=10){error="Replay stage bounds";return false;}
    const u32 ending=active<0?0:stages[active].recording.ending_frames();
    auto& s=stages[index];s.headers={};s.present=true;s.recording.begin(ending);active=i32(index);
    for(u32 side=0;side<3;++side)write16(s.headers[side].data()+4,settings.players[0].seed);
    for(u32 side=0;side<2;++side){const auto& p=settings.players[side];auto* h=s.headers[side].data();write32(h,p.points);h[6]=p.character;h[7]=p.cpu;h[8]=p.lives;write16(h+10,p.cpu_level);}
    auto* h=s.headers[0].data();h[9]=u8(settings.players[0].selector);h[12]=settings.players[0].background;write32(h+16,settings.players[0].extends);return true;
}
bool ReplayArchive::record(u32 flags,bool paused,const GameInput (&inputs)[3],const bool (&cpu)[2],u8 rate){return active>=0&&stages[active].recording.advance(flags,paused,inputs,cpu,rate);}
ReplayFile ReplayArchive::finish(Rng& rng,const char* name,const char* date,i32 left,i32 right){
    ReplayFile result;error.clear();if(active<0||metadata.mode>2||metadata.versus>3||metadata.difficulty>4||left<0||left>=16||right<0||right>=16){error="Replay metadata bounds";return result;}
    if(!name||!date||std::strlen(name)>8||std::strlen(date)>9){error="Replay name/date bounds";return result;}
    std::vector<u8> bytes(ReplayFile::metadata_end);write32(bytes.data(),fourcc('T','9','R','P'));write16(bytes.data()+4,2);bytes[7]=1;
    bytes[0xc1]='a';write16(bytes.data()+0xc2,0x150);std::memcpy(bytes.data()+0xc4,date,std::strlen(date));std::memcpy(bytes.data()+0xce,name,std::strlen(name));
    bytes[0xd7]=metadata.difficulty;std::copy(metadata.configuration.begin(),metadata.configuration.end(),bytes.begin()+0xdc);
    write32(bytes.data()+0x1d0,30);write32(bytes.data()+0x1d4,metadata.version_time_a);write32(bytes.data()+0x1d8,metadata.version_time_b);std::memcpy(bytes.data()+0x1dc,"0150a",6);
    bytes[0x1e4]=metadata.mode;bytes[0x1e5]=metadata.versus;std::copy_n(metadata.health,2,bytes.data()+0x1e6);std::copy_n(metadata.alternate,2,bytes.data()+0x1e8);
    for(u32 group=0;group<4;++group)for(u32 index=0;index<10;++index){const auto& stage=stages[index];if(!stage.present)continue;
        const auto stream=stage.recording.stream(group);if(bytes.size()+stream.size()+32>=ReplayFile::max_size){error="Replay recording too large";return result;}
        write32(bytes.data()+32+(group*10+index)*4,u32(bytes.size()));if(group<3)bytes.insert(bytes.end(),stage.headers[group].begin(),stage.headers[group].end());bytes.insert(bytes.end(),stream.begin(),stream.end());
    }
    // The native writer omits an incomplete final 30-frame sample. A recording
    // without any complete sample cannot form a playable native replay.
    if(stages[active].recording.frame()<30){error="Replay shorter than one frame-rate sample";return result;}
    write32(bytes.data()+28,u32(bytes.size())-ReplayFile::header_size);
    std::string description;char line[256];
    const auto append=[&](const char* format,auto... args){const int n=std::snprintf(line,sizeof(line),format,args...);if(n>0&&n<int(sizeof(line)))description.append(line,u32(n));};
    append(replay_name,name);append(replay_date,date);append(replay_difficulty,replay_difficulties[metadata.difficulty]);append(replay_mode,replay_modes[metadata.mode==2?metadata.versus+2:metadata.mode]);
    if(metadata.mode==2)append(replay_pair,replay_characters[left],replay_characters[right]);else append(replay_character,replay_characters[left]);
    append(replay_health,double(metadata.health[0])*.5,double(metadata.health[1])*.5);append(replay_version,replay_version_label);
    const u32 tail=u32(bytes.size()),length=(u32(description.size())+14)&~1u;bytes.resize(tail+length);write32(bytes.data()+tail,fourcc('U','S','E','R'));write32(bytes.data()+tail+4,length);std::memcpy(bytes.data()+tail+12,description.data(),description.size());
    bytes[21]=u8(rng.below(128)+64);bytes[0xc0]=u8(rng.below(256));bytes[20]=u8(rng.below(256));
    if(!result.assign(bytes.data(),u32(bytes.size())))error="Replay serialization";return result;
}
}
