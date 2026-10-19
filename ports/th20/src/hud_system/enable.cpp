#include "hud.hpp"
#include "../program_entry/program_entry.hpp"
#include "../gameplay/gameplay.hpp"
#include "../gameplay/player_state.hpp"
#include "../player_entity/owner.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../stone_menu/update.hpp"
#include <algorithm>
#include <bit>
namespace th20::source::hud {
namespace ps=gameplay::player_state;
namespace {
int clamp_player(std::size_t offset,int lower,int upper){auto& player=*game_session::context(0).current_player;const auto value=std::clamp(ps::read<int>(player,offset),lower,upper);ps::write(player,offset,value);return value;}
}
void FrontInf::enable_callbacks(){
    runtime::CallbackOwner::enable_callbacks();if(secondary_draw)scheduler::enable(*secondary_draw);
    auto& c=environment::sprites();
    if(background_handle==0)sprite::spawn_named_animation(c,*front_file,background_handle,"front",0,nullptr,0,-1,4);
    if(life_animations[0]==nullptr){
        for(unsigned i=0;i<7;++i)sprite::spawn_named_animation(c,*front_file,life_handles[i],"front",i+32,nullptr,0,-1,4,&life_animations[i]);
        for(unsigned i=0;i<7;++i)sprite::spawn_named_animation(c,*front_file,bomb_handles[i],"front",i+40,nullptr,0,-1,4,&bomb_animations[i]);
        for(unsigned i=0;i<2;++i){number_handles[i]=sprite::spawn_named_animation(c,environment::notice_file(),nullptr,i+2,-1,&number_animations[i]);sprite::hide_animation_tree(*number_animations[i]);number_animations[i]->base.flags[2]&=~0x03000000u;}
    }
    const int lives_max=clamp_player(0xbc,0,7),life_fragments=clamp_player(0xc0,0,10),lives=clamp_player(0xb8,-1,7);
    set_lives(*this,lives,life_fragments,lives_max);
    const int bombs_max=clamp_player(0xd8,0,7),bomb_fragments=clamp_player(0xd0,0,10),bombs=clamp_player(0xcc,0,10);
    set_bombs(*this,bombs,bomb_fragments,bombs_max);
    const bool practice=(game_session::flags()&0x20u)!=0;
    if(program_entry::graphics_state.field_0b0c!=8&&!practice&&game_session::mode()!=2)sprite::spawn_named_animation(c,*stage_file,nullptr,1);
    if(practice)sprite::spawn_named_animation(c,*front_file,"front",110);
    if(handle_90==0)handle_90=sprite::spawn_named_animation(c,*front_file,"front",100);
    bool initial_stage=false;
    if(ps::stage(game_session::session.player_table)==1&&gameplay::controller->restart_mode==0){auto& count=game_session::session.player_table.continue_count;count=std::clamp(count,0,9);initial_stage=count==0;}
    if(initial_stage||(flags&0x2000u)){
        const auto handle=sprite::spawn_named_animation(c,*front_file,"front",57);
        const auto& player=*static_cast<player_entity::Player*>(game_session::context(0).objects_04[0]);
        if(auto* a=sprite::find_animation(c,handle))a->vector_5bc={0,recovered::add32(recovered::mul32(std::bit_cast<float>(player.fields_2080[4]),2),-80),0};
        flags&=~0x2000u;
    }
    const int difficulty=ps::difficulty(game_session::session.player_table);
    if(program_entry::graphics_state.field_0b18){handles_f8[0]=sprite::spawn_named_animation(c,*front_file,"front",difficulty+69);sprite::interrupt_animation_children(c,handles_f8[0],3);stone_menu::open(*stone_menu::controller,1);}
    handles_f8[1]=sprite::spawn_named_animation(c,*front_file,"front",difficulty+75);
    handles_f8[2]=sprite::spawn_named_animation(c,*front_file,"front",game_session::context(0).current_player->fields_00[2]+101);
    fields_174[3]=0;for(auto& panel:panels)panel.field_4c=0;
    if(handles_f8[3]){sprite::interrupt_animation_children(c,handles_f8[3],1);handles_f8[3]=0;}
}
}
