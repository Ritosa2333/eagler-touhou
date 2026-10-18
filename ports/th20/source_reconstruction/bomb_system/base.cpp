#include "bomb.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../gameplay/enemy.hpp"
#include <cstring>
namespace th20::source::bomb {
Bomb::Bomb():field_04(0),timer{},secondary_timer{},motion{},handle_70(0),handle_74(0),field_78(0),field_7c(0),interpolation{},handle_ac(0),view_index(0),context(nullptr){}
Bomb::~Bomb(){
    sprite::request_animation_deletion(*program_entry::sprite_controller,handle_70);
    sprite::request_animation_deletion(*program_entry::sprite_controller,handle_74);
    // Original explicitly resets Context0 even for a Bomb bound to Context1.
    controller(0)->active_bomb=nullptr;controller(0)->active_state=0;
}
void Bomb::select_context(std::int32_t index) noexcept {view_index=index;context=&game_session::context(index);}
int Bomb::start(std::int32_t index){
    select_context(index);recovered::timer_set(timer,0);handle_ac=0;
    notify_bomb_start(context->objects_04[3]);
    add_enemy_bomb_counter(*static_cast<gameplay::EnemyController*>(context->objects_04[1]),1);
    std::memcpy(&motion.position,static_cast<const std::uint8_t*>(context->objects_04[0])+0x614,sizeof(motion.position));
    return 0;
}
int Bomb::update(){return 0;}
int Bomb::draw(){return 0;}
int Bomb::finish(){return 0;}
int Bomb::event(std::uint32_t,std::uint32_t){return 0;}
void retire_bomb(Bomb* value){if(!value)return;value->~Bomb();std::lock_guard<std::recursive_mutex> lock(runtime::shared_locks().slot(1));::operator delete(value);}
}
