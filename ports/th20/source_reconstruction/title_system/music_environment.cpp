#include "music.hpp"
#include "music_data.hpp"
#include "pages.hpp"
#include "../archive/resource_manager.hpp"
#include "../input/input.hpp"
#include "../audio_runtime/audio.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../program_entry/program_entry.hpp"
#include <stdexcept>
namespace th20::source::title {
namespace {
class SourceMusic final:public MusicEnvironment {
    bool pressed(unsigned mask) override{const auto* b=input::button_slot(0);return b&&(b->pressed&mask)!=0;}
    bool repeated(unsigned mask) override{const auto* b=input::button_slot(0);return b&&((b->repeat8|b->pressed)&mask)!=0;}
    bool unlocked(int index) override{if(index<0||index>=32)throw std::out_of_range("Music unlock index");return progress::manager->current.metadata.bytes[0x36+index]!=0;}
    void spawn_background(TitleInf& o) override{o.handle390=sprite::spawn_named_animation(*program_entry::sprite_controller,*text::renderer->animation_file,nullptr,19);}
    void spawn_heading(TitleInf& o) override{spawn(o,40);}
    void retire_heading(TitleInf& o) override{retire_animation(o,40);}
    void begin_read(TitleInf& o) override{std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(6));runtime::detach_worker(o.worker);o.worker.close_requested.store(false,std::memory_order_seq_cst);o.worker.thread=TH20_WORKER_THREAD("music-comments",[&o]{read_music_comments(o);});}
    void interrupt(unsigned handle,int event) override{sprite::interrupt_animation_children(*program_entry::sprite_controller,handle,event);}
    void sound(int id) override{program_entry::thread_registry.request_effect(id,0);}
    void play(const char* name) override{play_music(name);}
    void stop() override{stop_music();}
} environment;
}
MusicEnvironment& music_environment(){return environment;}
namespace unrecovered {
void update_music_005205d0(TitleInf& o){update_music(o,music_environment());}
void draw_music_00520c80(TitleInf& o){draw_music(o,*text::renderer,progress::manager->current.metadata);}
}
}
