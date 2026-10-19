#include "enemy_defeat.hpp"
#include "enemy_drop.hpp"
#include "../program_entry/program_entry.hpp"
#include "../audio_runtime/audio.hpp"
#include "../runtime_state/state.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../player_entity/shot_hit.hpp"
#include "../effect_system/effect.hpp"
#include "../item_system/rewards.hpp"
#include "../bomb_system/bomb.hpp"
#include "../special_state/special.hpp"
#include "../damage_regions/hit_callbacks.hpp"
namespace th20::source::gameplay {
namespace {struct Host final:EnemyDefeatServices {
    game_session::Session& session()override{return game_session::session;}
    const float* timer_rate()override{return state::timer_rate;}
    void sound(int id,float x)override{program_entry::thread_registry.request_effect_at(id,x);}
    void effect(Enemy& e,unsigned file,int script,const sprite::Vec3& p,float angle)override{auto& c=*static_cast<EnemyController*>(e.context->objects_04[1]);auto& f=*c.services->existing_animation(c,file);unsigned handle;sprite::spawn_named_animation(*program_entry::sprite_controller,f,handle,nullptr,script,&p,angle,-1,0,nullptr);player_entity::remember_shot_effect(*static_cast<effects::Controller*>(e.context->objects_04[7]),handle,player_entity::shot_callback_environment());}
    void drop(EnemyPatternState& p,const sprite::Vec3& v,bool bomb)override{emit_enemy_drop(p,v,bomb);}
    bool special_active()override{return special_state::is_active(*special_state::controller);}
    unsigned random()override{return state::next(state::random_streams[0]);}
    void special_items(game_session::Player& p,int amount)override{item::add_special_items(p,amount);}
    void meter(game_session::Player& p,int amount)override{bomb::add_player_meter(p,amount);}
    void counter(game_session::Player& p,unsigned stone,int amount)override{const unsigned index=stone==2||stone==3?1:stone==4||stone==5?2:stone==6||stone==7?3:0;item::add_special_counter(p,index,amount);}
    void reward(runtime::CallbackOwner* o,const sprite::Vec3& p,int amount,int type)override{damage::accumulate_damage_reward(o,p,amount,type);}
    void death_script(Enemy& e,int index)override{e.clear_async();e.reset();e.current->subroutine=index;e.current->instruction_offset=0;e.current->time=0;auto random=state::stream(state::random_streams[0]);EnemyVmEnvironment env{&random,state::timer_rate};e.tick(0,env);}
};}
EnemyDefeatServices& enemy_defeat_services(){static Host host;return host;}
namespace unrecovered {int defeat_enemy_004a5640(void* e){return defeat_enemy(*static_cast<Enemy*>(e));}}
}
