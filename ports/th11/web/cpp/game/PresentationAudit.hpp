#pragma once
#include "AnmVm.hpp"
#include "AnmGeometry.hpp"
namespace th11 { struct GameSession; }
namespace th11::presentation_audit {
constexpr u32 StateGroupCount=9;
#ifdef TH_PRESENTATION_AUDIT
constexpr u32 MaxVertices=16;
constexpr u32 MaxRecords=8192;
// 456-byte normalized record shared with the common workbench decoder.
struct Record {
    u32 owner_tag,object_id,generation,part,draw_id,script_index,sprite_index,flags;
    u32 vertex_count,continuous,file_index,layer;
    float position[3],script_position[3],child_position[3],rotation[3],scale[2],uv_offset[2];
    u32 color,secondary_color;
    float bounds[4];
    float vertices[MaxVertices][5];
};
static_assert(sizeof(Record)==456);
void enable(bool);
bool enabled() noexcept;
u32 session_epoch() noexcept;
u32 current_tick() noexcept;
u32 current_draw_serial() noexcept;
void simulation_tick() noexcept;
void begin_reference() noexcept;
void begin_sample(float alpha) noexcept;
void end_frame() noexcept;
void scope(u32 owner) noexcept;
u32 scope() noexcept;
void fault(bool) noexcept;
void capture(const AnmVm&,const AnmVertex* vertices,u32 count,u32 part=0) noexcept;
void sample_state(GameSession&) noexcept;
const u32* state() noexcept;
#else
inline void enable(bool){} inline bool enabled() noexcept{return false;}
inline u32 session_epoch() noexcept{return 0;} inline u32 current_tick() noexcept{return 0;}
inline u32 current_draw_serial() noexcept{return 0;} inline void simulation_tick() noexcept{}
inline void begin_reference() noexcept{} inline void begin_sample(float) noexcept{}
inline void end_frame() noexcept{} inline void scope(u32) noexcept{} inline u32 scope() noexcept{return 0;}
inline void fault(bool) noexcept{} inline void capture(const AnmVm&,const AnmVertex*,u32,u32=0) noexcept{}
inline void sample_state(GameSession&) noexcept{} inline const u32* state() noexcept{static const u32 z[StateGroupCount]{};return z;}
#endif
}
