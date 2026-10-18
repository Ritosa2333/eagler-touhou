#include "../th10_web/cpp/game/Ending.hpp"
#include <cassert>
#include <cstdio>
#include <vector>
#include <type_traits>
using namespace th10;
struct Environment final:EndingEnvironment {
    u32 begin_thread(CallbackToken,void*,u32,u32&)override{return 0;}
    u32 wait_thread(u32,u32)override{return 0;}void close_thread(u32)override{}void sleep(u32)override{}
    void* allocate(u32)override{return nullptr;}void delete_object(void*)override{}void free_bytes(void*)override{}
    u8* read_file(const char*)override{return nullptr;}void report_error()override{}void show_loading()override{}
    u32 create_animation(AnmFile&,i32)override{return 0;}void draw_text(AnmVm*,u32,const char*)override{}
    void sound(i32)override{}void fade(i32,i32)override{}void load_music(const char*)override{}
    void play_music(i32)override{}void fade_music(float)override{}AnmFile* load_animations(i32,const char*)override{return nullptr;}
    void unload_animations(i32)override{}void release_animations(AnmFile&)override{}void release_capture()override{}
};
int main(){
    Environment env{};float rate=1;u32 held=0,engine=0;i32 pending=0,menu=0;
    std::remove_cv_t<std::remove_pointer_t<decltype(env.pressed)>> pressed=0;
    env.rate=&rate;env.held=&held;env.pressed=&pressed;env.engine_flags=&engine;env.pending_screen=&pending;env.menu_state=&menu;
    std::vector<u8> bytes(12);MessageInstruction* instruction=reinterpret_cast<MessageInstruction*>(bytes.data());
    instruction->time=300;instruction->opcode=5;instruction->length=4;i32 wait=600;std::memcpy(bytes.data()+4,&wait,4);
    auto* end=reinterpret_cast<MessageInstruction*>(bytes.data()+8);end->time=301;end->opcode=0;
    auto initialize=[&](EndingScript& s,u32 flags){s={};s.flags=flags;s.instruction=instruction;s.script_time.rate=s.wait.rate=s.elapsed.rate=&rate;s.script_time_flags=s.wait_flags=s.elapsed_flags=1;};
    EndingScript script{};initialize(script,1);
    pressed=1;assert(script.update(env)==0);assert(script.instruction==end&&script.wait.current==0); // Z during timestamp delay.
    pressed=0;assert(script.update(env)==-1);
    initialize(script,1);held=256;assert(script.update(env)==0); // First-view Ctrl starts the page wait.
    for(unsigned i=0;i<8&&script.instruction==instruction;++i)assert(script.update(env)==0);
    assert(script.instruction==end);
    initialize(script,1);held=0;script.script_time.current=300;assert(script.update(env)==0);assert(script.wait.current==599);
    pressed=4096;assert(script.update(env)==0);assert(script.wait.current==0&&script.instruction==end);
    initialize(script,2);held=256;pressed=0;Ending ending{};ending.script=&script;ending.newly_unlocked=3;
    unsigned repeats=0;while(ending.update(env)==6){assert(++repeats<12);}assert(ending.frames==12&&script.script_time.current==12);
    initialize(script,2);held=0;pressed=1;assert(script.update(env)==0);assert(script.instruction==end); // Staff Z also advances.
    wait=-1;std::memcpy(bytes.data()+4,&wait,4);initialize(script,1);script.script_time.current=300;pressed=0;held=0;
    assert(script.update(env)==0&&script.instruction==instruction);held=256;assert(script.update(env)==0);assert(script.instruction==end);
    std::puts("TH10 first-view Ending / timestamp Z / wait Z-Enter / first-view staff Ctrl: PASS");
}
