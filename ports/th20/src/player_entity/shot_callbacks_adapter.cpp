#include "shot_callbacks.hpp"
#include "../gameplay/enemy_variables.hpp"
#include "../runtime_state/state.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../damage_regions/regions.hpp"
#include "../overlay_system/overlay.hpp"
#include "../hud_system/hud.hpp"
#include "../audio_runtime/audio.hpp"
#include "../sprite_renderer/menu_animation.hpp"
#include "../bullet_system/bullet.hpp"
#include "../laser_system/laser.hpp"
#include <stdexcept>
namespace th20::source::player_entity {
namespace {
class GameShotCallbacks final:public ShotCallbackEnvironment {
public:
    FiringServices& firing() override{return firing_services();}
    scheduler::List& enemies(game_session::Context& context) override{return static_cast<gameplay::EnemyController*>(context.objects_04[1])->enemies;}
    bool excluded_enemy(const void* enemy) override{return gameplay::enemy_excluded(enemy);}
    sprite::Vec3 enemy_position(const void* enemy) override{return gameplay::enemy_position(enemy);}
    std::uint32_t nearest_enemy(game_session::Context& context,const sprite::Vec2& position,float radius) override{return gameplay::nearest_enemy_identifier(enemies(context),position,radius);}
    void* find_enemy(std::uint32_t& handle) override{if(!handle)return nullptr;auto* enemy=gameplay::find_enemy_in_list(gameplay::enemy_controller(0).enemies,handle);if(!enemy)handle=0;return enemy;}
    float random_angle() override{auto& random=state::random_streams[0];const auto value=state::next(random);return shot_random_angle(value,random.modulus);}
    sprite::Animation* find_animation(std::uint32_t handle) override{return sprite::find_animation(*program_entry::sprite_controller,handle);}
    void interrupt(std::uint32_t handle,int event) override{sprite::interrupt_animation_children(*program_entry::sprite_controller,handle,event);}
    void sound_pan(int id,float x) override{program_entry::thread_registry.set_effect_pan(id,x);}
    void stop_sound(int id) override{program_entry::thread_registry.stop_effects(id);}
    void retire_damage(damage::Region& region) override{damage::retire(region);}
    bool dialogue_active() override{return hud::controller&&hud::controller->collecting;}
    int weapon_pattern() override{return overlay::shot_script_index(*overlay::controller(0));}
    int weapon_phase() override{return overlay::controller(0)->phase;}
    bool weapon_shooting() override{return overlay::controller(0)->main_shooting!=0;}
    sprite::Animation* animation_child(std::uint32_t& handle,int script,int occurrence) override{return sprite::find_animation_child(*program_entry::sprite_controller,handle,script,occurrence);}
    void cancel_rectangles(game_session::Context& context,const sprite::Vec3& center,const sprite::Vec3& size,float angle) override{
        bullet::cancel_rectangle(*static_cast<bullet::Controller*>(context.primary_owner),center,size,angle,0,0);
        static_cast<laser::Controller*>(context.objects_04[4])->cancel_rectangle(center,size,angle,0,1);
    }
};
template<unsigned Index>int __fastcall initialize(Shot* shot,void*,int frame){return initialize_shot_callback(*shot,Index,frame,shot_callback_environment());}
template<unsigned Index>int __fastcall update(Shot* shot,void*){return update_shot_callback(*shot,Index,shot_callback_environment());}
const std::uintptr_t updates[]{0,reinterpret_cast<std::uintptr_t>(&update<1>),reinterpret_cast<std::uintptr_t>(&update<2>),reinterpret_cast<std::uintptr_t>(&update<3>),reinterpret_cast<std::uintptr_t>(&update<4>),reinterpret_cast<std::uintptr_t>(&update<5>),reinterpret_cast<std::uintptr_t>(&update<6>),reinterpret_cast<std::uintptr_t>(&update<7>),reinterpret_cast<std::uintptr_t>(&update<8>),reinterpret_cast<std::uintptr_t>(&update<9>),reinterpret_cast<std::uintptr_t>(&update<10>),reinterpret_cast<std::uintptr_t>(&update<11>),reinterpret_cast<std::uintptr_t>(&update<12>),reinterpret_cast<std::uintptr_t>(&update<13>),reinterpret_cast<std::uintptr_t>(&update<14>),reinterpret_cast<std::uintptr_t>(&update<15>),reinterpret_cast<std::uintptr_t>(&update<16>),0};
const std::uintptr_t initializers[]{0,
    reinterpret_cast<std::uintptr_t>(&initialize<1>),reinterpret_cast<std::uintptr_t>(&initialize<2>),reinterpret_cast<std::uintptr_t>(&initialize<3>),reinterpret_cast<std::uintptr_t>(&initialize<4>),
    reinterpret_cast<std::uintptr_t>(&initialize<5>),reinterpret_cast<std::uintptr_t>(&initialize<6>),reinterpret_cast<std::uintptr_t>(&initialize<7>),reinterpret_cast<std::uintptr_t>(&initialize<8>),
    reinterpret_cast<std::uintptr_t>(&initialize<9>),reinterpret_cast<std::uintptr_t>(&initialize<10>),reinterpret_cast<std::uintptr_t>(&initialize<11>),reinterpret_cast<std::uintptr_t>(&initialize<12>),
    reinterpret_cast<std::uintptr_t>(&initialize<13>),reinterpret_cast<std::uintptr_t>(&initialize<14>),reinterpret_cast<std::uintptr_t>(&initialize<15>),reinterpret_cast<std::uintptr_t>(&initialize<16>),
    reinterpret_cast<std::uintptr_t>(&initialize<17>),reinterpret_cast<std::uintptr_t>(&initialize<18>),reinterpret_cast<std::uintptr_t>(&initialize<19>),reinterpret_cast<std::uintptr_t>(&initialize<20>),
    reinterpret_cast<std::uintptr_t>(&initialize<21>),reinterpret_cast<std::uintptr_t>(&initialize<22>),reinterpret_cast<std::uintptr_t>(&initialize<23>),reinterpret_cast<std::uintptr_t>(&initialize<24>),
    reinterpret_cast<std::uintptr_t>(&initialize<25>),reinterpret_cast<std::uintptr_t>(&initialize<26>),reinterpret_cast<std::uintptr_t>(&initialize<27>),reinterpret_cast<std::uintptr_t>(&initialize<28>),
    reinterpret_cast<std::uintptr_t>(&initialize<29>),reinterpret_cast<std::uintptr_t>(&initialize<30>),reinterpret_cast<std::uintptr_t>(&initialize<31>),0};
}
ShotCallbackEnvironment& shot_callback_environment(){static GameShotCallbacks result;return result;}
std::uintptr_t shot_initialization_callback(unsigned index){if(index>=std::size(initializers))throw std::out_of_range("Invalid SHT initialization callback");return initializers[index];}
namespace unrecovered {
std::uintptr_t shot_extra_callback(unsigned index){if(index!=0)throw std::out_of_range("Invalid SHT extra callback index");return 0;} //5731fc: one null entry before573200
std::uintptr_t shot_update_callback(unsigned index){if(index>=std::size(updates))throw std::out_of_range("Invalid SHT update callback index");return updates[index];}
ShotCallbacks shot_callbacks(const ShotRecord& row){return {shot_initialization_callback(row.callbacks[0]),row.callbacks[1]?shot_update_callback(row.callbacks[1]):0,row.callbacks[2]?shot_extra_callback(row.callbacks[2]):0,row.callbacks[3]?shot_hit_callback(row.callbacks[3]):0};}
}
}
