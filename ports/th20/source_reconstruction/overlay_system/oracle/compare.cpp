#define wmain unused_native_cpu_main
#include "../../../tests/native_cpu_compare.cpp"
#undef wmain
#include "../overlay.hpp"
#include "../basic_weapons.hpp"
#include "../../gameplay/player_state.hpp"
#include "../../player_entity/power.hpp"
namespace ov=th20::source::overlay;namespace gs=th20::source::game_session;namespace pe=th20::source::player_entity;
namespace ps=th20::source::gameplay::player_state;
template<class R,class... A>R cpu(unsigned va,void* self,A... args){using F=R(__thiscall*)(void*,A...);return reinterpret_cast<F>(mapped_image_base+va-0x400000)(self,args...);}
gs::Context fixture_context{};gs::Player fixture_stats{};
alignas(pe::Player) unsigned char player_storage[sizeof(pe::Player)]{};
alignas(ov::WeaponStoneInf) unsigned char owner_storage[sizeof(ov::WeaponStoneInf)]{};
auto& fixture_player=*reinterpret_cast<pe::Player*>(player_storage);
auto& fixture_owner=*reinterpret_cast<ov::WeaponStoneInf*>(owner_storage);
int fixture_level=0;
struct ShotCall {unsigned self;int first,second,pattern;};std::vector<ShotCall> calls;
gs::Player* __cdecl stats_boundary(int){return &fixture_stats;}
pe::Player* __cdecl player_boundary(int){return static_cast<pe::Player*>(fixture_context.objects_04[0]);}
ov::WeaponStoneInf* __cdecl owner_boundary(int){return &fixture_owner;}
int __fastcall level_boundary(void*,void*){return fixture_level;}
void __fastcall shoot_boundary(void* self,void*,int first,int second,int pattern){calls.push_back({reinterpret_cast<unsigned>(self),first,second,pattern});}
namespace th20::source::game_session {Context& context(int) noexcept{return fixture_context;}}
namespace th20::source::player_entity {int power_level(game_session::Player&) noexcept{return fixture_level;}}

namespace th20::source::overlay {
WeaponStoneInf* controller(int) noexcept{return &fixture_owner;}
void fire_player_shots(player_entity::ShotController& self,int first,int second,int pattern){shoot_boundary(&self,nullptr,first,second,pattern);}
}
#include "frame_fixture.hpp"
#include "additional_fixture.hpp"
int wmain(int argc,wchar_t** argv){try{
 if(argc!=3)throw std::runtime_error("Usage: th20_overlay_cpu_compare ORIGINAL.exe REPORT.json");
 const auto bytes=th20::read_file(argv[1]);if(sha256(bytes)!=expected_sha)throw std::runtime_error("Original SHA mismatch");const auto info=th20::parse_pe(bytes);Mapping image(bytes,info);mapped_image_base=image.address();
 AddVectoredExceptionHandler(1,[](PEXCEPTION_POINTERS p)->LONG{if(p->ExceptionRecord->ExceptionCode==EXCEPTION_ACCESS_VIOLATION)std::cerr<<"access violation VA="<<std::hex<<p->ContextRecord->Eip-mapped_image_base+0x400000<<" address="<<p->ExceptionRecord->ExceptionInformation[1]<<std::dec<<std::endl;return EXCEPTION_CONTINUE_SEARCH;});
 unsigned char saved_filtered[5],saved_firing[5];std::memcpy(saved_filtered,reinterpret_cast<void*>(mapped_image_base+0x7d240),5);std::memcpy(saved_firing,reinterpret_cast<void*>(mapped_image_base+0x105e40),5);
 auto hook=[&](unsigned va,void* target){auto* p=reinterpret_cast<unsigned char*>(mapped_image_base+va-0x400000);p[0]=0xe9;*reinterpret_cast<unsigned*>(p+1)=reinterpret_cast<unsigned>(target)-reinterpret_cast<unsigned>(p+5);FlushInstructionCache(GetCurrentProcess(),p,5);};
 hook(0x464080,reinterpret_cast<void*>(&stats_boundary));hook(0x460830,reinterpret_cast<void*>(&player_boundary));hook(0x464230,reinterpret_cast<void*>(&owner_boundary));hook(0x4b81a0,reinterpret_cast<void*>(&level_boundary));hook(0x504d40,reinterpret_cast<void*>(&shoot_boundary));
 fixture_context.current_player=&fixture_stats;fixture_context.objects_04[0]=&fixture_player;fixture_context.overlay_owner=&fixture_owner;
 unsigned char sht[0x1000]{};fixture_player.shot_data=sht;
 std::mt19937 random(0x532880);unsigned passed=0,failed=0;std::vector<std::string> failures;
 auto check=[&](const char* label,unsigned test,const void* expected,const void* actual,std::size_t count){if(!std::memcmp(expected,actual,count)){++passed;return;}++failed;if(failures.size()<40){std::ostringstream s;s<<label<<" test="<<test;for(std::size_t i=0;i<count;++i)if(static_cast<const unsigned char*>(expected)[i]!=static_cast<const unsigned char*>(actual)[i]){s<<" +0x"<<std::hex<<i<<" expected="<<unsigned(static_cast<const unsigned char*>(expected)[i])<<" actual="<<unsigned(static_cast<const unsigned char*>(actual)[i]);break;}failures.push_back(s.str());}};
 auto fill=[&](auto& p){auto* at=reinterpret_cast<unsigned char*>(&p);for(unsigned i=0;i<sizeof(p);++i)at[i]=std::uint8_t(random());};
 for(unsigned test=0;test<8192;++test){
  alignas(ov::Weapon) unsigned char raw[sizeof(ov::Weapon)],source[sizeof(ov::Weapon)];fill(raw);std::memcpy(source,raw,sizeof(raw));cpu<void>(0x52fad0,raw);auto* w=new(source)ov::StandardWeapon;check("base_constructor_fields",test,raw+4,source+4,sizeof(raw)-4);
  fill(fixture_stats);w->stone_id=int(random()%9);w->role=int(random()%7)-2;w->active=std::uint8_t(random());w->passive=std::uint8_t(random());std::memcpy(raw+4,source+4,sizeof(raw)-4);
  fixture_level=int(random()%5);const int ei=cpu<int>(0x534180,raw),ai=w->shot_script_index();check("shot_script_index",test,&ei,&ai,4);
  fixture_context.objects_04[0]=test%7?&fixture_player:nullptr;fixture_owner.main_shooting=std::uint8_t(test%3==0);fixture_player.focused_204c=std::uint8_t(test%2);r::timer_set(fixture_player.shots.timer_12400,int(random()%21)-10);
  const bool ef=cpu<unsigned char>(0x534320,raw)!=0,af=w->focused_shooting(),eu=cpu<unsigned char>(0x5343b0,raw)!=0,au=w->unfocused_shooting();check("focused_guard",test,&ef,&af,1);check("unfocused_guard",test,&eu,&au,1);fixture_context.objects_04[0]=&fixture_player;
  const int level=int(random()%9)-2,index=int(random()%10);const void *eo=cpu<const void*>(0x5301c0,raw,level,index),*ao=w->focused_offset(level,index),*ep=cpu<const void*>(0x530370,raw,level,index),*ap=w->unfocused_offset(level,index);check("focused_offset",test,&eo,&ao,4);check("unfocused_offset",test,&ep,&ap,4);
  ps::write(fixture_stats,0x54,int(random()%150)-25);ps::write(fixture_stats,0xa8,int(random()%160)-30);auto before=fixture_stats;const int ed=cpu<int>(0x534260,nullptr);auto expected_stats=fixture_stats;fixture_stats=before;const int ad=ov::phase_duration(fixture_stats);check("phase_duration",test,&ed,&ad,4);check("duration_clamp_fields",test,&expected_stats,&fixture_stats,sizeof(fixture_stats));
  before=fixture_stats;cpu<void>(0x52ffd0,raw);expected_stats=fixture_stats;fixture_stats=before;w->start_phase();check("start_phase",test,raw+4,source+4,sizeof(raw)-4);check("start_phase_stats",test,&expected_stats,&fixture_stats,sizeof(fixture_stats));
  for(int mode=0;mode<3;++mode){const auto owner_before=fixture_owner.main_shooting;calls.clear();cpu<void>(mode==0?0x52fbd0:mode==1?0x52fd10:0x52ff70,raw,int(test),-int(test),fixture_level);const auto expected_calls=calls;const auto owner_after=fixture_owner.main_shooting;fixture_owner.main_shooting=owner_before;calls.clear();if(mode==0)w->shoot_main(int(test),-int(test),fixture_level);else if(mode==1)w->shoot_focused(int(test),-int(test),fixture_level);else w->shoot_unfocused(int(test),-int(test),fixture_level);const auto ec=expected_calls.size(),ac=calls.size();check("shoot_count",test,&ec,&ac,4);if(ec==ac)check("shoot_arguments",test,expected_calls.data(),calls.data(),ec*sizeof(ShotCall));check("shoot_owner",test,&owner_after,&fixture_owner.main_shooting,1);}
  r::timer_set(w->phase_age,int(random()%1400)-100);r::timer_set(w->idle_age,int(random()%60)-10);std::memcpy(raw+4,source+4,sizeof(raw)-4);fixture_player.shots.field_1255c=random();ps::write(fixture_stats,0x50,int(random()%900)-100);before=fixture_stats;th20::source::state::clock_scale=float(random()%13)*.25f;*reinterpret_cast<const float**>(mapped_image_base+0x1aefe0)=th20::source::state::timer_rate;
  FloatingEnvironment::prepare();const int er=cpu<int>(0x52fd70,raw);expected_stats=fixture_stats;fixture_stats=before;FloatingEnvironment::prepare();const int ar=w->update_phase();check("phase_update_return",test,&er,&ar,4);check("phase_update_object",test,raw+4,source+4,sizeof(raw)-4);check("phase_update_stats",test,&expected_stats,&fixture_stats,sizeof(fixture_stats));
  cpu<void>(0x52fb80,raw);w->reset();check("weapon_reset",test,raw+4,source+4,sizeof(raw)-4);w->~StandardWeapon();
 }
 for(unsigned test=0;test<4096;++test){
  ov::FocusBoostWeapon<0> focus;ov::FlagBoostWeapon<0> flag;ov::OrbitWeapon<0> orbit0;ov::OrbitWeapon<1> orbit1;
  fill(fixture_stats);fixture_stats.bytes_2c[3]=test%2;auto before=fixture_stats;cpu<void>(0x5323e0,&focus);auto ep=fixture_stats;fixture_stats=before;focus.initialize_passive();check("focus_passive",test,&ep,&fixture_stats,sizeof(fixture_stats));before=fixture_stats;cpu<void>(0x532530,&flag);ep=fixture_stats;fixture_stats=before;flag.initialize_passive();check("flag_passive",test,&ep,&fixture_stats,sizeof(fixture_stats));before=fixture_stats;cpu<void>(0x52fe80,&orbit0);ep=fixture_stats;fixture_stats=before;orbit0.initialize_passive();check("orbit_passive",test,&ep,&fixture_stats,sizeof(fixture_stats));
  fixture_level=int(random()%5);pe::Option oa,ob;fill(oa);ps::write(oa,0xd4,float(int(random()%10000)-5000)/100.f);ob=oa;const bool character=test%2;const int index=int(random()%8);const bool focused=test%4<2;
  cpu<void>(character?(focused?0x530d60:0x530df0):(focused?0x52fc80:0x52fee0),character?static_cast<ov::Weapon*>(&orbit1):static_cast<ov::Weapon*>(&orbit0),&oa,index);
  auto& orbit=character?static_cast<ov::StandardWeapon&>(orbit1):static_cast<ov::StandardWeapon&>(orbit0);if(focused)orbit.initialize_focused_option(&ob,index);else orbit.initialize_unfocused_option(&ob,index);
  const unsigned callback_offset=focused?0x11c:0x120;const auto callback=ps::read<unsigned>(oa,callback_offset),expected_callback=mapped_image_base+(character?0x130e80:0x130010);check("option_callback_original",test,&expected_callback,&callback,4);ps::write(oa,callback_offset,ps::read<unsigned>(ob,callback_offset));check("option_initialize",test,&oa,&ob,sizeof(oa));
  fixture_player.focused_204c=std::uint8_t(random()%2);fixture_player.fixed_position={int(random()),int(random())};ps::write(oa,0x128,&fixture_context);ob=oa;FloatingEnvironment::prepare();cpu<void>(character?0x530e80:0x530010,&oa);FloatingEnvironment::prepare();ov::orbit_option(ob,int(character));check("orbit_option_frame",test,&oa,&ob,sizeof(oa));
  const int slot=int(random()%8)-2;const int es=cpu<int>(0x4641d0,&fixture_stats,slot),as=ov::selected_stone(fixture_stats,slot);const auto eh=cpu<std::uint8_t>(0x464420,&fixture_stats,slot),ah=ov::inherited_stone(fixture_stats,slot);check("selected_stone",test,&es,&as,4);check("inherited_stone",test,&eh,&ah,1);
 }
 #include "frame_cases.inc"
 #include "bar_cases.inc"
 #include "ring_cases.inc"
 #include "shield_cases.inc"
 #include "cloud_cases.inc"
 #include "yellow_cases.inc"
 #include "additional_cases.inc"
 std::ofstream out(argv[2]);out<<"{\n  \"module\": \"overlay_system\",\n  \"original_sha256\": "<<th20::json_string(expected_sha)<<",\n  \"passed\": "<<passed<<",\n  \"failed\": "<<failed<<",\n  \"failures\": [";for(std::size_t i=0;i<failures.size();++i){if(i)out<<",";out<<th20::json_string(failures[i]);}out<<"]\n}\n";
 for(const auto& f:failures)std::cerr<<f<<'\n';std::cout<<"passed="<<passed<<" failed="<<failed<<'\n';return failed?1:0;
}catch(const std::exception& error){std::cerr<<error.what()<<'\n';return 2;}}
