#include "ReplayFile.hpp"
#include <initializer_list>
#ifdef TH_ENABLE_THPRAC
#include "PracticeConfig.hpp"
#endif
namespace th10 {
namespace {
u32 address(const void* value){return static_cast<u32>(reinterpret_cast<uintptr_t>(value));}
void write(ReplaySaveEnvironment& env,const void* bytes,u32 length){if(env.is_open()&&env.write(bytes,length)!=length)env.close_and_unlock();}
struct ReplayText {
    char* cursor;ReplaySaveEnvironment& environment;
    void append(const char* format,std::initializer_list<u32> arguments={}){cursor+=environment.format(cursor,format,arguments.begin(),arguments.size());}
};
void finish_user_chunk(u8* bytes,char* end,ReplaySaveEnvironment& env){const u32 length=(static_cast<u32>(end-reinterpret_cast<char*>(bytes))+1+3)&~3u;std::memcpy(bytes+4,&length,4);write(env,bytes,length);}
}
// 0x429b60. The file combines a compressed game stream with two USER chunks.
// Saving again retains the original accumulation of each stage's frame count.
i32 save_replay(Replay& replay,const char* file_name,const char* player_name,ReplaySaveEnvironment& env){
    const auto name_length=std::strlen(player_name),file_length=std::strlen(file_name);if(name_length>=sizeof(replay.info->name)||file_length+7>=256)return -1;std::memcpy(replay.info->name,player_name,name_length+1);for(u32 i=name_length;i<8;i++)replay.info->name[i]=' ';
    env.create_directory("replay");char path[256];std::memcpy(path,"replay/",7);std::memcpy(path+7,file_name,file_length+1);
    i32 first_stage=0,last_stage=0,stage_count=0;u32 length=sizeof(ReplayInfo);
    for(i32 stage=0;stage<8;stage++)if(auto* snapshot=replay.stages[stage]){if(!first_stage)first_stage=stage;last_stage=stage;snapshot->stream_bytes=0;length+=sizeof(ReplayStage);for(auto* node=replay.buffers[stage].next;node;node=node->next){const auto* buffer=node->value;const auto frames=static_cast<i32>(buffer->input_cursor-buffer->inputs);const i32 bytes=frames*6+static_cast<i32>(buffer->rate_cursor-buffer->frame_rates);length+=bytes;snapshot->stream_bytes=wrapping_add(snapshot->stream_bytes,bytes);snapshot->frames=wrapping_add(snapshot->frames,frames);}++stage_count;}
    replay.info->stage_count=stage_count;replay.info->score=env.game->score;replay.info->slow_rate=env.cheat_movement_used&&*env.cheat_movement_used?100.f:(number(100)-(Extended::from_double(*env.active_time)/Extended::from_double(*env.total_time))*number(100)).to_float();
    auto* plain=env.allocate_bytes(length);if(!plain)return -1;std::memcpy(plain,replay.info,sizeof(ReplayInfo));u32 position=sizeof(ReplayInfo);
    for(i32 stage=0;stage<8;stage++)if(replay.stages[stage]){std::memcpy(plain+position,replay.stages[stage],sizeof(ReplayStage));position+=sizeof(ReplayStage);for(auto* node=replay.buffers[stage].next;node;node=node->next){const auto& block=*node->value;const u32 bytes=(block.input_cursor-block.inputs)*6;std::memcpy(plain+position,block.inputs,bytes);position+=bytes;}for(auto* node=replay.buffers[stage].next;node;node=node->next){const auto& block=*node->value;const u32 bytes=block.rate_cursor-block.frame_rates;std::memcpy(plain+position,block.frame_rates,bytes);position+=bytes;}}
#ifdef TH_ENABLE_THPRAC
    // thprac_th10.cpp:2397 th10_rep_power_fix. repBuffer is the plain payload
    // (ReplayInfo at 0, first ReplayStage at 0x64); the fix writes at +0x198,
    // which is ReplayStage::option_targets (offsetof 0x134). ReplayInfo's
    // character/shot_type sit at 0x50/0x54, matching upstream's selection.
    if(env.practice_mode)practice_replay_power_fix(plain,replay.info->character,replay.info->shot_type,env.practice_power);
#endif
    u32 packed_length=0;auto* packed=encode_lzss(plain,position,packed_length,env.search,env);env.release_bytes(plain);if(!packed)return -1;
    transform_resource(packed,packed_length,0x3d,0x7a,128,packed_length,true,env);transform_resource(packed,packed_length,0xaa,0xe1,1024,packed_length,true,env);
    replay.header->unpacked_bytes=position;replay.header->packed_bytes=packed_length;replay.header->user_offset=packed_length+sizeof(ReplayHeader);env.open(path);write(env,replay.header,sizeof(ReplayHeader));write(env,packed,packed_length);env.release_bytes(packed);
    auto* user=env.allocate_bytes(65535);if(!user){if(env.is_open())env.close_and_unlock();return -1;}std::memset(user,0,65535);const u32 signature=0x52455355;std::memcpy(user,&signature,4);ReplayText text{reinterpret_cast<char*>(user+12),env};text.append(env.title);text.append("Version %s\r\n",{address(env.version)});text.append("Name %s\r\n",{address(replay.info->name)});
    const auto date=env.local_date(replay.info->timestamp);text.append("Date %.2d/%.2d/%.2d %.2d:%.2d\r\n",{static_cast<u32>(date.year%100),static_cast<u32>(date.month+1),static_cast<u32>(date.day),static_cast<u32>(date.hour),static_cast<u32>(date.minute)});text.append("Chara %s\r\n",{address(env.characters[replay.info->character*3+replay.info->shot_type])});text.append("Rank %s\r\n",{address(env.difficulties[replay.info->difficulty])});
    if(replay.info->last_stage>=8)text.append(first_stage==7?"Extra Stage Clear\r\n":"Stage All Clear\r\n");else if(first_stage!=last_stage)text.append(env.stage_range,{static_cast<u32>(first_stage),static_cast<u32>(last_stage)});else if(first_stage==7)text.append("Extra Stage\r\n");else text.append("Stage %d\r\n",{static_cast<u32>(first_stage)});
    text.append("Score %d\r\n",{static_cast<u32>(replay.info->score)});const double slow=Extended::from_float(replay.info->slow_rate).to_double();u32 slow_bits[2];std::memcpy(slow_bits,&slow,8);text.append("Slow Rate %2.2f\r\n",{slow_bits[0],slow_bits[1]});finish_user_chunk(user,text.cursor,env);
    std::memset(user,0,65535);std::memcpy(user,&signature,4);user[8]=1;ReplayText comment{reinterpret_cast<char*>(user+12),env};comment.append(env.comment);finish_user_chunk(user,comment.cursor,env);env.release_bytes(user);if(env.is_open())env.close_and_unlock();return 0;
}
}
