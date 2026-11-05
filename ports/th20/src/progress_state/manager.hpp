#pragma once
#include "records.hpp"
#include "../runtime_core/worker.hpp"
namespace th20::source::progress {
class alignas(8) SaveManager {
public:
    Snapshot current,backup;
    std::uint32_t field_124280=0;
    std::uint32_t fields_124284[16]{};
    runtime::Worker worker;
    SaveManager(); //50e5d0
    ~SaveManager(); //50e990
    int commit(); //50f660, joins previous operation then starts real save thread
    void load(); //50f3b0
    void save(); //50fb60
    void verify_metadata(); //463fb0 ->463fe0
    std::int32_t selected_profile(int slot,int character); //464100
    void select_profile(int slot,int character,int index); //51c920
    bool extra_unlocked(int character,int index); //51bc10
    std::uint32_t stone_count(unsigned index); //4bd610
    std::uint32_t used_stone_count(unsigned index); //51b970
    void set_used_stone_count(unsigned index,std::uint32_t); //51c7e0
    void consume_stone(unsigned index); //51b0c0
private:
    void launch(void(SaveManager::*)()); //50ab10 + standard jthread wrappers
};
static_assert(offsetof(SaveManager,backup)==0x92140 && offsetof(SaveManager,worker)==0x1242c4);
// Total size depends on the STL layout (worker holds a jthread); binding on
// the MSVC x86 build only.
#ifndef TH_SDL3
static_assert(sizeof(SaveManager)==0x1242d8);
#endif
extern SaveManager* manager; //5c6108, unique actual source storage
Profile* current_profile(SaveManager&) noexcept; //50fc50, preserves4bd460 null for invalid selectors
Profile& fallback_profile(SaveManager&) noexcept; //488700: current.profiles[18], not backup Snapshot
inline Profile* current_profile() noexcept{return current_profile(*manager);}
inline Profile& fallback_profile() noexcept{return fallback_profile(*manager);}
SaveManager* create_manager(); //50adc0
void initialize(); //50fce0
void release(); //50fc10
}
