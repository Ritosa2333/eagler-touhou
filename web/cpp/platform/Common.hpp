#pragma once
#include "AnimationEngine.hpp"
#include "../game/CommonResources.hpp"
namespace th10::browser {
struct CommonTextRenderer final:AsciiRenderEnvironment {
    AnimationEngine& engine;GraphicsRenderer graphics;
    explicit CommonTextRenderer(AnimationEngine& engine):engine(engine),graphics(engine.renderer()){}
    void select_camera(bool world) override;
    void draw_character(AnmVm&,bool pixel) override;
};
struct Common final:CommonResourceEnvironment,CallbackReceiver {
    AnimationEngine& engine;CommonResources* value=nullptr;UpdateChain* update_chain;
    i32 error=0;
    explicit Common(AnimationEngine&);~Common();
    void bind_callbacks(Callbacks&) override;
#ifndef TH_NATIVE_PLATFORM
    bool invoke(CallbackToken,void*,i32&) override;
#endif
    CommonResources* allocate() override;
    void delete_object(void*) override;void free_geometry(void*) override;
    AnmFile* load_animations(i32,const char*) override;void release_animations(AnmFile&) override;
    void bind_sprite(AnmFile&,AnmVm&,i32) override;
    void report_error() override {error=-1;}
    bool initialize();i32 draw(bool early);
};
}
