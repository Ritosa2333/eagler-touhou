#pragma once
#include "stones.hpp"
#include "../trophy_system/trophy.hpp"
namespace stones_fixture {
namespace ti=th20::source::title;namespace tr=th20::source::trophy;
struct Environment final:ti::StonesEnvironment {
    int last{};ti::StonesUnlockState state{};
    Environment(ti::MainEnvironment& main):StonesEnvironment(main,last,state){}
    const char* title(int id)override{auto& record=tr::messages[id];return record.id==id?tr::decode_string(record.title):nullptr;}
    const char* description(int id,bool achieved,int line)override{auto& record=tr::messages[id];return record.id==id?tr::decode_string(record.description[achieved][line]):nullptr;}
    bool achieved(unsigned id)override{return tr::achieved(id);}
    [[noreturn]]static void excluded(){throw std::runtime_error("Stones drawing fixture reached menu-update dependency");}
    void background(ti::TitleInf&)override{excluded();}void heading(ti::TitleInf&,bool)override{excluded();}void fade(float)override{excluded();}void ending(int)override{excluded();}bool effects_ready()override{excluded();}void loading()override{excluded();}void prepare_replay(ti::TitleInf&)override{excluded();}int keyboard(std::span<std::uint8_t,256>)override{excluded();}void unlock_progress()override{excluded();}
};
}
