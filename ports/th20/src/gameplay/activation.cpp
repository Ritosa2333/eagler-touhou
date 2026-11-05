#include "frame.hpp"
#include "subsystems.hpp"
#include "loading_dependencies.hpp"
#include "stage_data.hpp"
#include "enemy.hpp"
#include "enemy_spawn.hpp"
#include "../program_entry/program_entry.hpp"
#include "../bullet_system/stage_reset.hpp"
#include "../player_entity/owner.hpp"
#include "../item_system/item.hpp"
#include "../laser_system/laser.hpp"
#include "../stage_background/background.hpp"
#include "../screen_effect/effect.hpp"
#include "../hud_system/dialogue.hpp"
#include "../text_renderer/text.hpp"
#include "../stone_menu/stone.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../sprite_renderer/render_mesh.hpp"
namespace th20::source::gameplay {
namespace pe=program_entry;
namespace {
void enable(void* value){static_cast<runtime::CallbackOwner*>(value)->enable_callbacks();}
void interrupt(unsigned handle){sprite::interrupt_animation_children(*pe::sprite_controller,handle,1);}
void reset_entities(){
    bullet::reset_for_stage(*bullet::controller());player_entity::reset_for_stage(*static_cast<player_entity::Player*>(game_session::context(0).objects_04[0]));
    item::controller()->initialize_pool();enemy_controller().clear_entities();laser::controller()->clear();
}
void spawn_main(){SpawnParameters parameters;construct_spawn_parameters(parameters);spawn_enemy(enemy_controller(),"main",parameters,nullptr);}
void enable_objects(bool initial){
    hud::controller->enable_callbacks();unrecovered::owner(Owner::global_005c60bc)->enable_callbacks();auto& c=game_session::context(0);
    if(initial)enable(c.object_28);
    enable(c.objects_04[0]);enable(c.primary_owner);enable(c.objects_04[1]);enable(c.objects_04[2]);enable(c.objects_04[4]);
    //4ba940/4bbf90 place EffectInf before/after Bomb, SmallScore and Spell.
    if(!initial)enable(c.objects_04[7]);
    enable(c.objects_04[5]);enable(c.objects_04[6]);enable(c.objects_04[3]);
    if(initial)enable(c.objects_04[7]);
}
void hide_loading(){auto& handle=text::renderer->loading_handle;interrupt(handle);if(auto* animation=sprite::find_animation(*pe::sprite_controller,handle))sprite::hide_animation_tree(*animation);sprite::request_animation_deletion(*pe::sprite_controller,handle);}
}
int start_game_frame(GameController& game){
    game.game_flags|=0x04000000u;
    if(game.game_flags&8u){runtime::sync_close_worker(game.services->worker());request_scene(3);return 1;}
    background::primary->enable_callbacks();
    if(background::secondary){
        screen::create_effect(2,30,0,0,0,10);recovered::timer_set(background::secondary->fade_timer,30);background::secondary->state_flags|=2u;
        recovered::timer_set(background::primary->fade_timer,60);background::primary->state_flags|=4u;game.game_flags|=0x1000u;interrupt(hud::controller->notice_handles[0]);return 0;
    }
    game.game_flags&=~0x1000u;reset_entities();auto& table=game_session::session.player_table;player_state::set_table_counter(table,0x1ec,0,999999999);player_state::set_table_counter(table,0x1f0,0,999999999);
    if(auto* replay=unrecovered::owner(Owner::global_005c60fc))replay->enable_callbacks();spawn_main();enable_objects(true);
    if(game_session::mode()!=2&&!(game_session::flags()&0x20u))hud::play_stage_track(0,selected_stage->fields_58[0]);
    game_session::set_flag0(game_session::session,0);game_session::set_flag1(game_session::session,0);interrupt(text::renderer->animation_handle);hide_loading();interrupt(pe::graphics_state.unknown_01c4);
    game_session::overlay_owner()->enable_callbacks();stone_menu::controller->enable_callbacks();return 0;
}
void complete_background_transition(GameController& game){
    if(!(game.game_flags&0x1000u))return;
    game.game_flags&=~0x1000u;reset_entities();unrecovered::owner(Owner::global_005c60fc)->enable_callbacks();spawn_main();enable_objects(false);
    game_session::overlay_owner()->enable_callbacks();stone_menu::controller->enable_callbacks();game.services->stop_music();hud::play_stage_track(0,selected_stage->fields_58[0]);hide_loading();recovered::timer_set(game.frame_timer,0);
}
}
