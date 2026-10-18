#pragma once
#include "enemy.hpp"
#include <span>
namespace th20::source::ecl {struct Program;}
namespace th20::source::gameplay {
struct ScriptRecord {const char* name;std::uint8_t* header;};
static_assert(sizeof(ScriptRecord)==8);
#pragma pack(push,4)
class ScriptLoader {
public:
    // Original vtable order is load, include handler, destructor.
    virtual void load(const char*);                           //4a74d0
    virtual int load_includes(std::uint8_t*);                 //4a5be0
    virtual ~ScriptLoader();                                 //4a3900->4a3d40
    std::uint32_t file_count,subroutine_count;                //+4/+8
    std::uint8_t* files[64];                                  //+c
    std::uint32_t fields_10c[64];                             //+10c, zero by4a3650
    std::pmr::vector<ScriptRecord> records;                   //+20c
    std::string string_21c;                                   //+21c,24 bytes (derived index starts234)
    std::int32_t player_index;                               //+234
    game_session::Context* context;                          //+238
    EnemyServices* services;                                 //source-only+23c
    std::vector<std::span<std::uint8_t>> loaded_spans;         //source-only bounds metadata, buffers remain shared
    explicit ScriptLoader(EnemyServices&);                    //4a2870->4a2e40->4a3650
    void bind_player(std::int32_t);                          //4ab8b0
    void append(std::span<std::uint8_t>);                     //53fe60
    int find(const char*) const;                             //540340, midpoint match including duplicates
    std::uint8_t* instruction(std::int32_t sub,std::int32_t byte_offset) const; //53e8e0, ECLH+16+offset
    void bind_program(ecl::Program&) const;                  //non-owning source VM adapter, call after includes load
};
#pragma pack(pop)
// std::string/pmr::vector sizes here are MSVC x86 STL layout evidence;
// binding on the Windows build only (TH_SDL3 recompiles with libc++ layouts).
#ifndef TH_SDL3
static_assert(offsetof(ScriptLoader,records)==0x20c && offsetof(ScriptLoader,string_21c)==0x21c);
static_assert(offsetof(ScriptLoader,services)==0x23c);
#endif
void clear_script_cache();                                  //4a3920 process-global cache sweep
std::size_t script_cache_size() noexcept;
}
