#include "replay_menu.hpp"
#ifdef TH_SDL3
#include "platform/Files.hpp"
#endif
#include "pages.hpp"
#include "../program_entry/program_entry.hpp"
#include "../gameplay/loading_dependencies.hpp"
#include "../gameplay/stage_data.hpp"
#include "../effect_system/effect.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../hud_system/dialogue.hpp"
#include <filesystem>
#include <cstdio>
#include <string>
namespace th20::source::title {
namespace {
struct Production final:ReplayMenuEnvironment {
    Production():ReplayMenuEnvironment(selection_environment(),title::last_replay){}
    void begin_read(TitleInf& o)override{std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(6));runtime::detach_worker(o.worker);o.worker.close_requested.store(false,std::memory_order_seq_cst);o.worker.thread=TH20_WORKER_THREAD("replay-list",[&o]{read_replay_list(o);});}
    bool effects_ready()override{return effects::controller(0)->ready!=0;}
    void loading_transition()override{auto& handle=program_entry::graphics_state.unknown_01c4;handle=effects::controller(0)->spawn(0,nullptr,nullptr,true);sprite::interrupt_animation_children(*program_entry::sprite_controller,handle,7);text::renderer->create_loading_text(480.f,392.f);}
    void fade(float time)override{hud::fade_stage_track(time);}
    void request_start(int stage,const char* filename)override{gameplay::select_stage(selection.main.session.player_table,stage);program_entry::graphics_state.field_0b0c=13;strcpy_s(gameplay::replay_file,256,filename);}
    void retire(runtime::CallbackOwner* value)override{runtime::retire_callback_owner(value);}
};
}
ReplayMenuEnvironment& replay_menu_environment(){static Production value;return value;}
void read_replay_list(TitleInf& o){
    for(int i=1;i<26;++i){char filename[64];sprintf_s(filename,"th20_%.2d.rpy",i);o.metadata[i-1]=replay::read_metadata(filename);if(o.ui_flags&4u)break;}
    auto directory=std::filesystem::path(program_entry::window_state.user_data_directory)/"replay";
#ifdef TH_SDL3
    for(int i=25;i<75;++i){char filename[4096]{};
        if(!web::files::list("replay","th20_ud????.rpy",static_cast<std::uint32_t>(i-25),filename,4096))break;
        const auto path=(directory/filename).string();o.metadata[i]=replay::read_metadata(path.c_str());if(o.ui_flags&4u)break;}
#else
    const auto pattern=(directory/"th20_ud????.rpy").string();
    wchar_t wide[4096]{};MultiByteToWideChar(932,0,pattern.c_str(),-1,wide,4096);WIN32_FIND_DATAW data{};const auto search=FindFirstFileW(wide,&data);
    if(search!=INVALID_HANDLE_VALUE){
        for(int i=25;i<75;++i){char filename[4096]{};WideCharToMultiByte(932,0,data.cFileName,-1,filename,4096,nullptr,nullptr);const auto path=(directory/filename).string();o.metadata[i]=replay::read_metadata(path.c_str());if((o.ui_flags&4u)||!FindNextFileW(search,&data))break;}
    }
    FindClose(search);
#endif
    o.ui_flags|=8u;o.ui_flags&=~4u;
}
namespace unrecovered {void update_replay_00523440(TitleInf& o){update_replay_menu(o,replay_menu_environment());}}
}
