// th16 EffectManager 实现——按 th16.exe v1.00a 逐函数还原（地址见各函数头）。
// 充能环（种类 1）的随机量走 DAT_004a6d88（回放同步 RNG），调用顺序与原版逐一对应；
// x87 乘加（r × π、200 + r × 200）在 double 内完成后单次舍入到 float（与扩展精度结果一致）。
#include "Effect.hpp"
#include "AnmRenderer.hpp"

#include "Motion.hpp"
#include "Rng.hpp"

#include <cstring>

namespace th16 {
namespace {

constexpr float kPi = 3.1415927410125732f;  // 0x494588

// DAT_004a2250：效果种类表（update / draw / delete / interrupt 回调下标，+0x5d4 / +0x5d8）
struct EffectKind {
    i16 anm_index;   // 0 = effect.anm（+0xc）/ 1 = bullet.anm（+0x10）
    i16 script;      // host == 0 时生成的脚本号
    i32 callbacks[6];
};
constexpr EffectKind kKindTable[EffectManager::kKinds] = {
    {0, 0, {1, 1, 1, 1, 0, 0}},  // 0x4071a0 过场
    {0, 0, {2, 2, 2, 2, 1, 1}},  // 0x405670 充能环
    {0, 0, {3, 3, 3, 3, 0, 0}},  // 0x406510 爆散粒子
    {0, 0, {3, 3, 3, 3, 0, 0}},  // 0x406930 爆散粒子（变体）
};

// 回调下标 → 种类（update 表 DAT_004919e8：1 → 0x407330，2 → 0x405700，3 → 0x406690，4 → 0x46a0b0 ANM 模式 10）
constexpr int kTransitionCallback = 1;  // 0x407330 / 0x4073a0 / 空 / 0x407900
constexpr int kRingCallback = 2;
constexpr int kBurstCallback = 3;   // delete / interrupt 回调 0x406910 / 0x406920 为空函数

inline float mul_pi(float r) noexcept { return float(double(r) * double(kPi)); }
inline float scale_f(float r, float k) noexcept { return float(double(r) * double(k)); }
inline float base_scale_f(float base, float r, float k) noexcept { return float(double(base) + double(r) * double(k)); }

inline Vec3 add(const Vec3& a, const Vec3& b) noexcept { return {a.x + b.x, a.y + b.y, a.z + b.z}; }
inline Vec3 sub(const Vec3& a, const Vec3& b) noexcept { return {a.x - b.x, a.y - b.y, a.z - b.z}; }
inline Vec3 mul(const Vec3& a, float k) noexcept { return {a.x * k, a.y * k, a.z * k}; }

}  // namespace

void EffectManager::create(AnmManager& animations, AnmFile* effect, AnmFile* bullet) {
    animations_ = &animations;
    effect_anm = effect;
    bullet_anm = bullet;
    cursor = 0;
    std::memset(ids, 0, sizeof ids);
    animations.hooks = this;
}

// 更新链 0x1f（0x418ab0）：清掉已消亡的 id
void EffectManager::sweep() {
    for (u32& id : ids) {
        if (id != 0 && animations_->find(id) == nullptr) id = 0;
    }
}

// 0x40e6c0
i32 EffectManager::allocate_slot() {
    // 原版逐字：返回游标旧值，但检查的是游标新值所指的槽——新槽为空或其 VM 仍活着即返回；死 id 清零后再找
    for (int tries = 0; tries < kIdSlots; ++tries) {
        const u32 previous = cursor;
        cursor = (cursor + 1) & 0x3ffu;
        if (ids[cursor] == 0 || animations_->find(ids[cursor]) != nullptr) return i32(previous);
        ids[cursor] = 0;
    }
    return -1;
}

// 0x41aa00
u32 EffectManager::register_id(u32 id) {
    const i32 slot = allocate_slot();
    if (slot < 0) return 0;
    ids[slot] = id;
    return u32(slot) | 0x80000000u;
}

// 0x418af0
u32 EffectManager::attach(i32 kind, AnmVm* host, const Vec3& pos) {
    if (kind < 0 || kind >= kKinds || kKindTable[kind].script < 0) return 0;
    const EffectKind& k = kKindTable[kind];
    u32 id = 0;
    AnmVm* vm = host;
    if (vm == nullptr) {
        AnmFile* file = k.anm_index == 0 ? effect_anm : bullet_anm;
        if (file == nullptr) return 0;
        id = animations_->spawn(*file, k.script, -1, &vm);
        if (animations_->find(id) == nullptr) { id = 0; vm = nullptr; }
    }
    if (vm == nullptr) return 0;
    switch (kind) {
        case 0: transition_init(*vm); break;
        case 1: ring_init(*vm); break;
        case 2: burst_init(*vm, pos, true); break;
        case 3: burst_init(*vm, pos, false); break;
        default: break;
    }
    vm->update_callback = k.callbacks[0];
    vm->draw_callback = k.callbacks[1];
    vm->delete_callback = k.callbacks[2];
    vm->interrupt_callback = k.callbacks[3];
    std::memcpy(vm->unknown_5d4, &k.callbacks[4], sizeof(i32));
    std::memcpy(vm->unknown_5d4 + 4, &k.callbacks[5], sizeof(i32));
    return host != nullptr ? host->id : id;
}

// 0x418ba0 creates the title transition in the UI list via 0x418fe0.
u32 EffectManager::spawn_title_transition() {
    if (effect_anm == nullptr) return 0;
    const u32 id = animations_->spawn_ui(*effect_anm, kKindTable[0].script);
    AnmVm* vm = animations_->find(id);
    return vm == nullptr ? 0 : attach(0, vm, {});
}

// 0x40e730
u32 EffectManager::spawn(i32 kind, const Vec3& pos) {
    const i32 slot = allocate_slot();
    if (slot < 0) return 0;
    ids[slot] = attach(kind, nullptr, pos);
    return u32(slot) | 0x80000000u;
}

int EffectManager::update(AnmVm& vm) {
    switch (vm.update_callback) {
        case kTransitionCallback: return transition_update(vm);
        case kRingCallback: return ring_update(vm);
        case kBurstCallback: return burst_update(vm);
        default: return 0;
    }
}

void EffectManager::deleted(AnmVm& vm) {
    // 种类 0 的 delete 回调下标 1 对应空 / 仅依赖 vertex_buffer_free；种类 1 走 ring_deleted
    if (vm.delete_callback == kRingCallback) ring_deleted(vm);
}

void EffectManager::interrupted(AnmVm& vm, i32 value) {
    if (vm.interrupt_callback == kTransitionCallback) transition_interrupted(vm, value);
    else if (vm.interrupt_callback == kRingCallback) ring_interrupted(vm, value);
}

// ---------------------------------------------------------------------------------------------
// 种类 1：充能环

// 0x405670
void EffectManager::ring_init(AnmVm& vm) {
    auto* ring = new Ring();
    ring->timer.reset();
    vm.vertex_buffer = ring;
    vm.vertex_buffer_size = 0x193c;
    vm.vertex_buffer_free = [](void* p) { delete static_cast<Ring*>(p); };
}

// 0x405700：光点全部消亡时返回 -1（VM 随之删除）
int EffectManager::ring_update(AnmVm& vm) {
    auto* ring = static_cast<Ring*>(vm.vertex_buffer);
    if (ring == nullptr) return -1;
    AnmManager& anm = *animations_;
    Rng& rng = anm.visual_rng;

    ring->center = vm.entity_offset;
    ring->source = add(vm.entity_offset, polar(vm.rotation.z, 300.0f));
    ring->target = add(vm.entity_offset, polar(vm.float_vars2[2] + vm.rotation.z, 150.0f));

    const i32 cur = ring->timer.current;
    if (cur != ring->timer.previous && cur < 50 && effect_anm != nullptr) {
        static constexpr int kScripts[4] = {0x99, 0x99, 0x99, 0x9a};
        for (int k = 0; k < 4; ++k) ring->ids[cur][k] = anm.spawn(*effect_anm, kScripts[k], -1);
        u8 color[4];
        std::memcpy(color, vm.color, 4);
        for (int k = 0; k < 4; ++k) {
            AnmVm* spawned = anm.find(ring->ids[cur][k]);
            if (spawned == nullptr) { ring->ids[cur][k] = 0; continue; }
            if (k == 3) { for (int c = 0; c < 3; ++c) color[c] = u8(0x2c - color[c]); }
            std::memcpy(spawned->color, color, 4);
            spawned->int_vars[0] = vm.int_vars[0];
        }
    }

    const float slow = AnmManager::root_slowdown(vm);
    int live = 0;
    u32* flat_ids = &ring->ids[0][0];
    for (int i = 0; i < 200; ++i) {
        AnmVm* dot = anm.find(flat_ids[i]);
        if (dot == nullptr) { flat_ids[i] = 0; continue; }
        dot->slowdown = slow;
        if (ring->state[i] == 0) {
            float a = mul_pi(rng.signed_unit());
            float r = scale_f(rng.unit(), 150.0f);
            Vec3 start = add(ring->source, polar(a, r));
            a = mul_pi(rng.signed_unit());
            r = scale_f(rng.unit(), 50.0f);
            Vec3 end = add(ring->target, polar(a, r));
            const Vec3 leg = sub(end, start);
            Vec3 n1 = add(normalize3(leg), normalize3(sub(ring->center, end)));
            const float len1 = base_scale_f(200.0f, rng.unit(), 200.0f);
            n1 = mul(normalize3(n1), len1);
            const float len2 = base_scale_f(100.0f, rng.unit(), 100.0f);
            const Vec3 n2 = mul(normalize3(leg), len2);
            // 0x406240：模式 8 贝塞尔（start → end，切线 n2 / n1）
            dot->pos_interp.end_time = vm.int_vars[0];
            dot->pos_interp.mode = 8;
            dot->pos_interp.initial = start;
            dot->pos_interp.goal = end;
            dot->pos_interp.bezier1 = n2;
            dot->pos_interp.bezier2 = n1;
            dot->pos_interp.timer.reset();
            ring->pos[i] = end;
            ring->dir[i] = n1;
            ring->state[i] = 1;
        } else if (vm.int_vars[0] + 1 <= dot->age.current && ring->state[i] == 1) {
            float a = mul_pi(rng.signed_unit());
            float r = scale_f(rng.unit(), 20.0f);
            const Vec3 goal = add(vm.entity_offset, polar(a, r));
            a = mul_pi(rng.signed_unit());
            r = scale_f(rng.unit(), 20.0f);
            const Vec3 tangent = polar(a, r);
            dot->pos_interp.end_time = vm.int_vars[0];
            dot->pos_interp.mode = 8;
            dot->pos_interp.initial = ring->pos[i];
            dot->pos_interp.goal = goal;
            dot->pos_interp.bezier1 = ring->dir[i];
            dot->pos_interp.bezier2 = tangent;
            dot->pos_interp.timer.reset();
            ring->state[i] = 2;
        }
        ++live;
    }
    if (live == 0) return -1;
    ring->timer.tick(anm.game_speed);
    return 0;
}

// 0x405ed0：删除时停掉尚存的光点
void EffectManager::ring_deleted(AnmVm& vm) {
    auto* ring = static_cast<Ring*>(vm.vertex_buffer);
    if (ring == nullptr) return;
    u32* flat_ids = &ring->ids[0][0];
    for (int i = 0; i < 200; ++i) {
        AnmVm* dot = animations_->find(flat_ids[i]);
        if (dot == nullptr) { flat_ids[i] = 0; continue; }
        dot->flags_lo &= ~1u;
        dot->ins_offset = -1;
    }
}

// 0x405f20：中断 1 → 计时器一次跳过 300 帧（停止继续生成光点）
void EffectManager::ring_interrupted(AnmVm& vm, i32 value) {
    if (value != 1) return;
    auto* ring = static_cast<Ring*>(vm.vertex_buffer);
    if (ring == nullptr) return;
    ring->timer.advance(300.0f, animations_->game_speed);
}

// ---------------------------------------------------------------------------------------------
// 种类 2 / 3：爆散粒子

// 0x406510（orange：以 (320, 16) 为中心偏移、层 0xf、64 帧淡出）/ 0x406930（层 0x13）
void EffectManager::burst_init(AnmVm& vm, const Vec3& pos, bool orange) {
    auto* burst = new Burst();
    vm.vertex_buffer = burst;
    vm.vertex_buffer_size = 0x318;
    vm.vertex_buffer_free = [](void* p) { delete static_cast<Burst*>(p); };
    burst->pos[0] = orange ? Vec2{pos.x + 320.0f, pos.y + 16.0f} : Vec2{0.0f, 0.0f};
    burst->angle = mul_pi(animations_->game_rng.signed_unit());
    burst->timer.set(1);
    // 前 8 个光点按下标调亮一个通道，后 32 个按下标递减透明度
    u8 fade = 0;
    for (int i = 0; i < Burst::kDots; ++i) {
        u8* c = burst->color[i];
        if (orange) { c[0] = 0xff; c[1] = 0x80; c[2] = 0x00; c[3] = 0xff; }
        else { c[0] = 0x50; c[1] = 0x50; c[2] = 0x50; c[3] = 0xff; }
        if (i < 8) c[orange ? 2 : 0] = u8(~(i << 5));
        if (i >= 0x20) c[3] = u8(~(fade++ << 4));
    }
    vm.entity_offset = pos;
    vm.flags_hi = (vm.flags_hi & ~0x80000u) | 0x40000u;
    if (orange) {
        vm.layer = 0xf;
        vm.flags_lo = (vm.flags_lo & ~0x1c0u) | 0x20u;
        // 0x406ce0(0x40, 0, 0xff, 0)：alpha 0xff → 0 线性 64 帧
        vm.alpha_interp.end_time = 0x40;
        vm.alpha_interp.mode = 0;
        vm.alpha_interp.initial = 0xff;
        vm.alpha_interp.goal = 0;
        vm.alpha_interp.bezier1 = 0;
        vm.alpha_interp.bezier2 = 0;
        vm.alpha_interp.timer.reset();
    } else {
        vm.flags_lo &= ~0x1e0u;
        vm.layer = 0x13;
    }
}

bool EffectManager::burst_geometry(const AnmVm& vm, BurstGeometry& out) {
    const auto* burst = static_cast<const Burst*>(vm.vertex_buffer);
    if (vm.draw_callback != kBurstCallback || burst == nullptr) return false;
    out.points = burst->pos;
    out.colors = burst->color;
    out.count = burst->timer.current;
    return true;
}

// 0x406690：每帧放出第 n 个光点（速度 4 + [0,5)，方向 = 上一方向 + [-π,π)/5），同时把已放出的光点透明度 -0x10；
// 第 64 帧返回 1 → VM 删除
int EffectManager::burst_update(AnmVm& vm) {
    auto* burst = static_cast<Burst*>(vm.vertex_buffer);
    if (burst == nullptr) return 1;
    const i32 n = burst->timer.current;
    if (n > 0x3f) return 1;
    if (n != burst->timer.previous) {
        for (int i = 0; i < n; ++i) {
            u8& alpha = burst->color[i][3];
            alpha = alpha < 0x10 ? u8(0) : u8(alpha - 0x10);
        }
        Rng& rng = animations_->game_rng;
        const float speed = rng.unit() * 5.0f + 4.0f;
        const Vec3 step = polar(burst->angle, speed);
        burst->pos[n].x = burst->pos[n - 1].x + step.x;
        burst->pos[n].y = burst->pos[n - 1].y + step.y;
        burst->angle = wrap_angle(mul_pi(rng.signed_unit()) / 5.0f + burst->angle);
    }
    burst->timer.speed_slot = 0;
    burst->timer.tick(animations_->game_speed);
    return 0;
}


// 0x4071a0：过场种类 init——4 个面板（effect.anm 脚本 3..6）+ 覆盖（脚本 0xc5），中心 (320, 240)
void EffectManager::transition_init(AnmVm& vm) {
    auto* tr = new Transition{};
    vm.vertex_buffer = tr;
    vm.vertex_buffer_size = 0x1df4;
    vm.vertex_buffer_free = [](void* p) { delete static_cast<Transition*>(p); };
    vm.flags_hi &= 0xfff3ffffu;
    anm_set_layer(vm, 0x28);
    if (effect_anm == nullptr || animations_ == nullptr) return;
    for (int i = 0; i < 4; ++i) {
        AnmVm& panel = tr->panels[i];
        animations_->init_from_template(*effect_anm, panel, i + 3);
        panel.root = nullptr;
        panel.parent = nullptr;
        animations_->run(panel);
        panel.entity_offset = {320.0f, 240.0f, 0.0f};
    }
    tr->mode = 0;
    vm.flags_hi = (vm.flags_hi & 0xff93ffffu) | 0x100000u;
    anm_set_layer(vm, 0x28);
    for (int i = 0; i < 4; ++i) tr->panels[i].entity_offset = {320.0f, 240.0f, 0.0f};
    animations_->init_from_template(*effect_anm, tr->overlay, 0xc5);
    tr->overlay.root = nullptr;
    tr->overlay.parent = nullptr;
    animations_->run(tr->overlay);
}

// 0x407330
int EffectManager::transition_update(AnmVm& vm) {
    auto* tr = static_cast<Transition*>(vm.vertex_buffer);
    if (tr == nullptr || animations_ == nullptr) return 0;
    int stopped = 0;
    for (int i = 0; i < 4; ++i) {
        if (animations_->run(tr->panels[i]) != 0) ++stopped;
    }
    if (stopped > 3) return 1;
    animations_->run(tr->overlay);
    ++tr->frames;
    return 0;
}

// 0x407900
void EffectManager::transition_interrupted(AnmVm& vm, i32 value) {
    auto* tr = static_cast<Transition*>(vm.vertex_buffer);
    if (tr == nullptr || animations_ == nullptr || effect_anm == nullptr) return;
    switch (value) {
    case 1:
        for (int i = 0; i < 4; ++i) {
            animations_->init_from_template(*effect_anm, tr->panels[i], i + 7);
            tr->panels[i].root = nullptr;
            tr->panels[i].parent = nullptr;
            animations_->run(tr->panels[i]);
        }
        return;
    case 7:
        tr->mode = 0;
        anm_set_layer(vm, 0x1e);
        vm.flags_hi = (vm.flags_hi & 0xff93ffffu) | 0x100000u;
        break;
    case 8:
        tr->mode = 1;
        anm_set_layer(vm, 0x17);
        vm.flags_hi = (vm.flags_hi & 0xff9bffffu) | 0x180000u;
        break;
    case 9:
        tr->mode = 0;
        anm_set_layer(vm, 0x24);
        vm.flags_hi = (vm.flags_hi & 0xff93ffffu) | 0x100000u;
        break;
    case 10:
        tr->mode = 3;
        anm_set_layer(vm, 0x1e);
        vm.flags_hi = (vm.flags_hi & 0xff93ffffu) | 0x100000u;
        break;
    default:
        return;
    }
    for (int i = 0; i < 4; ++i) tr->panels[i].entity_offset = {320.0f, 240.0f, 0.0f};
}


// 0x4073a0（绘制钩子 1）
void EffectManager::transition_draw(AnmVm& vm, AnmRenderer& renderer, u32 width, u32 height) {
    auto* tr = static_cast<Transition*>(vm.vertex_buffer);
    if (tr == nullptr || renderer.device == nullptr) return;
    // 原版：mode==2，或 mode==0 且后缓冲格式 == D3DFMT_A8R8G8B8(0x15)；移植的 RT 恒为等价 32 位色
    const bool clear_first = tr->mode == 2 || tr->mode == 0;
    if (clear_first) {
        // 先把目标的 alpha 通道清零（颜色不动）：ALPHATESTENABLE=0、SEPARATEALPHABLEND=1、
        // SRCBLEND=ZERO / DESTBLEND=ONE、SRCBLENDALPHA=ONE / DESTBLENDALPHA=ZERO，SELECTARG1=DIFFUSE，
        // 顶点色 0 的全幅（mode 2：场地 112,16–512,464）XYZRHW|DIFFUSE 条带；
        // 之后面板按自身混合模式写 alpha，覆盖（模式 7 DESTALPHA）只在面板处显形
        renderer.flush();   // 0x465a80
        using namespace touhou::graphics;
        PipelineState& p = renderer.device->pipeline();
        p.alphaTest = false;
        p.separateAlphaBlend = true;
        p.sourceBlend = BlendFactor::Zero;
        p.destinationBlend = BlendFactor::One;
        p.blendEquation = BlendEquation::Add;
        p.sourceAlphaBlend = BlendFactor::One;
        p.destinationAlphaBlend = BlendFactor::Zero;
        p.alphaBlendEquation = BlendEquation::Add;
        p.alpha.operation = p.color.operation = ColorOperation::First;
        p.alpha.first = p.color.first = {ArgumentSource::Diffuse};
        const ScreenRect mask = tr->mode == 2 ? ScreenRect{112, 16, 512, 464}
                                              : ScreenRect{0, 0, i32(width), i32(height)};
        renderer.device->draw_alpha_mask(mask);
        p.alphaTest = true;
        p.alpha.operation = p.color.operation = ColorOperation::Multiply;
        p.alpha.first = p.color.first = {ArgumentSource::Texture};
        renderer.invalidate_blend_cache();   // +0x184fbb4 = 10：面板首个 VM 重新下发 SRCBLEND / DESTBLEND
        p.sourceAlphaBlend = BlendFactor::SourceAlpha;
        p.destinationAlphaBlend = BlendFactor::One;
        p.alphaBlendEquation = BlendEquation::Add;
    }
    for (int i = 0; i < 4; ++i) renderer.draw_vm(tr->panels[i]);
    if (clear_first) {
        renderer.device->pipeline().separateAlphaBlend = false;
        renderer.draw_vm(tr->overlay);
        renderer.flush();
    }
}

}  // namespace th16
