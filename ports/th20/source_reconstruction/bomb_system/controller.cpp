#include "bomb.hpp"
#include "../program_entry/program_entry.hpp"
#include "../runtime_state/state.hpp"
#include "../gameplay/enemy.hpp"
#include "../gameplay/enemy_frame.hpp"
#include <cstring>
#include <new>
namespace th20::source::bomb {
namespace pe=program_entry;
namespace {int __cdecl update_callback(void* value){return static_cast<Controller*>(value)->update();}
int __cdecl draw_callback(void* value){return static_cast<Controller*>(value)->draw();}}
Controller::Controller():field_10(0),active_bomb(nullptr),active_state(0),timer{},field_2c(0),field_30(0),view_index(0),context(nullptr){}
Controller::~Controller(){
    retire_bomb(active_bomb);active_bomb=nullptr;
    scheduler::remove(*pe::function_controller,pe::scheduler_environment,update_node);
    scheduler::remove(*pe::function_controller,pe::scheduler_environment,draw_node);
}
void Controller::select_context(std::int32_t index) noexcept {view_index=index;context=&game_session::context(index);}
int Controller::initialize(std::int32_t index){
    update_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,33,&update_callback,this,false,true);
    draw_node=scheduler::register_callback(*pe::function_controller,pe::scheduler_environment,44,&draw_callback,this,true,true);
    recovered::timer_set(timer,0);active_bomb=nullptr;select_context(index);return 0;
}
int Controller::update(){
    pe::sprite_controller->field_6c4=static_cast<std::uint32_t>(view_index);
    if(active_bomb){if(active())mark_enemies_for_bomb(gameplay::enemy_controller(0));if(active_bomb->update())finish();}
    recovered::timer_tick(timer,state::timer_rate);return 1;
}
int Controller::draw(){if(active_bomb)active_bomb->draw();return 1;}
int Controller::event(std::uint32_t a,std::uint32_t b){if(active_bomb)active_bomb->event(a,b);return 0;}
int Controller::finish(){return active_bomb?active_bomb->finish():0;}
bool Controller::can_trigger(){
    if(bomb_count(*context->current_player)<=0||active())return false;
    const auto* hud=gameplay::unrecovered::boss_hud_005c06a4();if(!hud)return false;
    std::uint32_t state;std::memcpy(&state,static_cast<const std::uint8_t*>(hud)+0x1bc,4);if(state)return false;
    const auto* enemy=static_cast<gameplay::EnemyController*>(context->objects_04[1]);return enemy&&enemy->field_124!=0;
}
Controller* controller(std::int32_t index) noexcept {return static_cast<Controller*>(game_session::context(index).objects_04[5]);}
Controller* create_controller(std::int32_t index){
    auto* memory=::operator new(sizeof(Controller),std::nothrow);if(!memory)return nullptr;
    std::memset(memory,0,sizeof(Controller));auto* value=new(memory)Controller;
    if(value->initialize(index)){runtime::retire_callback_owner(value);return nullptr;}
    game_session::context(index).objects_04[5]=value;return value;
}
void destroy_controller(std::int32_t index){auto& value=game_session::context(index).objects_04[5];runtime::retire_callback_owner(static_cast<Controller*>(value));value=nullptr;}
}
