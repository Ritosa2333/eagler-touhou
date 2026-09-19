#include "ReplayExport.hpp"
#include "ReplayText.hpp"
#include <cstdio>
#include <string>
namespace th08 {
std::vector<u8> export_replay(ReplayRecording& recording,GameplaySession& session,const EclGlobals& game,const ReplayExportContext& context){
    if(!recording.ready()||(game.game_flags&8)||game.shot<0||game.shot>=12||game.difficulty>=5)return {};
    auto metadata=recording.metadata();if(metadata.configuration.slow_mode)return {};
    if(!(game.game_flags&1)&&game.difficulty<4&&std::memcmp(&session.display_config,&metadata.configuration,sizeof(GameConfiguration)))return {};
    recording.stop();recording.finish_score(session.stage_copy,session.numbers.score);metadata.spell_score=session.numbers.display_score;
    if(metadata.spell_number>=0){if(metadata.spell_number>=222)return {};std::memcpy(metadata.spell_name,session.records[metadata.spell_number].name,sizeof(metadata.spell_name));}
    const auto fraction=number(context.rendered_frames)/number(context.total_frames)-number(.5f);
    float rate=(fraction+fraction).to_float();if(rate<0)rate=0;else if(rate>=1)rate=1;
    metadata.lag=context.cheat_movement_used?100.f:((number(1)-number(rate))*number(100)).to_float();
    session.history.humanity=(Extended::from_int(context.human_frames)/Extended::from_int(context.active_frames)*number(10000)).truncate_int();
    char name[9]{},timestamp[21]{},spell[49]{};std::memcpy(name,context.player_name,8);std::memcpy(timestamp,context.timestamp,20);std::memcpy(spell,metadata.spell_name,48);
    std::string info;auto append=[&](i32 format,auto... args){char buffer[256]{};const i32 size=std::snprintf(buffer,sizeof(buffer),replay_text::formats[format],args...);if(size>0)info.append(buffer,std::min<u32>(u32(size),sizeof(buffer)-1));};
    append(0,name);append(1,timestamp);append(2,replay_text::characters[game.shot]);append(3,i32(session.numbers.display_score));append(4,replay_text::difficulties[game.difficulty]);
    if(metadata.spell_number>=0)append(5,i32(metadata.spell_number)+1,spell);else append(6,(game.game_flags&16)||game.stage>=9?"Clear":replay_text::stages[game.stage]);
    append(7,Scalar::truncate(session.numbers.deaths));append(8,Scalar::truncate(session.numbers.bombs_used));append(9,double(metadata.lag));append(10,(Extended::from_int(session.history.humanity)/number(100)).to_double());append(11,1,0,100);
    metadata.header.unknown7=1;std::memcpy(metadata.player_name,name,8);metadata.unknown7a=0;std::memcpy(metadata.date,context.date,5);metadata.date[5]=0;
    metadata.header.key=u8(session.random.bounded16(128)+64);metadata.unknown68=u8(session.random.bounded16(256));metadata.header.unknown14=u8(session.random.bounded16(256));
    const float legacy_lag=Scalar::add(metadata.lag,1.12f);std::memcpy(metadata.reservedf0+36,&legacy_lag,4);metadata.unknown120=30;
    auto bytes=recording.assemble(metadata);if(bytes.empty())return {};
    const u32 tail=u32(bytes.size()),length=(12+u32(info.size())+1)&~1u;bytes.resize(tail+length,0);
    const u32 magic=fourcc('U','S','E','R');std::memcpy(bytes.data()+tail,&magic,4);std::memcpy(bytes.data()+tail+4,&length,4);std::memcpy(bytes.data()+tail+12,info.data(),info.size());
    ReplayFile file;if(!file.assign_decoded(bytes.data(),bytes.size()))return {};return file.encode();
}
}
