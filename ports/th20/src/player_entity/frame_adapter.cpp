#include "frame.hpp"
#include "../program_entry/program_entry.hpp"
#include "../bullet_system/player_cancellation.hpp"
#include "../laser_system/laser.hpp"
#include "../bomb_system/bomb.hpp"
#include "../damage_regions/regions.hpp"
#include "../item_system/item.hpp"
#include "../replay_system/replay.hpp"
#include "../pause_system/pause.hpp"
#include "../gameplay/loading_dependencies.hpp"
#include "../gameplay/enemy_interpolation.hpp"
#include "../runtime_state/state.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../input/input.hpp"
namespace th20::source::player_entity {
namespace {
struct GameFrame final:FrameServices {
    MovementServices& movement() override{return movement_services();}
    DeathServices& death() override{return death_services();}
    ShotControllerServices& shots() override{return shot_controller_services();}
    void select_view(int index) override{program_entry::sprite_controller->field_6c4=static_cast<std::uint32_t>(index);}
    std::uint32_t pressed(int slot,std::uint32_t mask) override{if(slot<0)return 0;const auto* buttons=input::button_slot(static_cast<unsigned>(slot));return buttons?buttons->retained_298[4]&mask:0;}
    bool bomb_exists(game_session::Context& context) override{return context.objects_04[5]!=nullptr;}
    bool can_bomb(game_session::Context& context) override{return static_cast<bomb::Controller*>(context.objects_04[5])->can_trigger();}
    void trigger_bomb(game_session::Context& context) override{static_cast<bomb::Controller*>(context.objects_04[5])->trigger();}
    void cancel_bullets(game_session::Context& context,const sprite::Vec3& position,float radius) override{bullet::cancel_circle(*static_cast<bullet::Controller*>(context.primary_owner),position,radius,0,99999,0);}
    void cancel_near_bullets(game_session::Context& context,const sprite::Vec3& position,float radius) override{bullet::cancel_near_circle(*static_cast<bullet::Controller*>(context.primary_owner),position,radius,0);}
    void cancel_lasers(game_session::Context& context,const sprite::Vec3& position,float radius,int kind) override{static_cast<laser::Controller*>(context.objects_04[4])->cancel_circle(position,radius,0,kind);}
    void finish_lasers(game_session::Context& context,int first,int second) override{static_cast<laser::Controller*>(context.objects_04[4])->erase_all(first,second);}
    void create_damage(game_session::Context& context,const sprite::Vec3& position,float radius,float growth,int duration,int amount,bool activate) override{auto handle=damage::create_circle(*static_cast<damage::HitCtrlInf*>(context.object_28),position,radius,growth,duration,amount);if(activate)damage::activate_handle(handle);}
    void spawn_power_item(const sprite::Vec3& position,float angle) override{item::spawn(*item::controller(0),1,position,0xffffffffu,angle,3,0,0,-1);}
    bool replay_playing() override{return replay::controller()->mode==1;}
    void finish_game() override{pause::finish_game(*pause::controller(),pause::services());}
    void set_bombs(game_session::Player& player,int count) override{gameplay::player_state::set_bombs(player,count,gameplay::unrecovered::bomb_observer());}
    void clock_scale(float value) override{state::set_clock_scale(value);}
    void execute_animation(sprite::Animation& animation) override{sprite::execute_animation(animation);}
    float sample_expansion(sprite::Interpolation<float>& interpolation) override{return gameplay::sample_enemy_scalar_interpolation(interpolation,state::timer_rate);}
};
}
FrameServices& frame_services(){static GameFrame result;return result;}
int update_player(Player& player){return update_player(player,frame_services());}
}
