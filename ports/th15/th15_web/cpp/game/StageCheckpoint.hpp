#pragma once
#include "StageScene.hpp"
namespace th15 {
// Background animations are embedded copies, independent of the pooled
// checkpoint handles used by players, enemies and effects.
class StageCheckpoint {
    StageScene& scene;StageScriptState state;Vec3 direction;
    std::array<AnmVm,8> embedded;std::vector<AnmVm> primitives;bool available=false;
public:
    std::string error;explicit StageCheckpoint(StageScene& s):scene(s){}
    bool capture();bool restore();bool ready()const noexcept{return available;}
    bool write_file(std::vector<u8>&);
    bool read_file(const u8*,u32,u32& consumed);
    const AnmVm* primitive(u32 i)const noexcept{return i<primitives.size()?&primitives[i]:nullptr;}
    const AnmVm* slot(u32 i)const noexcept{return i<8?&embedded[i]:nullptr;}
    const StageScriptState& script_state()const noexcept{return state;}
};
}
