// Generated purple ElBgmTest and TH15 boss-range arithmetic (MIT).
// TH15 LF source sha256 2b412168cd8d71dd7ef8e1f79a1a5d33cf64f983792cbc9d0f4ccbc56e60ce03; shared LF sha256 f6fca603ab15b96c5bef2894989fb0791d7dd1a2b1926c54f4134054bb4881ca.
#pragma once
#include "Types.hpp"
namespace th15 {
inline constexpr u32 practice_stars_bgm_byte_offset=0x8fc768;
inline constexpr float practice_boss_range_default=0.5f;
inline void practice_boss_range(float* y_pos,float* y_range,float g_bossMoveDownRange){
float y_max = (*y_pos) + (*y_range) * 0.5f;
        float y_min2 = y_max - (*y_range) * (1.0f - g_bossMoveDownRange);
        *y_pos = (y_max + y_min2) * 0.5f;
        *y_range = (y_max - y_min2);
}
struct PracticeMusic {
 static constexpr u32 play_addr=0x44d413,stop_addr=0x43cb40,pause_addr=0x450efd,resume_addr=0x452a63,caller_addr=0xffffffff;
 bool mElStatus=false;int mLockBgmId=-1;
 bool suppress(bool hotkey_status,bool practice_status,u32 retn_addr,int bgm_param,u32 caller=caller_addr){

    bool hotkey = hotkey_status;
    bool is_practice = practice_status;

    switch (retn_addr) {
    case play_addr:
        if (caller == caller_addr) {
            if (mLockBgmId == -1)
                mLockBgmId = bgm_param;
            if (mLockBgmId != bgm_param) {
                mLockBgmId = -1;
                mElStatus = 0;
            } else if (!mElStatus && hotkey) {
                mElStatus = 1;
                return false;
            }
        }
        if (mLockBgmId >= 0 && mLockBgmId != bgm_param) {
            mLockBgmId = -1;
            mElStatus = 0;
        }
        break;
    case stop_addr:
        if (mLockBgmId >= 0) {
            mLockBgmId = -1;
            // Quitting or disabled
            if (!is_practice || !hotkey)
                mElStatus = 0;
        }
        break;
    case pause_addr:
        if (mLockBgmId >= 0) {
            if (hotkey)
                mElStatus = 1;
            else
                mElStatus = 0;
        }
        break;
    case resume_addr:
        if (mLockBgmId >= 0) {
            if (!mElStatus && hotkey) {
                mElStatus = 1;
                return false;
            }
        }
        break;
    default:
        break;
    }

    return mElStatus;
 }
};
}
