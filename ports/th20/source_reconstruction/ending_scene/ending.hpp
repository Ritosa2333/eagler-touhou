#pragma once
#include "../runtime_core/callback_owner.hpp"
#include "../runtime_core/worker.hpp"
#include "../sprite_renderer/sprite.hpp"
#include "../input/input.hpp"
namespace th20::source::ending {
struct Script {
    std::int32_t ending_id; //0
    recovered::Timer elapsed,script_time,wait_time;
    std::uint32_t text_handles[5],ruby_handles[5]; //34,48
    std::uint8_t* instruction; //5c
    sprite::Vec3 positions[2]; //60
    const char* pending_file; //78
    std::uint32_t flags,line,foreground,background; //7c
    sprite::AnimationFile* files[4]; //8c
    std::uint32_t handles[16]; //9c
    runtime::Worker worker; //dc
    std::int32_t pending_slot; //ec
    Script(std::uint8_t*,int); //49ecd0
    ~Script(); //49f0b0
};
static_assert(offsetof(Script,instruction)==0x5c&&offsetof(Script,worker)==0xdc);
// Total size depends on the STL layout (runtime::Worker holds a jthread);
// binding on the MSVC x86 build only.
#ifndef TH_SDL3
static_assert(sizeof(Script)==0xf0);
#endif
struct EndingInf final:runtime::CallbackOwner {
    std::uint32_t field_10;
    std::uint8_t* message_data;
    Script* script;
    std::int32_t ending_id;
    std::uint32_t ending_flags,frames;
    EndingInf(); //49ec30
    ~EndingInf() override; //49ef90
    void enable_callbacks() override {scheduler::enable(*update_node);scheduler::enable(*draw_node);} //4a0a70
};
static_assert(sizeof(EndingInf)==0x28&&offsetof(EndingInf,ending_flags)==0x20);
EndingInf* controller() noexcept;
extern int selected_gallery_ending; //5afc98,-1
EndingInf* create(); //4a1010
int initialize(EndingInf&); //4a0600
int update(EndingInf&); //4a0440
int run_script(Script&); //49f3d0
bool update_script(Script&); //4a0590
void queue_line(Script&); //49f3d0 opcode3 and49f200/49f2a0 tasks
void begin_resource_load(Script&); //49fb49..49fc29
void load_pending_resource(Script&); //4a0990
std::uint8_t* replace_script_data(EndingInf&,const char*); //4a0f30
int ending_record(unsigned); //4a0df0, signed byte
int select_ending(); //4bd4e0
unsigned held_frames(const input::ButtonState*,unsigned); //4a0a20
}
