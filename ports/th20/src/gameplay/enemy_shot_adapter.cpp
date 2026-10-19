#include "enemy_shot.hpp"
#include "enemy_variables.hpp"
#include "../program_entry/program_entry.hpp"
#include "../runtime_core/runtime_core.hpp"
#include "../bullet_system/shoot.hpp"
#include "../bullet_system/player_cancellation.hpp"
#include "../laser_system/laser.hpp"
#include "../player_entity/player.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../stage_background/background.hpp"
#include "../damage_regions/damage.hpp"
#include "../damage_regions/geometry.hpp"
#include "../small_score/score.hpp"
#include <bit>
namespace th20::source::gameplay {
namespace {
game_session::Context& context(EnemyState& s){return *reinterpret_cast<game_session::Context*>(s.fields_2c8[9]);}
bullet::Controller& bullets(game_session::Context& c){return *static_cast<bullet::Controller*>(c.primary_owner);}
laser::Controller& lasers(game_session::Context& c){return *static_cast<laser::Controller*>(c.objects_04[4]);}
void player_rate(float value){std::memcpy(static_cast<std::uint8_t*>(game_session::context(0).objects_04[0])+0x20ec,&value,4);} //479080, actual global0 target
int __fastcall slow_in_rectangle(EnemyState* s,void*){ //488b80, explicit original EAX=0 at488e5b
    auto& c=context(*s);auto* a=sprite::resolve_animation_handle(*program_entry::sprite_controller,s->animations.at(0).handle);
    if(!a)throw std::logic_error("Slowdown callback requires its actual Enemy animation");
    const float width=a->base.vector_50.x,height=a->base.vector_50.y,angle=a->base.vector_38.z;const auto origin=enemy_position(s->entity);const float rate=std::bit_cast<float>(s->fields_78[4]);
    for(scheduler::Iterator it(bullets(c).active.sentinel.next);it.current;it.advance()){
        auto& b=*reinterpret_cast<bullet::Bullet*>(it.current->value);
        if(geometry::rectangle_circle(origin.x,origin.y,width,height,angle,b.position.x,b.position.y,6.f)){bullet::assign_timer_float(b.timer_4d8,rate);b.animation->base.field_490=0xffa05000u;b.field_18=10;}
    }
    const auto player=player_entity::position(c.objects_04[0]);if(geometry::rectangle_circle(origin.x,origin.y,width,height,angle,player.x,player.y,10.f))player_rate(rate);return 0;
}
int __fastcall reset_slowdown(EnemyState* s,void*){ //488e80, explicit original EAX=0 at488f88
    for(scheduler::Iterator it(bullets(context(*s)).active.sentinel.next);it.current;it.advance()){
        auto& b=*reinterpret_cast<bullet::Bullet*>(it.current->value);bullet::assign_timer_float(b.timer_4d8,1.f);b.animation->base.field_490=0xffffffffu;b.field_18=0;
    }player_rate(1.f);return 0;
}
using Callback=int(__fastcall*)(EnemyState*,void*);
Callback callback_pointer(unsigned table,int index){ //immutable table56fe8c,56fe98,56fe9c
    static const Callback callbacks[]{nullptr,slow_in_rectangle,reset_slowdown};
    if(table==0){if(index<0||index>=3)throw std::out_of_range("Enemy callback table index");return callbacks[index];}
    if((table==1||table==2)&&index==0)return nullptr;
    throw std::out_of_range("Enemy empty callback table index");
}
struct Host final:EnemyShotOpcodeServices {
    std::shared_ptr<void> metadata(const bullet::ShotMetadata* copy) override{if(copy)return std::allocate_shared<bullet::ShotMetadata>(std::pmr::polymorphic_allocator<bullet::ShotMetadata>{},*copy);return std::allocate_shared<bullet::ShotMetadata>(std::pmr::polymorphic_allocator<bullet::ShotMetadata>{});}
    void fire(game_session::Context& c,const bullet::ShotParameters& p,const std::shared_ptr<void>& metadata,std::uint32_t sound) override{auto& b=bullets(c);b.field_48=std::bit_cast<float>(sound);bullet::shoot(b,p,std::shared_ptr<bullet::ShotMetadata>(metadata,static_cast<bullet::ShotMetadata*>(metadata.get())));b.field_48=0;}
    void cancel_all(game_session::Context& c) override{for(scheduler::Iterator it(bullets(c).active.sentinel.next);it.current;it.advance()){auto& b=*reinterpret_cast<bullet::Bullet*>(it.current->value);if(b.state!=0&&b.state!=3)bullet::cancel(b,0);}lasers(c).erase_all(1,0);}
    void cancel_circle(game_session::Context& c,const sprite::Vec3& p,float radius,bool nearby) override{if(nearby)bullet::cancel_near_circle(bullets(c),p,radius,0);else bullet::cancel_circle(bullets(c),p,radius,0,99999,0);lasers(c).cancel_circle(p,radius,0,1);}
    sprite::Vec3 player_position(game_session::Context& c) override{return player_entity::position(c.objects_04[0]);}
    void mesh(EnemyState& s,float radius,std::uint32_t color) override{ //48af30/48b010 and original621 setup
        auto*& owner=*reinterpret_cast<EnemyMeshOwner**>(&s.fields_2c8[3]);if(owner){sprite::destroy_render_mesh(owner->mesh);owner->mesh=nullptr;runtime::release_bytes(owner);owner=nullptr;}
        owner=static_cast<EnemyMeshOwner*>(runtime::allocate_bytes(sizeof(EnemyMeshOwner)));if(!owner)throw std::bad_alloc();*owner={};owner->radius=radius;owner->current_radius=16.f;owner->color=color;if(radius>0)owner->mesh=sprite::create_render_mesh(17,17,0,int(s.fields_2c8[8]));
    }
    void background_event(int event) override{ //4773c0, original script event16 search from start
        auto& b=*background::primary;auto* instruction=b.instructions;
        while(instruction->time>=0){std::int32_t value;std::memcpy(&value,instruction+1,4);if(instruction->opcode==16&&value==event){b.state.instruction_offset=unsigned(reinterpret_cast<std::uint8_t*>(instruction)-reinterpret_cast<std::uint8_t*>(b.instructions));recovered::timer_set(b.state.timer,instruction->time);return;}
            if(instruction->size<8)throw std::runtime_error("Invalid background event instruction size");instruction=reinterpret_cast<background::Instruction*>(reinterpret_cast<std::uint8_t*>(instruction)+instruction->size);
        }
    }
    unsigned callback(unsigned table,int index) override{return reinterpret_cast<unsigned>(callback_pointer(table,index));}
    void invoke_callback(EnemyState& s,int index) override{const auto fn=callback_pointer(0,index);if(!fn)throw std::logic_error("Original Enemy callback is null");fn(&s,nullptr);}
    void add_score(game_session::Context& c,const sprite::Vec3& p,int amount) override{damage::add_score(game_session::player(0),unsigned(amount));small_score::spawn(*static_cast<small_score::SmallScoreInf*>(c.objects_04[6]),p,amount,0xffffffffu);}
};
}
EnemyShotOpcodeServices& enemy_shot_opcode_services(){static Host host;return host;}
EnemyOpcodeResult execute_enemy_bullet_opcode(EnemyOpcodeReader& r){return execute_enemy_bullet_opcode(r,enemy_shot_opcode_services());}
}
