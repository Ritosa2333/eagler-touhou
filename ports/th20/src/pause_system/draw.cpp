#include "draw.hpp"
#include "draw_constants.hpp"
#include "menu_support.hpp"
#include "../sprite_renderer/menu_animation.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../progress_state/manager.hpp"
#include "../gameplay/player_state.hpp"
#include "../program_entry/program_entry.hpp"
namespace th20::source::pause {
namespace d=draw_data;
int draw(PauseInf& o){
    auto& renderer=*text::renderer;auto& sprites=*program_entry::sprite_controller;renderer.fields_1a1d4[2]=1;
    if(sprite::resolve_animation_handle(sprites,o.background_handle))if(auto* child=sprite::find_animation_child(sprites,o.background_handle,92,0))sprite::set_animation_color(*program_entry::graphics_state.surface_sprites[2],child->base.field_490|0xff000000);
    if(o.state==1||o.state==3){if((o.menu_flags&3)==1)draw_replay_slots(o);else if((o.menu_flags&3)==2)draw_replay_name(o);}
    else if(o.state==2){
        if(o.substate==15)draw_score_ranking(o);
        if((o.menu_flags&3)==1)draw_replay_slots(o);else if((o.menu_flags&3)==2)draw_replay_name(o);
        else if(o.substate!=14&&o.substate!=16&&game_session::mode()!=2)renderer.write_ascii_format({d::f_0057265c,d::f_0056d7c0,0},d::s_005725f8,credits(game_session::session));
        auto& table=game_session::session.player_table;
        if(gameplay::player_state::stage(table)>=4&&gameplay::player_state::stage(table)<=6&&game_session::mode()==0&&o.substate==6&&gameplay::controller->restart()==0){
            const auto index=gameplay::player_state::read<std::uint32_t>(*game_session::context(0).current_player,0xc);
            if(progress::manager->stone_count(index)<9){reset_pause_text(renderer);renderer.color=0xffffff80;renderer.fields_1a1d4[2]=1;renderer.write_text_literal({d::f_0056fa28,d::f_00572660,0},d::s_00572604);reset_pause_text(renderer);}
        }
    }
    renderer.fields_1a1d4[2]=0;return 1;
}
}
