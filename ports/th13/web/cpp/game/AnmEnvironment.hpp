#pragma once
#include "GameRandom.hpp"
#include "Types.hpp"
#include <array>
namespace th13 {
class AnmResource;
// Scene-owned values and the game RNG are supplied by the game. No global memory
// addresses or graphics API handles are visible to animation instructions.
struct AnmEnvironment {
    GameRandom game_rng;
    GameRandom* shared_game_rng=nullptr;
    GameRandom& distortion_random() noexcept {
        return shared_game_rng ? *shared_game_rng : game_rng;
    }
    Vec3 camera_origin{},screen_translation{},reference_position{};
    AnmResource* fallback_sprite_resource=nullptr;
    float resolution_scale=1;std::array<i32,4> screen_offsets{};
    // Physical pixel grid; logical game coordinates and Replay stay unchanged.
    float raster_scale=1;
    Vec3 background_delta{};bool paused=false;
    Vec2 screen_shake_offset{}; // 4dcbe8/4dcbec, copied to the STD camera by406d90/407310
    Vec3 stage_direction{}; // raw camera direction dcb00; reference_position is normalized dcb18
    i32 stage_difficulty=0; // original be824, extra depth displacement above Easy
    // 0x4019d0 实际读的是 [0x4be824] = ECL ins_524 写入的 chapter（SessionState
    // .chapter），不是难度。生产路径绑定实时指针；未绑定时回落到
    // stage_difficulty 以保持既有单测夹具（只设 difficulty 的样本）不变。
    const i32* chapter=nullptr;
    u32 screen_width=640,screen_height=480;
    // Overlay masking follows the screen alpha format, not a menu mode.
    bool render_target_has_alpha=false;
    // Original logical shot clipping rectangle (51bc08/51bc0c), distinct
    // from the centered sprite offsets used by ANM coordinates.
    Vec2 playfield_origin{};
};
// 原版启动期全局 2D 相机块（0x4dcaf4 起，0043bbc0.c:55-58 / 0043bed0.c:51-53）：
// position=(0,0,1000)（0x4dcafc=0x447a0000）、direction 零、up=(0,1,0)
// （0x4dcb10=1.0）、0x4dcb18 参考方向与 0x4dcb30 eye 偏移均为 BSS 初值 0
// （.data raw 只覆盖到 0x4c1400，0x4dcaf4+ 是零初始化段）。
// ANM 外部变量 10016-10018 = 0x4dcb30..38 + 0x4dcaf4..fc、10019-10021 =
// 0x4dcb18..20（00461e80.c:56-64），即 camera_origin+screen_translation /
// reference_position。0x406680:82 每帧用 stage+0x409c 块覆盖该全局块，但
// 场景 ECL 构造（如 ins_308 的模板拷贝+立即 tick，0x4243e0）发生在首帧覆盖
// 之前，故新开局必须在 battle.load 前恢复启动种子。
// 仅当相机块仍是零态（release_run 清零后的新局起步）才播种；换关延续时
// 块内是上一关末态（0x406680:82 每帧覆盖），不覆盖以保持原版语义。
// 返回 true = 本次执行了播种。
inline bool seed_native_camera_block_if_cleared(AnmEnvironment& environment) noexcept {
    if (environment.screen_translation.x != 0.f || environment.screen_translation.y != 0.f ||
        environment.screen_translation.z != 0.f)
        return false;
    environment.camera_origin = {};
    environment.screen_translation = {0.f, 0.f, 1000.f};
    environment.reference_position = {};
    environment.stage_direction = {};
    return true;
}
}
