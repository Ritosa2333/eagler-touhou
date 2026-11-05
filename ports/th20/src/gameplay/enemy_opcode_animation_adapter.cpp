#include "enemy_opcode_animation.hpp"
#include "enemy_variables.hpp"
#include "enemy_movement.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/pool.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/binding.hpp"
#include "../sprite_renderer/render_mesh.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../sprite_renderer/anm_vm.hpp"
#include "../player_entity/shot_hit.hpp"
#include "../effect_system/effect.hpp"
namespace th20::source::gameplay {
namespace {
class AnimationServices final:public EnemyAnimationOpcodeServices {
    sprite::AnimationFile& file(EnemyController& owner,unsigned slot) override{return *owner.services->existing_animation(owner,slot);}
    std::uint32_t spawn(sprite::AnimationFile& file,int script,const sprite::Vec3* position,float angle,int layer,unsigned flags) override{std::uint32_t handle;sprite::spawn_named_animation(*program_entry::sprite_controller,file,handle,nullptr,script,position,angle,layer,flags);return handle;}
    void delete_animation(std::uint32_t& handle) override{sprite::request_animation_deletion(*program_entry::sprite_controller,handle);}
    sprite::Animation* animation(std::uint32_t& handle) override{return sprite::resolve_animation_handle(*program_entry::sprite_controller,handle);}
    float height(sprite::Animation& a) override{return enemy_movement_services().animation_height(a);}
    float width(sprite::Animation& a) override{return enemy_movement_services().animation_width(a);}
    void hide(std::uint32_t handle) override{if(auto* a=animation(handle))sprite::hide_animation_tree(*a);}
    void interrupt(std::uint32_t handle,int event) override{sprite::interrupt_animation_children(*program_entry::sprite_controller,handle,event);}
    void layer(sprite::Animation& a,int layer) override{sprite::set_animation_layer(a,layer);}
    void execute(sprite::Animation& a) override{sprite::execute_animation(a);}
    void remember(game_session::Context& context,std::uint32_t handle) override{player_entity::remember_shot_effect(*static_cast<effects::Controller*>(context.objects_04[7]),handle,player_entity::shot_callback_environment());}
    void effect(game_session::Context& context,int type,const sprite::Vec3& position) override{auto& owner=*static_cast<effects::Controller*>(context.objects_04[7]);const auto slot=player_entity::reserve_effect_slot(owner,player_entity::shot_callback_environment());if(slot==0xffffffffu||recovered::signed_bits(slot)>1023)return;owner.handles[slot]=owner.spawn(type,&position);}
    void create_enemy(EnemyController& owner,const char* name,const SpawnParameters& p,Enemy* parent) override{spawn_enemy(owner,name,p,parent);}
    Enemy* selected(EnemyController& owner,unsigned slot) override{return static_cast<Enemy*>(selected_enemy(&owner,slot));}
    Enemy* find(EnemyController& owner,unsigned identifier) override{return static_cast<Enemy*>(find_enemy_in_list(owner.enemies,identifier));}
};
}
EnemyAnimationOpcodeServices& enemy_animation_opcode_services(){static AnimationServices value;return value;}
EnemyOpcodeResult execute_enemy_animation_opcode(EnemyOpcodeReader& reader){return execute_enemy_animation_opcode(reader,enemy_animation_opcode_services());}
}
