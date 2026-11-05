#include "enemy_opcode_state.hpp"
#include "enemy_drop.hpp"
#include "enemy_damage.hpp"
#include "enemy_variables.hpp"
#include "../program_entry/program_entry.hpp"
#include "../audio_runtime/audio.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../card_system/card.hpp"
#include "../hud_system/dialogue.hpp"
#include "../bullet_system/bullet.hpp"
#include "../laser_system/laser.hpp"
#include "../screen_effect/effect.hpp"
#include "../runtime_state/state.hpp"
#include "../stage_background/fog.hpp"
namespace th20::source::gameplay {
namespace {
void show_animation_tree(sprite::Animation& a){ //44fa70
    a.base.flags[1]|=1u;
    scheduler::Iterator it(reinterpret_cast<scheduler::Link*>(a.links[3].next));
    while(it.current){show_animation_tree(*reinterpret_cast<sprite::Animation*>(it.current->value));it.advance();}
}
class Source final:public EnemyStateOpcodeServices {
    game_session::Session& session() override{return game_session::session;}
    GameController& game() override{return *controller;}
    hud::FrontInf& hud() override{return *hud::controller;}
    void visibility(std::uint32_t handle,bool visible) override{if(auto* a=sprite::resolve_animation_handle(*program_entry::sprite_controller,handle)){if(visible)show_animation_tree(*a);else sprite::hide_animation_tree(*a);}}
    void drop(EnemyPatternState& p,const sprite::Vec3& position,bool bomb) override{emit_enemy_drop(p,position,bomb);}
    void sound(int effect,float x) override{program_entry::thread_registry.request_effect(effect,x);}
    void shake(int duration,int a,int b) override{screen::create_effect(1,duration,unsigned(a),unsigned(b),0,0,0);}
    void dialogue(int index) override{hud::start_dialogue(*hud::controller,index);}
    void cancel_bullets(game_session::Context& context) override{ //47cb90
        auto& owner=*static_cast<bullet::Controller*>(context.primary_owner);
        for(scheduler::Iterator it(owner.active.sentinel.next);it.current;it.advance()){
            auto& b=*reinterpret_cast<bullet::Bullet*>(it.current->value);if(b.state!=0&&b.state!=3)bullet::cancel(b,0);
        }
    }
    void erase_lasers(game_session::Context& context,bool reset) override{
        auto& owner=*static_cast<laser::Controller*>(context.objects_04[4]);
        if(!reset){owner.erase_all(0,0);return;}
        //4d2430 processes every laser, including state1, before virtual erase.
        for(scheduler::Iterator it(owner.active.sentinel.next);it.current;it.advance()){
            auto& l=*reinterpret_cast<laser::Laser*>(it.current->value);recovered::timer_set(l.timer_6ac,0);l.field_6cc=0;l.erase(1,0);
        }
    }
    void clear(EnemyController& c,EnemyClearKind kind,int group) override{clear_enemy_group(c,kind,group);}
    void card_start(game_session::Context& c,int id,const char* name,int duration,int portrait) override{card::start(*static_cast<card::CardInf*>(c.objects_04[3]),id,name,duration,portrait);}
    void card_finish(game_session::Context& c) override{card::finish(*static_cast<card::CardInf*>(c.objects_04[3]));}
    void delete_animation(std::uint32_t& h) override{sprite::request_animation_deletion(*program_entry::sprite_controller,h);}
    void clock_scale(float value) override{state::set_clock_scale(value);}
    void stage_title() override{ //4b96f0
        if(program_entry::graphics_state.field_0b0c!=8&&!(game_session::session.flags&32u))sprite::spawn_named_animation(*program_entry::sprite_controller,*hud::controller->stage_file,nullptr,0);
    }
    void fog(int duration,int mode,std::uint32_t color,float near_distance,float far_distance) override{ //49c5b0->4776c0
        auto& state=background::primary->state;auto& p=state.fog_interpolation;
        p.duration=duration;p.mode=mode;std::memcpy(&p.start,state.camera.final_state,sizeof(p.start));
        p.end=background::make_fog(near_distance,far_distance,float(color&255),float((color>>8)&255),float((color>>16)&255),float(color>>24));recovered::timer_set(p.timer,0);
    }
    void death_effect(EnemyController& c,unsigned file,int script,const sprite::Vec3& position,float angle) override{
        std::uint32_t handle;sprite::spawn_named_animation(*program_entry::sprite_controller,*c.services->existing_animation(c,file),handle,nullptr,script,&position,angle,3,0);
    }
    int defeat(Enemy& enemy) override{return unrecovered::defeat_enemy_004a5640(&enemy);}
    Enemy* selected(EnemyController& c,unsigned index) override{return static_cast<Enemy*>(selected_enemy(&c,index));}
} source;
}
EnemyStateOpcodeServices& enemy_state_opcode_services(){return source;}
EnemyOpcodeResult execute_enemy_state_opcode(EnemyOpcodeReader& r){return execute_enemy_state_opcode(r,source);}
}
