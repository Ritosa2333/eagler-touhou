#include "ending.hpp"
#include "data_constants.hpp"
#include "../program_entry/program_entry.hpp"
#include "../progress_state/records.hpp"
#include "../gameplay/gameplay.hpp"
#include "../gameplay/loading_dependencies.hpp"
#include "../gameplay/player_state.hpp"
#include "../runtime_state/state.hpp"
#include "../hud_system/dialogue.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../audio_runtime/audio.hpp"
#include "../screen_effect/effect.hpp"
#include "../trophy_system/trophy.hpp"
#include <atomic>
#include <cstring>
#include <string>
#include <stdexcept>
namespace th20::source::ending {
int run_script(Script& o){
    if(std::atomic_ref(o.flags).load()&4u)return 0;
    auto& sprites=*program_entry::sprite_controller;
    for(;;){
        if(o.script_time.current<progress::read<std::uint16_t>(o.instruction,0)){recovered::timer_tick(o.script_time,state::timer_rate);return 0;}
        const unsigned opcode=o.instruction[2];auto arg=[&](unsigned i=0){return progress::read<int>(o.instruction,4+i*4);};
        switch(opcode){
        default:return -1;
        case 3:queue_line(o);break;
        case 4:for(unsigned i=0;i<5;++i){sprite::interrupt_animation_children(sprites,o.text_handles[i],3);sprite::interrupt_animation_children(sprites,o.ruby_handles[i],3);}break;
        case 5:case 6:{
            if(o.wait_time.current<1)recovered::timer_set(o.wait_time,arg());recovered::timer_add(o.wait_time,-1,state::timer_rate);
            if(opcode==5&&arg()<0)recovered::timer_set(o.wait_time,999);
            auto* buttons=input::button_slot(0);
            if(!(buttons&&(buttons->pressed&0x80001u))&&o.wait_time.current>0){
                if(controller()->ending_flags&1u)return 0;
                if(!(buttons&&(buttons->current&0x200u))&&held_frames(buttons,0)<20)return 0;
                if(o.wait_time.current%6!=0)return 0;recovered::timer_set(o.wait_time,0);
            }else{program_entry::thread_registry.request_effect(0,0);recovered::timer_set(o.wait_time,0);}
            if(opcode==6){o.line=0;gameplay::slowdown_frames=0;}break;
        }
        case 7:begin_resource_load(o);o.instruction+=4+o.instruction[3];return 0;
        case 8:case 15:case 16:case 17:{
            const auto difficulty=gameplay::player_state::difficulty(game_session::session.player_table);if(opcode!=8&&difficulty!=static_cast<int>(opcode)-14)break;
            const auto slot=arg(),file=arg(1),script=arg(2);if(slot<0||slot>=16||file<0||file>=4)throw std::out_of_range("Ending animation index outside original arrays");
            sprite::request_animation_deletion(sprites,o.handles[slot]);o.handles[slot]=sprite::spawn_named_animation(sprites,*o.files[file],nullptr,script);break;
        }
        case 9:o.foreground=static_cast<unsigned>(arg());o.background=static_cast<unsigned>(arg(1));break;
        case 10:{const char* name=reinterpret_cast<const char*>(o.instruction+4);std::string wav(name);wav+=".wav";program_entry::thread_registry.enqueue(1,0,wav.c_str());hud::play_stage_track(0,std::strcmp(name,"bgm/th20_15")==0?15:std::strcmp(name,"bgm/th20_14")==0?16:17);break;}
        case 11:hud::fade_stage_track(data::f_0056c8d8);o.flags&=~1u;break;
        case 12:{
            if(controller()->ending_flags&4u)return -1;trophy::announce(static_cast<unsigned>(o.ending_id));
            for(unsigned i=0;i<5;++i){sprite::request_animation_deletion(sprites,o.text_handles[i]);sprite::request_animation_deletion(sprites,o.ruby_handles[i]);}
            //Original iterates these handles by value, preserving stored words.
            for(auto handle:o.handles)sprite::request_animation_deletion(sprites,handle);
            const auto difficulty=gameplay::player_state::difficulty(game_session::session.player_table);
            const unsigned credits=(o.ending_id==16||o.ending_id==17)?4u:(difficulty>=1&&difficulty<=3?static_cast<unsigned>(difficulty):0u);
            auto* bytes=replace_script_data(*controller(),data::credits[credits]);if(!bytes)return -1;
            o.instruction=bytes+progress::read<unsigned>(bytes,4);recovered::timer_set(o.elapsed,0);recovered::timer_set(o.script_time,0);recovered::timer_set(o.wait_time,0);o.foreground=0xffffff;o.flags|=2;continue;
        }
        case 13:case 14:screen::create_effect(opcode==13?0:5,arg(),0,0,0,109);break;
        }
        o.instruction+=4+o.instruction[3];
    }
}
}
