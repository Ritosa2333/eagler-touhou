#pragma once
#include "AnmResource.hpp"
#include "Interpolation.hpp"
#include "Rng.hpp"
namespace th11 {
struct AnmEnvironment;
struct Matrix4 {float m[16];void identity() noexcept {std::memset(m,0,sizeof m);m[0]=m[5]=m[10]=m[15]=1;}};
struct AnmVm {
    u32 id;
    struct Node {AnmVm* value;Node* next;Node* previous;} registry,child;
    AnmVm* draw_next;
    u32 layer;
    Vec3 rotation,angular_velocity;
    Vec2 scale,scale_velocity,sprite_size,uv_offset;
    Timer timer;
    Vec2 uv[4];
    Vec3Interpolator position_interpolation;
    RgbInterpolator color_interpolation;
    AlphaInterpolator alpha_interpolation;
    Vec3Interpolator rotation_interpolation;
    Vec2Interpolator scale_interpolation;
    RgbInterpolator color2_interpolation;
    AlphaInterpolator alpha2_interpolation;
    FloatInterpolator scroll_x_interpolation,scroll_y_interpolation;
    Vec2 scroll_velocity;
    Matrix4 sprite_matrix,transform_matrix,uv_matrix;
    u32 color,secondary_color;
    i16 pending_interrupt;u16 reserved_37e;
    Timer saved_timer;
    AnmInstruction* saved_instruction;
    i32 sprite_frame;
    i16 sprite_index;u16 file_index,reserved_3a0; i16 script_index;
    AnmInstruction* script_begin;AnmInstruction* instruction;
    const AnmSprite* sprite;AnmResource* resource;
    i32 integers[4];float floats[4];i32 extra_integers[2];
    Vec3 script_position,position,child_position;
    void* geometry;
    u32 flags,flags2,reserved_40c;
    void (*before_update)(AnmVm&);
    void (*draw_callback)(AnmVm&);
    u32 reserved_418;
    u8 rectangle_columns,rectangle_rows;u16 reserved_41e;
    u32 reserved_420[2];
    int (*after_update)(AnmVm&);
    i32 (*sprite_callback)(AnmVm&,i32);
    u32 reserved_430;
    void initialize() noexcept;
    bool bind_sprite(i32 index) noexcept;
    bool bind_script(AnmResource& file,i32 script,u16 file_id,const float* rate) noexcept;
    int update(AnmEnvironment& env);
    i32 integer_value(i32 value,AnmEnvironment& env);
    double float_value(float value,AnmEnvironment& env);
    i32* integer_destination(i32* argument) noexcept;
    float* float_destination(float* argument) noexcept;
};
// Layout assertions apply to the 32-bit comparison/WebAssembly target only.
#if UINTPTR_MAX == UINT32_MAX
static_assert(sizeof(AnmVm)==0x434);
static_assert(offsetof(AnmVm,uv)==0x70);
static_assert(offsetof(AnmVm,position_interpolation)==0x90);
static_assert(offsetof(AnmVm,color)==0x374);
static_assert(offsetof(AnmVm,instruction)==0x3a8);
static_assert(offsetof(AnmVm,flags)==0x404);
#endif
struct AnmEnvironment {
    float rate=1;
    Rng script_rng,visual_rng;
    Vec3 reference_positions[2]{},camera_delta{},default_tangent{};
    virtual ~AnmEnvironment()=default;
    Rng& random(const AnmVm& vm) noexcept {return vm.flags&0x40000000?visual_rng:script_rng;}
    // Effects requiring a manager or renderer are explicit integration points.
    // Returning false aborts the update rather than silently dropping an effect.
    virtual AnmVm* spawn(AnmVm&,i32,u32){return nullptr;}
    virtual bool change_draw_mode(AnmVm&){return false;}
    virtual bool allocate_geometry(AnmVm&,u32){return false;}
    virtual bool update_geometry(AnmVm&){return false;}
    virtual bool screen_uv(AnmVm&);
};
}
