#pragma once
#include "../runtime_core/callback_owner.hpp"
#include "../runtime_core/worker.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../game_session/session.hpp"
namespace th20::source::effects {
struct Parameters { //47ba30, 0x38 bytes; names stay neutral where interpretation varies by effect.
    sprite::Vec3 vector_00,vector_0c;
    std::uint32_t value_18,value_1c,value_20,value_24;
    std::uint8_t enabled,padding_29[3];
    sprite::Vec3 vector_2c;
};
struct Request { //49ce30
    std::int32_t type;
    const Parameters* original_parameters;
    sprite::Animation* animation;
    std::int32_t delay;
    Parameters parameters;
};
static_assert(sizeof(Parameters)==0x38 && sizeof(Request)==0x48);
void construct_parameters(Parameters&) noexcept;
void construct_request(Request&) noexcept;
using Initializer=void(__cdecl*)(sprite::Animation*,const void*,std::int32_t);
struct Descriptor {std::int16_t file,script;Initializer initialize;std::uint32_t reserved[6];};
static_assert(sizeof(Descriptor)==32);
extern const Descriptor descriptors[16]; //5afab8,15 records plus the read-only terminator fields
class Controller final:public runtime::CallbackOwner { //49ce80, EffectInf
public:
    sprite::AnimationFile* files[6];
    runtime::Worker worker;
    std::uint32_t ready;
    std::uint32_t handles[1024];
    Request requests[1024];
    std::int32_t view_index;
    game_session::Context* context;
    Controller();
    ~Controller() override; //49d2f0
    int initialize(std::int32_t); //49d790
    int load_assets(); //49da70
    void clear(); //49d400
    int update(); //49d4a0
    int draw() noexcept {return 1;} //49dea0 ->478bf0
    void select_context(std::int32_t); //49e010
    std::uint32_t spawn(std::int32_t,const void* parameters,sprite::Animation* existing=nullptr,bool secondary=false); //49db70/49dcf0
    void* enqueue(std::int32_t delay,std::int32_t type,const Parameters*,sprite::Animation*); //49dc60 exact returned pointer
};
static_assert(offsetof(Controller,files)==0x10 && offsetof(Controller,worker)==0x28);
// Members past the embedded runtime::Worker sit at STL-layout-dependent
// offsets (jthread size differs); binding on the MSVC x86 build only.
#ifndef TH_SDL3
static_assert(offsetof(Controller,handles)==0x3c && offsetof(Controller,requests)==0x103c);
static_assert(offsetof(Controller,view_index)==0x1303c && sizeof(Controller)==0x13044);
#endif
Controller* create_controller(std::int32_t); //49cc90/49e0c0
void destroy_controller(std::int32_t); //49deb0/4bd240
Controller* controller(std::int32_t index) noexcept; //437520/41cad0
void enable_animation_tree(sprite::Animation&); //44fa70
namespace environment {
sprite::Controller& sprites();
game_session::Context& context(std::int32_t);
sprite::AnimationFile* load(std::int32_t,const char*);
void unload(std::int32_t);
void load_error();
}
namespace unrecovered {
void __cdecl initialize_0(sprite::Animation*,const void*,std::int32_t); //465960
void __cdecl initialize_1(sprite::Animation*,const void*,std::int32_t); //45d000
void __cdecl initialize_2(sprite::Animation*,const void*,std::int32_t); //45e550
void __cdecl initialize_3(sprite::Animation*,const void*,std::int32_t); //45e520
void __cdecl initialize_4(sprite::Animation*,const void*,std::int32_t); //466270
void __cdecl initialize_5(sprite::Animation*,const void*,std::int32_t); //460800
void __cdecl initialize_6(sprite::Animation*,const void*,std::int32_t); //460f30
void __cdecl initialize_7(sprite::Animation*,const void*,std::int32_t); //45ec70
void __cdecl initialize_8(sprite::Animation*,const void*,std::int32_t); //45dc70
void __cdecl initialize_9(sprite::Animation*,const void*,std::int32_t); //45fd10
void __cdecl initialize_10(sprite::Animation*,const void*,std::int32_t); //466860
void __cdecl initialize_11(sprite::Animation*,const void*,std::int32_t); //466f80
void __cdecl initialize_12(sprite::Animation*,const void*,std::int32_t); //463280
void __cdecl initialize_13(sprite::Animation*,const void*,std::int32_t); //4613a0, implemented in rounded_panel.cpp
void __cdecl initialize_14(sprite::Animation*,const void*,std::int32_t); //467c20
}
}
