#pragma once
#include "AnimationResources.hpp"
#include "Callbacks.hpp"
#include "../game/AnmSystems.hpp"
#include "../game/AnmFrame.hpp"
#include "../game/AnmLayers.hpp"
#include "../game/AnmDistortion.hpp"
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
#include "../multiplayer/PresentationCache.hpp"
#include "../multiplayer/RollbackPool.hpp"
namespace th10::multiplayer { class RollbackState; }
#endif
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
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    // The native ANM owner has a fixed 4096-slot pool.  MP keeps its
    // draw-side first samples in a matching bounded table; a full table only
    // disables interpolation for the missing sample.
    multiplayer::PresentationCache<const AnmVm*,PresentationVmSample,4096> presentation_previous;
    multiplayer::RollbackPool<sizeof(AnmVm),2048> rollback_animation_overflow{};
    // ANM geometry is referenced from rewindable VMs. Keep it at stable
    // addresses as well; retail distortion is 0x4b0 and script-created vertex
    // buffers are bounded here to 64 KiB rather than falling back to the heap.
    multiplayer::RollbackPool<65536,128> rollback_geometry{};
    multiplayer::RollbackState* rollback_state=nullptr;
    // Historical authored Draw still runs (callbacks, VM state and RNG).
    // Sprite submission retains projected-VM transform/dirty-bit updates but
    // omits geometry, fog, batching and upload for its unpresented pixels.
    bool suppress_rollback_sprite_output=false;
    bool enhance_local_player_visibility=false;
    const void* player_view_owner=nullptr;
    u8 (*player_view_alpha)(const void*,const AnmVm&)=nullptr;
#else
    std::map<const AnmVm*,PresentationVmSample> presentation_previous;
#endif
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
#ifdef TH_ENABLE_MULTIPLAYER_GAMEPLAY
    void preserve_animation_slot(AnmVm&) override;
#endif
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
