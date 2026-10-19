#pragma once
#include "Binary.hpp"
#include "AnmLayout.hpp"
#include <vector>
namespace th09 {
struct StageInstruction {i32 time=0;i16 opcode=0;u16 size=0;u32 arguments[3]{};i32 integer(u32 n)const{return signed_bits(arguments[n]);}float real(u32 n)const{float f;std::memcpy(&f,arguments+n,4);return f;}Vec3 vector()const{return {real(0),real(1),real(2)};}};
struct StagePrimitive {i16 type=0,script=0;u32 animation=0;Vec3 position,end;Vec2 size;float width=0;};
struct StageObject {i16 id=0;u8 layer=0,flags=0;Vec3 position,size;std::vector<StagePrimitive> primitives;};
struct StageInstance {i16 object=0;u16 flags=0;Vec3 position;};
class StageResource {
public:
    u32 animation_count=0;std::vector<StageObject> objects;std::vector<StageInstance> instances;std::vector<StageInstruction> instructions;
    bool load(const u8*,u32);
};
}
