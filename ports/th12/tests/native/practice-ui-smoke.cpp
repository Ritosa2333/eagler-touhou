#include "game/Practice.hpp"
#include "game/PracticeTools.hpp"
#include "game/EclInterpreter.hpp"
#include "game/GameState.hpp"
#include "imgui.h"
#include "imgui_internal.h"
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <cstring>
extern "C" {
int th12_practice_ui_init(const unsigned char*,int,int);
void th12_practice_ui_key(int,int);
void th12_practice_ui_context(int,int,int,int,int);
void th12_practice_ui_mobile(int,int);
void th12_practice_ui_tick(int);
int th12_practice_ui_pointer(int,int,float,float,int);
int th12_practice_ui_capture();
int th12_practice_ui_flags();
int th12_practice_ui_action();
void th12_practice_ui_params(const int32_t*,int);
const int32_t* th12_practice_ui_parameters();
void th12_practice_ui_cancel();
void th12_practice_ui_shutdown();
}
void require(bool value,const char* message){if(!value){std::fprintf(stderr,"FAIL: %s\n",message);std::exit(1);}}
void tick(){th12_practice_ui_tick(0);}
void key(int code){th12_practice_ui_key(code,1);tick();th12_practice_ui_key(code,0);tick();}
ImGuiWindow* window(const char* name){auto* value=ImGui::FindWindowByName(name);require(value&&value->Active&&!value->Hidden,name);return value;}
void tap(int id,float x,float y){require(th12_practice_ui_pointer(0,id,x,y,0),"native window must own its down");tick();require(th12_practice_ui_pointer(2,id,x,y,0),"native window must retain its up");tick();tick();}
int main(){
  using namespace th12::practice;
  setEnabled(true);require(th12_practice_ui_init(nullptr,0,1)==1,"native ImGui init");th12_practice_ui_context(2,1,0,1,1);tick();
  key(112);require(tools()==0,"F1 outside the cheat menu must not change gameplay");key(8);require(th12_practice_ui_flags()&1,"Backspace opens original cheat window");
  key(112);require(tools()&Invulnerable,"F1 uses original invincibility bit");key(85);require(tools()&EnemyInvulnerable,"U toggles enemy invincibility");
  setReplay(true);th12_practice_ui_context(2,1,1,1,1);key(112);require(tools()&Invulnerable,"Replay must reject live cheats");setReplay(false);th12_practice_ui_context(2,1,0,1,1);
  th12_practice_ui_mobile(1,1);tick();tick();
  auto* advanced=window("Advanced Options###th12-thprac-advanced");auto* cheats=window("Mod Menu###th12-thprac-overlay");
  require(cheats->Pos.x==8&&cheats->Pos.y==8,"common mobile cheat header position");require(advanced->Pos.y==8&&advanced->Pos.x>cheats->Pos.x,"paired common mobile headers");
  require(!th12_practice_ui_capture(),"collapsed advanced panel cannot capture gameplay");
  require(!th12_practice_ui_pointer(0,91,630,470,0),"game down belongs to game");require(!th12_practice_ui_pointer(1,91,advanced->Pos.x+10,20,0),"game drag crossing a panel cannot be stolen");th12_practice_ui_pointer(2,91,630,470,0);
  // Cancel a different game finger while the original native header is held.
  const float x=advanced->Pos.x+30,y=advanced->Pos.y+14;
  require(th12_practice_ui_pointer(0,7,x,y,0),"header owns touch");tick();require(!th12_practice_ui_pointer(3,92,0,0,0),"unrelated cancel is not a UI gesture");
  require(th12_practice_ui_pointer(2,7,x,y,0),"unrelated cancel must preserve the active header finger");tick();tick();
  require(th12_practice_ui_capture(),"expanded common Advanced captures gameplay");
  th12_practice_ui_mobile(1,0);tick();require(!th12_practice_ui_capture(),"hidden group releases capture");th12_practice_ui_mobile(1,1);tick();tick();require(!th12_practice_ui_capture(),"reopened group folds every panel");
  th12_practice_ui_tick(1);th12_practice_ui_tick(0);require(!th12_practice_ui_capture(),"navigation/confirm cannot unfold pointer-only headers");
  auto& settings=toolSettings();settings.input.disable_xkey=true;settings.input.force_shiftkey=true;
  int held=3,pressed=3;applyToolInput(held,pressed);require((held&3)==1&&(held&8),"shared input filtering affects native gameplay");
  setReplay(true);held=3;pressed=3;applyToolInput(held,pressed);require(held==3,"Replay uses recorded resolved input");setReplay(false);
  settings.force_boss_move_down=true;settings.boss_move_down_range=.5f;float center=100,range=80;bossMoveRange(center,range);require(center==120&&range==40,"boss range is the original purple half-range transform");
  unsigned char instruction[32]{};const float operands[]{0,100,160,80};std::memcpy(instruction+16,operands,sizeof operands);th12::Context context(instruction);th12::Enemy enemy;
  th12::command(enemy,context,404);require(enemy.clampY==120&&enemy.clampHeight==40,"purple range applies at the native ECL404 write");
  std::memset(instruction+16,0,16);th12::command(enemy,context,312);th12::command(enemy,context,312);require(enemy.clampY==120&&enemy.clampHeight==40,"repeated random movement must not shrink the range again");
  resetLockTimer();settings.lock_frame_pending=true;recordToolTick(0);recordToolTick(0);require(settings.lock_frames==1,"lock timer counts one native update pulse per tick");th12::command(enemy,context,412);require(settings.lock_frames==0,"boss-mode ECL resets the original lock timer");
  th12_practice_ui_mobile(0,0);settings.input.enable_fast_retry=true;key(82);require(th12_practice_ui_action()==3,"R issues a native fast-retry action");
  key(123);key(82);require(th12_practice_ui_action()==0,"Advanced capture prevents gameplay retry");key(123);
  th12_practice_ui_context(1,1,0,1,1);tick();tick();require(th12_practice_ui_capture(),"native Practice menu owns input");
  int32_t words[20]{1,1,0,0,0,0,0,0,99,-3,99,-2,999,999999,9999999};th12_practice_ui_params(words,20);tick();
  const auto* p=th12_practice_ui_parameters();require(p[8]==9&&p[9]==0&&p[10]==9&&p[11]==0&&p[12]==400&&p[13]==999990&&p[14]==999999,"native numeric edits retain the purple parameter bounds");
  th12_practice_ui_cancel();th12_practice_ui_shutdown();std::puts("PASS purple ImGui hotkeys, mobile headers, touch ownership, shared input and replay isolation");
}
