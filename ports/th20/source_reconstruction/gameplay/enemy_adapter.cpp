#include "enemy.hpp"
#include "enemy_frame.hpp"
#include "enemy_update.hpp"
#include "enemy_movement.hpp"
#include "enemy_damage.hpp"
#include "../program_entry/program_entry.hpp"
#include "../stage_background/background.hpp"
#include "../sprite_renderer/dispatch.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/quad.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../damage_regions/damage.hpp"
#include "../bomb_system/bomb.hpp"
#include "../audio_runtime/audio.hpp"
#include <cstring>
#include <stdexcept>
namespace pe=th20::source::program_entry;
namespace th20::source::gameplay {
namespace {
class GameEnemyDamageServices final:public EnemyDamageServices {
public:
    const float* timer_rate() override{return sprite::anm_environment::timer_rate();}
    game_session::Session& session() override{return game_session::session;}
    void* boss_hud() override{return unrecovered::boss_hud_005c06a4();}
    sprite::Animation* animation(std::uint32_t handle) override{return sprite::find_animation(*pe::sprite_controller,handle);}
    void replace_animation(std::uint32_t& handle,int script) override{
        //44bcd0 preserves an invalid input handle. It marks the old animation,
        //spawns a fresh one, then writes file identifiers back into the old one.
        auto* old=animation(handle);if(!old)return;auto& file=sprite::script_file(*pe::sprite_controller,*old);
        const std::uint32_t deleted=0xffffffff;std::memcpy(reinterpret_cast<std::uint8_t*>(old)+0x28,&deleted,4);
        reinterpret_cast<std::uint8_t*>(old)[0x49a]&=0xfe;
        handle=sprite::spawn_named_animation(*pe::sprite_controller,file,nullptr,script,-1,nullptr);
        std::memcpy(reinterpret_cast<std::uint8_t*>(old)+0x18,&file.id,4);std::memcpy(reinterpret_cast<std::uint8_t*>(old)+0x1c,&file.id,4);
    }
    int calculate_damage(game_session::Context& context,const sprite::Vec3& p,const sprite::Vec2* size,float angle,float radius,std::uint32_t* flag,sprite::Vec3* hit,int preview,std::uint32_t target) override{
        return damage::calculate_damage(*static_cast<damage::HitCtrlInf*>(context.object_28),p,size,angle,radius,flag,hit,preview,target);
    }
    int defeat(void* entity) override{return unrecovered::defeat_enemy_004a5640(entity);}
    void clear_scripts(void* entity) override{unrecovered::clear_enemy_async_004973c0(entity);unrecovered::reset_enemy_script_004972c0(entity);}
    void select_script(void* entity,const char* name) override{unrecovered::select_enemy_script_00540200(entity,name);}
    int run_scripts(void* entity,float delta) override{return unrecovered::tick_enemy_scripts_0053e2b0(entity,delta);}
    void notify_timeout(void* owner) override{bomb::notify_bomb_start(owner);}
    void sound(int id,float x) override{pe::thread_registry.request_effect_at(id,x);}
    int player_circle(void* player,const sprite::Vec3& p,float r,int flag) override{return unrecovered::player_circle_004f8ff0(player,p,r,flag);}
    int player_rectangle(void* player,const sprite::Vec3& p,float angle,float width,float height,int flag) override{return unrecovered::player_rectangle_004f91d0(player,p,angle,width,height,flag);}
    void player_graze(void* player,const sprite::Vec3& p,int flag) override{unrecovered::player_graze_004f8b90(player,p,flag);}
};
class GameEnemyMovementServices final:public EnemyMovementServices {
public:
    const float* timer_rate() override{return sprite::anm_environment::timer_rate();}
    float clock_scale() override{return sprite::anm_environment::clock_scale();}
    sprite::Animation* animation(std::uint32_t handle) override{return sprite::find_animation(*pe::sprite_controller,handle);}
    sprite::Vec3 viewport_offset() override{sprite::Vec3 result;std::memcpy(&result,pe::graphics_state.viewports[0].final_vector,12);return result;}
    sprite::AnimationFile& animation_file(EnemyState& enemy,unsigned slot) override {
        auto* context=reinterpret_cast<game_session::Context*>(enemy.fields_2c8[9]);
        auto* owner=static_cast<EnemyController*>(context->objects_04[1]);
        auto* file=owner->services->existing_animation(*owner,slot);
        if(!file)throw std::logic_error("Enemy direction change requires animation file");return *file;
    }
    void delete_animation(std::uint32_t& handle) override{sprite::request_animation_deletion(*pe::sprite_controller,handle);}
    std::uint32_t spawn_animation(sprite::AnimationFile& file,int script,const sprite::Vec3& position,int layer) override {
        std::uint32_t handle=0;sprite::spawn_named_animation(*pe::sprite_controller,file,handle,nullptr,script,&position,0.0f,layer,8,nullptr);return handle;
    }
    float animation_height(sprite::Animation& animation) override{return sprite::animation_height(animation);}
    float animation_width(sprite::Animation& animation) override{return sprite::animation_width(animation);}
};
class GameEnemyUpdateServices final:public EnemyUpdateServices {
public:
    const float* timer_rate() override{return sprite::anm_environment::timer_rate();}
    float script_delta(const recovered::Timer& timer) override {
        //Every recovered timer initializer/tick clears mode to0. Other indices
        //in456240 address adjacent non-pointer globals in this executable.
        if((timer.flags&6u)!=0)throw std::out_of_range("Invalid Enemy script clock mode");
        const auto* rate=timer_rate();if(!rate)throw std::logic_error("Enemy script clock is null");return *rate;
    }
    sprite::Animation* animation(std::uint32_t handle) override{return sprite::find_animation(*pe::sprite_controller,handle);}
    int move(EnemyState& state) override{return update_enemy_movement(state,enemy_movement_services());}
    int run_scripts(void* entity,float delta) override{return unrecovered::tick_enemy_scripts_0053e2b0(entity,delta);}
    int damage(EnemyState& state) override{return update_enemy_damage(state,enemy_damage_services());}
    void mesh(EnemyState& state) override{unrecovered::update_enemy_mesh_004a4190(state);}
};
class GameEnemyFrameServices final:public EnemyFrameServices {
public:
    const float* timer_rate() override{return sprite::anm_environment::timer_rate();}
    float& clock_scale() override{return sprite::anm_environment::clock_scale();}
    void update_boss_time(std::int32_t seconds,std::int32_t hundredths) override {
        store_boss_time(unrecovered::boss_hud_005c06a4(),seconds,hundredths);
    }
    int update_entity_state(void* state) override{return update_enemy_state(*static_cast<EnemyState*>(state),enemy_update_services());}
    void update_special_objects() override{unrecovered::update_special_objects_005120d0();}
    sprite::Animation* animation(std::uint32_t handle) override{return sprite::find_animation(*pe::sprite_controller,handle);}
};
class GameEnemyServices final:public EnemyServices {
public:
    runtime::Log& log() override{return pe::log_buffer;}
    scheduler::State& scheduler_state() override{return *pe::function_controller;}
    scheduler::Environment& scheduler_environment() override{return pe::scheduler_environment;}
    sprite::Controller& sprites() override{return *pe::sprite_controller;}
    std::uint32_t& graphics_flags() override{return pe::graphics_state.event_flags;}
    int update_enemy(EnemyController& enemy) override{return update_enemy_controller(enemy,enemy_frame_services());}
    void draw_enemy_overlay() override{unrecovered::draw_enemy_overlay_00512aa0();}
    void select_layer(int layer,int group) override{sprite::configure_animation_layer(sprites(),layer,group);}
    void retire_entity(void* entity) override{unrecovered::destroy_enemy_entity_004a2720(entity);}
    sprite::AnimationFile* existing_animation(EnemyController& enemy,unsigned slot) override {
        if(slot>=8)throw std::out_of_range("ECL animation reference");
        if(slot<2) {
            //4aae10->4859b0 or45d100 are non-owning views of the real
            //Context+0/+20 owners; no substitute storage is introduced.
            const void* owner=slot==0?static_cast<void*>(enemy.context->primary_owner):enemy.context->objects_04[7];
            if(!owner)throw std::logic_error("Enemy animation slot requires actual Context owner");
            sprite::AnimationFile* file;std::memcpy(&file,static_cast<const std::uint8_t*>(owner)+(slot==0?0x286d8c:0x10),4);return file;
        }
        if(slot==7) {
            if(!background::primary)throw std::logic_error("Slot7 requires actual Background");
            return background::primary->animation_file; //4aadf0 +3334
        }
        return enemy.animation_files[slot];
    }
};
}
EnemyServices& enemy_services(){static GameEnemyServices services;return services;}
EnemyFrameServices& enemy_frame_services(){static GameEnemyFrameServices services;return services;}
EnemyUpdateServices& enemy_update_services(){static GameEnemyUpdateServices services;return services;}
EnemyMovementServices& enemy_movement_services(){static GameEnemyMovementServices services;return services;}
EnemyDamageServices& enemy_damage_services(){static GameEnemyDamageServices services;return services;}
}
