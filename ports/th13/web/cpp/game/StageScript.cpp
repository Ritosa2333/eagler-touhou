// STD 相机脚本执行体（th15 同族 StageScript 的 th13 指令面；插值通道复用
// Interpolation.hpp 的 Vec3 通道与缓动权重，雾通道按 StageFog 独立插值——
// 通道语义同 th15 StageFog.cpp，浮点算术序保持一致）。
#include "StageScript.hpp"
#include <cmath>
namespace th13 {
void StageFog::set(u32 packed, float near_value, float far_value) noexcept {
    color = packed;
    near_distance = near_value;
    far_distance = far_value;
    for (u32 i = 0; i < 4; ++i) channels[i] = float((packed >> (i * 8)) & 255);
}
void StageFog::pack() noexcept {
    color = 0;
    for (u32 i = 0; i < 4; ++i) color |= (u32(truncate_int(channels[i])) & 255) << (i * 8);
}
namespace {
StageFog add(const StageFog& a, const StageFog& b) noexcept {
    StageFog out;
    out.near_distance = float(b.near_distance + a.near_distance);
    out.far_distance = float(b.far_distance + a.far_distance);
    for (u32 i = 0; i < 4; ++i) out.channels[i] = float(b.channels[i] + a.channels[i]);
    out.pack();
    return out;
}
StageFog subtract(const StageFog& a, const StageFog& b) noexcept {
    StageFog out;
    out.near_distance = float(a.near_distance - b.near_distance);
    out.far_distance = float(a.far_distance - b.far_distance);
    for (u32 i = 0; i < 4; ++i) out.channels[i] = float(a.channels[i] - b.channels[i]);
    out.pack();
    return out;
}
StageFog multiply(const StageFog& a, float t) noexcept {
    StageFog out;
    out.near_distance = float(a.near_distance * t);
    out.far_distance = float(a.far_distance * t);
    for (u32 i = 0; i < 4; ++i) out.channels[i] = float(a.channels[i] * t);
    out.pack();
    return out;
}
std::array<float, 3> vector(const StageInstruction& c, u32 i) {
    return {c.argument<float>(i), c.argument<float>(i + 1), c.argument<float>(i + 2)};
}
Vec3 vector(const std::array<float, 3>& a) { return {a[0], a[1], a[2]}; }
void interpolate(Vec3Interpolation& v, const Vec3& from, const StageInstruction& c) {
    const auto to = vector(vector(c, 2));
    v.begin(c.argument<i32>(0), c.argument<i32>(1), {from.x, from.y, from.z}, {to.x, to.y, to.z});
}
float sine(float phase) { return float(std::sin(double(phase))); }
float cosine(float phase) { return float(std::cos(double(phase))); }
}  // namespace
StageFog StageFogInterpolation::step(float rate) noexcept {
    if (duration > 0) {
        timer.tick(&rate);
        if (timer.current >= duration) {
            timer.set(duration);
            duration = 0;
        }
    }
    if (duration == 0) return mode == 7 || mode == 17 ? start : end;
    if (mode == 7) {
        start = add(start, end);
        current = start;
        return current;
    }
    if (mode == 17) {
        start = add(start, control2);
        control2 = add(control2, end);
        current = start;
        return current;
    }
    if (mode == 8) {
        const float t = float(timer.fractional / float(duration)), twice = float(t * 2);
        const float a = float(t - 1), b = float(1 - t);
        const float h00 = float(float(a * a) * float(twice + 1));
        const float h11 = float(float(a * t) * t);
        const float h01 = float(float(3 - twice) * float(t * t));
        const float h10 = float(b * b * t);
        current = add(add(add(multiply(start, h00), multiply(end, h01)), multiply(control1, h10)),
                      multiply(control2, h11));
    } else {
        current = add(multiply(subtract(end, start),
                               interpolation_weight(mode, timer.fractional, float(duration))),
                      start);
    }
    return current;
}
bool StageScript::step(float rate) {
    if (!error.empty()) return false;
    bool stopped = false;
    u32 operations = 0;
    for (;;) {
        const auto* c = file_.instruction(state.instruction_offset);
        if (!c) {
            error = "STD instruction offset outside stream";
            return false;
        }
        if (c->time > state.timer.current) break;
        if (c->time < 0) {
            error = "STD script reached terminator without stop or jump";
            return false;
        }
        if (++operations > 100000) {
            error = "STD instruction loop limit";
            return false;
        }
        // th13 实测指令面的最小参数量（st01-st07 dump 全集；ins_14 需 2 参）。
        static constexpr u8 minimum[13] = {0, 2, 3, 5, 3, 5, 3, 1, 3, 5, 11, 11, 1};
        if ((c->opcode >= 0 && c->opcode <= 12 && c->arguments.size() < minimum[u32(c->opcode)]) ||
            (c->opcode == 14 && c->arguments.size() < 2)) {
            error = "truncated STD arguments";
            return false;
        }
        const auto& command = *c;
        auto& s = state;
        switch (command.opcode) {
        case 0: stopped = true; break;
        case 1:
            s.timer.set(command.argument<i32>(1));
            s.instruction_offset = command.argument<u32>(0);
            continue;
        case 2: {
            const auto prior = s.camera.position;
            s.camera.position = vector(vector(command, 0));
            s.camera.animation_delta = {float(s.camera.position.x - prior.x),
                                        float(s.camera.position.y - prior.y),
                                        float(s.camera.position.z - prior.z)};
            break;
        }
        case 3: interpolate(s.position, s.camera.position, command); break;
        case 4: s.camera.direction = vector(vector(command, 0)); break;
        case 5: interpolate(s.direction, s.camera.direction, command); break;
        case 6: s.camera.up = vector(vector(command, 0)); break;
        case 7: s.camera.fov = command.argument<float>(0); break;
        case 8:
            s.camera.fog.set(command.argument<u32>(0), command.argument<float>(1),
                             command.argument<float>(2));
            break;
        case 9:
            s.fog.start = s.camera.fog;
            s.fog.end.set(command.argument<u32>(2), command.argument<float>(3),
                          command.argument<float>(4));
            s.fog.duration = command.argument<i32>(0);
            s.fog.mode = command.argument<i32>(1);
            s.fog.timer.set(0);
            break;
        case 12:
            s.camera_effect = u8(command.argument<u32>(0));
            if (!s.camera_effect) s.camera.eye_offset = {};
            s.effect_timer.set(0);
            break;
        case 14: {  // 0x408a43：背景子 ANM 槽隐藏/模板重挂（arg0=槽、arg1=模板号）
            const i32 slot = command.argument<i32>(0), template_index = command.argument<i32>(1);
            if (slot < 0 || slot >= i32(s.background.size())) {
                error = "STD ins_14 slot outside background pool";
                return false;
            }
            if (template_index >= 0 && !background_command) {
                error = "STD ins_14 background ANM host unavailable";
                return false;
            }
            if (background_command && !background_command(slot, template_index)) {
                error = "STD background ANM command failed";
                return false;
            }
            if (template_index < 0) {
                s.background[u32(slot)].visible = false;
            } else {
                s.background[u32(slot)].script = template_index;
                s.background[u32(slot)].visible = true;
            }
            break;
        }
        case 16: break;  // 中断标号（interrupt 扫描锚点，顺序执行无动作）
        default:
            // 未定域指令：显式停机，不静默近似。
            error = "STD camera opcode not yet implemented: " + std::to_string(command.opcode);
            return false;
        }
        if (stopped) break;
        state.instruction_offset += u32(command.length);
    }
    if (!stopped) state.timer.tick(&rate);
    if (state.direction.duration) state.camera.direction = vector(state.direction.step(rate));
    if (state.position.duration) state.camera.position = vector(state.position.step(rate));
    if (state.fog.duration) state.camera.fog = state.fog.step(rate);
    if (state.up.duration) state.camera.up = vector(state.up.step(rate));
    camera_effect(rate);
    return true;
}
bool StageScript::interrupt(i32 id) {
    for (const auto& command : file_.instructions) {
        if (command.time < 0) break;
        if (command.opcode == 16 && command.argument<i32>(0) == id) {
            const auto offset = command.offset + u32(command.length);
            const auto* target = file_.instruction(offset);
            if (!target) {
                error = "STD interrupt offset outside stream";
                return false;
            }
            state.instruction_offset = offset;
            state.timer.set(target->time);
            return true;
        }
    }
    return true;
}
void StageScript::camera_effect(float rate) noexcept {
    auto& s = state;
    const u32 kind = s.camera_effect;
    if (kind == 0 || kind == 5 || kind == 6 || kind > 9) return;
    constexpr float pi = 3.1415927410125732421875f;
    float phase = float(float(s.effect_timer.fractional * pi) * 2), wave = 0;
    i32 period = 512;
    switch (kind) {
    case 1:
        // 0x408c8a（unicorn 直跑实测 + 常量池 0x4aef78=-120.0/0x4aef70=-50.0）：
        // eye.x=−120·sin(φ)、eye.z=−50·sin(2φ)；up/target 公式不变。
        phase = normalize_angle(float(phase * (1.f / 512)));
        wave = sine(phase);
        s.camera.eye_offset.x = float(wave * -120);
        s.camera.eye_offset.z = float(sine(normalize_angle(float(phase * 2))) * -50);
        s.camera.up.x = float(wave * -.01f);
        s.camera.target_offset.x = float(s.camera.eye_offset.x * -.5f);
        s.camera.target_offset.z = float(s.camera.eye_offset.z * -.5f);
        break;
    case 2:
        period = 3072;
        phase = normalize_angle(float(phase / 3072));
        s.camera.eye_offset.x = float(sine(phase) * -700);
        s.camera.target_offset.x = -s.camera.eye_offset.x;
        s.camera.eye_offset.y = float(cosine(phase) * 700);
        s.camera.target_offset.y = -s.camera.eye_offset.y;
        break;
    case 3:
        period = 2048;
        phase = normalize_angle(float(phase * (1.f / 2048)));
        wave = sine(phase);
        s.camera.eye_offset.x = float(wave * 50);
        s.camera.eye_offset.z = float(sine(normalize_angle(float(phase * 2))) * 50);
        s.camera.target_offset.x = -s.camera.eye_offset.x;
        s.camera.target_offset.z = -s.camera.eye_offset.z;
        s.camera.up.x = float(wave * -.05f);
        break;
    case 4:
        period = 1536;
        phase = normalize_angle(float(phase / 1536));
        s.camera.eye_offset.x = float(sine(phase) * -550);
        s.camera.target_offset.x = -s.camera.eye_offset.x;
        s.camera.eye_offset.y = float(cosine(phase) * 550);
        s.camera.target_offset.y = -s.camera.eye_offset.y;
        break;
    case 7:
        period = 2048;
        phase = float(float(phase * (1.f / 2048)) - pi);
        wave = sine(phase);
        s.camera.eye_offset.x = float(wave * 70);
        s.camera.eye_offset.z = float(sine(normalize_angle(float(phase * 2))) * 200);
        s.camera.up.x = float(wave * -.1f);
        break;
    case 8:
        period = 1024;
        phase = float(float(phase * (1.f / 1024)) - pi);
        wave = sine(phase);
        s.camera.up.x = float(wave * -.1f);
        s.camera.eye_offset.x = float(wave * -50);
        break;
    case 9:
        phase = float(float(phase * (1.f / 512)) - pi);
        wave = sine(phase);
        s.camera.up.x = float(wave * -.01f);
        s.camera.eye_offset.x = float(wave * -15);
        break;
    }
    s.effect_timer.tick(&rate);
    if (s.effect_timer.current >= period) s.effect_timer.set(0);
}
}  // namespace th13
