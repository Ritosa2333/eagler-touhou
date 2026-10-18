#include "PresentationAudit.hpp"
#ifdef TH_PRESENTATION_AUDIT
#include "AnmManager.hpp"
#include <algorithm>
#include <cstdint>
#include <utility>
#include <vector>
namespace th10::presentation_audit {
namespace {
struct Frame {std::vector<Record> records;u32 tick=0,draw_serial=0,dropped=0;float alpha=1;};
bool enabled=false,reference=false;u32 session_epoch=0,tick=0,draw_serial=0,draw_id=0;Frame current,references[2];
u16 continuous_fields(const AnmVm& vm){
    u16 fields=0;if(vm.rotation_interpolation.duration>0||vm.angular_velocity.x||vm.angular_velocity.y||vm.angular_velocity.z)fields|=1;
    if(vm.scale_interpolation.duration>0||vm.scale_velocity.x||vm.scale_velocity.y)fields|=2;
    if(vm.color_interpolation.duration>0)fields|=4;if(vm.alpha_interpolation.duration>0)fields|=8;
    if(vm.color2_interpolation.duration>0)fields|=16;if(vm.alpha2_interpolation.duration>0)fields|=32;
    if(vm.uv_velocity.x)fields|=64;if(vm.uv_velocity.y)fields|=128;return fields;
}
void vector3(float* output,const Vec3& value){output[0]=value.x;output[1]=value.y;output[2]=value.z;}
}
void enable(bool value){
    enabled=value;current.records.clear();references[0].records.clear();references[1].records.clear();tick=draw_serial=draw_id=0;reference=false;
    if(value){++session_epoch;current.records.reserve(MaxRecords);references[0].records.reserve(MaxRecords);references[1].records.reserve(MaxRecords);}
}
u32 current_session_epoch() noexcept{return session_epoch;}
u32 current_tick() noexcept{return tick;}
u32 current_draw_serial() noexcept{return draw_serial;}
void simulation_tick() noexcept{if(enabled)++tick;}
void begin_reference() noexcept{if(!enabled)return;reference=true;current.records.clear();current.tick=tick;current.draw_serial=++draw_serial;current.alpha=1;current.dropped=0;draw_id=0;}
void begin_sample(float alpha) noexcept{if(!enabled)return;reference=false;current.records.clear();current.tick=tick;current.draw_serial=draw_serial;current.alpha=alpha;current.dropped=0;draw_id=0;}
void end_frame() noexcept{if(!enabled)return;if(reference){std::swap(references[0],references[1]);std::swap(references[1],current);current.records.clear();current.tick=current.draw_serial=current.dropped=0;current.alpha=1;}reference=false;}
void capture(const AnmVm& vm,const AnmVertex* vertices,u32 count,u32 part) noexcept{
    if(!enabled||!vertices||!count)return;if(current.records.size()>=MaxRecords){++current.dropped;return;}
    // Draw-only interpolation commonly renders a stack copy of the VM.  The
    // registry id survives that copy; the VM address does not.
    Record record{};record.owner_tag=vm.owner_tag;record.object_id=vm.id;record.generation=vm.id;record.part=part;record.draw_id=draw_id++;
    record.script_index=static_cast<u32>(vm.script_index);record.sprite_index=static_cast<u32>(vm.sprite_index);record.flags=vm.flags;record.vertex_count=std::min(count,MaxVertices);
    record.continuous=continuous_fields(vm);record.file_index=vm.file_index;vector3(record.position,vm.position);vector3(record.script_position,vm.script_position);vector3(record.child_position,vm.child_position);vector3(record.rotation,vm.rotation);
    record.scale[0]=vm.scale.x;record.scale[1]=vm.scale.y;record.uv_offset[0]=vm.uv_offset.x;record.uv_offset[1]=vm.uv_offset.y;record.color=vm.color;record.secondary_color=vm.secondary_color;
    float left=vertices[0].position.x,top=vertices[0].position.y,right=left,bottom=top;
    for(u32 i=0;i<record.vertex_count;++i){const auto& vertex=vertices[i];left=std::min(left,vertex.position.x);top=std::min(top,vertex.position.y);right=std::max(right,vertex.position.x);bottom=std::max(bottom,vertex.position.y);record.vertices[i][0]=vertex.position.x;record.vertices[i][1]=vertex.position.y;record.vertices[i][2]=vertex.position.z;record.vertices[i][3]=vertex.uv.x;record.vertices[i][4]=vertex.uv.y;}
    record.bounds[0]=left;record.bounds[1]=top;record.bounds[2]=right;record.bounds[3]=bottom;current.records.push_back(record);
}
void capture_model(const AnmVm& vm,const AnmModelVertex* vertices,u32 count,u32 part) noexcept{
    if(!vertices||!count)return;AnmVertex converted[MaxVertices]{};const u32 copied=std::min(count,MaxVertices);
    for(u32 i=0;i<copied;++i){converted[i].position=vertices[i].position;converted[i].uv=vertices[i].uv;}
    capture(vm,converted,count,part);
}
}
extern "C" {
using namespace th10::presentation_audit;
__attribute__((export_name("audit_enable"))) void audit_enable(th10::u32 value){enable(value!=0);}
__attribute__((export_name("audit_session_epoch"))) th10::u32 audit_session_epoch(){return current_session_epoch();}
__attribute__((export_name("audit_records"))) const Record* audit_records(){return current.records.data();}
__attribute__((export_name("audit_count"))) th10::u32 audit_count(){return static_cast<th10::u32>(current.records.size());}
__attribute__((export_name("audit_tick"))) th10::u32 audit_tick(){return current.tick;}
__attribute__((export_name("audit_draw_serial"))) th10::u32 audit_draw_serial(){return current.draw_serial;}
__attribute__((export_name("audit_dropped"))) th10::u32 audit_dropped(){return current.dropped;}
__attribute__((export_name("audit_reference"))) const Record* audit_reference(th10::u32 index){return references[index&1].records.data();}
__attribute__((export_name("audit_reference_count"))) th10::u32 audit_reference_count(th10::u32 index){return static_cast<th10::u32>(references[index&1].records.size());}
__attribute__((export_name("audit_reference_tick"))) th10::u32 audit_reference_tick(th10::u32 index){return references[index&1].tick;}
__attribute__((export_name("audit_reference_draw_serial"))) th10::u32 audit_reference_draw_serial(th10::u32 index){return references[index&1].draw_serial;}
__attribute__((export_name("audit_reference_dropped"))) th10::u32 audit_reference_dropped(th10::u32 index){return references[index&1].dropped;}
__attribute__((export_name("audit_stride"))) th10::u32 audit_stride(){return sizeof(Record);}
}
#endif
