#pragma once
#include "world-fixture.hpp"
#include "../../cpp/game/GameSession.hpp"
namespace session_test {
using namespace th09;
struct Fixture:world_test::Fixture,EndingServices {
    PlayerRecords records;GameSession session{world,resources,animations,*this,*this,records};
    bool ending_picture(const char* n)override{std::vector<u8> bytes;return read(n,bytes);}
    void ending_music(i32 n)override{music(n);}void ending_music_fade(i32)override{fade_music();}
    void ending_text(AnmVm& vm,const char* s,u32 color)override{text(vm,s,color,0);}
    void ending_background(i32,i32)override{}void ending_sprite(AnmVm&)override{}void ending_cover(u32)override{}
    void encountered(i32 c)override{records.count_encounter(c);world_test::Fixture::encountered(c);}
    void defeated(i32 c)override{if(c>=0&&c<16)records.versus_unlocked[c]=1;world_test::Fixture::defeated(c);}
};
}
