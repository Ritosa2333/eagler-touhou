// th16 ScreenEffect kind 1（0x45cd30 震动）与淡入淡出 kind（0x45c900）。
#include "ScreenEffect.hpp"

namespace th16 {

bool screen_shake_step(ScreenShake& shake, Rng& game_rng, float window_scale, float game_speed,
                       ScreenShakeOffset& out) {
    shake.timer.speed_slot = 0;
    shake.timer.tick(game_speed);
    if (shake.duration <= shake.timer.current) return false;
    const float amp = (float(shake.amp_to - shake.amp_from) * shake.timer.current_f) / float(shake.duration) +
                      float(shake.amp_from);
    switch (game_rng.next32() % 3u) {
    case 0:
        out.offset.x = 0.0f;
        out.offset_scaled.x = 0.0f;
        break;
    case 1:
        out.offset_scaled.x = window_scale * amp;
        out.offset.x = amp;
        break;
    default:
        out.offset_scaled.x = window_scale * amp;
        out.offset.x = -amp;
        break;
    }
    switch (game_rng.next32() % 3u) {
    case 0:
        out.offset.y = 0.0f;
        out.offset_scaled.y = 0.0f;
        break;
    case 1:
        out.offset.y = amp;
        out.offset_scaled.y = window_scale * amp;
        break;
    default:
        out.offset.y = -amp;
        out.offset_scaled.y = -(window_scale * amp);
        break;
    }
    return true;
}

bool screen_shake_envelope_step(ScreenShakeEnvelope& shake, Rng& visual_rng, float window_scale, float game_speed,
                                i32 post_load_frames, bool game_frozen, ScreenShakeOffset& out) {
    if (post_load_frames != 0) return false;
    if (game_frozen) return true;
    shake.timer.tick(game_speed);
    const i32 t = shake.timer.current;
    float ratio;
    if (t < shake.attack) {
        ratio = shake.timer.current_f / float(shake.attack);
    } else if (t < shake.attack + shake.sustain) {
        ratio = 1.0f;
    } else {
        const i32 end = shake.decay + shake.sustain + shake.attack;
        if (end <= t) return false;
        ratio = (float(end) - shake.timer.current_f) / float(shake.decay);
    }
    const float amp = float(shake.amplitude) * ratio;
    switch (visual_rng.next32() % 3u) {
    case 0:
        out.offset.x = 0.0f;
        out.offset_scaled.x = 0.0f;
        break;
    case 1:
        out.offset_scaled.x = window_scale * amp;
        out.offset.x = amp;
        break;
    default:
        out.offset_scaled.x = -(window_scale * amp);
        out.offset.x = -amp;
        break;
    }
    switch (visual_rng.next32() % 3u) {
    case 0:
        out.offset.y = 0.0f;
        out.offset_scaled.y = 0.0f;
        break;
    case 1:
        out.offset.y = amp;
        out.offset_scaled.y = window_scale * amp;
        break;
    default:
        out.offset.y = -amp;
        out.offset_scaled.y = -(window_scale * amp);
        break;
    }
    return true;
}

bool screen_fade_step(ScreenFade& fade, i32 post_load_frames, bool game_frozen, float game_speed) {
    if (post_load_frames != 0) return false;
    const i32 d = fade.duration;
    if (d != 0) {
        fade.alpha = fade.timer.current < d ? i32(double(fade.timer.current_f) * 255.0 / double(float(d))) : 0xff;
        if (fade.alpha < 0) fade.alpha = 0;
    }
    if (fade.timer.current >= d + 2) return false;
    if (!game_frozen) fade.timer.tick(game_speed);
    return true;
}

bool screen_fade_in_step(ScreenFade& fade, i32 post_load_frames, float game_speed) {
    if (post_load_frames != 0) return false;
    const i32 d = fade.duration;
    if (d != 0) {
        // 单精度：255 − current_f × 255 / duration，截断后钳到 0
        fade.alpha = i32(255.0f - fade.timer.current_f * 255.0f / float(d));
        if (fade.alpha < 0) fade.alpha = 0;
    }
    if (fade.timer.current >= d) return false;
    fade.timer.tick(game_speed);
    return true;
}

bool screen_fade_hold_step(ScreenFade& fade, i32 post_load_frames, float game_speed) {
    // 0x45cac0：先把 alpha 写成 0xff；到时返回 7；否则按 0x406190 推进（speed_slot 经 DAT_00490eb0，运行时恒 0）
    if (post_load_frames != 0) return false;
    fade.alpha = 0xff;
    if (fade.timer.current >= fade.duration) return false;
    fade.timer.tick(game_speed);
    return true;
}

bool screen_fade_from_aux_step(ScreenFade& fade, i32 post_load_frames, float game_speed) {
    // 0x45cbd0：DAT_004c0f40 非零 → 7；否则以 (+0x20>>24) 为起点按 t/duration 线性降到 0。
    // t ≥ duration 时 alpha=0、对 +0x1c 做 --，≤0 自删，否则 0x406490(timer, 0) 重跑。
    if (post_load_frames != 0) return false;
    const i32 start = i32(fade.color_aux >> 24);
    const i32 d = fade.duration;
    if (fade.timer.current < d) {
        const i32 delta = d != 0 ? i32(float(start) * fade.timer.current_f / float(d)) : 0;
        fade.alpha = start - delta;
        if (fade.alpha < 0) fade.alpha = 0;
    } else {
        fade.color = u32(i32(fade.color) - 1);
        fade.alpha = 0;
        if (i32(fade.color) <= 0) return false;
        fade.timer.set(0);
    }
    fade.timer.tick(game_speed);
    return true;
}

bool screen_fade_half_step(ScreenFade& fade, float game_speed) {
    // 0x45c9e0：+0x28==0 → alpha = t×128/duration（t≤duration）；+0x28≠0 → t>8 自删，否则 128−t×16
    if (fade.mode_flag == 0) {
        const i32 d = fade.duration;
        if (d != 0 && fade.timer.current <= d) {
            fade.alpha = i32(fade.timer.current_f * 128.0f / float(d));
        }
    } else {
        if (fade.timer.current > 8) return false;
        fade.alpha = 0x80 - i32(fade.timer.current_f * 128.0f * 0.125f);
    }
    fade.timer.tick(game_speed);
    return true;
}

}  // namespace th16
