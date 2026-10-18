#pragma once
#include "Types.hpp"
#include <vector>
namespace th08 {
struct StageHeader {
    i16 object_count,quad_count;u32 instances_offset,script_offset;i32 reserved;
    char name[128],song_names[4][128],song_paths[4][128];
};
struct StageObject {i16 id;u8 layer,flags;Vec3 position,dimensions;};
struct StageQuad {i16 type,size,script,vm;};
struct StageSpriteQuad:StageQuad {Vec3 position;float width,height;};
struct StageBeamQuad:StageQuad {Vec3 start,end;float width;};
struct StageInstance {i16 object;u16 reserved;Vec3 position;};
struct StageInstruction {
    i32 frame;i16 opcode,size;u32 args[3];
    i32 integer(u32 i)const{return signed_bits(args[i]);}
    float real(u32 i)const{float value;std::memcpy(&value,args+i,4);return value;}
    Vec3 vector()const{return {real(0),real(1),real(2)};}
};
static_assert(sizeof(StageHeader)==0x490&&sizeof(StageObject)==28);
static_assert(sizeof(StageSpriteQuad)==28&&sizeof(StageBeamQuad)==36);
static_assert(sizeof(StageInstance)==16&&sizeof(StageInstruction)==20);
class StageProgram {
public:
    bool load(const u8* bytes,u32 size);
    void clear(){data.clear();object_list.clear();instruction_count=instance_count=0;}
    StageHeader* header(){return data.empty()?nullptr:reinterpret_cast<StageHeader*>(data.data());}
    StageObject** objects(){return object_list.data();}
    StageInstruction* instructions(){return header()?reinterpret_cast<StageInstruction*>(data.data()+header()->script_offset):nullptr;}
    StageInstance* instances(){return header()?reinterpret_cast<StageInstance*>(data.data()+header()->instances_offset):nullptr;}
    u32 instructions_size()const{return instruction_count;}
    u32 instances_size()const{return instance_count;}
    u32 bytes_size()const{return data.size();}
    static StageQuad* first(StageObject& object){return reinterpret_cast<StageQuad*>(&object+1);}
    static StageQuad* next(StageQuad& quad){return reinterpret_cast<StageQuad*>(reinterpret_cast<u8*>(&quad)+quad.size);}
private:
    std::vector<u8> data;std::vector<StageObject*> object_list;
    u32 instruction_count=0,instance_count=0;
};
}
