#include "dialogue.hpp"
#include "../sprite_renderer/pool.hpp"
#include "dialogue_text.hpp"
#include "../input/input.hpp"
#include "../gameplay/gameplay.hpp"
#include "../gameplay/stage_data.hpp"
#include "../gameplay/enemy.hpp"
#include "../player_entity/owner.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/menu_animation.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../audio_runtime/audio.hpp"
#include "../progress_state/manager.hpp"
#include "../runtime_state/state.hpp"
#include <algorithm>
#include <cstring>
#include <string>
#include <stdexcept>
namespace th20::source::hud {
namespace {
template<class T>T load(const void* p,unsigned offset=0){T value;std::memcpy(&value,static_cast<const std::uint8_t*>(p)+offset,sizeof(T));return value;}
int signed_word(unsigned value){return recovered::signed_bits(value);}
unsigned pressed(unsigned mask){const auto* input=input::button_slot(0);return input?input->retained_298[4]&mask:0;} //4b58e0 +2a8
unsigned held(unsigned index){const auto* input=input::button_slot(0);return input&&(input->retained_298[1]&(1u<<index))?input->retained_218[index]:0;} //4b5b60 +29c/+218
auto& sprites(){return environment::sprites();}
void signal(unsigned handle,int event){sprite::interrupt_animation_children(sprites(),handle,event);}
void immediate(unsigned handle,int event){sprite::execute_animation_interrupt(sprites(),handle,event);}
unsigned& slot(unsigned (&handles)[4],int index){if(index<0||index>=4)throw std::out_of_range("Dialogue portrait outside original four slots");return handles[index];}
int stage_field(unsigned offset){if(offset<0x58||offset>=sizeof(gameplay::StageDefinition))throw std::out_of_range("Dialogue stage definition offset");return gameplay::selected_stage->fields_58[(offset-0x58)/4];}
unsigned spawn(sprite::AnimationFile& file,int script){return sprite::spawn_named_animation(sprites(),file,nullptr,script);}
unsigned stage_portrait(unsigned offset,unsigned index){auto& owner=gameplay::enemy_controller();return spawn(*owner.services->existing_animation(owner,stage_field(offset+index*0x34)),stage_field(offset+4+index*0x34));}
void reset_text_state(Dialogue& d){for(unsigned i=1;i<=4;++i)sprite::resolve_animation_handle(sprites(),d.handles[i])->base.vector_484.y=0;d.fields_108[0]=d.fields_108[1]=0;d.flags&=~2u;}
void boss_name(FrontInf& o){if(sprite::resolve_animation_handle(sprites(),o.handle_cc))return;auto& table=game_session::session.player_table;const auto n=std::clamp(gameplay::player_state::read<int>(table,0x1fc),0,999);gameplay::player_state::write(table,0x1fc,n);const int script=stage_field(n<41?0xc8:0x94);if(script>=0)o.handle_cc=spawn(*o.front_file,script+150);}
}
int run_dialogue(Dialogue& d){
    if(signed_word(d.field_100)>0)--d.field_100;
    if(signed_word(d.fields_108[2])<1){if(pressed(0x200)||pressed(1))d.flags|=0x40u;}
    else{--d.fields_108[2];d.flags&=~0x40u;}
    if((d.flags&0x41u)==0x41u&&(held(9)>=20||held(0)>=20))recovered::timer_set(d.timers[1],load<std::uint16_t>(d.script));
    while(static_cast<int>(load<std::uint16_t>(d.script))<=d.timers[1].current){
        const auto op=d.script[2];const auto argument=[&](unsigned offset=4){return load<int>(d.script,offset);};
        switch(op){
        case 0:return -1;
        case 1:{const auto index=argument();auto& handle=slot(d.portraits,index);
            if(index==0){auto& player=*static_cast<player_entity::Player*>(game_session::context(0).objects_04[0]);const auto character=game_session::context(0).current_player->fields_00[2];if(character>=2)throw std::out_of_range("Dialogue original two-character script table");handle=spawn(*player.animation_file,character?77:66);immediate(handle,index+17+(signed_word(game_session::context(0).current_player->fields_00[3])/2)*10);}
            else handle=stage_portrait(0x8c,index);break;}
        case 2:{const auto index=argument();slot(d.portrait_overlays,index)=stage_portrait(0x8c,index);d.fields_138[0]=0;break;}
        case 4:{auto& handle=slot(d.portraits,argument());signal(handle,1);handle=0;break;}
        case 5:{auto& handle=slot(d.portrait_overlays,argument());signal(handle,1);handle=0;signal(d.handles[5],1);break;}
        case 6:clear_dialogue_text(d,false);break;
        case 7:case 8:{const auto selected=argument();for(int i=0;i<4;++i){auto& active=op==7?d.portraits:d.portrait_overlays;auto& inactive=op==7?d.portrait_overlays:d.portraits;immediate(inactive[i],3);immediate(active[i],i==selected?2:3);}signal(d.handles[0],op==7?2:3);d.fields_108[3]=op==7?0:1;reset_text_state(d);break;}
        case 9:for(int i=0;i<4;++i){immediate(d.portraits[i],3);immediate(d.portrait_overlays[i],3);}signal(d.handles[0],3);d.fields_108[3]=2;reset_text_state(d);break;
        case 10:d.flags=(d.flags&~1u)|(static_cast<unsigned>(d.script[4])&1u);break;
        case 11:{
            if(d.timers[2].current<1)recovered::timer_set(d.timers[2],argument());recovered::timer_add(d.timers[2],-1,state::timer_rate);
            if((!pressed(0x80001)&&d.timers[2].current>0)||signed_word(d.fields_108[2])>0){
                if((d.flags&0x41u)!=0x41u||(held(9)<20&&held(0)<20)){position_dialogue_text(d);return 0;}
                recovered::timer_set(d.timers[2],0);d.fields_108[0]=d.fields_108[1]=0;
            }else{if(d.timers[2].current<1)d.fields_108[2]=40;program_entry::thread_registry.request_effect(0,0);recovered::timer_set(d.timers[2],0);d.fields_108[0]=d.fields_108[1]=0;}
            break;}
        case 12:d.field_100=1;break;
        case 13:immediate(slot(d.portraits,argument(8)),argument()+17+(signed_word(game_session::context(0).current_player->fields_00[3])/2)*10);break;
        case 14:immediate(slot(d.portrait_overlays,argument(8)),argument()+17);break;
        case 15:queue_dialogue_text(d,false);break;
        case 16:queue_dialogue_text(d,true);break;
        case 17:queue_dialogue_line(d);break;
        case 18:clear_dialogue_text(d,true);break;
        case 19:play_stage_track(1,stage_field(0x5c));spawn(*controller->stage_file,2);gameplay::player_state::set_table_counter(game_session::session.player_table,0x1f0,0,999999999);break;
        case 20:d.handles[5]=stage_portrait(0x84,argument());boss_name(*controller);break;
        case 21:(void)unrecovered::create_scene_005115c0(0);break;
        case 22:fade_stage_track(gameplay::player_state::stage(game_session::session.player_table)==6?2.f:8.f);break;
        case 23:signal(d.portraits[0],7);signal(d.portraits[1],7);break;
        case 24:signal(d.portrait_overlays[0],7);signal(d.portrait_overlays[1],7);break;
        case 25:for(unsigned i=1;i<=4;++i)sprite::resolve_animation_handle(sprites(),d.handles[i])->base.vector_484.y=recovered::int_float(argument());break;
        case 26:d.flags|=2u;break;
        case 27:fade_stage_track(load<float>(d.script,4));break;
        case 28:d.vector_128.x=recovered::mul32(load<float>(d.script,4),2);d.vector_128.y=recovered::mul32(load<float>(d.script,8),2);break;
        case 29:d.flags=(d.flags&~0x3cu)|((static_cast<unsigned>(argument())&15u)<<2);break;
        case 31:d.portrait_overlays[1]=stage_portrait(0x8c,1);d.fields_138[0]=0;break;
        case 32:signal(d.handles[0],3);d.fields_108[3]=argument();reset_text_state(d);break;
        case 33:case 34:immediate(slot(argument()==0?d.portraits:d.portrait_overlays,argument(8)),op==33?3:2);break;
        case 35:controller->handles_f8[3]=sprite::spawn_named_animation(sprites(),*controller->front_file,"front",48);break;
        case 36:gameplay::controller->game_flags|=0x20000u;d.script+=4+d.script[3];return 0;
        default:break; //original jump-table defaults include3/30 andcodes>36
        }
        d.script+=4+d.script[3];
    }
    recovered::timer_tick(d.timers[1],state::timer_rate);position_dialogue_text(d);return 0;
}
void start_dialogue(FrontInf& o,int index){
    if(index==-1||index==-3){const int track=index==-1?1:0;
        if(game_session::mode()==2&&gameplay::controller->restart_mode==0){std::string path(gameplay::selected_stage->tracks[track]);path+=".wav";if(path==program_entry::thread_registry.current_track)return;}
        play_stage_track(track,stage_field(0x58+track*4));spawn(*controller->stage_file,track+1);
    }else if(index==-2){if(load<unsigned>(game_session::context(0).objects_04[3],0x78)&0x80u)unrecovered::finish_spell_004e5bd0();else unrecovered::complete_stage_004bc570();}
    else{if(o.collecting){destroy_dialogue(o.collecting);o.collecting=nullptr;}const auto offset=load<unsigned>(o.message_data,static_cast<unsigned>(index)*8u+4);o.collecting=create_dialogue(o.message_data+offset);o.collecting->state=index;}
}
}
