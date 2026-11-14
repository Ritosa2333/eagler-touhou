#pragma once
#include "Types.hpp"
#include <string>
#include <vector>
namespace th11 {
struct StagePrimitive {i16 type=0,script=0;u32 animation=0;Vec3 position{};Vec2 size{};};
struct StageObject {i16 id=0;u8 layer=0,flags=0;Vec3 position{},size{};std::vector<StagePrimitive> primitives;};
struct StageInstance {i16 object=0,flags=0;Vec3 position{};};
struct StageInstruction {
    i32 time=0;i16 opcode=0,length=0;u32 offset=0;std::vector<u32> arguments;
    template<class T>T argument(u32 index)const noexcept{T out{};if(index<arguments.size())std::memcpy(&out,&arguments[index],4);return out;}
};
class StageResource {
public:
    std::vector<StageObject> objects;
    std::vector<StageInstance> instances;
    std::vector<StageInstruction> instructions;
    std::string animation_name,error;
    u32 animation_count=0;
    bool open(const u8*,u32);
    const StageInstruction* instruction(u32 offset)const noexcept;
};
}
