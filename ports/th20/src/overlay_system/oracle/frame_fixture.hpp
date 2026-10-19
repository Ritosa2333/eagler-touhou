#include "../frame.hpp"
#include "../bar_weapon.hpp"
#include "../ring_weapon.hpp"
#include "../shield_weapon.hpp"
#include "../cloud_weapon.hpp"
#include "../yellow_weapon.hpp"
std::vector<unsigned> frame_trace;
bool end_game=false,active_game=true,present_game=true,present_hud=true,collecting=false,existing_aura=true;
unsigned next_aura=1234;int phase_result=0,end_result=0;
int cancelled_count=0;
th20::source::sprite::Animation fixture_animation{};
th20::source::damage::Region fixture_regions[4]{};unsigned region_sequence=0;
void event(unsigned id,unsigned a=0,unsigned b=0){frame_trace.insert(frame_trace.end(),{id,a,b});}
unsigned fbits(float f){unsigned v;std::memcpy(&v,&f,4);return v;}
void position_event(unsigned id,unsigned handle,const th20::source::sprite::Vec3& p){event(id,handle,fbits(p.x));event(0,fbits(p.y),fbits(p.z));}
struct ControlledWeapon:ov::Weapon {
 void initialize_main()override{event(20);}void initialize_passive()override{event(21);}
 int update_phase()override{event(22);return phase_result;}int end_phase()override{event(23);return end_result;}int cancel_phase()override{event(24);return 0;}
 void update_main()override{event(25);}void update_focused()override{event(26);}void update_unfocused()override{event(27);}void update_passive()override{event(28);passive=0;}
 void start_phase()override{event(29);}void activate_main()override{event(30);}void activate_focused()override{event(31);}void activate_unfocused()override{event(32);}
 void shoot_main(int a,int b,int c)override{event(33,a,b);event(0,c);}void shoot_focused(int a,int b,int c)override{event(34,a,b);event(0,c);}void shoot_unfocused(int a,int b,int c)override{event(35,a,b);event(0,c);}
};
struct FrameHost final:ov::FrameEnvironment,ov::WeaponServices {
 bool game_requests_end()override{return present_game&&end_game;}bool game_active()override{return present_game&&active_game;}bool hud_present()override{return present_hud;}bool boss_collecting()override{return collecting;}
 th20::source::sprite::Vec3 player_position()override{return fixture_player.position_614;}
 bool animation_exists(unsigned& handle)override{event(1,handle);return existing_aura;}
 void animation_position(unsigned& handle,const th20::source::sprite::Vec3& p)override{position_event(2,handle,p);}
 unsigned spawn_aura(th20::source::sprite::AnimationFile&,const th20::source::sprite::Vec3& p)override{position_event(3,29,p);return next_aura;}
 void interrupt(unsigned& handle,int value)override{event(4,handle,value);}void delete_animation(unsigned& handle)override{event(5,handle);handle=0;}
 void phase_visuals(ov::WeaponStoneInf& owner,bool ending)override{event(6,ending,owner.animation_handle);}
 void clear_bullets(const th20::source::sprite::Vec3& p,float radius)override{position_event(7,fbits(radius),p);}
 void clear_lasers(const th20::source::sprite::Vec3& p,float radius)override{position_event(8,fbits(radius),p);}
 void sound(int id,float x)override{event(9,id,fbits(x));}
 void show_animation(unsigned& handle,bool show)override{event(10,handle,show);}
 unsigned spawn_player_animation(const char* name,int script,const th20::source::sprite::Vec3& p)override{event(11,unsigned(name[3]),script);position_event(0,0,p);return next_aura;}
 void clear_bullet_rectangle(const th20::source::sprite::Vec3& p,const th20::source::sprite::Vec3& size)override{position_event(12,0,p);position_event(0,0,size);}
 void clear_laser_rectangle(const th20::source::sprite::Vec3& p,const th20::source::sprite::Vec3& size)override{position_event(13,0,p);position_event(0,0,size);}
 th20::source::sprite::Animation& animation(unsigned&)override{return fixture_animation;}
 unsigned spawn_effect(int type,const th20::source::effects::Parameters& p)override{event(14,type);const auto* bytes=reinterpret_cast<const unsigned char*>(&p);for(unsigned i=0;i<sizeof(p);++i)if(i<0x29||i>=0x2c)frame_trace.push_back(bytes[i]);return next_aura;}
 void pause_animation(unsigned& handle,bool pause)override{event(15,handle,pause);}
 void spawn_item(int type,const th20::source::sprite::Vec3& p,float angle,float speed)override{event(16,type);position_event(0,0,p);event(0,fbits(angle),fbits(speed));}
 unsigned create_damage_circle(const th20::source::sprite::Vec3& p,float radius,float growth,int frames,int amount)override{const auto index=region_sequence++%4;fixture_regions[index]={};fixture_regions[index].motion.position=p;fixture_regions[index].radius=radius;fixture_regions[index].radius_step=growth;fixture_regions[index].damage=amount;event(17,100+index,amount);position_event(0,frames,p);event(0,fbits(radius),fbits(growth));return 100+index;}
 th20::source::damage::Region& damage(unsigned& handle)override{return fixture_regions[(handle-100)%4];}
 void retire_damage(unsigned& handle)override{event(18,handle);handle=0;}
 void damage_position(unsigned& handle,const th20::source::sprite::Vec3& p)override{position_event(19,handle,p);damage(handle).motion.position=p;}
 int cancel_counted(const th20::source::sprite::Vec3& p,float radius)override{position_event(40,fbits(radius),p);return cancelled_count;}
 int cancel_filtered(const th20::source::sprite::Vec3& p,float radius,std::function<int(const th20::source::sprite::Vec3&)> callback)override{position_event(41,fbits(radius),p);for(int i=0;i<cancelled_count;++i){auto point=p;point.x+=float(i);const int result=callback(point);event(42,result);}return cancelled_count;}
 void fire_shots_at_position(int pattern,int first,int second,const th20::source::sprite::Vec3& p)override{event(43,pattern,first);position_event(0,second,p);}
} frame_host;
namespace th20::source::overlay {FrameEnvironment& frame_environment(){return frame_host;}WeaponServices& weapon_services(){return frame_host;}}
unsigned __fastcall game_end_boundary(void*,void*){return end_game;}int __fastcall game_active_boundary(void*,void*){return active_game;}
unsigned __fastcall boss_boundary(void*,void*){return collecting;}
int __fastcall exists_boundary(unsigned* handle,void*){return frame_host.animation_exists(*handle);}
void __fastcall position_boundary(unsigned* handle,void*,const th20::source::sprite::Vec3* p){frame_host.animation_position(*handle,*p);}
unsigned* __fastcall aura_boundary(void* file,void*,unsigned* output,const char* name,int script,const th20::source::sprite::Vec3* p,float rotation,int layer,unsigned* data){if(rotation!=0||layer!=-1||data)throw std::logic_error("Unexpected animation arguments");if(!std::strcmp(name,"aura")){if(script!=29)throw std::logic_error("Unexpected aura script");*output=frame_host.spawn_aura(*static_cast<th20::source::sprite::AnimationFile*>(file),*p);}else if(!std::strcmp(name,"pl00")||!std::strcmp(name,"pl01"))*output=frame_host.spawn_player_animation(name,script,*p);else throw std::logic_error("Unexpected animation name");return output;}
void __fastcall interrupt_boundary(unsigned* handle,void*,int value){frame_host.interrupt(*handle,value);}
void __fastcall delete_boundary(unsigned* handle,void*){frame_host.delete_animation(*handle);}
void __fastcall ending_boundary(ov::WeaponStoneInf* owner,void*){frame_host.phase_visuals(*owner,true);}
void __fastcall starting_boundary(ov::WeaponStoneInf* owner,void*){frame_host.phase_visuals(*owner,false);}
void* __cdecl cancel_owner_boundary(int){return reinterpret_cast<void*>(1);}
void __fastcall clear_bullets_boundary(void*,void*,const th20::source::sprite::Vec3* p,float radius,int mode,int limit,int type){if(mode||limit!=99999||type)throw std::logic_error("Unexpected bullet arguments");frame_host.clear_bullets(*p,radius);}
void __fastcall clear_lasers_boundary(void*,void*,const th20::source::sprite::Vec3* p,float radius,int mode,int type){if(mode||type!=1)throw std::logic_error("Unexpected laser arguments");frame_host.clear_lasers(*p,radius);}
void __fastcall sound_boundary(void*,void*,int id,float x){frame_host.sound(id,x);}
void __fastcall show_boundary(unsigned* handle,void*){frame_host.show_animation(*handle,true);}void __fastcall hide_boundary(unsigned* handle,void*){frame_host.show_animation(*handle,false);}
void __fastcall bullet_rectangle_boundary(void*,void*,const th20::source::sprite::Vec3* p,const th20::source::sprite::Vec3* size,float angle,int drop,int kind){if(angle||drop||kind)throw std::logic_error("Unexpected bullet rectangle");frame_host.clear_bullet_rectangle(*p,*size);}
void __fastcall laser_rectangle_boundary(void*,void*,const th20::source::sprite::Vec3* p,const th20::source::sprite::Vec3* size,float angle,int drop,int kind){if(angle||drop||kind)throw std::logic_error("Unexpected laser rectangle");frame_host.clear_laser_rectangle(*p,*size);}
unsigned* __fastcall effect_boundary(void*,void*,unsigned* output,int type,const th20::source::effects::Parameters* p,void* animation){if(animation)throw std::logic_error("Unexpected effect reuse");*output=frame_host.spawn_effect(type,*p);return output;}
th20::source::sprite::Animation* __fastcall resolve_boundary(unsigned*,void*){return &fixture_animation;}
th20::source::sprite::Animation* __fastcall find_boundary(void*,void*,unsigned){return &fixture_animation;}
void __fastcall resume_boundary(unsigned* handle,void*){frame_host.pause_animation(*handle,false);}void __fastcall pause_boundary(unsigned* handle,void*){frame_host.pause_animation(*handle,true);}
void* __fastcall item_boundary(void*,void*,int type,const th20::source::sprite::Vec3* p,unsigned color,float angle,float speed,int delay,unsigned extra,int sound){if(color!=0xffffffff||delay||extra||sound!=-1)throw std::logic_error("Unexpected ring item");frame_host.spawn_item(type,*p,angle,speed);return nullptr;}
unsigned* __fastcall damage_circle_boundary(void*,void*,unsigned* out,const th20::source::sprite::Vec3* p,float radius,float growth,int frames,int damage){*out=frame_host.create_damage_circle(*p,radius,growth,frames,damage);return out;}
th20::source::damage::Region* __fastcall damage_find_boundary(unsigned* handle,void*){return &frame_host.damage(*handle);}
void __fastcall damage_retire_boundary(unsigned* handle,void*){frame_host.retire_damage(*handle);}
void __fastcall damage_position_boundary(unsigned* handle,void*,const th20::source::sprite::Vec3* p){frame_host.damage_position(*handle,*p);}
int __fastcall counted_boundary(void*,void*,const th20::source::sprite::Vec3* p,float radius,int mode,int limit,int type){if(mode||limit!=99999||type)throw std::logic_error("Unexpected counted circle");return frame_host.cancel_counted(*p,radius);}
int __fastcall filtered_boundary(void*,void*,const th20::source::sprite::Vec3* p,float radius,std::function<int(const th20::source::sprite::Vec3&)> callback){return frame_host.cancel_filtered(*p,radius,std::move(callback));}
int __fastcall shots_at_boundary(void*,void*,int pattern,int first,int second,const th20::source::sprite::Vec3* p){frame_host.fire_shots_at_position(pattern,first,second,*p);return 0;}
float __fastcall unit_boundary(void*,void*){return th20::source::state::unit(th20::source::state::random_streams[0]);}
float __fastcall signed_unit_boundary(void*,void*){return th20::source::state::signed_unit(th20::source::state::random_streams[0]);}
