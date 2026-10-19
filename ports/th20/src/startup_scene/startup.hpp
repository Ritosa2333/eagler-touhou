#pragma once
#include "../runtime_core/callback_owner.hpp"
#include "../runtime_core/worker.hpp"
#include "../sprite_renderer/sprite.hpp"

namespace th20::source::startup {
class LoadingScene : public runtime::CallbackOwner {
public:
    LoadingScene();                           // 4d7ef0, followszeroallocator4d7ea0
    ~LoadingScene() override;                 // 4d7fd0
    runtime::Worker worker;                  // +10
    std::uint32_t field_20;
    sprite::Animation animation;             // +24
    std::uint32_t animation_handle;          // +608
    sprite::AnimationFile* signature_file;    // +60c
    std::uint32_t signature_ready,text_ready,draw_frames; // +610/+614/+618
    int register_callbacks();                // 4d8220
    int update();                            // 4d80f0
    int draw();                              // 4d8160
};
static_assert(offsetof(LoadingScene,worker)==0x10);
// Members past the embedded runtime::Worker sit at STL-layout-dependent
// offsets; binding on the MSVC x86 build only.
#ifndef TH_SDL3
static_assert(offsetof(LoadingScene,animation)==0x24);
static_assert(offsetof(LoadingScene,animation_handle)==0x608);
static_assert(sizeof(LoadingScene)==0x61c);
#endif
extern LoadingScene* loading_scene;            // actualBSSglobal5c4d2c
LoadingScene* create_loading_scene();           // 4d85c0
int load_worker();                             // 4d8350
int initialize_shared_scene_resources();       // 4d82c0
int release_shared_scene_resources();          // 4d8560
void shutdown_scene_objects();                 // 4dd730
namespace unrecovered {
// Required source interfaces. No definitions are supplied until their actual
// owning subsystem is recovered; these are never dummy-success functions.
void initialize_loading_cache();               //50fce0 ->50adc0
void release_loading_cache();                  //50fc10 ->50ad50
int initialize_resource_004b5900();
runtime::CallbackOwner* create_resource_0049e0c0(int);
runtime::CallbackOwner* create_resource_00534dd0(int);
runtime::CallbackOwner* create_resource_0051cc20(int);
int initialize_resource_0052e210();
void release_resource_0049deb0(int);
void release_resource_00534110(int);
void release_resource_0051b6d0();
void release_resource_004b6560();
void release_resource_0052e730();
extern runtime::CallbackOwner* owner_005c4d28;
extern runtime::CallbackOwner* owner_005c60b8;
extern runtime::CallbackOwner* owner_005c60fc;
extern runtime::CallbackOwner* owner_005c4d24;
extern runtime::CallbackOwner* owner_005c5b38;
extern runtime::CallbackOwner* owner_005c6114;
}
}
