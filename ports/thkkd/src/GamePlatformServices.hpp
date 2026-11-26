#pragma once
#include "GameState.hpp"
#include "core/math/GameMath.hpp"
namespace kaikeidou {
class GamePlatformServices:public MenuScenePlatform,public SaveIO,public MenuBootstrapPlatform,public EndingCreditsPlatform,
 public StageBackgroundServices,public GameStartEntryServices,public MusicPresentationServices,
 public GameplayPresentationDispatchServices,public PauseGameoverOverlayServices,public BossPhaseServices,public MenuPresentationServices {
protected:
 using U=std::uint32_t;using Q=std::uint64_t;
 GameState& world_;unsigned precision_;bool variant_;
 MenuScenePlatformResult api(U n,std::vector<U> a={},std::vector<std::string_view> t={}){return invoke(MenuSceneRequest{static_cast<MenuSceneApi>(n),std::move(a),std::move(t),{}});}
 static std::int32_t signed32(U v){return static_cast<std::int32_t>(menu_scene_detail::signed_word(v));}
public:
 GameStartEntryMemory* memory=nullptr;RecoveredGameMath math;
 GamePlatformServices(GameState& w,unsigned bits,bool variant):world_(w),precision_(bits),variant_(variant){}
 virtual MenuScenePlatformResult invoke(const MenuSceneRequest&,MenuReplayFormatMemory* =nullptr)override=0;
 SaveIO& savedata_io()override{return *this;}
 MenuBootstrapPlatformResult invoke(const MenuBootstrapRequest& r)override{return invoke(MenuSceneRequest{static_cast<MenuSceneApi>(r.api),r.words,r.texts,{}});}
 EndingCreditsPlatformResult invoke(const EndingCreditsPlatformRequest&r)override{auto a=api(r.api,r.words,r.texts);EndingCreditsPlatformResult out{a.eax,a.ecx,a.edx,{}};for(auto&w:a.writes)out.writes.push_back({w.address,w.bytes});return out;}
 GameStartEntryMachineFrame& replay_input_frame(U,U)override{return world_.frame;}
 std::int32_t delete_graph(U h,U f)override{return signed32(api(0x403e20,{h,f}).eax);}U load_graph(std::string_view p,U f)override{return api(0x43c090,{f},{p}).eax;}U platform_random(U n)override{return api(0x466760,{n}).eax;}
 std::int32_t movie_seek(U h,U p)override{return signed32(api(0x406770,{h,p}).eax);}std::int32_t movie_play(U h,U m,U f)override{return signed32(api(0x406690,{h,m,f}).eax);}std::int32_t movie_pause(U h,U f)override{return signed32(api(0x406700,{h,f}).eax);}
 std::int32_t set_viewport_matrix(const std::array<U,16>& m)override{return signed32(api(0x40a9a0,std::vector<U>(m.begin(),m.end())).eax);}
 std::int32_t set_create_sound_data_type(U n)override{return signed32(api(0x43e0d0,{n}).eax);}std::int32_t stop_sound(U h)override{return signed32(api(0x440240,{h}).eax);}std::int32_t delete_sound(U h,U f)override{return signed32(api(0x43e010,{h,f}).eax);}
 std::int32_t load_sound(std::string_view p,U n,U h)override{return signed32(api(0x4434d0,{n,h},{p}).eax);}std::int32_t set_volume(U v,U h)override{return signed32(api(0x43fd40,{v,h}).eax);}std::int32_t play_sound(U h,U m,U f)override{return signed32(api(0x4425f0,{h,m,f}).eax);}
 GameStartEntryPlatformResult platform(GameStartEntryPlatformApi a,const std::vector<U>& args)override{auto result=api(static_cast<U>(a),args);GameStartEntryPlatformResult out{result.eax,result.ecx,result.edx,{}};for(auto& w:result.writes)out.writes.push_back({w.address,w.bytes});return out;}
 std::int32_t physical_input_count(U binding)override{return signed32(read_menu_scene_input(*memory,binding,false,[&](const MenuSceneRequest& r){return invoke(r);}));}
 std::int32_t input_count(U binding)override{return signed32(read_menu_scene_input(*memory,binding,variant_,[&](const MenuSceneRequest& r){return invoke(r);}));}
 U color(U r,U g,U b)override{return api(0x4049a0,{r,g,b}).eax;}void blend(U m,U a)override{api(0x404830,{m,a});}
 void rectangle(U l,U t,U r,U b,U c,U f)override{api(0x421760,{l,t,r,b,c,f});}
 void rectangle(std::int32_t l,std::int32_t t,std::int32_t r,std::int32_t b,U c,U f)override{rectangle(U(l),U(t),U(r),U(b),c,f);}
 void draw_font_text(std::int32_t x,std::int32_t y,std::string_view t,U c,U font,U edge,U vertical)override{api(0x437c90,{U(x),U(y),c,font,edge,vertical},{t});}
 void tint_color(U r,U g,U b)override{brightness(r,g,b);}void brightness(U r,U g,U b)override{api(0x4048a0,{r,g,b});}
 void present_component(GameplayPresentationComponent component)override{api(static_cast<U>(component));}
 void draw_slow_marker(Q x,Q y,Q scale,Q angle,U graph,U transparent,U turn)override{present_menu_sprite_game(x,y,scale,angle,graph,transparent,turn,world_.shake,precision_,*this);}
 void draw_rotated_sprite(U x,U y,Q scale,Q angle,U graph,U transparent,U turn)override{api(0x41c910,{x,y,U(scale),U(scale>>32),U(angle),U(angle>>32),graph,transparent,turn});}
 void draw_fps(U x,U y)override{world_.frame.registers.ecx=present_menu_fps(world_.scene.timing,world_.scene.title,x,y,world_.frame.registers.ecx,precision_,*this,variant_);}
 void draw_graph(U x,U y,U g,U t)override{api(0x41bb70,{x,y,g,t});}U text_width(U font,std::string_view t)override{return api(0x437c20,{font},{t}).eax;}
 MenuPresentationRegisters draw_formatted(U x,U y,U c,std::string_view t,bool number,Q value)override{std::vector<U> args{x,y,c};if(number){args.push_back(U(value));args.push_back(U(value>>32));}auto out=api(0x437f00,std::move(args),{t});return {out.eax,out.ecx};}
 U now_count(U n)override{return api(0x437fd0,{n}).eax;}void sleep_ms(U n)override{api(0x69722c,{n});}
 void change_stage_resources(std::int32_t)override{throw std::logic_error("Unexpected host resource dispatcher");}void present_component(MenuStartComponent)override{throw std::logic_error("Unexpected host menu component");}
 void draw_help(std::string_view,std::string_view)override{throw std::logic_error("Unexpected host help");}void move_cursor(U x,U y)override{draw_fps(x,y);}void present_cursor()override{world_.frame.registers.ecx=throttle_menu_frame(world_.scene.timing,precision_,*this,&world_.scene.keyboard_counts);}
 void menu_body(U)override{throw std::logic_error("Unexpected host menu body");}void replay_ui_route()override{throw std::logic_error("Unexpected host Replay UI");}
 void advance_phase(U mode)override{advance_boss_phase(mode,world_.enemies,world_.player,world_.items,world_.effects,world_.shake,world_.projectiles,world_.savedata,world_.patterns,world_.death,world_.boss_phase,world_.random,math,static_cast<RandomPrecision>(precision_),*this);}
 void restore_stage_music(U)override{throw std::logic_error("Failure continuation unexpectedly requested boss music restore");}
 void persist_spell_records(const SaveData& data)override{store_spell_records(data,*this);}

 PauseGameoverHost pause_platform(){return {*this,*this,*this,*this,*this,*this};}
};
}
