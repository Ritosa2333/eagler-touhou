// XInput surface for the Emscripten/SDL3 (TH_SDL3) build. Declarations only;
// the wasm input host (Phase 2/3) supplies the implementation.
#pragma once

#include "windows.h"

#define XINPUT_GAMEPAD_DPAD_UP 0x0001
#define XINPUT_GAMEPAD_DPAD_DOWN 0x0002
#define XINPUT_GAMEPAD_DPAD_LEFT 0x0004
#define XINPUT_GAMEPAD_DPAD_RIGHT 0x0008
#define XINPUT_GAMEPAD_START 0x0010
#define XINPUT_GAMEPAD_BACK 0x0020
#define XINPUT_GAMEPAD_LEFT_THUMB 0x0040
#define XINPUT_GAMEPAD_RIGHT_THUMB 0x0080
#define XINPUT_GAMEPAD_LEFT_SHOULDER 0x0100
#define XINPUT_GAMEPAD_RIGHT_SHOULDER 0x0200
#define XINPUT_GAMEPAD_A 0x1000
#define XINPUT_GAMEPAD_B 0x2000
#define XINPUT_GAMEPAD_X 0x4000
#define XINPUT_GAMEPAD_Y 0x8000

typedef struct _XINPUT_GAMEPAD {
    WORD wButtons;
    BYTE bLeftTrigger;
    BYTE bRightTrigger;
    SHORT sThumbLX;
    SHORT sThumbLY;
    SHORT sThumbRX;
    SHORT sThumbRY;
} XINPUT_GAMEPAD, *PXINPUT_GAMEPAD;

typedef struct _XINPUT_STATE {
    DWORD dwPacketNumber;
    XINPUT_GAMEPAD Gamepad;
} XINPUT_STATE, *PXINPUT_STATE;

typedef struct _XINPUT_CAPABILITIES {
    BYTE Type;
    BYTE SubType;
    WORD Flags;
    XINPUT_GAMEPAD Gamepad;
    struct {
        WORD usMotorSpeed;
    } Vibration;
} XINPUT_CAPABILITIES, *PXINPUT_CAPABILITIES;

#define XUSER_MAX_COUNT 4
#define ERROR_DEVICE_NOT_CONNECTED 1167L

extern "C" {
DWORD WINAPI XInputGetState(DWORD, XINPUT_STATE*);
DWORD WINAPI XInputGetCapabilities(DWORD, DWORD, XINPUT_CAPABILITIES*);
}
