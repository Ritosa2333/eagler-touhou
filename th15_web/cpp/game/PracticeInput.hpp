// Generated from purple shared tools (MIT). Algorithm and drawing order remain source-owned.
#pragma once
#include "Types.hpp"
namespace th15 {
struct PracticeInput {
 bool enable_auto_shoot=false;int shoot_key_DIK=-1;bool last_is_auto_shoot_key_down=false,is_auto_shooting=false,is_th128=false;
 bool disable_xkey=false,disable_Ckey_at_same_time=true,disable_shiftkey=false,force_shiftkey=false,disable_zkey=false;
 bool enable_fast_retry=false;int fast_retry_count_down=0;static constexpr int fast_retry_cout_down_max=15;
 static bool is_key_down(u8 b){return (b&0x80)!=0;}
 void apply(u8* state){
    if (enable_auto_shoot) {
        bool cur_isdown = is_key_down(state[shoot_key_DIK]);
        bool last_isdown = last_is_auto_shoot_key_down;
        last_is_auto_shoot_key_down = cur_isdown;
        if (cur_isdown && !last_isdown) {
            is_auto_shooting = !is_auto_shooting;
        }
        if (is_auto_shooting) {
            bool is_other_down = false;
            is_other_down |= is_key_down(state[90]);
            is_other_down |= is_key_down(state[88]);
            is_other_down |= is_key_down(state[67]);
            is_other_down |= is_key_down(state[68]);
            is_other_down |= is_key_down(state[27]);
            is_other_down |= is_key_down(state[82]);
            is_other_down |= is_key_down(state[81]);
            if (is_other_down) {
                is_auto_shooting = false;
            } else {
                if (is_th128) {
                    state[67] = 0x80;
                } else {
                    state[90] = 0x80;
                }
            }
        }
    }

    if (disable_xkey) {
        state[88] = 0x0;
    }
    if (disable_xkey && disable_Ckey_at_same_time) {
        state[67] = 0x0;
    }
    if (disable_shiftkey) {
        state[160] = 0x0;
        state[161] = 0x0;
    }
    if (force_shiftkey)
    {
        state[160] = 0x80;
        state[161] = 0x80;
    }
    if (disable_zkey) {
        state[90] = 0x0;
    }

 if(fast_retry_count_down){if(fast_retry_count_down<=fast_retry_cout_down_max)state[27]=0x80;if(fast_retry_count_down<=1)state[82]=0x80;}
 }
 void begin_retry(int mode){if(mode&&enable_fast_retry)fast_retry_count_down=fast_retry_cout_down_max;}
 void gui_tick(){if(fast_retry_count_down)fast_retry_count_down--;}
 void reset(){last_is_auto_shoot_key_down=is_auto_shooting=false;fast_retry_count_down=0;}
};
}
