#include <eagler/thprac/PracticeInput.hpp>
#include <eagler/thprac/PracticeKeyMonitor.hpp>
#include <eagler/thprac/PracticeSpeed.hpp>
#include <cassert>
#include <cstdio>
int main() {
    eagler::thprac::PracticeInput input;
    std::uint8_t state[256]{};
    state[90]=state[88]=state[67]=state[160]=state[161]=0x80;
    input.disable_xkey=input.disable_zkey=input.disable_shiftkey=true;
    input.force_shiftkey=true;
    input.apply(state);
    assert(!state[90] && !state[88] && !state[67]);
    assert(state[160]==0x80 && state[161]==0x80); // source order: force wins
    input.enable_fast_retry=true;
    input.begin_retry(0); assert(input.fast_retry_count_down==0);
    input.begin_retry(1); assert(input.fast_retry_count_down==15);
    for(int i=15;i>0;--i) {
        std::uint8_t retry[256]{};
        input.apply(retry);
        assert(retry[27]==0x80 && bool(retry[82])==(i==1));
        input.gui_tick();
    }
    assert(input.fast_retry_count_down==0);
    eagler::thprac::PracticeInput automatic;
    automatic.enable_auto_shoot=true;automatic.shoot_key_DIK=84;
    std::uint8_t physical[256]{};physical[84]=0x80;
    automatic.apply(physical);assert(physical[90] && automatic.is_auto_shooting);
    physical[84]=physical[90]=0;automatic.apply(physical);assert(physical[90]);
    physical[90]=0;physical[67]=0x80;automatic.apply(physical);
    assert(!physical[90]&&!automatic.is_auto_shooting);
    automatic.reset();assert(!automatic.is_auto_shooting && !automatic.last_is_auto_shoot_key_down);
    eagler::thprac::PracticeSpeed speed;
    assert(speed.interval(false,false,false,false)==1./60);
    speed.fps=90;assert(speed.interval(false,false,false,false)==1./90);
    assert(speed.interval(true,false,true,false)==1./15);
    speed.fps_replay_fast=240;assert(speed.interval(true,true,false,false)==1./240);
    speed.fps_replay_fast=1260;assert(speed.interval(true,true,false,false)==1./9999);
    speed.fps_debug_acc=1;assert(speed.interval(true,false,false,true)==1./9999);
    eagler::thprac::PracticeKeyMonitor monitor;
    monitor.g_record_key_aps=true;
    monitor.record(6,0x115);
    assert(monitor.g_keys_down[eagler::thprac::Key_Up]);
    assert(monitor.g_keys_down[eagler::thprac::Key_Z]);
    assert(monitor.g_keys_down[eagler::thprac::Key_Ctrl]);
    assert(monitor.g_keys_down[eagler::thprac::Key_Shift]);
    assert(monitor.g_aps_cur==4 && monitor.g_recorded_keys.size()==1);
    for(int i=0;i<60;++i) monitor.record(6,0x115);
    assert(monitor.g_aps_cur==0 && monitor.keys_per_sec.size()==60);
    assert(monitor.csv().starts_with("frame,aps,up,down,left,right,Z,X,C,D,Ctrl,Shift\n"));
    monitor.clear_record();
    assert(monitor.g_recorded_aps.empty() && monitor.g_recorded_keys.empty());
    std::puts("TH06 purple shared input, auto-shoot, speed, key mapping, APS and CSV: PASS");
}
