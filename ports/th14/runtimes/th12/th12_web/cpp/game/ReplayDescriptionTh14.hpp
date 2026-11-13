#pragma once
#include "Replay.hpp"
#include <cmath>
#include <cstring>
#include <string>

namespace th11 {
#if PORT_TITLE == 14
// Launcher metadata comes from the same native decoder used for actual play.
// Never mutate an active session or infer stages from unvalidated JS bytes.
inline bool describe_replay_th14(const u8* data,u32 size,std::string& output,std::string& error){
    output.clear();error.clear();Replay replay;
    if(!replay.open(data,size)){error=replay.error();return false;}
    if(replay.character()>2||replay.subtype()>1||replay.difficulty()>4){error="invalid TH14 replay selection";return false;}
    const auto& bytes=replay.decoded();
    const auto word=[&](u32 offset){return u32(bytes[offset])|u32(bytes[offset+1])<<8|u32(bytes[offset+2])<<16|u32(bytes[offset+3])<<24;};
    float slowdown;std::memcpy(&slowdown,bytes.data()+0x74,4);
    if(!std::isfinite(slowdown)){error="invalid TH14 replay slowdown";return false;}
    const u64 timestamp=u64(word(12))|u64(word(16))<<32;
    output="{\"nameBytes\":[";
    for(u32 i=0;i<8;++i){if(i)output+=',';output+=std::to_string(bytes[i]);}
    output+="],\"timestamp\":"+std::to_string(timestamp)+",\"score\":"+std::to_string(u64(word(20))*10);
    output+=",\"slowdown\":"+std::to_string(slowdown)+",\"character\":"+std::to_string(replay.character());
    output+=",\"shot\":"+std::to_string(replay.subtype())+",\"difficulty\":"+std::to_string(replay.difficulty());
    output+=",\"lastStage\":"+std::to_string(word(0x88))+",\"mode\":\"";
    output+=(bytes[10]&2)?"spell-practice":(bytes[10]&1)?"practice":"normal";
    output+="\",\"stages\":[";bool first=true;
    for(u32 stage=1;stage<=7;++stage){
        const auto* entry=replay.stage(stage);if(!entry)continue;
        ReplayStageState snapshot;snapshot.read(replay.header(stage));
        if(!first)output+=',';first=false;
        output+="{\"id\":"+std::to_string(stage)+",\"score\":"+std::to_string(u64(u32(snapshot.score))*10);
        output+=",\"frames\":"+std::to_string(entry->frames)+"}";
    }
    output+="]}";return true;
}
#endif
}
