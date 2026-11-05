#include "ReplayFiles.hpp"
#include "../platform/GameState.hpp"
#include <algorithm>
#include <cstring>

namespace th10::multiplayer {
namespace {
bool full_path(const char* file,char(&path)[256]){
    if(!file||std::strlen(file)>240)return false;
    if(std::strncmp(file,"replay/",7)==0)std::strcpy(path,file);
    else{std::memcpy(path,"replay/",7);std::strcpy(path+7,file);}
    return ReplayArchive::SafePath(path);
}
}
bool ReadReplayFile(browser::FileSystem& files,const char* name,std::vector<u8>& bytes){
    char path[256];if(!full_path(name,path))return false;
    const auto handle=files.host.open(path,false);if(handle==0xffffffffu)return false;
    const auto size=files.host.size(handle);
    if(size<Netplay::InputReplay::HeaderBytes||size>Netplay::InputReplay::MaxBytes){files.host.close(handle);return false;}
    std::vector<u8> candidate(size);const bool complete=files.host.read(handle,candidate.data(),size)==size;
    files.host.close(handle);if(!complete)return false;bytes=std::move(candidate);return true;
}
i32 LoadReplayPreview(browser::ReplayDocument& document,const char* name){
    document.close();document.memory.clear();document.value.initialize();
    std::vector<u8> bytes;Netplay::InputReplayInfo info;ReplayDescription description;
    if(!ReadReplayFile(document.files,name,bytes)||
       !ReplayArchive::Inspect(bytes.data(),bytes.size(),info,description))return -1;
    auto& replay=document.value;
    if(std::strlen(name)>=sizeof(replay.filename))return -1;
    std::strcpy(replay.filename,name);replay.mode=2;
    replay.info=reinterpret_cast<ReplayInfo*>(document.memory.allocate(sizeof(ReplayInfo)));
    if(!replay.info)return -1;auto& preview=*replay.info;preview.initialize();
    std::memcpy(preview.name,description.name,12);preview.timestamp=description.timestamp;
    preview.score=description.score;preview.score_units=description.scoreUnits;
    preview.character=i32(description.setup.loadouts[0].character);preview.shot_type=i32(description.setup.loadouts[0].shot);
    preview.difficulty=i32(description.setup.difficulty);preview.last_stage=description.lastStage;
    std::memcpy(preview.configuration,&description.configuration,52);
    for(u32 chapter=0;chapter<info.chapterCount;++chapter){
        const auto stage=info.chapters[chapter].label&255u;
        if(stage==8)continue; // Extra clear/rank marker; readers has only 0..7.
        if(replay.readers[stage].stage)continue;
        auto* value=reinterpret_cast<ReplayStage*>(document.memory.allocate(sizeof(ReplayStage)));
        if(!value){document.memory.clear();replay.initialize();return -1;}
        value->initialize();value->stage=std::int16_t(stage);
        replay.readers[stage].stage=value;++preview.stage_count;
    }
    return 0;
}
bool SaveReplayFile(browser::FileSystem& files,browser::ReplayCalendar& calendar,browser::GameState& state,
                    const char* file,const char* name,i32 score,i32 scoreUnits,i32 lastStage,u32 frameCount){
    char path[256];if(!full_path(file,path)||!name||std::strlen(name)>11||
       scoreUnits<0||scoreUnits>9||lastStage<1||lastStage>8)return false;
    auto& tape=state.multiplayer_replay;auto description=tape.Description();
    std::memset(description.name,0,12);std::strcpy(description.name,name);
    if(!tape.Playing()){
        description.timestamp=calendar.timestamp();description.score=score;
        description.scoreUnits=scoreUnits;description.lastStage=lastStage;
    }
    std::vector<u8> bytes;if(!tape.Encode(bytes,&description,frameCount))return false;
    return files.host.replace(path,bytes.data(),u32(bytes.size()));
}
}
