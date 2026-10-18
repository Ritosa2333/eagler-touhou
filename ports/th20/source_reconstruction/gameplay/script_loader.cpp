#include "script_loader.hpp"
#include "../archive/resource_manager.hpp"
#include <algorithm>
#include <cstring>
#include <list>
#include <unordered_map>
#include <stdexcept>
namespace th20::source::gameplay {
namespace {
// Original5c49f4 is a 12-byte pmr::list of data-pointer/pmr::string pairs.
// The separate sizes map is source bounds metadata, not fabricated game data.
struct CachedScript {std::uint8_t* data;std::pmr::string name;};
// MSVC x86 STL layout evidence (pmr::list node bookkeeping and pmr::string
// member size); binding on the Windows build only.
#ifndef TH_SDL3
static_assert(sizeof(CachedScript)==32 && sizeof(std::pmr::list<CachedScript>)==12);
#endif
std::pmr::list<CachedScript> cache;
std::unordered_map<std::uint8_t*,std::size_t> sizes;
char path_buffer[260]={}; //actual BSS5c4c20,4bd4a0 copies empty prefix then filename
template<class T>T read(std::span<std::uint8_t> bytes,std::size_t offset) {
    if(offset>bytes.size()||sizeof(T)>bytes.size()-offset)throw std::out_of_range("Truncated ECL field");
    T value;std::memcpy(&value,bytes.data()+offset,sizeof(value));return value;
}
const char* string(std::span<std::uint8_t> bytes,std::size_t offset) {
    if(offset>=bytes.size())throw std::out_of_range("Truncated ECL name");
    const auto* begin=reinterpret_cast<const char*>(bytes.data()+offset);
    if(!std::memchr(begin,0,bytes.size()-offset))throw std::out_of_range("Unterminated ECL name");return begin;
}
EnemyController& owner(ScriptLoader& loader) {
    if(!loader.context||!loader.context->objects_04[1])throw std::logic_error("ScriptLoader requires actual EnemyController context");
    return *static_cast<EnemyController*>(loader.context->objects_04[1]);
}
}
ScriptLoader::ScriptLoader(EnemyServices& host):file_count(0),subroutine_count(0),files{},fields_10c{},records{},string_21c{},player_index(0),context(nullptr),services(&host) {}
ScriptLoader::~ScriptLoader()=default; //data belongs to process cache, not this loader
void ScriptLoader::bind_player(std::int32_t index) {player_index=index;context=&game_session::context(index);}
void clear_script_cache() {
    for(auto& entry:cache)if(entry.data){runtime::release_bytes(entry.data);entry.data=nullptr;}
    cache.clear();sizes.clear();
}
std::size_t script_cache_size() noexcept {return cache.size();}
void ScriptLoader::load(const char* path) {
    auto& enemy=owner(*this);
    for(const auto& loaded:enemy.loaded_names)if(loaded==path) {
        runtime::log_printf(services->log(),"%s is skiped.\n",path);return;
    }
    enemy.loaded_names.emplace_back(path); //before cache lookup/read; also breaks include cycles
    std::uint8_t* data=nullptr;
    for(const auto& loaded:cache)if(loaded.name==path) {
        data=loaded.data;runtime::log_printf(services->log(),"%s was loaded in 1P.\n",path);break;
    }
    if(!data) {
        if(strcpy_s(path_buffer,path)!=0)throw std::length_error("ECL resource path exceeds original buffer");
        auto bytes=resources::read(path_buffer);
        if(!bytes)throw std::runtime_error(std::string("Missing ECL resource: ")+path);
        data=static_cast<std::uint8_t*>(runtime::allocate_bytes(bytes->size()));if(!data)throw std::bad_alloc();
        std::memcpy(data,bytes->data(),bytes->size());sizes.emplace(data,bytes->size());
        cache.push_back({data,std::pmr::string(path)});
    }
    append({data,sizes.at(data)});
}
void ScriptLoader::append(std::span<std::uint8_t> bytes) {
    if(file_count>=std::size(files))throw std::out_of_range("Original ECL file-slot limit");
    files[file_count]=bytes.data();
    if(read<std::uint32_t>(bytes,0)!=0x54504353u) {
        runtime::log_printf(services->log(),"error : Spt FileHeader ID error\n");files[file_count]=nullptr;return;
    }
    if(read<std::uint16_t>(bytes,4)!=1) {
        runtime::log_printf(services->log(),"error : Spt FileHeader Version error\n");files[file_count]=nullptr;return;
    }
    const auto include_size=read<std::uint16_t>(bytes,6),count=read<std::uint16_t>(bytes,0x10);
    const auto offsets=std::size_t{0x24}+include_size;
    std::size_t names=offsets+std::size_t(count)*4;
    subroutine_count+=count;
    for(std::size_t i=0;i<count;++i) {
        const auto* name=string(bytes,names);const auto code_offset=read<std::uint32_t>(bytes,offsets+i*4);
        if(code_offset>bytes.size()||16>bytes.size()-code_offset)throw std::out_of_range("Truncated ECL subroutine header");
        const auto position=std::upper_bound(records.begin(),records.end(),name,[](const char* key,const ScriptRecord& record){return std::strcmp(key,record.name)<0;});
        records.insert(position,{name,bytes.data()+code_offset});names+=std::strlen(name)+1;
    }
    ++file_count;loaded_spans.push_back(bytes);
    if(include_size)load_includes(bytes.data()+0x24); //original ignores include-handler status
}
int ScriptLoader::load_includes(std::uint8_t* block) {
    std::span<std::uint8_t> bytes;
    const auto address=reinterpret_cast<std::uintptr_t>(block);
    for(auto candidate:loaded_spans) {
        const auto first=reinterpret_cast<std::uintptr_t>(candidate.data());
        if(address>=first&&address-first<candidate.size()){bytes=candidate.subspan(address-first);break;}
    }
    if(bytes.empty())throw std::out_of_range("Unowned ECL include block");
    if(read<std::uint32_t>(bytes,0)!=0x4d494e41u)return 0;
    const auto animations=read<std::uint32_t>(bytes,4);std::size_t cursor=8;
    for(std::uint32_t i=0;i<animations;++i) {
        auto& enemy=owner(*this);const auto* path=string(bytes,cursor);const auto slot=i+2;
        if(slot>=8)throw std::out_of_range("Original animation reference limit");
        if(!services->existing_animation(enemy,slot)) {
            sprite::AnimationFile* found=nullptr;
            for(auto* file:services->sprites().files)if(file&&file->filename==path){found=file;break;}
            if(found) {
                enemy.animation_files[slot]=found;runtime::log_printf(services->log(),"%s is skiped\n",path);
            } else {
                found=sprite::load_animation_file(services->sprites(),static_cast<int>(i*2)+player_index+0x19,path,services->log(),services->graphics_flags());
                enemy.animation_files[slot]=found;
                if(!found) {
                    // Original CP932 text at56f610, append-only (not log_error).
                    runtime::log_printf(services->log(),"\x83\x66\x81\x5b\x83\x5e\x82\xaa\x89\xf3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb7\r\n");return -1;
                }
            }
        }
        cursor+=std::strlen(path)+1;
    }
    cursor=(cursor+3)&~std::size_t{3};
    if(read<std::uint32_t>(bytes,cursor)==0x494c4345u) {
        const auto includes=read<std::uint32_t>(bytes,cursor+4);cursor+=8;
        for(std::uint32_t i=0;i<includes;++i) {
            const auto* path=string(bytes,cursor);load(path);cursor+=std::strlen(path)+1;
        }
    }
    return 0;
}
int ScriptLoader::find(const char* name) const {
    if(subroutine_count>records.size())throw std::out_of_range("Inconsistent ECL record count");
    int low=0,high=static_cast<int>(subroutine_count)-1;
    while(low<=high) {const int middle=low+(high-low)/2,order=std::strcmp(name,records[middle].name);
        if(order==0)return middle;if(order<0)high=middle-1;else low=middle+1;}
    return -1;
}
std::uint8_t* ScriptLoader::instruction(std::int32_t sub,std::int32_t byte_offset) const {
    const auto& record=records.at(static_cast<std::size_t>(sub));
    // The original treats this as 32-bit address arithmetic; the caller owns
    // instruction-bound validation, as it does when jumping inside an ECLH.
    const auto address=reinterpret_cast<std::uintptr_t>(record.header)+16u+static_cast<std::uint32_t>(byte_offset);
    return reinterpret_cast<std::uint8_t*>(address);
}
}
