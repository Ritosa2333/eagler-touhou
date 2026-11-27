// th16 应用：SDL/GLES 宿主——打开 th16.dat，把 GraphicsDevice 挂为 GameSession 的 RenderDevice，
// 每个逻辑帧由绘制链（0x45a8a0 帧序）直接向设备出图，最后 Present。
#pragma once
#include "AudioDevice.hpp"
#include "FontPainter.hpp"
#include "GraphicsDevice.hpp"
#include "../game/FrameClock.hpp"
#include "../game/GameSession.hpp"

#include <string>
#include <vector>

#if defined(__EMSCRIPTEN__) || defined(TH16_HAS_SDL)
#include <SDL3/SDL.h>
#endif

namespace th16::sdl {

class Application {
public:
    GraphicsDevice graphics;
    AudioDevice audio;
    FontPainter fonts;
    GameSession session;
    FrameClock clock;
    std::string failure;
    unsigned frames = 0;
    bool initialized = false;
    // 启动器把 OGG 乐曲挂在 /bgm-ogg（而非 th16.dat 同目录 /music）：非空则覆盖 AudioDevice::music_directory
    std::string music_directory;

    // 便捷访问（BrowserRuntime 导出用）
    Supervisor& supervisor() { return session.supervisor; }
    TitleScene& title() { return session.title; }

    // 打开 th16.dat / 建图形与音频设备 / 装载启动幕。无音频设备（无 SDL 或打不开）不致命：仅静音
    bool initialize(const char* archive_path);
    // 固定一个逻辑步（含绘制链）+ Present（测试/步进用）。返回 false = 致命错误。
    bool step();
    // 按墙钟推进（rAF 时间戳秒），可能执行 0..kMaxSteps 个逻辑步（每步都跑绘制链——0x409f90 等链带逻辑），
    // 只 Present 最后一帧
    bool step_at(double wall_seconds);

    // 启动器托管输入（eagler-touhou/1 keyboard / touch-controls 命令）：与 SDL 物理键盘 OR 后进 0x402130 快照
    void set_hosted_key(const char* dom_code, bool down);
    void clear_hosted_keys();
    struct TouchControls {
        bool fire = false, focus = false;
        u32 bomb_serial = 0, escape_serial = 0;
        i32 joystick_x = 0, joystick_y = 0;   // 启动器给的有符号 16 位量
        u32 pulse = 0;
        void update(bool fire_on, bool focus_on, u32 bomb, u32 escape, i32 x, i32 y);
        u32 input_bits();
    } touch_controls;

private:
    std::vector<u8> archive_bytes;
    u8 hosted_keys[256] = {};
#if defined(__EMSCRIPTEN__) || defined(TH16_HAS_SDL)
    SDL_Gamepad* gamepad = nullptr;   // 0x401c30 / 0x4018e0：首个手柄（joyGetPosEx(0)）；按钮号取 SDL 标准布局序
#endif

    void sample_input();
};

}  // namespace th16::sdl
