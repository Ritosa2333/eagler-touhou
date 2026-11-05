#include "events.hpp"
#include "../gameplay/enemy.hpp"
#include "../bomb_system/bomb.hpp"
#include "../effect_system/effect.hpp"
#include "../item_system/rewards.hpp"
#include "../damage_regions/hit_callbacks.hpp"
#include "../program_entry/program_entry.hpp"
#include "../audio_runtime/audio.hpp"
#include "../runtime_state/state.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include <cstring>
namespace th20::source::player_entity {
namespace {
template<class T>T get(const void* p,std::size_t offset){T value;std::memcpy(&value,static_cast<const std::uint8_t*>(p)+offset,sizeof(value));return value;}
class GameEvents final:public EventServices {
public:
    game_session::Session& session() override{return game_session::session;}
    void mark_enemies() override{bomb::mark_enemies_for_bomb(gameplay::enemy_controller(0));}
    void notify_secondary(void* owner) override{bomb::notify_bomb_start(owner);}
    void sound(int id) override{program_entry::thread_registry.request_effect(id,0);}
    void sound_at(int id,float x) override{program_entry::thread_registry.request_effect_at(id,x);}
    void spawn_hit_effect(game_session::Context& context,const sprite::Vec3& p) override{
        auto& effects=*static_cast<effects::Controller*>(context.objects_04[7]);std::uint32_t handle;
        sprite::spawn_named_animation(*program_entry::sprite_controller,*effects.files[0],handle,"effect",0x16,&p,0.0f,-1,0,nullptr);
    }
    void reset_player_animation(void* player) override{
        auto& file=*get<sprite::AnimationFile*>(player,0x1c);auto& animation=*reinterpret_cast<sprite::Animation*>(static_cast<std::uint8_t*>(player)+0x28);
        sprite::bind_animation_script(file,animation,0,nullptr);
    }
    std::uint32_t random_next() override{return state::next(state::random_streams[0]);}
    void enqueue_graze(game_session::Context& context,const sprite::Vec3& p,std::uint32_t color,int delay) override{
        effects::Parameters parameters;effects::construct_parameters(parameters);parameters.vector_00=p;parameters.value_20=color;
        static_cast<effects::Controller*>(context.objects_04[7])->enqueue(delay,4,&parameters,nullptr);
    }
    bool special_active() override{return get<std::uint8_t>(item::unrecovered::special_state_00513dd0(),0xb0)!=0;}
    bool selected_enemy_present() override{for(const auto handle:gameplay::enemy_controller(0).data.handles_44)if(handle)return true;return false;}
    void accumulate_reward(void* overlay,const sprite::Vec3& p,int amount,int type) override{damage::accumulate_damage_reward(overlay,p,amount,type);}
    void add_special_items(game_session::Player& player,int amount) override{item::add_special_items(player,amount);}
};
}
EventServices& event_services(){static GameEvents host;return host;}
namespace unrecovered {void player_hit_004f86f0(void* player){hit(player);}}
}
namespace th20::source::gameplay::unrecovered {
void player_graze_004f8b90(void* player,const sprite::Vec3& position,int color){player_entity::graze(player,position,static_cast<std::uint32_t>(color));}
}
