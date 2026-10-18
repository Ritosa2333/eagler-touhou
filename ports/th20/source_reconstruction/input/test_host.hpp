#pragma once
// Used only by test executables. Production Host calls actual Win32 APIs.
#include "input.hpp"
#include <cstring>
#include <vector>
struct RecordedHost : th20::source::input::Host {
    std::uint8_t keys[256]{}, last_set[256]{};
    BOOL keyboard_status=TRUE, set_status=TRUE;
    XINPUT_STATE xbox[4]{};
    DWORD xbox_status[4]={ERROR_DEVICE_NOT_CONNECTED,ERROR_DEVICE_NOT_CONNECTED,ERROR_DEVICE_NOT_CONNECTED,ERROR_DEVICE_NOT_CONNECTED};
    JOYINFOEX joysticks[2]{};
    MMRESULT joystick_status[2]={JOYERR_UNPLUGGED,JOYERR_UNPLUGGED};
    JOYCAPSW caps[2]{};
    MMRESULT caps_status[2]{};
    DIJOYSTATE2 pad{};
    HRESULT poll_status=S_OK, state_status=S_OK, acquire_status=S_OK;
    unsigned inputlost_count=0, acquire_calls=0;
    std::vector<unsigned> trace;
    void reset_trace() { trace.clear(); acquire_calls=0; }
    BOOL keyboard(std::uint8_t* output) override {
        trace.push_back(1); if(keyboard_status) std::memcpy(output,keys,256); return keyboard_status;
    }
    BOOL set_keyboard(std::uint8_t* input) override { trace.push_back(2);std::memcpy(last_set,input,256);return set_status; }
    DWORD xinput(DWORD index,XINPUT_STATE* output) override {
        trace.push_back(10+index); if(index>=4) return ERROR_BAD_ARGUMENTS;
        if(xbox_status[index]==ERROR_SUCCESS) *output=xbox[index]; return xbox_status[index];
    }
    MMRESULT joystick(UINT index,JOYINFOEX* output) override {
        trace.push_back(20+index); if(joystick_status[index]==JOYERR_NOERROR) *output=joysticks[index];return joystick_status[index];
    }
    MMRESULT joystick_caps(UINT index,JOYCAPSW* output) override {
        trace.push_back(30+index);if(caps_status[index]==JOYERR_NOERROR) *output=caps[index];return caps_status[index];
    }
    HRESULT poll(IDirectInputDevice8W*) override { trace.push_back(40);return poll_status; }
    HRESULT acquire(IDirectInputDevice8W*) override {
        trace.push_back(41);return acquire_calls++<inputlost_count?DIERR_INPUTLOST:acquire_status;
    }
    HRESULT device_state(IDirectInputDevice8W*,DIJOYSTATE2* output) override {
        trace.push_back(42);if(SUCCEEDED(state_status)) *output=pad;return state_status;
    }
};
