#include "environment.hpp"
#include "frame.hpp"
#include "basic_weapons.hpp"
#include "weapon_services.hpp"
#include "weapon_cancellation.hpp"
#include "../player_entity/firing_at_position.hpp"
#include "../effect_system/effect.hpp"
#include "../gameplay/gameplay.hpp"
#include "../gameplay/stage_data.hpp"
#include "../program_entry/program_entry.hpp"
#include "../sprite_renderer/animation_file.hpp"
#include "../sprite_renderer/named_spawn.hpp"
#include "../sprite_renderer/loading_interrupt.hpp"
#include "../audio_runtime/audio.hpp"
#include "../item_system/rewards.hpp"
#include "../bullet_system/bullet.hpp"
#include "../laser_system/laser.hpp"
namespace th20::source::overlay {
namespace pe=program_entry;
namespace {
void pause_tree(sprite::Animation& animation,bool pause){if(pause)animation.base.flags[7]|=1;else animation.base.flags[7]&=~1u;if(auto* first=animation.links[3].next){scheduler::Iterator iterator(reinterpret_cast<scheduler::Link*>(first));while(iterator.current){pause_tree(*reinterpret_cast<sprite::Animation*>(iterator.current->value),pause);iterator.advance();}}} //44f170/44f2c0
class GameEnvironment final:public Environment,public FrameEnvironment,public WeaponServices {
public:
 Weapon* create_weapon(int character,int stone) override{return overlay::create_weapon(character,stone);}
 void destroy_weapon(Weapon* value) override{overlay::destroy_weapon(value);}
 void destroy_mesh(sprite::RenderMesh* value) override{sprite::destroy_render_mesh(value);}
 void unload_animation(int index) override{sprite::unload_animation_file(*pe::sprite_controller,index);}
 void preserve_animation(int index,bool preserve) override{sprite::mark_file_animations(*pe::sprite_controller,pe::sprite_controller->files[index],preserve);}
 void select_view(int index) override{pe::sprite_controller->field_6c4=index;}
 sprite::AnimationFile* load_animation(int index,const char* name) override{return sprite::load_animation_file(*pe::sprite_controller,index,name,pe::log_buffer,pe::graphics_state.event_flags);}
 void load_error() override{runtime::log_printf(pe::log_buffer,"\x83\x66\x81\x5b\x83\x5e\x82\xaa\x89\xf3\x82\xea\x82\xc4\x82\xa2\x82\xdc\x82\xb7\r\n");}
 bool restarting() override{return gameplay::controller->restart()!=0;}
 std::uint8_t replay_inherited(int slot) override{return unrecovered::replay_inherited_stone(slot);}
 int stage_id() override{return gameplay::selected_stage->id;}
 void delete_animation(std::uint32_t& handle) override{sprite::request_animation_deletion(*pe::sprite_controller,handle);}
 bool game_requests_end() override{return gameplay::controller&&(gameplay::controller->game_flags&0x40)!=0;}
 bool game_active() override{return gameplay::controller&&gameplay::controller->update_node&&(gameplay::controller->update_node->flags&2)!=0;}
 bool hud_present() override{return item::reward_environment().hud_available();}
 bool boss_collecting() override{return item::environment().boss_collecting();}
 sprite::Vec3 player_position() override{return static_cast<player_entity::Player*>(game_session::context(0).objects_04[0])->position_614;}
 bool animation_exists(std::uint32_t& handle) override{return sprite::resolve_animation_handle(*pe::sprite_controller,handle)!=nullptr;}
 void animation_position(std::uint32_t& handle,const sprite::Vec3& position) override{auto* a=sprite::resolve_animation_handle(*pe::sprite_controller,handle);if(!a)a=&pe::sprite_controller->animation_dc;a->vector_5bc=position;}
 std::uint32_t spawn_aura(sprite::AnimationFile& file,const sprite::Vec3& position) override{std::uint32_t handle;sprite::spawn_named_animation(*pe::sprite_controller,file,handle,"aura",29,&position,0,-1,0);return handle;}
 void interrupt(std::uint32_t& handle,int value) override{sprite::interrupt_animation_children(*pe::sprite_controller,handle,value);}
 void phase_visuals(WeaponStoneInf& owner,bool ending) override{overlay::phase_visuals(owner,ending);}
 void clear_bullets(const sprite::Vec3& p,float radius) override{bullet::cancel_circle(*bullet::controller(0),p,radius,0,99999,0);}
 void clear_lasers(const sprite::Vec3& p,float radius) override{laser::controller(0)->cancel_circle(p,radius,0,1);}
 void sound(int id,float x) override{pe::thread_registry.request_effect_at(id,x);}
 void show_animation(std::uint32_t& handle,bool show) override{if(auto* animation=sprite::find_animation(*pe::sprite_controller,handle)){if(show)effects::enable_animation_tree(*animation);else sprite::hide_animation_tree(*animation);}}
 std::uint32_t spawn_player_animation(const char* name,int script,const sprite::Vec3& position) override{auto& file=*static_cast<player_entity::Player*>(game_session::context(0).objects_04[0])->animation_file;std::uint32_t handle;sprite::spawn_named_animation(*pe::sprite_controller,file,handle,name,script,&position,0,-1,0);return handle;}
 void clear_bullet_rectangle(const sprite::Vec3& p,const sprite::Vec3& size) override{bullet::cancel_rectangle(*bullet::controller(0),p,size,0,0,0);}
 void clear_laser_rectangle(const sprite::Vec3& p,const sprite::Vec3& size) override{laser::controller(0)->cancel_rectangle(p,size,0,0,0);}
 sprite::Animation& animation(std::uint32_t& handle) override{auto* value=sprite::resolve_animation_handle(*pe::sprite_controller,handle);return value?*value:pe::sprite_controller->animation_dc;}
 std::uint32_t spawn_effect(int type,const effects::Parameters& p) override{return effects::controller(0)->spawn(type,&p,nullptr,false);}
 void pause_animation(std::uint32_t& handle,bool paused) override{if(auto* value=sprite::find_animation(*pe::sprite_controller,handle))pause_tree(*value,paused);}
 void spawn_item(int type,const sprite::Vec3& p,float angle,float speed) override{item::spawn(*item::controller(0),type,p,0xffffffffu,angle,speed,0,0,-1);}
 std::uint32_t create_damage_circle(const sprite::Vec3& p,float radius,float growth,int frames,int amount) override{return damage::create_circle(*damage::controller(0),p,radius,growth,frames,amount);}
 damage::Region& damage(std::uint32_t& handle) override{return *damage::find_handle(handle);}
 void retire_damage(std::uint32_t& handle) override{damage::retire_handle(handle);}
 void damage_position(std::uint32_t& handle,const sprite::Vec3& p) override{damage::set_handle_position(handle,p);}
 int cancel_filtered(const sprite::Vec3& p,float radius,std::function<int(const sprite::Vec3&)> accept) override{return bullet::cancel_filtered_circle(*bullet::controller(0),p,radius,std::move(accept));}
 int cancel_counted(const sprite::Vec3& p,float radius) override{return bullet::cancel_circle(*bullet::controller(0),p,radius,0,99999,0);}
 void fire_shots_at_position(int pattern,int frame,int secondary,const sprite::Vec3& p) override{player_entity::fire_shots_at_position(static_cast<player_entity::Player*>(game_session::context(0).objects_04[0])->shots,pattern,frame,secondary,p);}
};
GameEnvironment& game_environment(){static GameEnvironment value;return value;}
}
Environment& environment(){return game_environment();}FrameEnvironment& frame_environment(){return game_environment();}
WeaponServices& weapon_services(){return game_environment();}
}
