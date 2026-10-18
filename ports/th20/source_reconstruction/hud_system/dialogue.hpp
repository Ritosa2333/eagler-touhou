#pragma once
#include "hud.hpp"
#include "../stone_menu/cursor.hpp"
namespace th20::source::hud {
// 4af310. This object has no vptr: 4b06e0 is a deleting-destructor helper,
// while the first word is script state. Cursor owns the only C++ containers.
struct Dialogue {
    std::uint32_t state;
    recovered::Timer timers[3];
    std::uint32_t portraits[4],portrait_overlays[4]; //34,44
    std::uint32_t handles[8]; //54..70; destructor intentionally excludes70
    std::uint32_t fields_74[2];
    menu::Cursor cursor; //7c
    std::uint32_t field_c8;
    std::uint8_t* script; //cc
    sprite::Vec3 vectors_d0[4];
    std::uint32_t field_100,flags,fields_108[8];
    sprite::Vec3 vector_128;
    float field_134;
    std::uint32_t fields_138[2];
    explicit Dialogue(std::uint8_t*); //4af310
    ~Dialogue(); //4afc00; Cursor then releases its three containers
};
static_assert(sizeof(Dialogue)==0x140&&offsetof(Dialogue,cursor)==0x7c&&offsetof(Dialogue,script)==0xcc&&offsetof(Dialogue,vector_128)==0x128);
void destroy_dialogue(Dialogue*); //4ae830
int run_dialogue(Dialogue&); //4b0720
bool update_dialogue(Dialogue&); //4b4450
Dialogue* create_dialogue(std::uint8_t*); //4ae930
const char* decode_dialogue_text(const std::uint8_t*); //4b7c90
void resize_dialogue_box(Dialogue&,float); //4b8dc0
void create_dialogue_box(Dialogue&,float,float,float,int); //4b8e70
void follow_dialogue_box(Dialogue&,sprite::Animation&); //4b89a0
void position_dialogue_text(Dialogue&); //4b0720 tail, uses distinct6px offset
void clear_dialogue_enemies(); //4a4a20
void start_dialogue(FrontInf&,int); //4b9740
void play_stage_track(int,int); //4d9b50, includes music-room unlock
void fade_stage_track(float); //4d99d0
void release_stage_resources(FrontInf&); //4b64a0, transition-specific cleanup
namespace unrecovered {
runtime::CallbackOwner* create_scene_005115c0(int);
void complete_stage_004bc570();
void finish_spell_004e5bd0();
}
}
