#include "Stage.hpp"

#include <cmath>
#include <cstring>

#include "AnmGeometry.hpp"
#include "Motion.hpp"   // wrap_angle / sin_f / cos_f（0x402d90 / 0x405510 / 0x4054f0）

namespace th16 {

namespace {

constexpr float kPi = 3.1415927f;

StageFog fog_from_color(u32 color, float near_, float far_) {
    StageFog f;
    f.color = color;
    f.near_ = near_;
    f.far_ = far_;
    for (int i = 0; i < 4; ++i) f.c[i] = float((color >> (8 * i)) & 0xffu);
    return f;
}

// 0x40d370：七元组相加，颜色字节由浮点截断重建
StageFog fog_add(const StageFog& a, const StageFog& b) {
    StageFog r;
    r.near_ = a.near_ + b.near_;
    r.far_ = a.far_ + b.far_;
    u32 packed = 0;
    for (int i = 0; i < 4; ++i) {
        r.c[i] = a.c[i] + b.c[i];
        packed |= u32(u8(i32(r.c[i]))) << (8 * i);
    }
    r.color = packed;
    return r;
}

StageFog fog_scale(const StageFog& a, float k) {
    StageFog r;
    r.near_ = a.near_ * k;
    r.far_ = a.far_ * k;
    for (int i = 0; i < 4; ++i) r.c[i] = a.c[i] * k;
    return r;
}

// 原版 0x5fc 结构体 memset 0 的纯数据区（链表结点等宿主指针保持默认）
void zero_vm(AnmVm& vm) {
    std::memset(static_cast<void*>(&vm), 0, 0x568);
}

}  // namespace

bool StageManager::load(const u8* bytes, u32 size, i32 stage, const Camera& global_camera) {
    stage_number = stage;
    if (!std.parse(bytes, size)) { error = std.error; return false; }
    // 相机整份抄自全局 0x4c168c（0x4b 个 dword：开局为 0x43cb10 的 fov π/6、视口 (116,4,408,472)；换关时是
    // 上一关最后抄出的相机，雾色 / 雾距因此沿用到本关 STD 设雾为止），再由 0x4097c0 覆盖：
    // pos=(0,0,-600) facing=(0,300,600) up=(0,1,0)，pos_offset / facing_offset 清零
    camera = global_camera;
    camera.pos = {0, 0, -600.0f};
    camera.facing = {0, 300.0f, 600.0f};
    camera.up = {0, 1.0f, 0};
    camera.pos_offset = {};
    camera.facing_offset = {};
    cull_distance_sq = 9610000.0f;  // +0x331c = 0x4b12a310（3100²）
    flags = 1;
    timer.reset();
    ins_index = 0;
    rocking_mode = 0;
    rocking_timer = Timer{};
    facing_interp = {};
    pos_interp = {};
    up_interp = {};
    fog_interp = {};
    frame = 0;
    drawn_instances = culled_instances = drawn_quads = 0;
    // 0x409670 先按 0x4093f0 构造 8 个层精灵 VM（sprite/ins_offset = -1），随后 _memset(this, 0, 0x66f0) 又把它们
    // 整体清零；0x40ac30 的面 VM 表也是 malloc + memset 0。于是未被 ins 14 指派的层精灵 VM 处于全零状态：
    // file_slot 0（text.anm）、script 0、ins_offset 0——0x409e50 每帧照样 0x45f980 推进它们，这是原版可观测行为。
    face_vms.assign(size_t(std.face_vm_count), AnmVm{});
    for (AnmVm& vm : face_vms) zero_vm(vm);
    for (AnmVm& vm : layer_vms) { vm = AnmVm{}; zero_vm(vm); }
    for (auto& id : layer_ids) id = 0;
    return true;
}

void StageManager::start(AnmManager& manager) {
    // 0x40add0：对象激活位 = 1；按遍历顺序分配面 VM
    i32 k = 0;
    for (StdObject& obj : std.objects) {
        obj.flags = 1;
        for (StdQuad& q : obj.quads) {
            if (k >= i32(face_vms.size())) { error = "std: more quads than face vms"; return; }
            AnmVm& vm = face_vms[size_t(k)];
            manager.init_from_template(*anm, vm, q.anm_script);
            vm.parent = nullptr;
            vm.root = nullptr;
            manager.run(vm);
            q.vm_index = i16(k);
            ++k;
        }
    }
    ins_index = 0;
}

Vec3 StageManager::eye() const {
    return {camera.pos.x + camera.pos_offset.x, camera.pos.y + camera.pos_offset.y, camera.pos.z + camera.pos_offset.z};
}

Vec3 StageManager::at() const {
    const Vec3 e = eye();
    return {e.x + camera.facing_norm.x, e.y + camera.facing_norm.y, e.z + camera.facing_norm.z};
}

void StageManager::step_faces(AnmManager& manager) {
    // 0x40aed0：只推进 flags bit0 的对象；全部面 VM 脚本停止（ins_offset < 0）后清 bit0
    for (StdObject& obj : std.objects) {
        if ((obj.flags & 1) == 0) continue;
        i32 alive = 0;
        for (const StdQuad& q : obj.quads) {
            AnmVm& vm = face_vms[size_t(q.vm_index)];
            manager.run(vm);
            if (vm.ins_offset >= 0) ++alive;
        }
        if (alive == 0) obj.flags &= u8(~1u);
    }
}

void StageManager::set_fog_time(i32 time, i32 mode, const StageFog& goal) {
    // 0x409490
    fog_interp.end_time = time;
    fog_interp.initial = camera.fog;
    fog_interp.mode = mode;
    fog_interp.goal = goal;
    fog_interp.timer.reset();
}

StageFog StageManager::step_fog(float game_speed) {
    // 0x40cd10
    Interp<StageFog>& it = fog_interp;
    const i32 end = it.end_time;
    bool finished = end == 0;
    if (end > 0) {
        it.timer.tick(game_speed);
        if (end <= it.timer.current) {
            it.timer.set(end);
            it.end_time = 0;
            finished = true;
        }
    }
    if (finished) return (it.mode != 7 && it.mode != 0x11) ? it.goal : it.initial;
    if (it.mode == 7) {
        it.initial = fog_add(it.initial, it.goal);
        it.current = it.initial;
    } else if (it.mode == 0x11) {
        it.initial = fog_add(it.initial, it.bezier1);
        it.bezier1 = fog_add(it.bezier1, it.goal);
        it.current = it.initial;
    } else if (it.mode == 8) {
        const float r = it.timer.current_f / float(end);
        const float s = r - 1.0f;
        const float h11 = s * r * r;
        const float h00 = s * s * (r + r + 1.0f);
        const float h10 = (1.0f - r) * (1.0f - r) * r;
        const float h01 = r * r * (3.0f - (r + r));
        const StageFog a = fog_add(fog_scale(it.initial, h00), fog_scale(it.goal, h01));
        const StageFog b = fog_add(fog_scale(it.bezier1, h10), fog_scale(it.bezier2, h11));
        it.current = fog_add(a, b);
    } else {
        const float f = interp_ease(it.mode, it.timer.current_f, float(end));
        StageFog delta;
        delta.near_ = (it.goal.near_ - it.initial.near_) * f;
        delta.far_ = (it.goal.far_ - it.initial.far_) * f;
        for (int i = 0; i < 4; ++i) delta.c[i] = (it.goal.c[i] - it.initial.c[i]) * f;
        it.current = fog_add(delta, it.initial);
    }
    return it.current;
}

void StageManager::run_script(AnmManager& manager, float game_speed) {
    // 0x40b3b0 前半：执行 time <= timer.current 的指令
    bool stopped = false;
    while (ins_index >= 0 && ins_index < i32(std.script.size())) {
        const StdInstruction& ins = std.script[size_t(ins_index)];
        if (ins.time > timer.current) break;
        bool jumped = false;
        switch (ins.opcode) {
        case 0:
            stopped = true;
            break;
        case 1: {
            const i32 t = ins.arg_i(1);
            if ((timer.flags & 1) == 0) { timer.current = 0; timer.previous = -999999; timer.current_f = 0; timer.speed_slot = 0; timer.flags |= 1; }
            timer.current = t;
            timer.previous = t - 1;
            timer.current_f = float(t);
            ins_index = std.instruction_at(u32(ins.arg_i(0)));
            jumped = true;
            break;
        }
        case 2: {
            const Vec3 old = camera.pos;
            camera.pos = {ins.arg_f(0), ins.arg_f(1), ins.arg_f(2)};
            camera.pos_delta = {camera.pos.x - old.x, camera.pos.y - old.y, camera.pos.z - old.z};
            break;
        }
        case 3:
            pos_interp.end_time = ins.arg_i(0);
            pos_interp.mode = ins.arg_i(1);
            pos_interp.initial = camera.pos;
            pos_interp.goal = {ins.arg_f(2), ins.arg_f(3), ins.arg_f(4)};
            pos_interp.timer.reset();
            break;
        case 4:
            camera.facing = {ins.arg_f(0), ins.arg_f(1), ins.arg_f(2)};
            break;
        case 5:
            facing_interp.end_time = ins.arg_i(0);
            facing_interp.mode = ins.arg_i(1);
            facing_interp.initial = camera.facing;
            facing_interp.goal = {ins.arg_f(2), ins.arg_f(3), ins.arg_f(4)};
            facing_interp.timer.reset();
            break;
        case 6:
            camera.up = {ins.arg_f(0), ins.arg_f(1), ins.arg_f(2)};
            break;
        case 7:
            camera.fov = ins.arg_f(0);
            break;
        case 8:
            camera.fog = fog_from_color(u32(ins.arg_i(0)), ins.arg_f(1), ins.arg_f(2));
            break;
        case 9:
            set_fog_time(ins.arg_i(0), ins.arg_i(1), fog_from_color(u32(ins.arg_i(2)), ins.arg_f(3), ins.arg_f(4)));
            break;
        case 10:
            pos_interp.end_time = ins.arg_i(0);
            pos_interp.initial = camera.pos;
            pos_interp.bezier1 = {ins.arg_f(2), ins.arg_f(3), ins.arg_f(4)};
            pos_interp.goal = {ins.arg_f(5), ins.arg_f(6), ins.arg_f(7)};
            pos_interp.bezier2 = {ins.arg_f(8), ins.arg_f(9), ins.arg_f(10)};
            pos_interp.mode = 8;
            pos_interp.timer.reset();
            break;
        case 11:
            facing_interp.end_time = ins.arg_i(0);
            facing_interp.initial = camera.facing;
            facing_interp.bezier1 = {ins.arg_f(2), ins.arg_f(3), ins.arg_f(4)};
            facing_interp.goal = {ins.arg_f(5), ins.arg_f(6), ins.arg_f(7)};
            facing_interp.bezier2 = {ins.arg_f(8), ins.arg_f(9), ins.arg_f(10)};
            facing_interp.mode = 8;
            facing_interp.timer.reset();
            break;
        case 12:
            rocking_mode = i32(i8(ins.arg_i(0)));
            if (rocking_mode == 0) camera.pos_offset = {0, 0, 0};
            rocking_timer.reset();
            if (rocking_mode == 2) rocking_timer.set(0x200);
            break;
        case 13:
            bg_color = u32(ins.arg_i(0));
            if (screen_clear_color != nullptr) *screen_clear_color = bg_color;
            break;
        case 14: {
            const i32 idx = ins.arg_i(0);
            const i32 script = ins.arg_i(1);
            if (idx < 0 || idx >= kLayerSprites) break;
            AnmVm& vm = layer_vms[idx];
            if (script < 0) {
                if (script == -1) vm.ins_offset = -1;
                if (script == -1 || script == -2) vm.flags_lo &= ~AnmVm::kVisible;
            } else if (anm != nullptr) {
                manager.init_from_template(*anm, vm, script);
                vm.parent = nullptr;
                vm.root = nullptr;
                manager.run(vm);
            }
            layer_ids[idx] = ins.arg_i(2);
            break;
        }
        case 17: {
            // 0x40b3b0 case 0x11：释放旧畸变网格，半径 / 计时 / 相位复位，按 arg0（1 → 7 行，否则 17 行）新建
            distortion.reset();
            distortion_radius_min = 112.0f;
            distortion_radius = 192.0f;
            distortion_unused = -1;
            distortion_angle_a = 0.0f;
            distortion_angle_b = 0.0f;
            distortion_timer.reset();
            distortion_kind = ins.arg_i(0);
            if (0.0f < distortion_radius_min) {
                AnmFile* text = manager.file(0);   // DAT_004c17f8 text.anm
                if (text != nullptr) {
                    const bool ready = render_targets_ready != nullptr && *render_targets_ready;
                    distortion = std::make_unique<DistortionGrid>(manager, *text, ready, distortion_kind == 1 ? 7 : 0x11);
                }
            }
            break;
        }
        case 18:
            up_interp.end_time = ins.arg_i(0);
            up_interp.mode = ins.arg_i(1);
            up_interp.initial = camera.up;
            up_interp.goal = {ins.arg_f(2), ins.arg_f(3), ins.arg_f(4)};
            up_interp.timer.reset();
            break;
        case 19:
            interrupt(manager, ins.arg_i(0) + 7);
            break;
        case 20:
            cull_distance_sq = float(ins.arg_i(0)) * float(ins.arg_i(0));   // 绘制距离：0x40a7d0 据此剔除远处实例
            break;
        default:
            break;
        }
        if (stopped) break;
        if (!jumped) ++ins_index;
    }
    if (!stopped) {
        // 脚本时钟按游戏速度推进（速度表槽恒为 0）
        timer.tick(game_speed);
    }
}

void StageManager::apply_rocking(float game_speed) {
    // 0x40b3b0 后半（EDI = this+0xc；0x21c pos_offset 0x228 facing_offset 0x1f8 up.x 0x200 up.z）
    const float t = rocking_timer.current_f;
    const float tp = t * kPi;
    auto finish = [&](i32 period) {
        rocking_timer.tick(game_speed);
        if (rocking_timer.current >= period) rocking_timer.set(0);
    };
    switch (rocking_mode) {
    case 1: {
        const float a = wrap_angle((tp + tp) * 0.001953125f);
        const float s = sin_f(a);
        const float px = s * -20.0f;
        camera.pos_offset.x = px;
        const float s2 = sin_f(wrap_angle(a * 2.0f)) * -10.0f;
        camera.pos_offset.z = s2;
        camera.up.x = s * -0.01f;
        camera.facing_offset.x = px * -0.5f;
        camera.facing_offset.z = s2 * -0.5f;
        finish(0x200);
        break;
    }
    case 2: {
        const float a = wrap_angle((tp + tp) / 3072.0f);
        camera.up.x = -sin_f(a);
        camera.up.z = cos_f(a);
        finish(0xc00);
        break;
    }
    case 3: {
        const float a = wrap_angle((tp + tp) * 0.00048828125f);
        const float s = sin_f(a);
        const float v = s * 50.0f;
        camera.pos_offset.x = v;
        camera.pos_offset.y = v;
        camera.pos_offset.z = sin_f(wrap_angle(a * 2.0f)) * -100.0f;
        camera.facing_offset.x = -v;
        camera.facing_offset.y = -v;
        camera.up.x = s * -0.05f;
        finish(0x800);
        break;
    }
    case 4: {
        const float a = wrap_angle((tp + tp) / 3072.0f);
        camera.up.x = -sin_f(a);
        camera.up.z = -cos_f(a);
        finish(0xc00);
        break;
    }
    case 7: {
        const float a = (tp + tp) * 0.00048828125f - kPi;
        const float s = sin_f(a);
        camera.pos_offset.x = s * 70.0f;
        camera.pos_offset.z = sin_f(wrap_angle(a + a)) * 200.0f;
        camera.up.x = s * -0.1f;
        finish(0x800);
        break;
    }
    case 8: {
        const float a = (tp + tp) * 0.0009765625f - kPi;
        const float s = sin_f(a);
        camera.pos_offset.x = s * -50.0f;
        camera.up.x = s * -0.1f;
        finish(0x400);
        break;
    }
    case 9: {
        const float a = (tp + tp) * 0.001953125f - kPi;
        const float s = sin_f(a);
        camera.up.x = s * -0.01f;
        camera.pos_offset.x = s * -15.0f;
        finish(0x200);
        break;
    }
    default:
        break;
    }
}

void StageManager::update(AnmManager& manager, float game_speed, const Rng*) {
    // 0x409e50
    if ((flags & 8) != 0) return;
    if ((flags & 4) != 0 && end_timer.current >= 0x3c) return;
    camera.shift_x = camera.shift_y = 0;
    camera.pos_delta = {0, 0, 0};
    // facing_norm = normalize(facing + facing_offset)
    {
        const float x = camera.facing.x + camera.facing_offset.x;
        const float y = camera.facing_offset.y + camera.facing.y;
        const float z = camera.facing_offset.z + camera.facing.z;
        const float len = std::sqrt(x * x + y * y + z * z);
        if (len > 0.0f) camera.facing_norm = {x / len, y / len, z / len};
        else camera.facing_norm = {x, y, z};
    }
    tint = 0x808080;
    if ((flags & 4) == 0 || end_timer.current < 0x1e) {
        step_faces(manager);
        run_script(manager, game_speed);
        // 0x40b3b0 尾部：四组插值
        if (facing_interp.end_time != 0) camera.facing = step_interp_vec3(facing_interp, game_speed);
        if (pos_interp.end_time != 0) camera.pos = step_interp_vec3(pos_interp, game_speed);
        if (fog_interp.end_time != 0) camera.fog = step_fog(game_speed);
        if (up_interp.end_time != 0) camera.up = step_interp_vec3(up_interp, game_speed);
        if (rocking_mode != 0) apply_rocking(game_speed);
    }
    if (camera_global != nullptr) *camera_global = camera;  // DAT_004c168c ← this+0x1ec（0x4b dword）
    // 该副本的 dword 0x41 即 DAT_004c1790：AnmVm 末尾（flags & 0x8000）把它累加到 entity_offset，
    // 使关卡脚本瞬移相机（如背景循环）时跟随相机的 VM 同步位移
    manager.entity_offset_delta = camera.pos_delta;
    for (AnmVm& vm : layer_vms) manager.run(vm);
    update_distortion(manager, game_speed);
    ++frame;
}

// 0x40c4a0
void StageManager::update_distortion(AnmManager& manager, float game_speed) {
    if (!distortion) return;
    DistortionGrid& grid = *distortion;
    static const AnmGeometry kDefaultGeometry;
    const AnmGeometry& geometry = manager.geometry != nullptr ? *manager.geometry : kDefaultGeometry;
    const u32 target_w = manager.render_target_width;
    const u32 target_h = manager.render_target_height;
    float angle_a = distortion_angle_a;
    float angle_b = distortion_angle_b;
    if (distortion_kind == 1) {
        // 种类 1：符卡进行中不动网格；否则在场地底部 (−192, 320) 起铺 384 × 128（7 行），内部节点按
        // sin(列相位) × 行振幅 / sin(行相位) 位移，振幅前 60 帧由 0 线性升到 6；全部顶点 alpha = 0x80
        if (spellcard_flags == nullptr || (*spellcard_flags & 1u) == 0) {
            grid.fill(-192.0f, 320.0f, 384.0f, 128.0f, geometry, target_w, target_h);
            const float amplitude = distortion_timer.current_f < 60.0f ? distortion_timer.current_f * 6.0f / 60.0f : 6.0f;
            const i32 columns = grid.columns();
            const i32 rows = grid.rows();
            ScreenColorUvVertex* vertex = grid.vertices().data();
            Vec3* node = grid.nodes().data();
            for (i32 column = 0; column < columns; ++column) {
                if (rows > 0) {
                    const float sin_b = float(std::sin(double(angle_b)));
                    for (i32 row = 0; row < rows; ++row, ++vertex, ++node) {
                        vertex->color = (vertex->color & 0x00ffffffu) | 0x80000000u;
                        const float t = float(row) * amplitude / float(rows - 1);
                        const float sin_a = float(std::sin(double(angle_a)));
                        const float offset_x = sin_a * t;
                        const float offset_y = sin_b * t;
                        if (column != 0 && row != 0 && column != columns - 1 && row != rows - 1) {
                            vertex->x = vertex->x + offset_x;
                            vertex->z = 0.0f;
                            vertex->y = vertex->y + offset_y;
                            node->z = 0.0f;
                        }
                        angle_a = wrap_angle(angle_a + 0.668424f);
                    }
                }
                angle_b = wrap_angle(angle_b - 1.4959966f);
            }
            distortion_angle_a = wrap_angle(distortion_angle_a + 0.049087387f);
            distortion_angle_b = wrap_angle(distortion_angle_b + 0.03926991f);
        }
    } else if (distortion_kind == 2) {
        // 种类 2（本作数据未用）：半径每帧 −2 至下限，以 (224, 240) 为中心的透镜；dz 用 r² 而非中心 z，照抄原版
        const float radius = distortion_radius;
        if (distortion_radius_min < radius) distortion_radius = radius - 2.0f;
        grid.fill(224.0f - radius, -radius, radius + radius, radius + radius, geometry, target_w, target_h);
        const float radius_sq = radius * radius;
        ScreenColorUvVertex* vertex = grid.vertices().data();
        Vec3* node = grid.nodes().data();
        const i32 total = grid.columns() * grid.rows();
        for (i32 i = 0; i < total; ++i, ++vertex, ++node) {
            float dz = node->z - radius_sq;
            float dx = node->x - 224.0f;
            float dy = node->y - 240.0f;
            const float inside = radius_sq - (dx * dx + dy * dy);
            if (inside < 0.0f) {
                vertex->color &= 0x00ffffffu;
            } else {
                const float strength = inside / radius_sq;
                u32 color = 0x60ffffffu;
                for (int shift = 16; shift >= 0; shift -= 8) {
                    const u32 channel = (color >> shift) & 0xffu;
                    const u32 lifted = u32(i32(255.0f - float(i32(0xff - channel)) * strength)) & 0xffu;
                    color = (color & ~(0xffu << shift)) | (lifted << shift);
                }
                vertex->color = color;
                const float push = strength * 32.0f;
                const float length = std::sqrt(dx * dx + dy * dy + dz * dz);
                if (length != 0.0f) { dx /= length; dy /= length; dz /= length; }
                const float offset_x = float(std::sin(double(angle_a))) * strength * 8.0f + push * dx;
                const float offset_y = float(std::sin(double(angle_b))) * strength * 8.0f + push * dy;
                vertex->x = vertex->x + offset_x;
                vertex->z = 0.0f;
                vertex->y = vertex->y + offset_y;
                node->z = 0.0f;
            }
            angle_a = wrap_angle(angle_a + 1.5707964f);
            angle_b = wrap_angle(angle_b - 0.69813174f);
        }
        distortion_angle_a = wrap_angle(distortion_angle_a + 0.049087387f);
        const float random = manager.game_rng.unit();   // 0x402c70(DAT_004a6d80)
        distortion_angle_b = wrap_angle(random * 3.1415927f / 40.0f + 0.03926991f + distortion_angle_b);
    }
    distortion_timer.tick(game_speed);
}

void StageManager::begin_end(i32 frames, u32 flag) {
    // 0x40c0d0 / 0x40c210：Timer::set(frames)（previous = frames - 1）后置位
    end_timer.set(frames);
    flags |= flag;
}

void StageManager::jump_to_label(i32 label) {
    for (size_t i = 0; i < std.script.size(); ++i) {
        const StdInstruction& ins = std.script[i];
        if (ins.opcode != 16 || ins.arg_i(0) != label) continue;
        ins_index = i32(i);
        const i32 t = ins.time;
        if ((timer.flags & 1) == 0) { timer.current = 0; timer.previous = -999999; timer.current_f = 0; timer.speed_slot = 0; timer.flags |= 1; }
        timer.current = t;
        timer.previous = t - 1;
        timer.current_f = float(t);
        return;
    }
}

void StageManager::interrupt(AnmManager& manager, i32 value) {
    // 0x40b2f0（interrupt_callback 表 DAT_00491b0c 全空）
    for (AnmVm& vm : face_vms) {
        vm.pending_interrupt = value;
        manager.run(vm);
    }
    for (AnmVm& vm : layer_vms) {
        vm.pending_interrupt = value;
        manager.run(vm);
    }
}

// ---------------------------------------------------------------------------------------------
// 绘制链
// ---------------------------------------------------------------------------------------------

namespace {

using touhou::graphics::Compare;

constexpr int kFieldWidth = 384;    // 0x40a7d0：场地矩形 DAT_004d9d38/3c 起 384×448
constexpr int kFieldHeight = 448;

bool draw_enabled_this_frame(u32 flags, i32 end_timer) {
    // 0x409f90 / 0x40a410 共同入口：bit3 隐藏；bit2（新关淡入）且计时 ≥ 0x3c 时跳过
    if ((flags & 8u) != 0) return false;
    return (flags & 4u) == 0 || end_timer < 0x3c;
}

ScreenRect viewport_rect(const Camera& camera) {
    const Viewport& vp = camera.viewport;
    return {vp.x, vp.y, vp.x + vp.width, vp.y + vp.height};
}

}  // namespace

void StageManager::look_through_stage_camera(AnmRenderer& renderer, Camera& global_camera, const ScreenShakeOffset& shake) {
    // 抖动偏移 DAT_004c1788/8c → 相机 +0xfc；整个 0x12c 相机抄到 DAT_004c168c；0x43c940 + SetViewport，kind = 3
    camera.shift_x = shake.offset.x;
    camera.shift_y = shake.offset.y;
    global_camera = camera;
    global_camera.kind = Camera::kRenderTarget;
    global_camera.look_along_facing();
    renderer.use_camera(global_camera);
}

void StageManager::step_fade_in() {
    // 0x409f90 中段：bit2（新关淡入）下，计时 < 0x1e 时建从黑淡入的 ScreenEffect kind 3（0x1e 帧、色 0、alpha 从 0xff 递减）、
    // 启用绘制并把计时改为 1 —— 下一帧 0x40a410 尾部就清掉 bit2；在此之前绘制位保持关闭
    if ((flags & 4u) == 0) return;
    if (end_timer.current < 0x1e) {
        if (on_screen_fade) on_screen_fade(3, 0x1e, 0u);
        flags |= 1u;
        end_timer.set(1);
    } else {
        tint &= 0x00ffffffu;
        flags &= ~1u;
    }
}

void StageManager::transition_step() {
    if ((flags & 8u) != 0) return;
    step_fade_in();
    tint &= 0x00ffffffu;   // +0x3353（闪白标志）每帧在绘制时取走
    drawn_instances = culled_instances = drawn_quads = 0;
}

void StageManager::end_timer_step(float game_speed) {
    // 0x40a410 尾部：结束计时倒数（0x40d490），归零时 bit1 → bit3 并清 bit1 / bit2
    if ((flags & 8u) != 0) return;
    if ((flags & 4u) != 0 && end_timer.current > 0x1d) tint &= 0x00ffffffu;
    if (end_timer.current > 0) {
        end_timer.untick(game_speed);
        if (end_timer.current < 1) {
            if ((flags & 2u) != 0) flags |= 8u;
            flags &= ~6u;
            tint = 0x00ffffffu;   // 先写 +0x3353 = 0xff，再整字写 0xffffff
        }
    }
}

void StageManager::draw_background(AnmManager& manager, AnmRenderer& renderer, Camera& global_camera, const ScreenShakeOffset& shake) {
    // 0x409f90
    if ((flags & 8u) != 0) return;
    if (renderer.device == nullptr) { transition_step(); return; }
    if (draw_enabled_this_frame(flags, end_timer.current)) {
        renderer.flush();
        look_through_stage_camera(renderer, global_camera, shake);
        renderer.set_depth_write(true);
        renderer.set_depth_compare(Compare::LessEqual);
        renderer.set_fog_parameters(camera.fog.color, camera.fog.near_, camera.fog.far_);
        // 清场地矩形（渲染目标相机视口）：颜色取雾色；新关淡入（bit2）且本关更新帧数 +0x66e0 ≤ 0x21 时清成全黑
        // （0x409f90 读的是帧计数而非结束计时）
        const bool fading_in_black = (flags & 4u) != 0 && frame <= 0x21;
        const ScreenRect field = viewport_rect(global_camera);
        renderer.device->clear(kClearColor | kClearDepth, fading_in_black ? 0u : camera.fog.color, &field);
    }
    step_fade_in();
    if ((tint & 0xff000000u) != 0) {
        renderer.set_tint(true, tint);   // ECL 闪白：本帧背景顶点色乘以 +0x3350
        tint &= 0x00ffffffu;
    }
    drawn_instances = culled_instances = drawn_quads = 0;
    if ((flags & 1u) != 0) {
        renderer.set_fog(true);
        for (int layer = 0; layer < 8; ++layer) draw_layer(layer, manager, renderer, global_camera, shake);
        renderer.flush();
    }
    renderer.set_tint(false, 0x80808080u);
    renderer.set_depth_write(false);
    renderer.flush();
    renderer.set_depth_compare(Compare::Always);
}

void StageManager::draw_foreground(AnmManager& manager, AnmRenderer& renderer, Camera& global_camera,
                                   const ScreenShakeOffset& shake, float game_speed) {
    // 0x40a410
    if ((flags & 8u) != 0) return;
    if (renderer.device == nullptr) { end_timer_step(game_speed); return; }
    if (draw_enabled_this_frame(flags, end_timer.current)) {
        renderer.flush();
        look_through_stage_camera(renderer, global_camera, shake);
        renderer.set_fog(false);
        renderer.set_depth_write(false);
        renderer.set_depth_compare(Compare::Always);
        renderer.draw_layer(0x20);
        renderer.set_depth_compare(Compare::LessEqual);
        renderer.draw_layer(0x21);
        renderer.set_depth_compare(Compare::LessEqual);
        renderer.set_fog_parameters(camera.fog.color, camera.fog.near_, camera.fog.far_);
    }
    if ((flags & 4u) != 0 && end_timer.current > 0x1d) tint &= 0x00ffffffu;
    if ((flags & 1u) != 0) {
        renderer.set_depth_write(false);
        renderer.set_fog(true);
        for (int layer = 8; layer < 12; ++layer) draw_layer(layer, manager, renderer, global_camera, shake);
        renderer.flush();
    }
    renderer.set_tint(false, 0x80808080u);
    end_timer_step(game_speed);
    renderer.set_depth_write(false);
    renderer.flush();
    renderer.set_depth_compare(Compare::Always);
    renderer.set_fog(false);
}

void StageManager::draw_layer_sprites(int layer, AnmManager& manager, AnmRenderer& renderer, Camera& global_camera, const ScreenShakeOffset& shake) {
    // 0x40c280：精灵表可用且层号匹配的层精灵 VM，以全局相机现有矩阵、无雾、Z 关直接画
    for (int i = 0; i < kLayerSprites; ++i) {
        AnmVm& vm = layer_vms[i];
        if (manager.file(vm.file_slot) == nullptr || layer_ids[i] != layer) continue;
        renderer.flush();
        global_camera.shift_x = shake.offset.x;
        global_camera.shift_y = shake.offset.y;
        renderer.use_camera(global_camera);
        renderer.set_fog(false);
        renderer.flush();
        renderer.set_depth_write(false);
        renderer.draw_vm(vm);
        renderer.set_depth_write(true);
        global_camera.look_along_facing();
        renderer.use_camera(global_camera);
    }
}

bool StageManager::instance_visible(const StdObject& object, const Vec3& instance_pos, const Camera& cam,
                                    const AnmGeometry& geometry) const {
    const Vec3 eye = cam.eye();
    const float dx = (object.pos.x + instance_pos.x) - eye.x;
    const float dy = (object.pos.y + instance_pos.y) - eye.y;
    const float dz = (object.pos.z + instance_pos.z) - eye.z;
    if (dy * dy + dx * dx + dz * dz > cull_distance_sq) return false;

    // 包围盒 8 个角 + 中轴线上 8 个点（原版采样顺序）
    const float cx = object.pos.x, cy = object.pos.y, cz = object.pos.z;
    const float hx = object.size.x * 0.5f, hy = object.size.y * 0.5f, hz = object.size.z * 0.5f;
    const float points[16][3] = {
        {cx + hx, cy + hy, cz + hz}, {cx + hx, cy + hy, cz - hz}, {cx + hx, cy - hy, cz + hz}, {cx + hx, cy - hy, cz - hz},
        {cx - hx, cy + hy, cz + hz}, {cx - hx, cy + hy, cz - hz}, {cx - hx, cy - hy, cz + hz}, {cx - hx, cy - hy, cz - hz},
        {cx, cy - hy, cz - hz},      {cx, cy + hy, cz - hz},      {cx, cy - hy, cz + hz},      {cx, cy + hy, cz + hz},
        {cx, cy - hy, cz},           {cx, cy + hy, cz},           {cx, cy - hy, cz - hz * 0.5f}, {cx, cy + hy, cz + hz * 0.5f},
    };
    // D3DXVec3ProjectArray(world = 平移(instance_pos))：只统计视口深度在 [0,1] 内的点
    float world[16], world_view[16], full[16];
    matrix::translation(world, instance_pos.x, instance_pos.y, instance_pos.z);
    matrix::multiply(world_view, world, cam.view);
    matrix::multiply(full, world_view, cam.projection);
    // 0x40ab1c / 0x40abcc：投影坐标与实际场地左上角比较，不能把场地当成从 (0,0) 开始。
    const float field_x = geometry.field_offset_x - float(kFieldWidth) * 0.5f;
    const float field_y = geometry.field_offset_y;
    float max_x = field_x - 8.0f, min_x = field_x + float(kFieldWidth) + 8.0f;
    float max_y = field_y - 8.0f, min_y = field_y + float(kFieldHeight) + 8.0f;
    for (const float* p : points) {
        float clip[4];
        matrix::transform_point(p, full, clip);
        const float inv = 1.0f / clip[3];
        const float sx = (1.0f + clip[0] * inv) * float(cam.viewport.width) * 0.5f + float(cam.viewport.x);
        const float sy = (1.0f - clip[1] * inv) * float(cam.viewport.height) * 0.5f + float(cam.viewport.y);
        const float sz = clip[2] * inv;
        if (sz < 0.0f || sz > 1.0f) continue;
        if (sx > max_x) max_x = sx;
        if (sx < min_x) min_x = sx;
        if (sy < min_y) min_y = sy;
        if (sy > max_y) max_y = sy;
    }
    return max_x >= field_x && min_x <= field_x + float(kFieldWidth) && max_y >= field_y && min_y <= field_y + float(kFieldHeight);
}

void StageManager::draw_layer(int layer, AnmManager& manager, AnmRenderer& renderer, Camera& global_camera, const ScreenShakeOffset& shake) {
    // 0x40af70
    draw_layer_sprites(layer, manager, renderer, global_camera, shake);
    renderer.flush();
    renderer.set_fog(true);
    global_camera.look_along_facing();
    renderer.use_camera(global_camera);
    for (StdInstance& inst : std.instances) {
        StdObject& obj = std.objects[inst.object];
        if (i32(obj.layer) != layer) continue;
        if (!instance_visible(obj, inst.pos, global_camera, renderer.geometry())) {
            ++culled_instances;
            inst.flags &= ~1u;
            continue;
        }
        obj.flags |= 2;
        for (const StdQuad& q : obj.quads) {
            if (q.type != 0) continue;
            AnmVm& vm = face_vms[size_t(q.vm_index)];
            if (vm.render_mode() >= 4) {
                vm.entity_offset = {q.pos.x + inst.pos.x, q.pos.y + inst.pos.y, q.pos.z + inst.pos.z};
                if (anm != nullptr && vm.sprite_index >= 0 && size_t(vm.sprite_index) < anm->sprites.size()) {
                    const AnmRuntimeSprite& sp = anm->sprites[size_t(vm.sprite_index)];
                    if (q.width != 0.0f) { vm.flags_lo |= AnmVm::kScaleDirty; vm.scale.x = q.width / sp.width; }
                    if (q.height != 0.0f) { vm.flags_lo |= AnmVm::kScaleDirty; vm.scale.y = q.height / sp.height; }
                }
            }
            // 世界四边形（mode 8 / 0xf）带雾，其余不带；Z 写由 VM 的 0x2000 位决定
            const u32 mode = vm.render_mode();
            renderer.set_fog(mode == u32(AnmRenderMode::World) || mode == u32(AnmRenderMode::WorldFogToggle));
            renderer.set_depth_write((vm.flags_lo & AnmVm::kZWriteDisable) == 0);
            renderer.draw_vm(vm);
            ++drawn_quads;
        }
        inst.flags |= 1u;
        ++drawn_instances;
    }
    renderer.set_depth_write(false);
}

}  // namespace th16
