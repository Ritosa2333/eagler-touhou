#pragma once
#include "title.hpp"
#include <array>
#include <span>
namespace th20::source::title {
extern int last_stone_record; //BSS5c6130, gallery cursor memory
struct StonesUnlockState {std::array<std::uint8_t,256> pressed{},previous{},current{};std::uint32_t matched{},idle{};};
static_assert(sizeof(StonesUnlockState)==0x308);
extern StonesUnlockState stones_unlock_state; //BSS5c6550..5c6854; distinct from PlayerData's sequence
class StonesEnvironment {
public:
    MainEnvironment& main;int& last_record;StonesUnlockState& unlock;
    StonesEnvironment(MainEnvironment& m,int& last,StonesUnlockState& u):main(m),last_record(last),unlock(u){}
    virtual ~StonesEnvironment()=default;
    virtual const char* title(int)=0; //52f540, actual trophy message decoder
    virtual const char* description(int,bool,int)=0; //52f4e0
    virtual bool achieved(unsigned)=0;
    virtual void background(TitleInf&)=0;
    virtual void heading(TitleInf&,bool)=0;
    virtual void fade(float)=0;
    virtual void ending(int)=0;
    virtual bool effects_ready()=0;
    virtual void loading()=0;
    virtual void prepare_replay(TitleInf&)=0;
    virtual int keyboard(std::span<std::uint8_t,256>)=0;
    virtual void unlock_progress()=0;
};
void advance_stones_unlock(StonesUnlockState&,StonesEnvironment&);
int update_stones(TitleInf&,StonesEnvironment&); //52a8a0
int draw_stones(TitleInf&,text::Renderer&,StonesEnvironment&); //52b480
StonesEnvironment& stones_environment();
}
