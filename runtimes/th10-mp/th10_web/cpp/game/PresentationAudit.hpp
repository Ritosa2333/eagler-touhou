#pragma once
#include "AnmVm.hpp"
#include "AnmFile.hpp"
namespace th10 { struct AnmModelVertex; }
namespace th10::presentation_audit {
#ifdef TH_PRESENTATION_AUDIT
constexpr u32 MaxVertices=16;
constexpr u32 MaxRecords=8192;
struct Record {
    u32 owner_tag,object_id,generation,part,draw_id,script_index,sprite_index,flags;
    u32 vertex_count,continuous,file_index,reserved;
    float position[3],script_position[3],child_position[3],rotation[3],scale[2],uv_offset[2];
    u32 color,secondary_color;
    float bounds[4];
    float vertices[MaxVertices][5];
};
static_assert(sizeof(Record)==456);
void enable(bool);
u32 current_session_epoch() noexcept;
u32 current_tick() noexcept;
u32 current_draw_serial() noexcept;
void simulation_tick() noexcept;
void begin_reference() noexcept;
void begin_sample(float alpha) noexcept;
void end_frame() noexcept;
void capture(const AnmVm&,const AnmVertex*,u32 count,u32 part=0) noexcept;
void capture_model(const AnmVm&,const AnmModelVertex* vertices,u32 count,u32 part) noexcept;
#else
inline void enable(bool){}
inline u32 current_tick() noexcept{return 0;}
inline u32 current_draw_serial() noexcept{return 0;}
inline void simulation_tick() noexcept{}
inline void begin_reference() noexcept{}
inline void begin_sample(float) noexcept{}
inline void end_frame() noexcept{}
inline void capture(const AnmVm&,const AnmVertex*,u32,u32=0) noexcept{}
inline void capture_model(const AnmVm&,const AnmModelVertex*,u32,u32) noexcept{}
#endif
}
