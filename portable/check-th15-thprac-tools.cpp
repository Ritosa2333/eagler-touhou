// Typed shared-tool regression. No claim of browser/audio/device equivalence.
#include <eagler/thprac/PracticeKeyMonitor.hpp>
#include <eagler/thprac/PracticeInput.hpp>
#include "../th15_web/cpp/game/Types.hpp"
#include <eagler/thprac/PracticeSpeed.hpp>
#include "../th15_web/cpp/game/PracticeCadence.hpp"
#include <cassert>
#include <iostream>
#include <memory>
#include <vector>
using namespace th15;
int main(){
 eagler::thprac::PracticeKeyMonitor monitor;monitor.g_record_key_aps=true;monitor.record(15,0x19);
 assert(monitor.g_aps_cur==3&&monitor.g_keys_down[eagler::thprac::Key_Up]&&monitor.g_keys_down[eagler::thprac::Key_Z]&&monitor.g_keys_down[eagler::thprac::Key_Shift]);
 assert(!monitor.g_key_mask[eagler::thprac::Key_C]&&!monitor.g_key_mask[eagler::thprac::Key_D]);monitor.record(15,0);
 assert(monitor.g_aps_cur==6);
 assert(monitor.csv()=="frame,aps,up,down,left,right,Z,X,C,D,Ctrl,Shift\n1,3,O,-,-,-,O,-,-,-,-,O\n2,6,-,-,-,-,-,-,-,-,-,-\n");
 monitor.clear_record();assert(monitor.g_recorded_aps.empty()&&monitor.g_aps_cur==6);monitor.g_record_key_aps=false;
 monitor={};for(int i=0;i<60;i++)monitor.record(15,1);assert(monitor.g_aps_cur==1);monitor.record(15,1);assert(monitor.g_aps_cur==0);monitor.record(15,0);assert(monitor.g_aps_cur==1);
 eagler::thprac::PracticeInput input;u8 state[256]{};state[90]=state[88]=state[67]=state[160]=state[161]=128;
 input.disable_xkey=input.disable_shiftkey=input.disable_zkey=true;input.force_shiftkey=true;input.apply(state);
 assert(!state[90]&&!state[88]&&!state[67]&&state[160]&&state[161]);
 input={};input.enable_auto_shoot=true;input.shoot_key_DIK=65;std::fill(std::begin(state),std::end(state),0);state[65]=128;input.apply(state);assert(input.is_auto_shooting&&state[90]);
 state[90]=0;input.apply(state);assert(input.is_auto_shooting&&state[90]);state[90]=0;state[65]=0;input.apply(state);assert(input.is_auto_shooting);state[90]=128;input.apply(state);assert(!input.is_auto_shooting);
 input.enable_fast_retry=true;input.begin_retry(0);assert(!input.fast_retry_count_down);input.begin_retry(1);assert(input.fast_retry_count_down==15);
 for(int i=15;i>0;i--){std::fill(std::begin(state),std::end(state),0);input.apply(state);assert(state[27]==128);assert(bool(state[82])==(i==1));input.gui_tick();}assert(!input.fast_retry_count_down);input.reset();assert(!input.is_auto_shooting&&!input.last_is_auto_shoot_key_down);
 eagler::thprac::PracticeSpeed speed;assert(speed.interval(false,false,false,false)==1./60.);assert(speed.interval(true,false,true,false)==1./15.);speed.fps=120;speed.fps_replay_fast=180;assert(speed.interval(true,true,false,false)==1./180.);speed.fps_debug_acc=1;assert(speed.interval(false,false,false,true)==1./180.);speed.fps_replay_fast=1260;assert(speed.interval(false,false,false,true)==1./9999.);
 PracticeCadence cadence;assert(cadence.advance(.1)==4);cadence.reset();cadence.period=1./120.;assert(cadence.advance(1./60.)==2);cadence.reset();cadence.period=1./15.;assert(cadence.advance(1./30.)==0);assert(cadence.advance(1./30.)==1);
 std::cout<<"Purple APS/CSV, input precedence/latch/retry, speed cadence passed\n";
}
