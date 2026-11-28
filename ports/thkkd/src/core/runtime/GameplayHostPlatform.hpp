// 共同宿主的平台桥只提交原实参与有序写入，恢复后的数值和状态计算仍在既有模块内。
#pragma once
#include "core/frontend/MenuScenePlatform.hpp"
#include "core/frontend/MenuSceneInput.hpp"
#include "core/frontend/MenuPresentation.hpp"
#include "core/stages/StageTitlePresentationServices.hpp"
#include "core/entities/BossSpriteServices.hpp"
#include "core/shared/BackgroundMeshPresentationServices.hpp"
#include "core/frontend/EndingFadePresentation.hpp"
namespace kaikeidou {
class GameplayHostPlatform {
public:
 using U=std::uint32_t;using Q=std::uint64_t;
 GameStartEntryMemory& memory;GameStartEntryMachineFrame& frame;MenuScenePlatform& platform;
 GameplayHostPlatform(GameStartEntryMemory& m,GameStartEntryMachineFrame& f,MenuScenePlatform& p):memory(m),frame(f),platform(p){}
 MenuScenePlatformResult call(U api,std::vector<U> words={},std::vector<std::string_view> texts={},std::vector<std::uint8_t> bytes={}){
  memory.reconcile();auto out=platform.invoke({static_cast<MenuSceneApi>(api),std::move(words),std::move(texts),std::move(bytes)});memory.reconcile();for(const auto& write:out.writes)memory.write(write.address,write.bytes);memory.reconcile();frame.registers.eax=out.eax;frame.registers.ecx=out.ecx;frame.registers.edx=out.edx;return out;
 }
 MenuScenePlatformResult call_drawing(U api,std::vector<U> words={}){
  auto out=platform.invoke({static_cast<MenuSceneApi>(api),std::move(words),{},{}});
  if(!out.writes.empty()){memory.reconcile();for(const auto& write:out.writes)memory.write(write.address,write.bytes);memory.reconcile();}
  frame.registers.eax=out.eax;frame.registers.ecx=out.ecx;frame.registers.edx=out.edx;return out;
 }
 U color_drawing(U r,U g,U b){return call_drawing(0x4049a0,{r,g,b}).eax;}
 void pixel_drawing(U x,U y,U ink){call_drawing(0x41e7b0,{x,y,ink});}
 U physical(U binding){return read_menu_scene_input(memory,binding,false,[&](const MenuSceneRequest& q){return call(static_cast<U>(q.api),q.words,q.texts,q.bytes);});}
};
class GameplayHostPresentation:public MenuPresentationServices,public StageTitlePresentationServices,public EndingFadePresentationServices {
 GameplayHostPlatform& p_;const EffectShake& shake_;unsigned precision_;
public:
 using U=std::uint32_t;using Q=std::uint64_t;
 GameplayHostPresentation(GameplayHostPlatform& p,const EffectShake& shake,unsigned precision):p_(p),shake_(shake),precision_(precision){}
 std::int32_t input_count(U b)override{return static_cast<std::int32_t>(p_.physical(b));}
 U color(U r,U g,U b)override{return p_.color_drawing(r,g,b);}
 void blend(U mode,U alpha)override{p_.call(0x404830,{mode,alpha});}
 void brightness(U r,U g,U b)override{p_.call(0x4048a0,{r,g,b});}
 void draw_font_text(std::int32_t x,std::int32_t y,std::string_view text,U ink,U font,U edge,U vertical)override{p_.call(0x437c90,{U(x),U(y),ink,font,edge,vertical},{text});}
 void draw_rotated_sprite(U x,U y,Q scale,Q angle,U graph,U transparent,U turn)override{p_.call(0x41c910,{x,y,U(scale),U(scale>>32),U(angle),U(angle>>32),graph,transparent,turn});}
 void sprite(Q x,Q y,Q scale,Q angle,U graph,U transparent,U turn)override{present_menu_sprite_game(x,y,scale,angle,graph,transparent,turn,shake_,precision_,*this);}
 void draw_graph(U x,U y,U graph,U transparent)override{p_.call(0x41bb70,{x,y,graph,transparent});}
 void box(U l,U t,U r,U b,U ink,U fill)override{rectangle(l,t,r,b,ink,fill);}
 void rectangle(U l,U t,U r,U b,U ink,U fill)override{p_.call(0x421760,{l,t,r,b,ink,fill});}
 U platform_random(U maximum)override{return p_.call(0x466760,{maximum}).eax;}
 U text_width(U font,std::string_view text)override{return p_.call(0x437c20,{font},{text}).eax;}
 MenuPresentationRegisters draw_formatted(U x,U y,U ink,std::string_view text,bool number,Q value)override{std::vector<U> words{x,y,ink};if(number){words.push_back(U(value));words.push_back(U(value>>32));}auto out=p_.call(0x437f00,std::move(words),{text});return {out.eax,out.ecx};}
 U now_count(U flag)override{return p_.call(0x437fd0,{flag}).eax;}
 void sleep_ms(U milliseconds)override{p_.call(0x69722c,{milliseconds});}
 void change_stage_resources(std::int32_t)override{throw std::logic_error("Gameplay presentation resource dispatch belongs to lifecycle services");}
 void present_component(MenuStartComponent)override{throw std::logic_error("Gameplay presentation cannot dispatch a menu body");}
 void draw_help(std::string_view,std::string_view)override{throw std::logic_error("Gameplay presentation has no menu help call");}
 void move_cursor(U,U)override{throw std::logic_error("Gameplay FPS is owned by explicit frame callback");}
 void present_cursor()override{throw std::logic_error("Gameplay throttle is owned by Outer");}
 void menu_body(U)override{throw std::logic_error("Gameplay presentation cannot dispatch a menu body");}
 void replay_ui_route()override{throw std::logic_error("Replay UI is owned by menu runtime");}
};
class GameplayHostBossSprites final:public BossSpriteServices {
 GameplayHostPlatform& p_;GameplayHostPresentation& drawing_;const EffectShake& shake_;unsigned precision_;
public:
 GameplayHostBossSprites(GameplayHostPlatform&p,GameplayHostPresentation&d,const EffectShake&s,unsigned bits):p_(p),drawing_(d),shake_(s),precision_(bits){}
 void blend(std::uint32_t m,std::uint32_t a)override{drawing_.blend(m,a);}
 void sprite(std::uint64_t x,std::uint64_t y,std::uint64_t s,std::uint64_t a,std::uint32_t g,std::uint32_t t,std::uint32_t f)override{present_menu_sprite_game(x,y,s,a,g,t,f,shake_,precision_,drawing_);}
 std::uint32_t sprite_handle(std::uint32_t address)const override{p_.memory.reconcile();return p_.memory.word(address);}
};
class GameplayHostMeshes final:public BackgroundMeshPresentationServices {
 GameplayHostPlatform& p_;BackgroundMeshPool& pool_;
public:
 GameplayHostMeshes(GameplayHostPlatform&p,BackgroundMeshPool&pool):p_(p),pool_(pool){}
 std::int32_t set_clip(std::int32_t l,std::int32_t t,std::int32_t r,std::int32_t b)override{return static_cast<std::int32_t>(p_.call(0x40dd90,{std::uint32_t(l),std::uint32_t(t),std::uint32_t(r),std::uint32_t(b)}).eax);}
 std::int32_t draw_triangles(const std::uint8_t* vertices,std::uint32_t count,std::uint32_t graph,std::uint32_t transparent)override{
  const auto start=reinterpret_cast<std::uintptr_t>(pool_.meshes.data()),at=reinterpret_cast<std::uintptr_t>(vertices);if(at<start||at-start>=sizeof(pool_.meshes))throw std::logic_error("Mesh vertices must reference the shared pool");const auto address=0xa82f90u+static_cast<std::uint32_t>(at-start);return static_cast<std::int32_t>(p_.call(0x41dd00,{address,count,graph,transparent}, {},std::vector<std::uint8_t>(vertices,vertices+count*3u*24u)).eax);
 }
};
}
