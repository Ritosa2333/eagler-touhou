// Generated from purple shared tools (MIT). Algorithm and drawing order remain source-owned.
#pragma once
#include <cstdint>
#include <deque>
#include <vector>
#include <string>
namespace th15 {
enum THKey { 
    Key_Up,
    Key_Down,
    Key_Left,
    Key_Right,
    Key_Z,
    Key_X,
    Key_C,
    Key_D,
    Key_Ctrl,
    Key_Shift,
    END
};


struct PracticeKeyMonitor {
 std::vector<int> g_recorded_aps;
 std::vector<uint16_t> g_recorded_keys;
 bool g_keys_down[END]{};uint32_t g_key_mask[END]{};int g_aps_cur=0;bool g_record_key_aps=false;std::deque<uint32_t> keys_per_sec;
void record(int ver, uint32_t cur_key)
{
    switch (ver) {
    default:
    case 6:
    case 7:
    case 8:
    case 10:
        g_key_mask[Key_Shift] = 0x4;
        g_key_mask[Key_Z] = 0x1;
        g_key_mask[Key_X] = 0x2;
        g_key_mask[Key_Ctrl] = 0x100;
        g_key_mask[Key_Up] = 0x10;
        g_key_mask[Key_Down] = 0x20;
        g_key_mask[Key_Left] = 0x40;
        g_key_mask[Key_Right] = 0x80;
        break;
    case 11:
    case 12:
        g_key_mask[Key_Shift] = 0x8;
        g_key_mask[Key_Z] = 0x1;
        g_key_mask[Key_X] = 0x2;
        g_key_mask[Key_Ctrl] = 0x200;
        g_key_mask[Key_Up] = 0x10;
        g_key_mask[Key_Down] = 0x20;
        g_key_mask[Key_Left] = 0x40;
        g_key_mask[Key_Right] = 0x80;
        break;
    case 128:
        g_key_mask[Key_Shift] = 0x8;
        g_key_mask[Key_Z] = 0x1;
        g_key_mask[Key_X] = 0x2;
        g_key_mask[Key_C] = 0x200;
        g_key_mask[Key_Up] = 0x10;
        g_key_mask[Key_Down] = 0x20;
        g_key_mask[Key_Left] = 0x40;
        g_key_mask[Key_Right] = 0x80;
        break;
    case 13:
    case 16:
        g_key_mask[Key_Shift] = 0x8;
        g_key_mask[Key_Z] = 0x1;
        g_key_mask[Key_X] = 0x2;
        g_key_mask[Key_C] = 0xA00;
        g_key_mask[Key_Ctrl] = 0x200;
        g_key_mask[Key_Up] = 0x10;
        g_key_mask[Key_Down] = 0x20;
        g_key_mask[Key_Left] = 0x40;
        g_key_mask[Key_Right] = 0x80;
        break;
    case 14:
    case 15:
    case 17:
        g_key_mask[Key_Shift] = 0x8;
        g_key_mask[Key_Z] = 0x1;
        g_key_mask[Key_X] = 0x2;
        g_key_mask[Key_Ctrl] = 0x200;
        g_key_mask[Key_Up] = 0x10;
        g_key_mask[Key_Down] = 0x20;
        g_key_mask[Key_Left] = 0x40;
        g_key_mask[Key_Right] = 0x80;
        break;
    case 18:
        g_key_mask[Key_Shift] = 0x8;
        g_key_mask[Key_Z] = 0x1;
        g_key_mask[Key_X] = 0x2;
        g_key_mask[Key_C] = 0x400;
        g_key_mask[Key_D] = 0x800;
        g_key_mask[Key_Up] = 0x10;
        g_key_mask[Key_Down] = 0x20;
        g_key_mask[Key_Left] = 0x40;
        g_key_mask[Key_Right] = 0x80;
        break;
    case 20:
        g_key_mask[Key_Shift] = 0x8;
        g_key_mask[Key_Z] = 0x1;
        g_key_mask[Key_X] = 0x4;
        g_key_mask[Key_Up] = 0x10;
        g_key_mask[Key_Down] = 0x20;
        g_key_mask[Key_Left] = 0x40;
        g_key_mask[Key_Right] = 0x80;
        break;
    }
    for (int i = 0; i < END; i++) {
        if (g_key_mask[i])
            g_keys_down[i] = ((cur_key & g_key_mask[i]) == g_key_mask[i]);
    }
    uint32_t key_cur = 0;
    for (int i = 0; i < END; i++) {
        if (g_keys_down[i])
            key_cur |= 1 << i;
    } // not use zun's keycode


    
    uint32_t key_last = 0;
    while (keys_per_sec.size() >= 60) {
        key_last = keys_per_sec.front();
        keys_per_sec.pop_front();
    }
    keys_per_sec.push_back(key_cur);
    g_aps_cur = 0;
    for (auto key : keys_per_sec) {
        if (key != key_last) {
            uint32_t diff = key ^ key_last;
            uint32_t diff_cnt = 0;
            for (int i = 0; i < END; i++) {
                if (diff & (1 << i))
                    diff_cnt++;
            }
            g_aps_cur += diff_cnt;
            key_last = key;
        }
    }
    if (g_record_key_aps){
        g_recorded_aps.push_back(g_aps_cur);
        g_recorded_keys.push_back((uint16_t)key_cur);
    }
}


 void clear_record(){g_recorded_aps.clear();g_recorded_keys.clear();}
 std::string csv()const{std::string out="frame,aps,up,down,left,right,Z,X,C,D,Ctrl,Shift\n";for(size_t j=0;j<g_recorded_aps.size();j++){out+=std::to_string(j+1)+","+std::to_string(g_recorded_aps[j]);for(int i=0;i<END;i++)out+=(g_recorded_keys[j]&(1<<i))?",O":",-";out+="\n";}return out;}
};
}
