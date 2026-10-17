#include "initialize.hpp"
#include "power.hpp"
#include "shot_data.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/animation_file.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/dispatch.hpp"
#include "../damage_regions/regions.hpp"
#include "../item_system/collect.hpp"
#ifdef TH_SDL3
#include "../../../th20_web/cpp/sdl/HitboxDisplay.hpp"
#endif
namespace th20::source::player_entity {
namespace {
class GameInitializationServices final:public InitializationServices {
public:
    game_session::Session& session() override{return game_session::session;}
    void select_view(int index) override{program_entry::sprite_controller->field_6c4=index;}
    sprite::AnimationFile* load_animation(int index,const char* name) override{return sprite::load_animation_file(*program_entry::sprite_controller,index,name,program_entry::log_buffer,program_entry::graphics_event_flags);}
    void* load_shots(const char* name) override{return load_shot_data(name);}
    void resource_error() override{runtime::log_printf(program_entry::log_buffer,"\x83\x66\x81\x5b\x83\x5e\x82\xaa\x89\xf3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb7\r\n");}
    void bind_animation(sprite::AnimationFile& file,sprite::Animation& animation,int script) override{sprite::bind_animation_script(file,animation,script,nullptr);}
    std::uint32_t create_feedback_animation() override{
        auto* owner=item::unrecovered::special_owner_0051b960();auto* file=*reinterpret_cast<sprite::AnimationFile**>(reinterpret_cast<std::uint8_t*>(owner)+0x10);
        return sprite::spawn_named_animation(*program_entry::sprite_controller,*file,"stone",14,-1);
    }
    void hide_feedback_animation(std::uint32_t handle) override{if(auto* animation=sprite::find_animation(*program_entry::sprite_controller,handle))animation->base.vector_50={0,0};}
    void create_damage(int index) override{damage::create_controller(index);}
};
}
InitializationServices& initialization_services(){static GameInitializationServices services;return services;}
Player* create_player(int index){return create_player(index,player_services(),initialization_services());}
void destroy_player(int index){
#ifdef TH_SDL3
    web::clear_hitbox_display();
#endif
    auto& context=game_session::context(index);if(context.object_28){runtime::retire_callback_owner(static_cast<damage::HitCtrlInf*>(context.object_28));context.object_28=nullptr;}
    if(context.objects_04[0]){runtime::retire_callback_owner(static_cast<Player*>(context.objects_04[0]));context.objects_04[0]=nullptr;}
}
int draw_player(Player& player){
    if(player.state!=2){auto& controller=*program_entry::sprite_controller;sprite::configure_animation_layer(controller,12,player.view_index);
        player.animation.vector_5bc=player.position_614;player.animation.base.flags[2]=(player.animation.base.flags[2]&0xfcffffffu)|0x01000000u;
        sprite::draw_animation(controller,player.animation);
#ifdef TH_SDL3
        web::draw_always_hitbox(player);
#endif
        sprite::flush_textured_quads(controller,*program_entry::graphics_state.device);
    }
    return 1;
}
}
namespace th20::source::gameplay::unrecovered {
runtime::CallbackOwner* create_004ffff0(int index){return player_entity::create_player(index);}
void finish_entity_initialization(){player_entity::refresh_power(*static_cast<player_entity::Player*>(game_session::context(0).objects_04[0]),-1);}
}
