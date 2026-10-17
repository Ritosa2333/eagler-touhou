#include <eagler/thprac/PracticeInput.hpp>
#include <eagler/thprac/PracticeKeyMonitor.hpp>
#include <eagler/thprac/PracticeSpeed.hpp>
#include <cassert>
#include <array>
using namespace eagler::thprac;
int main() {
    for (int title : {8,10,11,15}) {
        PracticeKeyMonitor keys;
        keys.g_record_key_aps=true;
        keys.record(title,title<11?0x115:0x219);
        assert(keys.g_keys_down[Key_Z] && keys.g_keys_down[Key_Shift]);
        assert(keys.g_keys_down[Key_Up] && keys.g_keys_down[Key_Ctrl]);
        assert(!keys.g_key_mask[Key_C] && !keys.g_key_mask[Key_D]);
        assert(keys.g_aps_cur==4);
        keys.record(title,0);
        assert(keys.g_aps_cur==8);
        assert(keys.csv().find("frame,aps,up,down,left,right,Z,X,C,D,Ctrl,Shift\n1,4,")==0);
        keys.clear_record();
        assert(keys.g_recorded_keys.empty() && keys.g_aps_cur==8);
    }
    PracticeInput input;
    std::array<std::uint8_t,256> state{};
    input.enable_auto_shoot=true; input.shoot_key_DIK=65;
    state[65]=128; input.apply(state.data()); assert(state[90]==128);
    state.fill(0); input.apply(state.data()); assert(state[90]==128);
    state.fill(0);state[88]=128;input.apply(state.data());assert(!input.is_auto_shooting);
    input.disable_xkey=input.disable_shiftkey=input.disable_zkey=true;
    state[67]=state[90]=state[160]=state[161]=128;
    input.apply(state.data());assert(!state[88]&&!state[67]&&!state[90]&&!state[160]&&!state[161]);
    input.force_shiftkey=true;input.apply(state.data());assert(state[160]==128&&state[161]==128);
    input.enable_fast_retry=true; input.begin_retry(0);assert(!input.fast_retry_count_down);
    input.begin_retry(1);
    for(int i=15;i>0;--i){state.fill(0);input.apply(state.data());assert(state[27]==128);assert(bool(state[82])==(i==1));input.gui_tick();}
    input.reset();assert(!input.fast_retry_count_down&&!input.is_auto_shooting);
    PracticeSpeed speed;
    assert(speed.interval(false,false,false,false)==1./60.);
    assert(speed.interval(true,false,true,false)==1./15.);
    speed.fps_replay_fast=180;assert(speed.interval(true,true,false,false)==1./180.);
    speed.fps_debug_acc=1;speed.fps_replay_fast=1260;assert(speed.interval(false,false,false,true)==1./9999.);
}
