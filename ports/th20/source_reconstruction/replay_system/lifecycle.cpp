#include "replay.hpp"
#include "../startup_scene/startup.hpp"
#include "../program_entry/program_entry.hpp"
#include "../gameplay/loading_dependencies.hpp"
#include <cstring>
#include <new>
#include <stdexcept>
namespace th20::source::replay {
namespace pe=program_entry;
ReplayInf::ReplayInf():mode(0),fast_forward(0),header(nullptr),user(nullptr),stages{},active_chunk(nullptr),chunk_count(0),decoded(nullptr),fps(0),frame(0),additional_update(nullptr),active_stage(0),filename{} {for(auto& link:recordings)scheduler::initialize_link(link,nullptr);flags&=~3u;}
ReplayInf::~ReplayInf(){
    runtime::release_bytes(header);for(int i=0;i<8;++i)clear_recording(i);runtime::release_bytes(user);user=nullptr;
    for(auto& record:stages){runtime::release_bytes(record);record=nullptr;}
    if(update_node)scheduler::remove(*pe::function_controller,pe::scheduler_environment,update_node);
    if(additional_update)scheduler::remove(*pe::function_controller,pe::scheduler_environment,additional_update);
    if(draw_node)scheduler::remove(*pe::function_controller,pe::scheduler_environment,draw_node);
    if(controller()==this)startup::unrecovered::owner_005c60fc=nullptr;
}
void ReplayInf::clear_recording(int index){
    if(index<0||index>=8)throw std::out_of_range("Replay stage index outside original eight slots");
    for(auto* link=recordings[index].next;link;){auto* next=link->next;auto* chunk=reinterpret_cast<RecordingChunk*>(link->value);chunk->~RecordingChunk();runtime::release_bytes(chunk);link=next;}
}
scheduler::Link* ReplayInf::add_recording_chunk(int index){
    if(index<0||index>=8)throw std::out_of_range("Replay recording stage index outside original eight slots");
    auto* memory=runtime::allocate_bytes(sizeof(RecordingChunk));if(!memory)throw std::bad_alloc();auto* chunk=new(memory)RecordingChunk;
    auto* tail=&recordings[index];while(tail->next)tail=tail->next;scheduler::insert_after(*tail,chunk->link);++chunk_count;return &chunk->link;
}
void ReplayInf::disable_callbacks(){if(additional_update)scheduler::disable(*additional_update);if(draw_node)scheduler::disable(*draw_node);}
namespace {ReplayInf* allocate(){auto* memory=::operator new(sizeof(ReplayInf),std::nothrow);if(!memory)return nullptr;std::memset(memory,0,sizeof(ReplayInf));return new(memory)ReplayInf;}}
ReplayInf* create(int mode,const char* path){auto* value=allocate();if(initialize(*value,mode,path)!=0){runtime::retire_callback_owner(value);return nullptr;}startup::unrecovered::owner_005c60fc=value;return value;}
ReplayInf* read_metadata(const char* path){auto* value=allocate();if(initialize(*value,2,path)!=0){runtime::retire_callback_owner(value);return nullptr;}return value;}
}
namespace th20::source::gameplay::unrecovered {
runtime::CallbackOwner* create_0050a930(int mode,const char* path){return replay::create(mode,path);}
void reset_replay_owner(){replay::controller()->reset_stage();}
}
