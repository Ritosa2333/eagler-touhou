#pragma once
#include "GameState.hpp"
#include "ScreenEffects.hpp"
#include "../game/StageResources.hpp"
#include "../game/StageRenderer.hpp"
#include <array>
namespace th10::browser {
struct Backgrounds;
struct BackgroundScript final : StageEnvironment {
    Backgrounds& owner;    explicit BackgroundScript(Backgrounds&);
    i32 update_animation(AnmVm&) override;
    void initialize_animation(AnmFile&,AnmVm&,i32) override;
    void normalize(Vec3&,const Vec3&) override;
};
struct BackgroundDraw final : StageRenderEnvironment {
    Backgrounds& owner;GraphicsRenderer renderer;GraphicsCamera graphics_camera;
    explicit BackgroundDraw(Backgrounds&);
    void translation(Matrix4&,const Vec3&) override;
    void project_points(Vec3*,const Vec3*,u32,const Camera&,const Matrix4&) override;
    void draw_animation(AnmVm&) override;
    void draw_layer(u32) override;
    void clear(u32,u32,const StageClearRect*) override;
    void fade(i32,i32) override;
};
struct Backgrounds final : StageResourceEnvironment,CallbackReceiver {
    GameState& state;AnimationEngine& engine;ScreenEffects& effects;FileSystem& files;
    Stage* current=nullptr;Stage* previous=nullptr;char source_name[260]{};i32 error=0;
    struct PresentationStage {Stage* owner=nullptr;Camera camera{};bool valid=false;};
    std::array<PresentationStage,2> presentation{};
    BackgroundScript script;
    Backgrounds(GameState&,AnimationEngine&,ScreenEffects&,FileSystem&);
    ~Backgrounds();
    Stage* create(const char* name,i32 offset=0);
    void destroy(Stage*);
    i32 update(Stage&);
    i32 draw(Stage&,bool foreground);
    void snapshot(Stage&);
    Camera presentation_camera(const Stage&)const;
    void bind_callbacks(Callbacks&) override;
#ifndef TH_NATIVE_PLATFORM
    bool invoke(CallbackToken,void*,i32&) override;
#endif
    Stage* allocate_stage() override;
    void* allocate_bytes(u32) override;
    void release_memory(void*) override;
    u8* read_file(const char*,u32*) override;
    AnmFile* load_animations(i32,const char*) override;
    void release_animations(AnmFile&) override;
    void report(StageResourceError) override;
};
}
