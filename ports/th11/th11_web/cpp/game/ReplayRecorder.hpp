#pragma once
#include "Replay.hpp"
#include <array>
namespace th11 {
class ReplayRecorder {
public:
    struct Stage {std::array<u8,0x90> header{};std::vector<u8> inputs,fps;std::vector<ReplayTouch> touches;u32 frames=0;bool present=false,terminal=false;};
    std::array<u8,0x70> header{};
    std::array<Stage,8> stages{};
    bool begin(i32 character,i32 subtype,i32 difficulty,bool practice,u64 timestamp);
    bool start_stage(u32 number,const ReplayStageState&,bool initial);
    bool tick(u32 held,u32 pressed,u32 released,float fps=60,i32 touch_mode=0,float x=0,float y=0);
    void spell_times(const std::array<u32,20>&);
    bool finish_stage(bool terminal=false);
    bool save(const char* name,i32 score,i32 reached,i32 continues,float slowdown,std::vector<u8>& output)const;
    u32 selected=0;
    bool uses_touch()const;
private:
    bool valid=false;
};
}
