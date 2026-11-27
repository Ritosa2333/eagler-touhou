#include "Application.hpp"

#include "../game/ScancodeMap.hpp"
#include "../game/TitleScene.hpp"

#include <cstdio>
#include <cstring>

#if defined(__EMSCRIPTEN__) || defined(TH16_HAS_SDL)
#include <SDL3/SDL.h>
#endif

namespace th16::sdl {

namespace {
bool read_file(const char* path, std::vector<u8>& out) {
    FILE* f = std::fopen(path, "rb");
    if (!f) return false;
    std::fseek(f, 0, SEEK_END);
    const long size = std::ftell(f);
    std::fseek(f, 0, SEEK_SET);
    out.resize(size_t(size));
    const size_t got = std::fread(out.data(), 1, out.size(), f);
    std::fclose(f);
    return got == out.size();
}

}  // namespace

// 0x401d50 / 0x402130：物理键盘（SDL）与启动器托管键（DOM code）先折成 DIK 快照，再按 DirectInput 分支合成输入位；
// 手柄：0x401c30 按钮快照交给 GameSession（0x4018e0 按表合成），左摇杆按原版“中心 ± 1/4 量程”阈值出方向位；
// 原版不读 POV 帽，这里把 SDL 标准布局的十字键一并映射到方向（现代手柄十字键多报为帽）
void Application::sample_input() {
    u8 keys[256];
    std::memcpy(keys, hosted_keys, sizeof(keys));
    u32 held = touch_controls.input_bits();
    u32 buttons = 0;
#if defined(__EMSCRIPTEN__) || defined(TH16_HAS_SDL)
#ifndef __EMSCRIPTEN__
    // Browser keys have one owner: the DOM/launcher bridge. SDL may retain a
    // pre-launch keydown whose keyup the bridge consumes, masking later edges.
    int n = 0;
    if (const bool* physical = SDL_GetKeyboardState(&n)) {
        for (int i = 0; i < n && i < SDL_SCANCODE_COUNT; ++i) {
            if (!physical[i]) continue;
            const i16 di = sdl_to_di(SDL_Scancode(i));
            if (di >= 0) keys[di] = 0x80;
        }
    }
#endif
    if (gamepad == nullptr) {
        int count = 0;
        if (SDL_JoystickID* ids = SDL_GetGamepads(&count)) {
            if (count > 0) gamepad = SDL_OpenGamepad(ids[0]);
            SDL_free(ids);
        }
    } else if (!SDL_GamepadConnected(gamepad)) {
        SDL_CloseGamepad(gamepad);
        gamepad = nullptr;
    }
    if (gamepad != nullptr) {
        for (int i = 0; i < SDL_GAMEPAD_BUTTON_COUNT && i < 32; ++i) {
            if (SDL_GetGamepadButton(gamepad, SDL_GamepadButton(i))) buttons |= 1u << i;
        }
        const i32 x = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTX);
        const i32 y = SDL_GetGamepadAxis(gamepad, SDL_GAMEPAD_AXIS_LEFTY);
        if (x < -16384) held |= InputState::kLeft;
        if (x > 16384) held |= InputState::kRight;
        if (y < -16384) held |= InputState::kUp;
        if (y > 16384) held |= InputState::kDown;
        if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_UP)) held |= InputState::kUp;
        if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_DOWN)) held |= InputState::kDown;
        if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_LEFT)) held |= InputState::kLeft;
        if (SDL_GetGamepadButton(gamepad, SDL_GAMEPAD_BUTTON_DPAD_RIGHT)) held |= InputState::kRight;
    }
#endif
    std::memcpy(session.keyboard_state, keys, sizeof(keys));
    session.host_input = held | keyboard_input_bits(keys);
    session.pad_buttons = buttons;
}

void Application::set_hosted_key(const char* dom_code, bool down) {
    const i16 di = dom_code_to_di(dom_code);
    if (di >= 0) hosted_keys[di] = down ? 0x80 : 0;
}

void Application::clear_hosted_keys() {
    std::memset(hosted_keys, 0, sizeof(hosted_keys));
    touch_controls = {};
}

u32 Application::TouchControls::input_bits() {
    u32 bits = pulse;
    pulse = 0;
    if (fire) bits |= InputState::kShot;
    if (focus) bits |= InputState::kSlow;
    if (joystick_x < -16384) bits |= InputState::kLeft;
    if (joystick_x > 16384) bits |= InputState::kRight;
    if (joystick_y < -16384) bits |= InputState::kUp;
    if (joystick_y > 16384) bits |= InputState::kDown;
    return bits;
}

void Application::TouchControls::update(bool fire_on, bool focus_on, u32 bomb, u32 escape, i32 x, i32 y) {
    fire = fire_on;
    focus = focus_on;
    joystick_x = x;
    joystick_y = y;
    // 序号变化 = 一次按下：下一逻辑帧给一帧脉冲（菜单确定/取消与炸弹都按边沿判定）
    if (bomb != bomb_serial) { bomb_serial = bomb; pulse |= InputState::kBomb; }
    if (escape != escape_serial) { escape_serial = escape; pulse |= InputState::kEscape; }
}

bool Application::initialize(const char* archive_path) {
    if (initialized) return true;
    if (!read_file(archive_path, archive_bytes)) { failure = std::string("cannot read ") + archive_path; return false; }
    if (!session.open(archive_bytes.data(), u32(archive_bytes.size()))) { failure = session.failure; return false; }
    // 0x44af80 demo 路径相对游戏目录：demo/demoN.rpy（与 th16.dat 同级）
    if (const char* slash = std::strrchr(archive_path, '/')) {
        session.demo_directory.assign(archive_path, slash);
    } else if (const char* slash = std::strrchr(archive_path, '\\')) {
        session.demo_directory.assign(archive_path, slash);
    } else {
        session.demo_directory = ".";
    }
    // 0x4497e0：scoreth16.dat 在游戏目录（0x479a65 切到该目录后相对路径读写）；0x43c050：th16.cfg 同目录
    session.score_path = session.replay_directory() + "/scoreth16.dat";
    session.config_path = session.replay_directory() + "/th16.cfg";
    session.load_config();
    graphics.animations = &session.animations;
    if (!graphics.initialize()) { failure = graphics.error; return false; }

    session.attach_device(&graphics);
#if defined(__EMSCRIPTEN__) || defined(TH16_HAS_SDL)
    // 0x401c30 手柄：无手柄或子系统不可用时不致命，按钮位图保持 0
    if (!SDL_InitSubSystem(SDL_INIT_GAMEPAD)) std::fprintf(stderr, "th16: gamepad disabled: %s\n", SDL_GetError());
#endif
    // 原版 SoundManager（DAT_004d9e10）的 DirectSound 侧：thbgm.dat 与 th16.dat 同目录（0x43adc0 以 "thbgm.dat" 打开）
    audio.thbgm_path = session.demo_directory + "/thbgm.dat";
    audio.music_directory = music_directory.empty() ? session.demo_directory + "/music" : music_directory;
    audio.music = &session.music;
    // 打不开音频时逻辑（命令队列 / 音效队列）照常推进，只是无输出——与 harness 同口径，逐帧行为不变
    if (audio.initialize(session.archive())) {
        session.sound.output = &audio.sfx_output();
        session.music.output = &audio.music_output();
    } else {
        std::fprintf(stderr, "th16: audio disabled: %s\n", audio.error.c_str());
    }
    session.on_loaded = [this](AnmResource& res) -> bool {
        graphics.unload(res);
        if (!graphics.preload(res)) { failure = graphics.error; session.failure = graphics.error; return false; }
        return true;
    };
    session.on_before_draw = [this]() {
        fonts.flush(graphics, session.animations, session.text_into_texture);
    };

    if (!session.begin_boot(GraphicsDevice::kRenderScale == 2)) { failure = session.failure; return false; }
    initialized = true;
    return true;
}

bool Application::step() {
    if (!initialized) return false;
    sample_input();
    if (!session.logic_step()) { failure = session.failure; return false; }
    audio.pump();
    graphics.present();
    ++frames;
    if (!graphics.error.empty()) failure = graphics.error;
    return failure.empty();
}

bool Application::step_at(double wall_seconds) {
    if (!initialized) return false;
    const u32 steps = clock.begin_frame(wall_seconds);
    sample_input();
    session.wall_clock_seconds = wall_seconds;
    for (u32 i = 0; i < steps; ++i) {
        if (!session.logic_step()) { failure = session.failure; return false; }
    }
    audio.pump();
    graphics.present();
    ++frames;
    if (!graphics.error.empty()) failure = graphics.error;
    return failure.empty();
}

}  // namespace th16::sdl
