#include "power.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/render_mesh.hpp"
namespace th20::source::player_entity {
namespace {
class GamePowerServices final:public PowerServices {
public:
    void select_view(int index) override{program_entry::sprite_controller->field_6c4=index;}
    Player& global_player() override{return *static_cast<Player*>(game_session::context(0).objects_04[0]);}
    void interrupt(std::uint32_t handle,int event) override{sprite::interrupt_animation_children(*program_entry::sprite_controller,handle,event);}
    void delete_animation(std::uint32_t& handle) override{sprite::request_animation_deletion(*program_entry::sprite_controller,handle);}
    sprite::Animation& animation(std::uint32_t& handle) override{return *sprite::resolve_animation_handle(*program_entry::sprite_controller,handle);}
    int script_variant() override{return unrecovered::overlay_script_variant_00534130(*game_session::context(0).overlay_owner);}
    sprite::Vec2 option_offset(game_session::Context& context,int level,int index,bool focus) override{return focus?unrecovered::overlay_option_offset_004ff760(*context.overlay_owner,level,index):unrecovered::overlay_option_offset_004ff630(*context.overlay_owner,level,index);}
    std::uint32_t spawn(sprite::AnimationFile& file,int script,int layer,std::uint32_t flags) override{std::uint32_t handle;sprite::spawn_named_animation(*program_entry::sprite_controller,file,handle,nullptr,script,nullptr,0,layer,flags);return handle;}
    void initialize_option(Option& option,int index) override{unrecovered::overlay_initialize_option_00533180(*game_session::context(0).overlay_owner,option,index);}
};
}
PowerServices& power_services(){static GamePowerServices services;return services;}
}
namespace th20::source::item::unrecovered {
void refresh_player_power_004faca0(runtime::CallbackOwner& player,int previous){player_entity::refresh_power(static_cast<player_entity::Player&>(player),previous);}
}
