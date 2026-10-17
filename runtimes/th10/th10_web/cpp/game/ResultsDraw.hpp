#pragma once
#include "Results.hpp"
#include "ReplayFile.hpp"
#include <initializer_list>
namespace th10 {
struct ResultsDrawEnvironment {
    GameEconomy* game;
    ScoreData** scores;
    Replay** replay;
    const char* alphabet;
    const char* const* characters;
    const char* const* difficulties;
    const char* const* stage_names;
    const char* const* replay_stage_names;
    u32* color;
    u32* text_mode;
    virtual ReplayDate local_date(i32 timestamp)=0;
    virtual void text(const Vec3& position,const char* format,const u32* arguments,u32 count)=0;
    void print(const Vec3& position,const char* format,std::initializer_list<u32> args={}){text(position,format,args.begin(),args.size());}
};
}
