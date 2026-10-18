#pragma once
#include "Camera.hpp"
#include "FogInterpolation.hpp"
#include "UpdateChain.hpp"
#include "AnmEnvironment.hpp"
namespace th10 {
struct StageEnvironment;
struct ScreenEffectEnvironment;
struct StageInstruction {
    i32 time;std::int16_t opcode,length;
    template<class T> T argument(u32 index) const noexcept {T value;std::memcpy(&value,reinterpret_cast<const u8*>(this)+8+index*4,sizeof(T));return value;}
    StageInstruction* next() noexcept {return reinterpret_cast<StageInstruction*>(reinterpret_cast<u8*>(this)+length);}
};
struct StagePrimitive {
    std::int16_t type,length,script,animation;
    Vec3 position;Vec2 size;
    StagePrimitive* next() noexcept {return reinterpret_cast<StagePrimitive*>(reinterpret_cast<u8*>(this)+length);}
};
struct StageObject {
    std::int16_t id;u8 layer,flags;Vec3 position,size;
    StagePrimitive* primitives() noexcept {return reinterpret_cast<StagePrimitive*>(this+1);}
};
struct StageInstance {std::int16_t object,reserved;Vec3 position;};
struct StageHeader {std::int16_t object_count,primitive_count;u32 instances_offset,script_offset,reserved;char animation_name[128];};
struct Stage {
    u32 flags,state;
    UpdateChainEntry* update_entry;
    UpdateChainEntry* draw_entry;
    StageHeader* file;
    StageObject** objects;
    StageInstance* instances;
    StageInstruction* script_begin;
    u8 camera_effect;u8 reserved_021[3];
    Timer effect_timer;u32 effect_timer_flags;
    Timer script_timer;u32 script_timer_flags;
    StageInstruction* instruction;
    Vec3Interpolator target_interpolation,position_interpolation;
    FogInterpolator fog_interpolation;
    u32 reserved_174;
    AnmFile* animation_file;
    AnmVm* object_animations;
    AnmVm script_animations[8];
    float draw_distance_squared;
    u32 effects_enabled,fade_color;
    i32 frame_effect;
    Vec3 effect_position,effect_size;
    AnmVm effect_animations[3];
    u32 drawn_objects,culled_objects,drawn_primitives;
    u32 draw_flags;
    Timer fade_timer;u32 fade_timer_flags;
    i32 stage_number;
    u32 frame_count;
    u32 effect_ids[2];
    UpdateChainEntry* foreground_entry;
    u8* source;u32 source_size;
    Camera camera;
    void initialize() noexcept;
    i32 update_script(StageEnvironment& environment);
    i32 update_objects(StageEnvironment& environment);
    i32 update(StageEnvironment& environment);
    void restart(StageEnvironment& environment);
    void apply_frame_effect(const Vec3& position,const Vec3& size) noexcept;
    void enable_effects() noexcept;
    void disable_effects(const AnmRegistry& registry) noexcept;
    void fade_to_black(ScreenEffectEnvironment& environment);
    void fade_in(float* rate) noexcept;
};
static_assert(sizeof(StageHeader)==0x90&&sizeof(StageObject)==0x1c&&sizeof(StageInstance)==16);
static_assert(sizeof(Stage)==0x2b64);
static_assert(offsetof(Stage,script_animations)==0x180&&offsetof(Stage,effect_animations)==0x1f08);
static_assert(offsetof(Stage,camera)==0x2a4c&&offsetof(Stage,instruction)==0x4c);
struct StageEnvironment {
    float* rate;
    u32* background_color;
    Camera* world;
    virtual i32 update_animation(AnmVm& vm)=0;
    virtual void initialize_animation(AnmFile& file,AnmVm& vm,i32 script)=0;
    virtual void normalize(Vec3& output,const Vec3& input)=0;
};
}
