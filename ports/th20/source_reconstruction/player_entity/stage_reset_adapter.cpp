#include "stage_reset.hpp"
#include "power.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
namespace th20::source::player_entity {
namespace {
class GameStageReset final:public StageResetServices {
public:
    game_session::Session& session() override{return game_session::session;}
    void refresh_power(Player& player) override{player_entity::refresh_power(player,-1);}
    void interrupt(std::uint32_t handle,int event) override{sprite::interrupt_animation_children(*program_entry::sprite_controller,handle,event);}
    void delete_animation(std::uint32_t& handle) override{sprite::request_animation_deletion(*program_entry::sprite_controller,handle);}
    void hide_animation(sprite::Animation& animation) override{sprite::hide_animation_tree(animation);}
};
}
StageResetServices& stage_reset_services(){static GameStageReset result;return result;}
void reset_for_stage(Player& player){reset_for_stage(player,stage_reset_services());}
void finish_stage_visibility(Player& player){finish_stage_visibility(player,stage_reset_services());}
void restore_stage_visibility(Player& player){restore_stage_visibility(player,stage_reset_services());}
}
