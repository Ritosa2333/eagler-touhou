#pragma once
#include "Interpolation.hpp"
namespace th10 {
struct AnmEnvironment;
struct AnmFile;
struct AnmSprite;
struct AnmInstruction {
    std::int16_t opcode;
    u16 length;
    std::int16_t time;
    u16 references;
    template<class T> T argument(u32 index) const noexcept {T value;std::memcpy(&value,reinterpret_cast<const u8*>(this)+8+4*index,sizeof(T));return value;}
    bool is_reference(u32 index) const noexcept {return references&(1u<<(index&31));}
    AnmInstruction* next() noexcept {return reinterpret_cast<AnmInstruction*>(reinterpret_cast<u8*>(this)+length);}
};
static_assert(sizeof(AnmInstruction)==8);
struct AnmVm {
    u32 id;                                // 000
    ListNode<AnmVm> registry_node;          // 004
    ListNode<AnmVm> child_node;             // 010
    AnmVm* draw_next;
    u32 owner_tag;                          // 020, retained by initialize
    Vec3 rotation;                         // 024
    Vec3 angular_velocity;                 // 030
    Vec2 scale;                            // 03c
    Vec2 scale_velocity;                   // 044
    Vec2 sprite_size;                      // 04c
    Vec2 uv_offset;                        // 054
    Timer script_timer;                    // 05c
    u32 script_timer_flags;                // 06c
    Vec3Interpolator position_interpolation; // 070
    RgbInterpolator color_interpolation;  // 0bc
    AlphaInterpolator alpha_interpolation;// 108
    Vec3Interpolator rotation_interpolation; // 134
    Vec2Interpolator scale_interpolation; // 180
    RgbInterpolator color2_interpolation; // 1bc
    AlphaInterpolator alpha2_interpolation; // 208
    Vec2 uv_velocity;                      // 234
    Matrix4 sprite_matrix;                 // 23c
    Matrix4 transform_matrix;              // 27c
    Matrix4 uv_matrix;                     // 2bc
    u32 color;                             // 2fc, white after initialize
    u32 secondary_color;                   // 300
    std::int16_t pending_interrupt;        // 304
    u16 reserved_306;
    AnmFile* animation_file;                // 308
    i32 integer_variables[4];              // 30c
    float float_variables[4];              // 31c
    i32 extra_integer_variables[2];        // 32c
    Vec3 script_position;                  // 334
    Vec3 position;                         // 340, retained by initialize
    Vec3 child_position;                   // 34c
    void* geometry;                        // 358
    u32 flags;                             // 35c
    u32 reserved_360[2];
    Timer saved_timer;                     // 368
    u32 saved_timer_flags;                 // 378
    AnmInstruction* saved_instruction;     // 37c
    i32 sprite_frame;                      // 380
    std::int16_t sprite_index;              // 384
    u16 file_index;
    u16 reserved_388;
    std::int16_t script_index;
    AnmInstruction* script_begin;          // 38c
    AnmInstruction* instruction;           // 390
    AnmSprite* sprite;                      // 394
    u32 update_callback,draw_callback;
    u8 text_settings[0x3ac-0x3a0];

    void clear() noexcept;
    void initialize() noexcept;
    void stop_interpolators() noexcept;
    i32 integer_variable(i32 value) const noexcept;
    Extended float_variable(float value,AnmEnvironment& environment) const noexcept;
    i32* integer_reference(u32 index,u16 references,i32* argument) noexcept;
    float* float_reference(u32 index,u16 references,float* argument) noexcept;
    i32 update(AnmEnvironment& environment);
    void update_ring_geometry() noexcept;
};
static_assert(sizeof(AnmVm) == 0x3ac);
static_assert(offsetof(AnmVm, position) == 0x340);
static_assert(offsetof(AnmVm, sprite_index) == 0x384);
static_assert(offsetof(AnmVm, position_interpolation) == 0x70);
static_assert(offsetof(AnmVm, scale_interpolation) == 0x180);
static_assert(offsetof(AnmVm, color) == 0x2fc);
}
