#pragma once
#include "AnimationResources.hpp"
#include "Callbacks.hpp"
#include "../game/AnmSystems.hpp"
#include "../game/AnmFrame.hpp"
#include "../game/AnmLayers.hpp"
#include "../game/AnmDistortion.hpp"
namespace th10::browser {
// A complete ANM owner sharing native files and graphics with the application.
// Script operands still come from the original DAT resources; no EXE is loaded.
struct AnimationEngine final:AnmEnvironment,AnmAllocationEnvironment,AnmSystemEnvironment,AnmDistortionEnvironment,AnmFrameEnvironment,CallbackReceiver {
    GraphicsDevice& device;Rng& script_random;Rng& visual_random;float& speed;
    AnmManager manager;Camera world{},ui{};Camera* active=&ui;
    u32 screen_space=1,fog_enabled=0;Vec3 tangent{};
    AnmVertex24 initial_vertices[4]{},model_template[4]{};AnmVertex vertices[4]{};
    UpdateChain chain_value{};Callbacks callback_environment;AnimationResources resources;
    struct PresentationVmSample {
        u32 id=0;std::int16_t script_index=-1,sprite_index=-1;AnmFile* file=nullptr;
        Vec3 position{},script_position{},child_position{},rotation{};Vec2 scale{},uv_offset{};
        u32 color=0,secondary_color=0,visible=0;i32 script_time=0;u16 continuous=0;
    };
    std::map<const AnmVm*,PresentationVmSample> presentation_previous;
#ifndef TH_NATIVE_PLATFORM
    CallbackReceiver* application_callbacks=nullptr;
    CallbackReceiver* receivers[16]{};
    // Resolved native receivers; invalidated before their owning scene dies.
    std::map<CallbackToken,CallbackReceiver*> resolved_callbacks;
#endif
    AnimationEngine(FileSystem&,GraphicsDevice&,Rng& script,Rng& visual,float& speed);
    ~AnimationEngine();
    GraphicsRenderer renderer();
#ifndef TH_NATIVE_PLATFORM
    bool invoke(CallbackToken,void*,i32&) override;
#endif
    void register_receiver(CallbackReceiver&);void unregister_receiver(CallbackReceiver&);
    void callback(u32,AnmVm&) override;
    i32 update(AnmVm&) override;
    void draw(AnmVm&) override;
    bool present(AnmVm& draw,const AnmVm& source) const;
    void bind_sprite(AnmVm&,i32) override;
    void change_draw_mode(AnmVm&) override;
    void* allocate_geometry(u32) override;
    AnmVm* spawn_child(AnmVm&,i32,u32) override;
    AnmVm* allocate_animation() override;
    void release_memory(void*) override;
    void* allocate(u32) override;
    void release(void*) override;
    void clear_pixel_shader() override;
    void create_model_buffer(void*&) override;
    void* lock_model_buffer(void*) override;
    void unlock_model_buffer(void*) override;
    void bind_model_buffer(void*) override;
    void begin_frame();void flush();i32 update_all();i32 draw_all();
    i32 draw_layer(u32);void configure_camera(bool flat);void snapshot_presentation();
};
}
