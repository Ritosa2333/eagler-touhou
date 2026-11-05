#include "ReplayFiles.hpp"
#include "../game/TextFormat.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "../multiplayer/ReplayFiles.hpp"
#endif
namespace th10::browser {
ReplayDocument::ReplayDocument(FileSystem& f,u32 mode_flags):files(f),flags(mode_flags){game_flags=&flags;dictionary=history;}
ReplayDocument::~ReplayDocument(){close();}
i32 ReplayDocument::load(const char* name){
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    return multiplayer::LoadReplayPreview(*this,name);
#else
    close();memory.clear();value.initialize();const i32 result=load_replay(value,name,*this);if(result){memory.clear();value.initialize();}return result;
#endif
}
u8* ReplayDocument::allocate_bytes(u32 bytes){return memory.allocate(bytes);}
void ReplayDocument::release_bytes(void* bytes){memory.release(bytes);}
bool ReplayDocument::exists(const char* path){const u32 id=files.host.open(path,false);if(id==0xffffffff)return false;files.host.close(id);return true;}
i32 ReplayDocument::open(const char* path){close();input=files.host.open(path,false);return input==0xffffffff?-1:0;}
u8* ReplayDocument::read(u32 bytes){
    if(input==0xffffffff)return nullptr;const u32 size=files.host.size(input),position=files.host.seek(input,0,1);
    if(position>size||bytes>size-position)return nullptr;
    auto* data=memory.allocate(bytes);if(data&&files.host.read(input,data,bytes)!=bytes){memory.release(data);return nullptr;}return data;
}
void ReplayDocument::close(){if(input!=0xffffffff){files.host.close(input);input=0xffffffff;}}
u8* ReplayDocument::archive(const char* name,u32& length){return static_cast<u8*>(memory.adopt(ResourceFiles{files}.load(name,&length,false)));}
namespace {
constexpr const char* characters[]={"ReimuA ","ReimuB ","ReimuC ","MarisaA","MarisaB","MarisaC"};
constexpr const char* difficulties[]={"Easy   ","Normal ","Hard   ","Lunatic","Extra  "};
// Byte strings from the respective original executable: CP932 / CP936.
constexpr const char* titles[]={
    "\x93\x8c\x95\xfb\x95\x97\x90\x5f\x98\x5e\x20\x83\x8a\x83\x76\x83\x8c\x83\x43\x83\x74\x83\x40\x83\x43\x83\x8b\x8f\xee\x95\xf1\r\n",
    "\xb6\xab\xb7\xbd\xb7\xe7\xc9\xf1\xc2\xbc\xd3\xce\xcf\xb7\xc2\xbc\xcf\xf1\xce\xc4\xbc\xfe\xd0\xc5\xcf\xa2\r\n"
};
constexpr const char* comments[]={"\x83\x52\x83\x81\x83\x93\x83\x67\x82\xf0\x8f\x91\x82\xaf\x82\xdc\x82\xb7","\xca\xe4\xc8\xeb\xd7\xa2\xca\xcd"};
constexpr bool no_cheat_movement=false;
}
ReplayWriter::ReplayWriter(FileSystem& f,ReplayCalendar& clock,GameEconomy& economy,const double& active,const double& total,bool chinese):ReplayWriter(f,clock,economy,active,total,no_cheat_movement,chinese){}
ReplayWriter::ReplayWriter(FileSystem& f,ReplayCalendar& clock,GameEconomy& economy,const double& active,const double& total,const bool& cheat,bool chinese):files(f),calendar(clock){
    game=&economy;active_time=&active;total_time=&total;cheat_movement_used=&cheat;search={nodes,dictionary};
    title=titles[chinese];version="1.00a";comment=comments[chinese];stage_range="Stage %d \x81\x60 %d\r\n";
    ReplaySaveEnvironment::characters=browser::characters;ReplaySaveEnvironment::difficulties=browser::difficulties;
}
ReplayWriter::~ReplayWriter(){close_and_unlock();}
i32 ReplayWriter::save(Replay& replay,const char* file,const char* name,const std::vector<u8>& trailer){
    // The original serializer adds the live buffer lengths to stage.frames.
    // Results can save the same run into another slot, so start each Web save
    // from the live buffers rather than accumulating the previous save again.
    for(auto* stage:replay.stages)if(stage)stage->frames=0;
    write_failed=false;motion_trailer=&trailer;
    const i32 result=save_replay(replay,file,name,*this);
    close_and_unlock();motion_trailer=nullptr;memory.clear();return write_failed?-1:result;
}
u8* ReplayWriter::allocate_bytes(u32 bytes){return memory.allocate(bytes);}
void ReplayWriter::release_bytes(void* bytes){memory.release(bytes);}
void ReplayWriter::open(const char* name){close_and_unlock();output=files.host.open(name,true);}
bool ReplayWriter::is_open(){return output!=0xffffffff;}
u32 ReplayWriter::write(const void* bytes,u32 length){const auto written=files.host.write(output,static_cast<const u8*>(bytes),length);if(written!=length){motion_trailer=nullptr;write_failed=true;}return written;}
void ReplayWriter::close_and_unlock(){if(output!=0xffffffff){if(motion_trailer&&!motion_trailer->empty())write_failed|=files.host.write(output,motion_trailer->data(),motion_trailer->size())!=motion_trailer->size();files.host.close(output);output=0xffffffff;}}
ReplayDate ReplayWriter::local_date(i32 timestamp){return calendar.local_date(timestamp);}
i32 ReplayWriter::format(char* output,const char* pattern,const u32* arguments,u32 count){const i32 length=format_text(output,512,pattern,arguments,count);if(length<0)__builtin_trap();return length;}
}
