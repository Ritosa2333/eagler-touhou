#include "enemy_opcode_laser.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/pool.hpp"
namespace th20::source::gameplay {
namespace {
struct Host final:EnemyLaserOpcodeServices {
    EnemyShotOpcodeServices& shots() override{return enemy_shot_opcode_services();}
    void create(game_session::Context& c,unsigned kind,const void* p) override{laser::spawn_laser(*static_cast<laser::Controller*>(c.objects_04[4]),kind,p);}
    laser::Laser* find(game_session::Context& c,int id) override{auto& owner=*static_cast<laser::Controller*>(c.objects_04[4]);for(scheduler::Iterator it(owner.active.sentinel.next);it.current;it.advance()){auto* value=reinterpret_cast<laser::Laser*>(it.current->value);if(value->handle==unsigned(id))return value;}return nullptr;} //498ff0
    float animation_angle(std::uint32_t& handle) override{auto* a=sprite::resolve_animation_handle(*program_entry::sprite_controller,handle);if(!a)throw std::logic_error("Enemy laser rectangle requires its animation");return a->base.vector_38.z;}
    void cancel_rectangle(game_session::Context& c,const sprite::Vec3& p,const sprite::Vec3& size,float angle) override{bullet::cancel_rectangle(*static_cast<bullet::Controller*>(c.primary_owner),p,size,angle,1,0);}
};
}
EnemyLaserOpcodeServices& enemy_laser_opcode_services(){static Host host;return host;}
EnemyOpcodeResult execute_enemy_laser_opcode(EnemyOpcodeReader& r){return execute_enemy_laser_opcode(r,enemy_laser_opcode_services());}
}
