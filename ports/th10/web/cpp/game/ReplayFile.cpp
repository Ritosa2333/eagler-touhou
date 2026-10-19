#include "ReplayFile.hpp"
namespace th10 {
// 0x42a200. Demo replays come from the archive, ordinary replays from replay/.
i32 load_replay(Replay& replay,const char* name,ReplayFileEnvironment& env){
    if(std::strlen(name)>=sizeof(replay.filename))return -1;std::strcpy(replay.filename,name);u8* packed;
    if(!(*env.game_flags&0x20)){
        char path[256];const auto length=std::strlen(name);if(length+7>=sizeof(path))return -1;std::memcpy(path,"replay/",7);std::memcpy(path+7,name,length+1);
        if(!env.exists(path)||env.open(path))return -1;
        replay.header=reinterpret_cast<ReplayHeader*>(env.read(sizeof(ReplayHeader)));
        if(!replay.header||replay.header->signature!=0x72303174||replay.header->version!=5){env.close();return -1;}
        packed=env.read(replay.header->packed_bytes);env.close();
    }else{u32 length;replay.header=reinterpret_cast<ReplayHeader*>(env.archive(name,length));if(!replay.header)return -1;packed=reinterpret_cast<u8*>(replay.header+1);}
    if(!packed)return -1;replay.unpacked=env.allocate_bytes(replay.header->unpacked_bytes);if(!replay.unpacked)return -1;
    transform_resource(packed,replay.header->packed_bytes,0xaa,0xe1,0x400,replay.header->packed_bytes,false,env);
    transform_resource(packed,replay.header->packed_bytes,0x3d,0x7a,0x80,replay.header->packed_bytes,false,env);
    decode_lzss(packed,replay.header->packed_bytes,replay.unpacked,replay.header->unpacked_bytes,env.dictionary);
    replay.info=reinterpret_cast<ReplayInfo*>(replay.unpacked);auto* stage=reinterpret_cast<ReplayStage*>(replay.info+1);
    for(i32 i=0;i<(replay.info->stage_count>=8?6:replay.info->stage_count);i++){auto& reader=replay.readers[stage->stage];reader.stage=stage;reader.inputs=reinterpret_cast<ReplayKeys*>(stage+1);reader.frame_rates=reinterpret_cast<u8*>(reader.inputs+stage->frames);stage=reinterpret_cast<ReplayStage*>(reinterpret_cast<u8*>(stage+1)+stage->stream_bytes);}
    if(!(*env.game_flags&0x20))env.release_bytes(packed);return 0;
}
}
