#pragma once
#include "AnmFile.hpp"
#include "AnmEnvironment.hpp"
#include "AnmRegistry.hpp"
namespace th10 {
struct AnmFrameEnvironment;
struct AnmSystemEnvironment;
struct AnmModelVertex {Vec3 position;Vec2 uv;};
enum class AnimationPlacement { WorldBack,WorldFront,UiBack,UiFront };
struct AnmAllocationEnvironment {
    virtual AnmVm* allocate_animation()=0;
    virtual void release_memory(void* memory)=0;
};
struct AnmManager {
    u8 header[0x4c];
    u32 started_scripts;
    u32 reserved_050,submitted_draws,flushed_batches;
    Vec2 draw_offset;
    u32 processed_count;
    AnmVm pool[4096];
    u8 occupied[4096];
    i32 cursor;
    AnmFile* files[33];
    Matrix4 render_world_matrix;
    AnmVm resource_animation;
    u8 resource_state[0x3ada60-0x3ad4dc];
    u32 current_material_color;
    void* current_texture;
    u8 cached_draw_state[8];
    AnmSprite* current_uv_sprite;
    void* model_vertex_buffer;
    AnmModelVertex model_vertices[4];
    u32 batch_quads;
    AnmVertex vertex_buffer[131072];
    AnmVertex* vertex_write;
    AnmVertex* batch_start;
    AnmRegistry registry;
    AnmVm draw_layers[20];
    u32 last_id;
    u32 tint,tint_enabled;
    void initialize(AnmSystemEnvironment& environment);
    void release(AnmAllocationEnvironment& environment);
    void initialize_model(AnmSystemEnvironment& environment);
    AnmFile* open(i32 slot,const char* name,AnmResourceEnvironment& environment);
    AnmFile* load(i32 slot,const char* name,AnmResourceEnvironment& environment);
    void unload(i32 slot,AnmResourceEnvironment& environment);
    i32 process_loading(AnmResourceEnvironment& environment);
    bool resources_ready() const noexcept;
    i32 update_world(AnmFrameEnvironment& environment);
    i32 update_ui(AnmFrameEnvironment& environment);
    i32 draw_layer(u32 layer,AnmFrameEnvironment& environment);
    AnmVm* allocate(AnmAllocationEnvironment& environment);
    bool is_pooled(const AnmVm* vm) const noexcept;
    u32 insert(AnmVm& vm,AnimationPlacement placement) noexcept;
    i32 remove(AnmVm& vm,AnmAllocationEnvironment& environment);
    u32 create(AnmFile& file,i32 script,u32 tag,AnimationPlacement placement,AnmEnvironment& animations,AnmAllocationEnvironment& allocation);
    u32 create_at(AnmFile& file,i32 script,const Vec3& position,bool playfield_coordinates,AnimationPlacement placement,AnmEnvironment& animations,AnmAllocationEnvironment& allocation);
};
static_assert(offsetof(AnmManager,pool)==0x68);
static_assert(offsetof(AnmManager,cursor)==0x3ad068);
static_assert(offsetof(AnmManager,files)==0x3ad06c);
static_assert(offsetof(AnmManager,registry)==0x72dad4);
static_assert(offsetof(AnmManager,last_id)==0x732454);
static_assert(offsetof(AnmManager,draw_layers)==0x72dae4);
static_assert(offsetof(AnmManager,current_texture)==0x3ada64);
static_assert(offsetof(AnmManager,batch_quads)==0x3adac8);
static_assert(offsetof(AnmManager,vertex_write)==0x72dacc);
static_assert(offsetof(AnmManager,tint)==0x732458);
static_assert(sizeof(AnmManager)==0x732460);
}
