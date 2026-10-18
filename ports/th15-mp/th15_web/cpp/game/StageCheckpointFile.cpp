#include "StageCheckpoint.hpp"
#include "AnmFile.hpp"
namespace th15 {
namespace {
constexpr u32 prefix=0x3394;
template<class T>void put(u8* p,u32 at,const T& value){std::memcpy(p+at,&value,sizeof value);}
template<class T>void get(const u8* p,u32 at,T& value){std::memcpy(&value,p+at,sizeof value);}
u32 word(const u8* p,u32 at){u32 value;get(p,at,value);return value;}
template<class S,class Copy>void script_fields(S& s,Copy copy){
    copy(0,s.timer);copy(0x14,s.instruction_offset);copy(0x18,s.camera_effect);copy(0x1c,s.effect_timer);
    copy(0x30,s.direction);copy(0x88,s.position);copy(0xe0,s.up);copy(0x138,s.fog);
    copy(0x1e0,s.camera.position);copy(0x1ec,s.camera.direction);copy(0x1f8,s.camera.up);copy(0x21c,s.camera.eye_offset);copy(0x228,s.camera.target_offset);copy(0x234,s.camera.fov);copy(0x2e4,s.camera.animation_delta);copy(0x2f0,s.camera.fog);
    copy(0x3350,s.animation_layers);copy(0x3370,s.culling_distance_squared);copy(0x3378,s.deformation_target);copy(0x337c,s.deformation_radius);copy(0x3380,s.deformation_color);copy(0x3384,s.phase_x);copy(0x3388,s.phase_y);copy(0x338c,s.deformation_mode);
}
}
bool StageCheckpoint::write_file(std::vector<u8>& output){
    error.clear();output.clear();if(!available){error="No background checkpoint to serialize";return false;}output.resize(prefix,0);
    script_fields(state,[&](u32 at,const auto& v){put(output.data(),at,v);});put(output.data(),0x204,direction);
    AnmFile codec;std::array<AnmFile::Block,8> records;
    for(u32 i=0;i<8;i++){if(!codec.write(embedded[i],scene.animations,records[i],true)){error=codec.error;output.clear();return false;}std::memcpy(output.data()+0x310+i*0x608,records[i].data(),0x608);}
    auto append=[&](const AnmVm& vm)->bool{AnmFile::Block record;if(!codec.write(vm,scene.animations,record,true)){error=codec.error;return false;}output.insert(output.end(),record.begin(),record.end());if(!codec.write_geometry(vm,scene.animations,output,{})){error=codec.error;return false;}return true;};
    for(const auto& vm:primitives)if(!append(vm)){output.clear();return false;}
    const u32 at=output.size();output.resize(at+4);u32 count=0;
    for(const auto& vm:embedded)if(vm.geometry.allocation_bytes){if(!append(vm)){output.clear();return false;}count++;}
    put(output.data(),at,count);return true;
}
bool StageCheckpoint::read_file(const u8* data,u32 size,u32& consumed){
    error.clear();consumed=0;if(!data||size<prefix){error="Truncated background checkpoint prefix";return false;}
    StageScriptState next{};Vec3 next_direction;script_fields(next,[&](u32 at,auto& v){get(data,at,v);});get(data,0x204,next_direction);
    if(next.instruction_offset!=0xffffffffu&&!scene.file.instruction(next.instruction_offset)){error="Background checkpoint instruction is unavailable";return false;}
    AnmFile codec;std::vector<AnmVm> next_primitives(scene.primitives.size());std::array<AnmVm,8> next_embedded;u32 offset=prefix;
    auto fixed=[&](const u8* record,u32 bytes,AnmVm& vm)->bool{if(!codec.read(record,bytes,vm,scene.animations,true)){error=codec.error;return false;}return true;};
    auto appended=[&](AnmVm& vm)->bool{if(size-offset<0x608){error="Truncated background checkpoint animation";return false;}const auto* record=data+offset;if(!fixed(record,size-offset,vm))return false;u32 geometry=0;if(!codec.read_geometry(record,record+0x608,size-offset-0x608,vm,scene.animations,geometry,{})){error=codec.error;return false;}offset+=0x608+geometry;return true;};
    for(auto& vm:next_primitives)if(!appended(vm))return false;
    if(size-offset<4){error="Missing background checkpoint geometry count";return false;}const u32 count=word(data,offset);offset+=4;if(count>8){error="Background checkpoint geometry count exceeds slots";return false;}u32 actual=0;
    for(u32 i=0;i<8;i++){const auto* record=data+0x310+i*0x608;if(word(record,0x5c8)){if(!appended(next_embedded[i]))return false;actual++;}else if(!fixed(record,0x608,next_embedded[i]))return false;}
    if(actual!=count){error="Background checkpoint geometry count differs from slot records";return false;}
    state=next;direction=next_direction;primitives=std::move(next_primitives);embedded=std::move(next_embedded);available=true;consumed=offset;return true;
}
}
