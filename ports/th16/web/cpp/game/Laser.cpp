// th16 LaserManager 实现（原版函数锚点见各函数头；总览见 Laser.hpp）
#include "Laser.hpp"

#include "AnmRenderer.hpp"
#include "Effect.hpp"
#include "Enemy.hpp"
#include "Motion.hpp"
#include "Player.hpp"
#include <cmath>
#include <cstring>

namespace th16 {

namespace {

constexpr float kPi = 3.1415927410125732f;       // 0x494588
constexpr float kTwoPi = 6.2831854820251465f;    // 0x4945b8
constexpr float kHalfPi = 1.5707963705062866f;   // 0x494534
constexpr float kThreePi = 9.424778f;            // 0x4945c8
constexpr float kFarTime = 999999.0f;            // 0x497423f0：轨迹段「无终点」
constexpr float kSampleSpacing = 16.0f;          // 消弹采样间距（像素）
constexpr float kMinFragmentLength = 24.0f;      // 切分后短于此长度的直线激光直接销毁
constexpr i32 kExCount = 18;

// 判定区域（场地坐标）：x ∈ (-192, 192)，y ∈ (0, 448)
constexpr float kFieldLeft = -192.0f;
constexpr float kFieldRight = 192.0f;
constexpr float kFieldTop = 0.0f;
constexpr float kFieldBottom = 448.0f;

bool point_near_field(float x, float y, float margin) noexcept {
    return kFieldLeft < x + margin && x - margin < kFieldRight && kFieldTop < y + margin && y - margin < kFieldBottom;
}

// 各 run_ex 的「本步能否执行」判定：空步骤或（同步步骤且已有活动 Ex）时停止
bool ex_blocked(const BulletExStep& step, u32 ex_flags) noexcept {
    return step.type == 0 || (step.async == 0 && ex_flags != 0);
}

// 弹种 → 消弹特效脚本族：直线 / 照射激光消散用 bullet.anm 的三档脚本（颜色 ×2 + 基号）；超出范围不出特效
bool cancel_effect_script(i32 bullet_type, i32 color, bool rect_thresholds, i32& script) noexcept {
    if (bullet_type < 0x12 || bullet_type == 0x22 || bullet_type == 0x26) { script = color * 2 + 0xd1; return true; }
    if (rect_thresholds) {
        // 0x433860 / 0x435880：矩形消弹对中档的弹种上限略有不同
        if (bullet_type < 0x1f) { script = color * 2 + 0x101; return true; }
        if (bullet_type <= 0x21) { script = color * 2 + 0x119; return true; }
        return false;
    }
    if (bullet_type < 0x20 || bullet_type == 0x1b) { script = color * 2 + 0x101; return true; }
    if (bullet_type < 0x22) { script = color * 2 + 0x119; return true; }
    return false;
}

// 0x404220：两条直线（点 + 朝向角）的交点。返回 false 表示平行 / 重合不可解（out 置为 P1）
bool intersect_lines(Vec2& out, const Vec3& p1, float angle1, const Vec3& p2, float angle2) noexcept {
    const float a1 = wrap_angle(angle1);
    const float a2 = wrap_angle(angle2);
    auto signed_difference = [&]() {
        const float d = a1 - a2;
        if (d <= kPi) return (kPi < a2 - a1) ? a1 - (a2 - kTwoPi) : d;
        return a1 - (a2 + kTwoPi);
    };
    const float diff = wrap_angle(signed_difference());
    out = {p1.x, p1.y};
    if (std::fabs(diff) < 0.001f) return false;
    const float again = wrap_angle(signed_difference());
    float opposite = again - kPi;
    if (opposite <= kPi) {
        if (kPi < kPi - again) opposite = again + kPi;
    } else {
        opposite = again - kThreePi;
    }
    opposite = wrap_angle(opposite);
    if (std::fabs(opposite) < 0.001f) return false;

    const Vec3 dir1 = polar(angle1, 10.0f);
    const bool sloped1 = 0.01f <= std::fabs(dir1.x);
    float slope1 = 0, intercept1 = p1.x;
    if (sloped1) { slope1 = dir1.y / dir1.x; intercept1 = p1.y - slope1 * p1.x; }
    const Vec3 dir2 = polar(angle2, 10.0f);
    const bool sloped2 = 0.01f <= std::fabs(dir2.x);
    float slope2 = 0, intercept2 = p2.x;
    if (sloped2) { slope2 = dir2.y / dir2.x; intercept2 = p2.y - slope2 * p2.x; }
    if (sloped1) {
        if (sloped2) {
            out.x = (intercept2 - intercept1) / (slope1 - slope2);
            out.y = ((intercept2 - intercept1) * slope1) / (slope1 - slope2) + intercept1;
            return true;
        }
        out.x = p2.x;
        out.y = slope1 * p2.x + intercept1;
        return true;
    }
    if (sloped2) {
        out.x = p1.x;
        out.y = slope2 * p1.x + intercept2;
        return true;
    }
    if (std::fabs(p1.x - p2.x) < 0.001f) { out = {p1.x, p1.y}; return true; }
    return false;
}

// 0x403ec0：线段 AB 与线段 CD 的交点（叉积判相交，共线时用包围盒）；不相交返回 false 且不写 out
bool intersect_segments(Vec2& out, Vec2 a, Vec2 b, Vec2 c, Vec2 d) noexcept {
    const float cross_c = (d.y - a.y) * (a.x - b.x) + (a.y - b.y) * (a.x - c.x);
    const float cross_d = (a.x - d.x) * (a.y - b.y) + (d.y - a.y) * (a.x - b.x);
    if (cross_d * cross_c > 0.0f) return false;
    if (cross_c == 0.0f && cross_d == 0.0f) {
        Vec2 lo1 = a, hi1 = b;
        if (b.x < a.x) { lo1 = b; hi1 = a; }
        Vec2 lo2 = c, hi2 = d;
        if (d.x < c.x) { lo2 = d; hi2 = c; }
        if (hi2.x < lo1.x || hi2.y < lo1.y || hi1.x < lo2.x || hi1.y < lo2.y) return false;
    } else if (0.0f < ((c.x - a.x) * (c.y - d.y) + (a.y - c.y) * (c.x - d.x)) *
                          ((c.x - b.x) * (c.y - d.y) + (b.y - c.y) * (c.x - d.x))) {
        return false;
    }
    const float dx1 = b.x - a.x;
    float slope1 = 0, intercept1 = a.x;
    if (0.01f <= std::fabs(dx1)) { slope1 = (b.y - a.y) / dx1; intercept1 = a.y - ((b.y - a.y) * a.x) / dx1; }
    const float dx2 = d.x - c.x;
    float slope2 = 0, intercept2 = c.x;
    const bool sloped2 = 0.01f <= std::fabs(dx2);
    if (sloped2) { slope2 = (d.y - c.y) / dx2; intercept2 = c.y - ((d.y - c.y) * c.x) / dx2; }
    if (0.01f <= std::fabs(dx1)) {
        if (sloped2) {
            out.x = (intercept2 - intercept1) / (slope1 - slope2);
            out.y = ((intercept2 - intercept1) * slope1) / (slope1 - slope2) + intercept1;
            return true;
        }
        out.x = c.x;
        out.y = slope1 * c.x + intercept1;
        return true;
    }
    if (sloped2) {
        out.x = a.x;
        out.y = slope2 * a.x + intercept2;
        return true;
    }
    if (std::fabs(a.x - c.x) < 0.001f) { out = a; return true; }
    return false;
}

// 0x431510 内联：两位延迟销毁计数自增（不向 bit3 进位）
void advance_destroy_count(Laser& l) noexcept {
    l.lifecycle = (l.lifecycle & ~kLaserDestroyCountMask) | ((l.lifecycle + 2u) & kLaserDestroyCountMask);
}

void mark_for_destroy(Laser& l) noexcept {
    l.lifecycle = (l.lifecycle & ~0x4u) | 0x2u;
}

// 原版各遍历（0x431510 / 0x431a70 / 0x431af0 / 0x431950 / 0x4319e0）都先取好 next 再处理当前激光：
// 链尾激光处理期间派生的子激光（反弹 / 切分）要到下一次遍历才被访问。fn 返回 true 表示摘除当前激光
template <typename Fn>
void visit_existing(std::vector<std::unique_ptr<Laser>>& lasers, Fn&& fn) {
    size_t i = 0;
    while (i < lasers.size()) {
        const bool had_next = i + 1 < lasers.size();
        if (fn(*lasers[i])) lasers.erase(lasers.begin() + i32(i));
        else ++i;
        if (!had_next) break;
    }
}

}  // namespace

// =============================================================================================
// 生命周期

// 0x4314a0 / 0x431330
void LaserManager::create(AnmManager& anm_manager, AnmFile& bullet_file, AnmFile* effect_file, BulletManager& bullet_mgr,
                          Player* pl, EnemyManager* enm_mgr, EffectManager* eff_mgr, const PlayerWorld* w) {
    animations = &anm_manager;
    bullet_anm = &bullet_file;
    effect_anm = effect_file;
    bullets = &bullet_mgr;
    player = pl;
    enemies = enm_mgr;
    effects = eff_mgr;
    world = w;
    animations->sprite_mappers[kBeamSpriteMapper] = &LaserManager::map_sprite_beam;
    animations->sprite_mappers[kCurveSpriteMapper] = &LaserManager::map_sprite_curve;
    reset();
}

// 0x42cb00
void LaserManager::reset() {
    lasers.clear();
    sfx_queue.clear();
}

void LaserManager::push_sfx(i32 sfx, float x) {
    if (sfx_sink != nullptr) sfx_sink->push(sfx, x);
    else sfx_queue.push_back(sfx);
}

// 0x431fa0
i32 LaserManager::map_sprite_beam(AnmVm& vm, i32 index) {
    const Laser* l = static_cast<const Laser*>(vm.owner);
    if (l == nullptr) return index;
    const BulletTypeInfo& info = bullet_type_info(l->bullet_type);
    if (info.sprite_map[0] < 0) return index;
    const i32 slot = l->bullet_color * 4 + index;
    if (slot < 0 || slot >= 64) return index;
    return info.sprite_map[slot];
}

// 0x43a840
i32 LaserManager::map_sprite_curve(AnmVm& vm, i32) {
    const Laser* l = static_cast<const Laser*>(vm.owner);
    return l == nullptr ? 0x20c : l->bullet_color + 0x20c;
}

// 0x41aa40
Laser* LaserManager::find_by_id(i32 id) noexcept {
    if (id == 0) return nullptr;
    for (auto& l : lasers)
        if (l->id == id) return l.get();
    return nullptr;
}

// 0x431760 前半（含 0x413860 / 0x432620 / 0x434730 等处的内联展开）
Laser* LaserManager::allocate(LaserKind kind) {
    if (i32(lasers.size()) >= kMaxLasers) return nullptr;
    ++next_handle;
    if (next_handle < kFirstHandle) next_handle = kFirstHandle;
    lasers.push_back(std::make_unique<Laser>());
    Laser& l = *lasers.back();
    l.kind = kind;
    l.id = next_handle;
    l.age_timer.reset();  // 0x430fc0：构造只复位存活计时（previous = -1），其余计时器留给各 init
    l.vm_beam.owner = &l;
    l.vm_origin.owner = &l;
    l.vm_tip.owner = &l;
    return &l;
}

i32 LaserManager::spawn_line(const LineLaserArgs& args) {
    Laser* l = allocate(LaserKind::Line);
    if (l == nullptr) return 0;
    l->line = args;
    init_line(*l);
    return l->id;
}

i32 LaserManager::spawn_infinite(const InfiniteLaserArgs& args) {
    Laser* l = allocate(LaserKind::Infinite);
    if (l == nullptr) return 0;
    l->infinite = args;
    init_infinite(*l);
    return l->id;
}

i32 LaserManager::spawn_curve(const CurveLaserArgs& args) {
    Laser* l = allocate(LaserKind::Curve);
    if (l == nullptr) return 0;
    l->curve = args;
    init_curve(*l);
    return l->id;
}

i32 LaserManager::spawn_beam(const BeamLaserArgs& args) {
    Laser* l = allocate(LaserKind::Beam);
    if (l == nullptr) return 0;
    l->beam = args;
    init_beam(*l);
    return l->id;
}

// 0x413860 Ex 0x8000000：弹幕在自身位置派生激光（a==0 直线 / a==1 照射）；参数取本步与下一步。
// 在弹幕更新内即时调用：生成音效先于该弹随后的消弹音效，新激光挂到链尾、下一帧起推进
void LaserManager::spawn_from_bullet(const BulletLaserRequest& req) {
    {
        const BulletExStep& st = req.step;
        const BulletExStep& next = req.next;
        if (req.kind == 0) {
            LineLaserArgs args;
            args.origin = req.pos;
            args.angle = req.angle;
            args.target_length = st.n;
            args.initial_length = st.m;
            args.max_extent = next.r;
            args.width = next.s;
            args.speed = req.speed;
            args.type = st.b;
            args.color = st.c;
            args.start_offset = next.m;
            args.start_ex = next.c;
            args.flags = 0x1u;
            std::memcpy(args.ex, req.ex, sizeof args.ex);
            args.spawn_sfx = next.a;
            args.step_sfx = next.b;
            spawn_line(args);
        } else {
            InfiniteLaserArgs args;
            args.origin = req.pos;
            args.angle = req.angle;
            args.target_length = st.n;
            args.initial_length = st.m;
            args.width = next.r;
            args.speed = req.speed;
            args.telegraph_time = next.a;
            args.expand_time = next.b;
            args.active_time = next.c;
            args.shrink_time = next.d;
            args.spawn_sfx = 0x12;
            args.step_sfx = -1;
            args.handle = 0;
            args.start_offset = next.s;
            args.start_ex = (st.d >> 8) & 0xff;
            args.type = st.b;
            args.color = st.c;
            args.flags = (u32(st.d) & 0xfdu) | 0x2u;
            std::memcpy(args.ex, req.ex, sizeof args.ex);
            spawn_infinite(args);
        }
    }
}

// =============================================================================================
// 每帧入口

// 0x4316b0 → 0x431510
void LaserManager::update(float gs) {
    game_speed = gs;
    visit_existing(lasers, [&](Laser& l) -> bool {
        if ((l.lifecycle & kLaserDestroyCountMask) != 0) {
            advance_destroy_count(l);
            if ((l.lifecycle & kLaserDestroyCountMask) >= 4u) return true;
        }
        if (l.state == LaserState::Dead) return true;
        if ((l.lifecycle & kLaserPaused) != 0) {
            collide_player(l, true);
            return false;
        }
        if (step(l)) return true;
        l.age_timer.tick(gs);
        l.lifecycle |= kLaserRanOnce;
        return false;
    });
}

// 0x431720
void LaserManager::draw(AnmRenderer& renderer) {
    for (auto& ptr : lasers) {
        Laser& l = *ptr;
        if (l.state == LaserState::Dead) continue;
        switch (l.kind) {
            case LaserKind::Line: draw_line(l, renderer); break;
            case LaserKind::Infinite: draw_infinite(l, renderer); break;
            case LaserKind::Curve: draw_curve(l, renderer); break;
            case LaserKind::Beam: break;   // 0x43ac10
        }
    }
}

// 0x433720
void LaserManager::draw_line(Laser& l, AnmRenderer& renderer) {
    l.vm_beam.pos = l.pos;
    l.vm_beam.rotation.z = wrap_angle(l.angle + kHalfPi);
    l.vm_beam.flags_lo |= AnmVm::kRotationDirty;
    renderer.draw_vm(l.vm_beam);
    const Vec3 tip = polar(l.angle, l.length);
    l.vm_tip.pos = {tip.x + l.pos.x, l.pos.y + tip.y, 0.0f};
    renderer.draw_vm(l.vm_tip);
    if (l.start_drawn_len == 0.0f) {
        l.vm_origin.pos = l.pos;
        renderer.draw_vm(l.vm_origin);
    }
}

// 0x4357a0
void LaserManager::draw_infinite(Laser& l, AnmRenderer& renderer) {
    l.vm_beam.pos = l.pos;
    l.vm_beam.rotation.z = wrap_angle(l.angle + kHalfPi);
    l.vm_beam.flags_lo |= AnmVm::kRotationDirty;
    renderer.draw_vm(l.vm_beam);
    if (l.start_drawn_len == 0.0f) {
        l.vm_origin.pos = l.pos;
        renderer.draw_vm(l.vm_origin);
    }
}

// 0x438750
void LaserManager::draw_curve(Laser& l, AnmRenderer& renderer) {
    const i32 n = l.curve.node_count;
    if (n <= 0 || i32(l.curve_nodes.size()) < n) return;
    build_curve_strip(l, renderer.geometry().field_offset_x, renderer.geometry().field_offset_y);
    renderer.draw_host_strip(l.vm_beam, l.curve_strip.data(), u32(n) * 2);
    // 采样时刻已覆盖全部节点（frame_timer.current ≤ 节点数时尾节点仍在原点附近）→ 在末节点画根部光球
    if (l.frame_timer.current <= n) {
        l.vm_origin.pos = l.curve_nodes[u32(n - 1)].pos;
        renderer.draw_vm(l.vm_origin);
    }
}

// 0x438750 前半
void LaserManager::build_curve_strip(Laser& l, float field_offset_x, float field_offset_y) const {
    const i32 n = l.curve.node_count;
    const float half_width = l.curve.width * 0.5f;
    // 0x438750：x 加 DAT_004d9d50（场地中线），y 加 DAT_004d9d3c（实际场地上沿），z 清零。
    const float screen_x = field_offset_x;
    const float screen_y = field_offset_y;
    const float v_top = l.vm_beam.uv[0].y;      // +0x3ac
    const float v_bottom = l.vm_beam.uv[2].y;   // +0x3bc
    // 0x405390 形：prev − cur 跨 ±π 时取另一侧
    auto signed_difference = [](float prev, float cur) {
        const float d = prev - cur;
        if (d <= kPi) return (kPi < cur - prev) ? prev - (cur - kTwoPi) : d;
        return prev - (cur + kTwoPi);
    };
    // 第 i 点的侧向角：首点取自身朝向 ± π/2；其余取与前一点朝向（同样 ± π/2）的角平分线
    auto side_angle = [&](i32 i, float quarter_turn) {
        const float cur = wrap_angle(l.curve_nodes[u32(i)].angle + quarter_turn);
        if (i == 0) return cur;
        const float prev = wrap_angle(l.curve_nodes[u32(i - 1)].angle + quarter_turn);
        const float half = wrap_angle(wrap_angle(signed_difference(prev, cur)) * 0.5f);
        return wrap_angle(half + cur);
    };

    l.curve_strip.resize(u32(n) * 2);
    float u = 0.0f;
    const float u_step = 1.0f / float(n - 1);   // n == 1 时为 +∞，与原版一致（只写第一对顶点的 u = 0）
    for (i32 i = 0; i < n; ++i) {
        const Vec3& p = l.curve_nodes[u32(i)].pos;
        ScreenColorUvVertex* pair = &l.curve_strip[u32(i) * 2];
        const float sides[2] = {kHalfPi, -kHalfPi};
        const float vs[2] = {v_top, v_bottom};
        for (int k = 0; k < 2; ++k) {
            const Vec3 offset = polar(side_angle(i, sides[k]), half_width);
            pair[k].x = offset.x + p.x + screen_x;
            pair[k].y = offset.y + p.y + screen_y;
            pair[k].z = 0.0f;
            pair[k].rhw = 1.0f;
            pair[k].color = 0xffffffffu;
            pair[k].u = u;
            pair[k].v = vs[k];
        }
        u = u_step + u;
    }
}

bool LaserManager::step(Laser& l) {
    switch (l.kind) {
        case LaserKind::Line: return step_line(l);
        case LaserKind::Infinite: return step_infinite(l);
        case LaserKind::Curve: return step_curve(l);
        case LaserKind::Beam: return false;  // 0x43ac00
    }
    return false;
}

void LaserManager::collide_player(Laser& l, bool force_graze) {
    switch (l.kind) {
        case LaserKind::Line: collide_player_line(l, force_graze); break;
        case LaserKind::Infinite: collide_player_infinite(l, force_graze); break;
        case LaserKind::Curve: collide_player_curve(l, force_graze); break;
        case LaserKind::Beam: break;
    }
}

// =============================================================================================
// VM 绑定

// 激光柱 VM：reset → 绑弹种脚本并跑首帧 → 中断 2 再跑一帧 → 混合 / 层位标志
void LaserManager::bind_beam_vm(Laser& l, int script, bool curve_mapper) {
    AnmVm& vm = l.vm_beam;
    AnmManager::reset_vm(vm);
    if (!curve_mapper || l.bullet_type != 1) {
        vm.sprite_mapper = curve_mapper ? kCurveSpriteMapper : kBeamSpriteMapper;
        vm.owner = &l;
    }
    animations->init_script(*bullet_anm, vm, script);
    vm.pending_interrupt = 2;
    animations->run(vm);
    vm.flags_lo = (vm.flags_lo & 0xfffffe3fu) | 0x20u;
    vm.flags_lo = (vm.flags_lo & 0xc31fffffu) | 0x3000000u;
    vm.flags_hi = (vm.flags_hi & 0xfff7ffffu) | 0x40000u;
}

// 根部 / 前端光球 VM：模板复制 → 跑首帧 →（extra_flags：中断 2 再跑一帧并置混合 / 层位）
void LaserManager::bind_glow_vm(AnmVm& vm, int script, bool extra_flags) {
    animations->init_from_template(*bullet_anm, vm, script);
    vm.root = nullptr;
    vm.parent = nullptr;
    animations->run(vm);
    if (extra_flags) {
        vm.pending_interrupt = 2;
        animations->run(vm);
        vm.flags_lo = (vm.flags_lo & 0xfffffe3fu) | 0x20u;
        vm.flags_lo = (vm.flags_lo & 0xc3ffffffu) | 0x2000000u;
    }
    vm.flags_hi = (vm.flags_hi & 0xfff7ffffu) | 0x40000u;
}

void LaserManager::sync_beam_scale(Laser& l) {
    const AnmFile* file = animations->file(l.vm_beam.file_slot);
    const i32 sprite = l.vm_beam.sprite_index;
    if (file == nullptr || sprite < 0 || u32(sprite) >= file->sprites.size()) return;
    const AnmRuntimeSprite& s = file->sprites[u32(sprite)];
    l.vm_beam.flags_lo |= AnmVm::kScaleDirty;
    l.vm_beam.scale.x = l.width / s.width;
    l.vm_beam.scale.y = l.length / s.height;
}

// =============================================================================================
// 直线激光

// 0x431b30
void LaserManager::init_line(Laser& l) {
    LineLaserArgs& a = l.line;
    l.bullet_type = a.type;
    l.state = LaserState::Active;
    l.bullet_color = a.color;
    bind_beam_vm(l, bullet_type_info(l.bullet_type).script, false);
    bind_glow_vm(l.vm_origin, a.color + 0x38, true);
    const bool round_tip = l.bullet_type < 0x12 || l.bullet_type == 0x26;
    animations->init_from_template(*bullet_anm, l.vm_tip, a.color + (round_tip ? 0x5b : 0x53));
    l.vm_tip.root = nullptr;
    l.vm_tip.parent = nullptr;
    animations->run(l.vm_tip);
    if (round_tip) l.vm_tip.flags_lo = (l.vm_tip.flags_lo & 0xfffffe3fu) | 0x20u;
    l.vm_tip.flags_hi = (l.vm_tip.flags_hi & 0xfff7ffffu) | 0x40000u;

    l.spawn_grace_timer.set(30);
    l.invuln_timer.set(3);
    if (a.spawn_sfx >= 0) push_sfx(a.spawn_sfx);
    l.graze_timer.reset();
    l.frame_timer.reset();
    l.pos = a.origin;
    if (a.start_offset != 0.0f) {
        const Vec3 shift = polar(a.angle, a.start_offset);
        l.pos.x = l.pos.x + shift.x;
        l.pos.y = l.pos.y + shift.y;
    }
    l.width = a.width;
    l.length = a.initial_length;
    l.speed = a.speed;
    l.angle = a.angle;
    l.start_drawn_len = (a.initial_length <= a.target_length) ? 0.0f : 0.01f;
    l.velocity = polar(a.angle, a.speed);
    l.ex_index = a.start_ex;
}

// 各 update 的 Ex 循环尾段：等待计时倒数（归零时结束等待并要求再跑一轮）、消弹免疫倒数
void LaserManager::run_ex_wait_and_immunity(Laser& l, int* retry_count) {
    if ((l.ex_flags & kExWait) != 0) {
        if (l.ex.wait_timer.current < 1) {
            if (retry_count) ++*retry_count;
            l.ex_flags ^= kExWait;
        } else {
            l.ex.wait_timer.untick(game_speed);
        }
    }
    if (l.no_cancel != 0) --l.no_cancel;
}

// 0x432f40
bool LaserManager::step_line(Laser& l) {
    LineLaserArgs& a = l.line;
    int retry;
    do {
        run_ex_line(l);
        if (l.ex_flags == 0) break;
        retry = 0;
        if ((l.ex_flags & kExAccel) != 0) retry += ex_accelerate(l);
        if ((l.ex_flags & kExStopTurn) != 0 && l.ex.turn_mode == 0) retry += ex_turn(l, a.step_sfx);
        if ((l.ex_flags & kExBounce) != 0) retry += ex_bounce_line(l);
        run_ex_wait_and_immunity(l, &retry);
    } while (retry != 0);

    if (a.target_length <= l.length) {
        // 已达目标长度：整段前进；越过 max_extent 后从尾部缩短
        l.start_drawn_len = l.speed * game_speed + l.start_drawn_len;
        const float vy = l.velocity.y * game_speed;
        const float vz = l.velocity.z * game_speed;
        l.pos.x = l.velocity.x * game_speed + l.pos.x;
        l.pos.y = l.pos.y + vy;
        l.pos.z = l.pos.z + vz;
        const float extent = a.max_extent;
        if (0.0f < extent && extent < l.length + l.start_drawn_len) {
            const float remaining = extent - l.start_drawn_len;
            l.length = remaining;
            a.target_length = remaining;
            if (remaining <= 0.0f) return true;
        }
    } else {
        const float grown = l.speed * game_speed + l.length;
        l.length = grown;
        if (a.target_length < grown) l.length = a.target_length;
    }

    bool skip_timers = false;
    if (l.spawn_grace_timer.current < 1) {
        if (l.invuln_timer.current < 1) {
            // 起点与末端都在场外（含宽度余量）时销毁
            const Vec3 tip = polar(l.angle, l.length);
            const float w = l.width;
            const bool origin_out = l.pos.x + w <= kFieldLeft || kFieldRight <= l.pos.x - w ||
                                    l.pos.y + w <= kFieldTop || kFieldBottom <= l.pos.y - w;
            const bool tip_out = w + l.pos.x + tip.x <= kFieldLeft || kFieldRight <= (l.pos.x + tip.x) - w ||
                                 l.pos.y + tip.y + w <= kFieldTop || kFieldBottom <= (l.pos.y + tip.y) - w;
            if (origin_out && tip_out) return true;
            skip_timers = true;
        }
    } else {
        l.spawn_grace_timer.untick(game_speed);
    }
    if (!skip_timers && 0 < l.invuln_timer.current) l.invuln_timer.untick(game_speed);

    collide_player_line(l, false);
    sync_beam_scale(l);
    animations->run(l.vm_beam);
    if (l.start_drawn_len == 0.0f) animations->run(l.vm_origin);
    animations->run(l.vm_tip);
    l.frame_timer.tick(game_speed);
    return false;
}

// Ex 0x2000：以激光末端为发射点，按本步 / 下一步参数再发射一组弹幕
void LaserManager::ex_reshoot(Laser& l, const BulletExStep* steps) {
    const BulletExStep& st = steps[l.ex_index];
    const BulletExStep& next = steps[l.ex_index + 1 < kExCount ? l.ex_index + 1 : kExCount - 1];
    EtShooter shooter;
    std::memset(&shooter, 0, sizeof shooter);
    shooter.step_sfx = -1;
    const Vec3 tip = polar(l.angle, l.length);
    const u32 packed = u32(st.a);
    shooter.pos = {tip.x + l.pos.x, l.pos.y + tip.y, 0.0f};
    shooter.aim_mode = i16((packed >> 24) & 0x7fu);
    shooter.type = i16((packed >> 16) & 0xffu);
    shooter.color = i16((packed >> 8) & 0xffu);
    shooter.speed1 = st.r;
    shooter.speed2 = st.s;
    shooter.start_ex = i32(packed & 0xffu);
    shooter.way = i16(st.b);
    shooter.stack = i16(next.a);
    shooter.angle1 = next.r;
    shooter.flags = u32(next.b);
    shooter.angle2 = next.s;
    std::memcpy(shooter.ex, steps, sizeof shooter.ex);
    ++l.ex_index;
    if (bullets) bullets->shoot(shooter);
    ++l.ex_index;
    if (st.a < 0) {
        cancel_single(l, false, false);
        ++l.ex_index;
    }
}

// 0x431fe0：直线激光的 Ex 解释（每帧从 ex_index 起连续执行，遇到需要等待的同步步骤停下）
void LaserManager::run_ex_line(Laser& l) {
    BulletExStep* steps = l.line.ex;
    while (l.ex_index < kExCount) {
        BulletExStep& st = steps[l.ex_index];
        if (ex_blocked(st, l.ex_flags)) break;
        switch (st.type) {
            case kExInitialBoost:
                l.ex_flags |= kExInitialBoost;
                l.ex.boost_timer.set(0);
                break;
            case kExAccel: {
                l.ex_flags |= kExAccel;
                l.ex.accel = st.r;
                float dir = st.s;
                if (-990.0f < st.s) {
                    if (990.0f <= st.s) dir = bullets ? bullets->angle_to_player(l.pos) : st.s;
                } else {
                    dir = l.angle;
                }
                l.ex.accel_angle = dir;
                l.ex.accel_timer.set(0);
                l.ex.accel_duration = st.a;
                l.ex.accel_vec = polar(l.ex.accel_angle, l.ex.accel);
                if (l.ex_index != 0 && l.line.step_sfx >= 0) push_sfx(l.line.step_sfx);
                break;
            }
            case kExAccelTurn:
                l.ex_flags |= kExAccelTurn;
                l.ex.ang_vel_accel = st.r;
                l.ex.ang_vel = st.s;
                l.ex.ang_vel_timer.set(0);
                l.ex.ang_vel_duration = st.a;
                if (l.ex_index != 0 && l.line.step_sfx >= 0) push_sfx(l.line.step_sfx);
                break;
            case kExStopTurn:
                l.ex_flags |= kExStopTurn;
                l.ex.turn_angle = st.r;
                l.ex.turn_speed = (st.s <= -999.0f) ? l.speed : st.s;
                l.ex.turn_timer.set(0);
                l.ex.turn_duration = st.a;
                l.ex.turn_max = st.b;
                l.ex.turn_count = 0;
                l.ex.turn_mode = st.c;
                break;
            case kExBounce:
                if (0 < st.a) {
                    l.ex_flags |= kExBounce;
                    l.ex.bounce_speed = (st.r < 0.0f) ? l.speed : st.r;
                    // 反弹次数就地递减：派生出的子激光复制本参数包时自然少弹一次
                    st.a = st.a - 1;
                    l.ex.bounce_max = st.a;
                    l.ex.bounce_count = 0;
                    l.ex.bounce_walls = u32(st.b);
                }
                break;
            case kExImmunity: l.no_cancel = st.a; break;
            case kExBoundsGuard: l.invuln_timer.set(st.a); break;
            case kExChangeType:
                animations->init_from_template(*bullet_anm, l.vm_beam, bullet_type_info(st.a).script + st.b);
                l.vm_beam.root = nullptr;
                l.vm_beam.parent = nullptr;
                animations->run(l.vm_beam);
                break;
            case kExCancel: l.state = LaserState::Telegraph; break;
            case kExSfx: push_sfx(st.a, l.pos.x); break;
            case kExWrap:
                l.ex_flags |= kExWrap;
                l.ex.wrap_timer.set(st.a);
                break;
            case kExReshoot: ex_reshoot(l, steps); continue;
            case kExParam: l.id = st.a; break;
            case kExLoop: l.ex_index = st.a; continue;
            case kExBlend:
                if (st.a == 0) l.vm_beam.flags_lo &= 0xfffffe1fu;
                else l.vm_beam.flags_lo = (l.vm_beam.flags_lo & 0xfffffe3fu) | 0x20u;
                break;
            case kExWait:
                l.ex_flags |= kExWait;
                l.ex.wait_timer.set(st.a);
                break;
            default: break;
        }
        ++l.ex_index;
    }
}

// 0x432dc0 / 0x4395b0：匀加速——速度与速度向量累加；角度随速度向量更新（不折回、不重算标量速度）
int LaserManager::ex_accelerate(Laser& l) {
    if (l.ex.accel_duration <= l.ex.accel_timer.current) { l.ex_flags &= ~kExAccel; return 1; }
    const float gs = game_speed;
    l.speed = l.ex.accel * gs + l.speed;
    const float ay = l.ex.accel_vec.y * gs;
    const float az = l.ex.accel_vec.z * gs;
    l.velocity.x = l.velocity.x + l.ex.accel_vec.x * gs;
    l.velocity.y = l.velocity.y + ay;
    l.velocity.z = l.velocity.z + az;
    if (0.0001f < std::fabs(l.velocity.x) || 0.0001f < std::fabs(l.velocity.y))
        l.angle = atan2_f(l.velocity.y, l.velocity.x);
    l.ex.accel_timer.tick(gs);
    return 0;
}

// 0x439460：角速度 + 切向加速（仅曲线激光）
int LaserManager::ex_angular_velocity(Laser& l) {
    if (l.ex.ang_vel_duration <= l.ex.ang_vel_timer.current) { l.ex_flags &= ~kExAccelTurn; return 1; }
    const float gs = game_speed;
    l.angle = wrap_angle(l.ex.ang_vel * gs + l.angle);
    l.speed = l.ex.ang_vel_accel * gs + l.speed;
    l.velocity = polar(l.angle, l.speed);
    l.ex.ang_vel_timer.tick(gs);
    return 0;
}

// 0x432c20 / 0x4392c0：模式 0 折返——duration 帧内线性减速到 0，然后转 turn_angle、以 turn_speed 重新出发
int LaserManager::ex_turn(Laser& l, i32 step_sfx) {
    LaserExState& ex = l.ex;
    float speed_now;
    if (ex.turn_timer.current < ex.turn_duration) {
        speed_now = l.speed - (ex.turn_timer.current_f * l.speed) / float(ex.turn_duration);
    } else {
        if (step_sfx >= 0) push_sfx(step_sfx);
        const float new_angle = ex.turn_angle + l.angle;
        speed_now = ex.turn_speed;
        ++ex.turn_count;
        l.speed = speed_now;
        l.angle = new_angle;
        ex.turn_timer.reset();
        if (ex.turn_max <= ex.turn_count) {
            l.velocity = polar(new_angle, speed_now);
            l.ex_flags &= ~kExStopTurn;
            return 1;
        }
    }
    l.velocity = polar(l.angle, speed_now);
    ex.turn_timer.tick(game_speed);
    return 0;
}

// 0x432620：直线激光末端触及场外时，在对应墙面镜像派生一条新直线激光（就地改写本激光的参数包后复用）
int LaserManager::ex_bounce_line(Laser& l) {
    const Vec3 dir = polar(l.angle, l.length);
    const Vec2 tip{l.pos.x + dir.x, l.pos.y + dir.y};
    if (kFieldLeft < tip.x && tip.x < kFieldRight && kFieldTop < tip.y && tip.y < kFieldBottom) return 0;
    const u32 walls = l.ex.bounce_walls;
    const bool spawn_children = (walls & 0x10u) == 0;
    bool bounced = false;
    auto reflect = [&](Vec2 wall_a, Vec2 wall_b, bool horizontal_wall) {
        if (spawn_children) {
            Vec2 hit;
            if (intersect_segments(hit, wall_a, wall_b, tip, {l.pos.x, l.pos.y})) {
                l.line.origin.x = hit.x;
                l.line.origin.y = hit.y;
            }
            l.line.origin.z = 0.0f;
            l.line.angle = horizontal_wall ? -l.angle : wrap_angle(-l.angle - kPi);
            l.line.speed = l.ex.bounce_speed;
            l.line.start_offset = 0.0f;
            spawn_line(l.line);
        }
        bounced = true;
    };
    if ((walls & 0x1u) != 0 && tip.y < kFieldTop) reflect({-256.0f, 0.0f}, {256.0f, 0.0f}, true);
    if ((walls & 0x2u) != 0 && kFieldBottom < tip.y) reflect({-256.0f, 448.0f}, {256.0f, 448.0f}, true);
    if ((walls & 0x4u) != 0 && tip.x < kFieldLeft) reflect({-192.0f, -192.0f}, {-192.0f, 640.0f}, false);
    if ((walls & 0x8u) != 0 && kFieldRight < tip.x) reflect({192.0f, -192.0f}, {192.0f, 640.0f}, false);
    if (!bounced) return 0;
    l.ex_flags &= ~kExBounce;
    if (l.line.step_sfx >= 0) push_sfx(l.line.step_sfx);
    return 1;
}

// 0x443af0：自机相对线段的局部坐标判定。返回 0 无接触 / 1 被弹 / 2 擦弹圈内
int LaserManager::test_player_segment(const Vec3& seg_origin, float seg_angle, float seg_length, float seg_width,
                                      bool force_graze) {
    if (player == nullptr) return 0;
    const float dx = player->pos.x - seg_origin.x;
    const float dy = player->pos.y - seg_origin.y;
    // 保持 dx * cos - dy * sin 的单精度乘减顺序以对齐原版
    const float sin_a = sin_f(-seg_angle);
    const float cos_a = cos_f(-seg_angle);
    const float perp = dy * cos_a + dx * sin_a;
    const float along = dx * cos_a - dy * sin_a;
    const float hr_x = player->hit_radius.x;
    const float hr_y = player->hit_radius.y;
    const float half_w = seg_width * 0.5f;
    if (along - hr_x * 16.0f > seg_length || perp - hr_y * 16.0f > half_w || 0.0f > along + hr_x * 16.0f ||
        seg_width * -0.5f > hr_y * 16.0f + perp)
        return 0;
    if (seg_length < along - hr_x || half_w < perp - hr_y || along + hr_x < 0.0f || hr_y + perp < seg_width * -0.5f)
        return 2;
    if (world != nullptr && world->dialogue) return 0;
    if (force_graze) return 2;
    const i32 st = player->state;
    if (st != Player::kRespawnWait && st != Player::kDying && st != Player::kState3 && player->invincible.current < 1) {
        player->hit(*animations, effect_anm);
        return 1;
    }
    return 0;
}

// 擦弹点取自机到激光轴的垂足（0x404220 交点）。原版的 z 来自未初始化栈槽（直线非全长 / 照射 / 曲线三处，
// 0x43ad00 极坐标只写 x/y），得点道具会继承这个堆指针位；道具逻辑不读 z，这里统一传 0，
// 仅直线全长判定（z = 起点 z）是确定值照搬。oracle 侧 normalize_laser_graze_z 同口径。
void LaserManager::graze_at_player_foot(const Vec3& seg_origin, float seg_angle, float graze_z) {
    const float perpendicular = wrap_angle(seg_angle + kHalfPi);
    Vec2 foot;
    intersect_lines(foot, seg_origin, seg_angle, player->pos, perpendicular);
    if (bullets) bullets->graze(Vec3{foot.x, foot.y, graze_z});
}

// 0x433510：直线激光默认跳过前 1/10 长度、只取 4/5 长参与判定（flags bit1 = 全长）
void LaserManager::collide_player_line(Laser& l, bool force_graze) {
    if (!(16.0f < l.length && 3.0f < l.width)) return;
    const bool full_length = (l.line.flags & 0x2u) != 0;
    Vec3 origin = l.pos;
    if (!full_length) {
        const Vec3 skip = polar(l.angle, l.length / 10.0f);
        origin = {skip.x + l.pos.x, l.pos.y + skip.y, l.pos.z + skip.z};
    }
    const float seg_length = full_length ? l.length : (l.length * 4.0f) / 5.0f;
    const float hit_width = (32.0f > l.width) ? l.width * 0.5f : l.width - (l.width + 16.0f) * 0.5f;
    const int r = test_player_segment(origin, l.angle, seg_length, hit_width, force_graze);
    if (r == 1) {
        cancel_line_rect(l, player->pos, Vec3{32.0f, 32.0f, 0.0f}, 0.0f, 0, true);
        return;
    }
    if (r == 2) {
        if (l.graze_timer.current % 3 == 0) graze_at_player_foot(origin, l.angle, full_length ? l.pos.z : 0.0f);
        l.graze_timer.tick(game_speed);
    }
}

// =============================================================================================
// 照射激光

// 0x435050
void LaserManager::init_infinite(Laser& l) {
    InfiniteLaserArgs& a = l.infinite;
    l.bullet_type = a.type;
    l.state = LaserState::Telegraph;
    l.bullet_color = a.color;
    bind_beam_vm(l, bullet_type_info(l.bullet_type).script, false);
    bind_glow_vm(l.vm_origin, a.color + 0x38, true);
    if (a.spawn_sfx >= 0) push_sfx(a.spawn_sfx);
    l.pos = a.origin;
    if (a.start_offset != 0.0f) {
        const Vec3 shift = polar(a.angle, a.start_offset);
        l.pos.x = l.pos.x + shift.x;
        l.pos.y = l.pos.y + shift.y;
    }
    l.length = a.initial_length;
    l.speed = a.speed;
    l.angle = a.angle;
    l.ex_index = a.start_ex;
    l.width = 2.0f;
    l.id = a.handle;
    l.graze_timer.reset();
    l.curve_full_width = 0.0f;
}

// 0x436fd0：照射激光只认免疫 / 终止 / 混合三类 Ex，其余步骤直接跳过
void LaserManager::run_ex_infinite(Laser& l) {
    BulletExStep* steps = l.infinite.ex;
    while (l.ex_index < kExCount) {
        const BulletExStep& st = steps[l.ex_index];
        if (ex_blocked(st, l.ex_flags)) break;
        switch (st.type) {
            case kExImmunity: l.no_cancel = st.a; break;
            case kExCancel: l.state = LaserState::Telegraph; break;
            case kExBlend:
                if (st.a == 0) l.vm_beam.flags_lo &= 0xfffffe1fu;
                else l.vm_beam.flags_lo = (l.vm_beam.flags_lo & 0xfffffe3fu) | 0x20u;
                break;
            default: break;
        }
        ++l.ex_index;
    }
}

// 0x4352f0：预警 → 展开 → 维持 → 收束；期间沿角速度旋转、跟随 Boss 0 或按 velocity 平移
bool LaserManager::step_infinite(Laser& l) {
    InfiniteLaserArgs& a = l.infinite;
    const float gs = game_speed;
    run_ex_infinite(l);
    if (l.ex_flags != 0) run_ex_wait_and_immunity(l, nullptr);

    if (l.length < a.target_length) {
        const float grown = l.speed * gs + l.length;
        l.length = grown;
        if (a.target_length < grown) l.length = a.target_length;
    }
    l.angle = wrap_angle(a.angular_vel * gs + l.angle);
    // 0x4353d1：跟随位置走 0x417580(0)，槽号过期时落在链表最后一只上
    if ((a.flags & 0x1u) != 0 && enemies != nullptr) {
        if (Enemy* boss = enemies->boss_slot(0)) l.pos = boss->pos;
    }
    const float vy = a.velocity.y * gs;
    const float vz = a.velocity.z * gs;
    l.pos.x = a.velocity.x * gs + l.pos.x;
    l.pos.y = l.pos.y + vy;
    l.pos.z = l.pos.z + vz;

    const Timer& age = l.age_timer;
    switch (l.state) {
        case LaserState::Telegraph:
            if (a.telegraph_time <= age.current) { l.age_timer.set(0); l.state = LaserState::Expand; }
            break;
        case LaserState::Expand:
            if (age.current < a.expand_time) {
                l.width = (a.width * age.current_f) / float(a.expand_time);
                break;
            }
            l.age_timer.set(0);
            l.state = LaserState::Active;
            l.width = a.width;
            [[fallthrough]];
        case LaserState::Active:
            if (a.active_time <= age.current) {
                l.age_timer.set(0);
                l.state = LaserState::Shrink;
                if (a.shrink_time <= age.current) return true;
                l.width = a.width - (age.current_f * a.width) / float(a.shrink_time);
            }
            break;
        case LaserState::Shrink:
            if (a.shrink_time <= age.current) return true;
            l.width = a.width - (age.current_f * a.width) / float(a.shrink_time);
            break;
        default: break;
    }
    collide_player_infinite(l, false);
    sync_beam_scale(l);
    animations->run(l.vm_beam);
    if (l.start_drawn_len == 0.0f) animations->run(l.vm_origin);
    return false;
}

// 0x435610：仅展开 / 维持期判定，取 9/10 长
void LaserManager::collide_player_infinite(Laser& l, bool force_graze) {
    if (!(l.state == LaserState::Expand || l.state == LaserState::Active) || !(16.0f < l.length)) return;
    const float hit_width = (32.0f > l.width) ? l.width * 0.5f : l.width - (l.width + 16.0f) / 3.0f;
    const int r = test_player_segment(l.pos, l.angle, l.length * 0.9f, hit_width, force_graze);
    if (r == 1) {
        cancel_infinite_rect(l, player->pos, Vec3{32.0f, 32.0f, 0.0f}, 0.0f, 0, true);
        return;
    }
    if (r == 2) {
        if (l.graze_timer.current % 3 == 0) graze_at_player_foot(l.pos, l.angle, 0.0f);
        l.graze_timer.tick(game_speed);
    }
}

// =============================================================================================
// 曲线激光

// 0x431190：在链尾追加一段，前一段在 time 结束
CurveSegment& LaserManager::append_curve_segment(Laser& l, float time) {
    if (!l.curve_segments.empty()) l.curve_segments.back().end_time = time;
    l.curve_segments.emplace_back();
    l.curve_segments.back().start_time = time;
    return l.curve_segments.back();
}

const CurveSegment* LaserManager::find_curve_segment(const Laser& l, float time) const {
    for (const CurveSegment& seg : l.curve_segments)
        if (seg.start_time <= time && time < seg.end_time) return &seg;
    return nullptr;
}

// 0x437ee0：头节点在段内时刻 time 的位置 / 速度 / 朝向
void LaserManager::eval_curve_head(const CurveSegment& seg, float time, Vec3& pos, float& speed, float& angle) {
    const float dt = time - seg.start_time;
    switch (seg.mode) {
        case CurveSegment::kUniform:
            pos.x = seg.start_pos.x + seg.direction.x * dt * seg.start_speed;
            pos.y = seg.start_pos.y + seg.direction.y * dt * seg.start_speed;
            pos.z = seg.start_pos.z + seg.direction.z * dt * seg.start_speed;
            speed = seg.start_speed;
            angle = seg.start_angle;
            return;
        case CurveSegment::kAccelerate: {
            if (seg.accel_angle < -990.0f) {
                const float v2 = seg.start_speed + seg.start_speed + seg.accel * dt;
                const float k = dt + 1.0f;
                pos.x = seg.start_pos.x + seg.direction.x * v2 * k * 0.5f;
                pos.y = seg.start_pos.y + seg.direction.y * v2 * k * 0.5f;
                pos.z = seg.start_pos.z + seg.direction.z * v2 * k * 0.5f;
                speed = seg.accel * dt + seg.start_speed;
                angle = seg.start_angle;
                return;
            }
            const Vec3 base = polar(seg.start_angle, seg.start_speed);
            const Vec3 push = polar(seg.accel_angle, seg.accel);
            const float vy = push.y + base.y;
            const float vx = push.x + base.x;
            pos.x = vx * dt + seg.start_pos.x;
            pos.y = seg.start_pos.y + vy * dt;
            pos.z = seg.start_pos.z + (0.0f + 0.0f) * dt;
            speed = float(std::sqrt(double(vx * vx + vy * vy)));
            angle = atan2_f(vy, vx);
            return;
        }
        case CurveSegment::kAngularStep: {
            Vec3 p = seg.start_pos;
            float a = seg.start_angle;
            float s = seg.start_speed;
            for (int n = i32(dt); n > 0; --n) {
                const Vec3 step = polar(a, s);
                a = wrap_angle(a + seg.accel_angle);
                p.x = step.x + p.x;
                s = s + seg.accel;
                p.y = p.y + step.y;
                p.z = p.z + 0.0f;
            }
            const Vec3 step = polar(a, s);
            const float frac = dt - floor_f(dt);
            pos.x = frac * step.x + p.x;
            pos.y = p.y + step.y * frac;
            pos.z = p.z + frac * 0.0f;
            speed = s;
            angle = a;
            return;
        }
        default: return;
    }
}

// 0x438370：后续节点由前一节点倒推一帧
void LaserManager::eval_curve_follow(const CurveSegment& seg, float time, const CurveNode& prev, Vec3& pos,
                                     float& speed, float& angle) {
    switch (seg.mode) {
        case CurveSegment::kUniform:
            pos.x = prev.pos.x - seg.direction.x * seg.start_speed;
            pos.y = prev.pos.y - seg.direction.y * seg.start_speed;
            pos.z = prev.pos.z - seg.direction.z * seg.start_speed;
            speed = seg.start_speed;
            angle = seg.start_angle;
            return;
        case CurveSegment::kAccelerate: {
            if (seg.accel_angle < -990.0f) {
                const float v = prev.speed - seg.accel;
                pos.x = prev.pos.x - v * seg.direction.x;
                pos.y = prev.pos.y - seg.direction.y * v;
                pos.z = prev.pos.z - seg.direction.z * v;
                speed = seg.start_speed - seg.accel;
                angle = prev.angle;
                return;
            }
            const Vec3 back = polar(prev.angle, -prev.speed);
            const Vec3 push = polar(seg.accel_angle, -seg.accel);
            const float vx = push.x + back.x;
            const float vy = push.y + back.y;
            pos.x = prev.pos.x + vx;
            pos.y = prev.pos.y + vy;
            pos.z = prev.pos.z + 0.0f + 0.0f;
            speed = float(std::sqrt(double(vx * vx + vy * vy)));
            angle = atan2_f(vy, vx);
            return;
        }
        case CurveSegment::kAngularStep: {
            const Vec3 step = polar(prev.angle, prev.speed);
            const float whole = floor_f(time);
            const float frac = time - whole;
            pos.x = prev.pos.x - step.x * frac;
            pos.y = prev.pos.y - step.y * frac;
            pos.z = prev.pos.z - frac * 0.0f;
            speed = prev.speed - seg.accel;
            angle = wrap_angle(prev.angle - seg.accel_angle);
            const Vec3 step2 = polar(angle, speed);
            const float k = (1.0f - time) + whole;
            pos.x = pos.x - k * step2.x;
            pos.y = pos.y - step2.y * k;
            pos.z = pos.z - k * 0.0f;
            return;
        }
        default: return;
    }
}

// 0x4370a0 / 0x4377d0 共用：按 frame_timer 重算全部采样点（第 i 点对应时刻 t - i；首个落在段内的点用头节点公式）
void LaserManager::recompute_curve_nodes(Laser& l) {
    const CurveLaserArgs& a = l.curve;
    bool head_done = false;
    for (i32 i = 0; i < a.node_count && i < i32(l.curve_nodes.size()); ++i) {
        CurveNode& node = l.curve_nodes[u32(i)];
        const float t = l.frame_timer.current_f - float(i);
        if (t < 0.0f) {
            node.pos = a.origin;
            node.velocity = Vec3{};
            node.angle = a.angle;
            node.speed = a.speed;
            continue;
        }
        const CurveSegment* seg = find_curve_segment(l, t);
        if (seg == nullptr) { head_done = true; continue; }
        if (!head_done) {
            eval_curve_head(*seg, t, node.pos, node.speed, node.angle);
            head_done = true;
        } else {
            eval_curve_follow(*seg, t, l.curve_nodes[u32(i - 1)], node.pos, node.speed, node.angle);
        }
    }
}

// 0x4370a0
void LaserManager::init_curve(Laser& l) {
    CurveLaserArgs& a = l.curve;
    l.bullet_type = a.type;
    l.state = LaserState::Active;
    l.bullet_color = a.color;
    bind_beam_vm(l, l.bullet_type == 1 ? 0x142 : l.bullet_type + 0x8e, true);
    bind_glow_vm(l.vm_origin, a.color + 0x38, true);

    l.curve_nodes.assign(u32(a.node_count > 0 ? a.node_count : 0), CurveNode{});
    l.pos = a.origin;
    if (a.start_offset != 0.0f) {
        const Vec3 shift = polar(a.angle, a.start_offset);
        a.start_offset = 0.0f;
        l.pos.x = l.pos.x + shift.x;
        l.pos.y = l.pos.y + shift.y;
        a.origin = l.pos;
    }
    l.width = a.width;
    l.angle = a.angle;
    l.speed = a.speed;
    l.velocity = polar(a.angle, a.speed);
    for (CurveNode& node : l.curve_nodes) {
        node.pos = l.pos;
        node.velocity = Vec3{};
        node.angle = a.angle;
        node.speed = a.speed;
    }
    // 采样时刻从 initial_time 起算（派生片段继承母激光的时刻）
    l.frame_timer.set(0);
    l.frame_timer.current_f = a.initial_time;
    l.frame_timer.current = i32(a.initial_time);
    l.frame_timer.previous = l.frame_timer.current - 1;

    if (a.inherited_segments == nullptr) {
        l.curve_segments.clear();
        CurveSegment& head = l.curve_segments.emplace_back();
        head.start_speed = l.speed;
        head.start_angle = wrap_angle(l.angle);
        head.direction = polar(l.angle, 1.0f);
        head.start_pos = l.pos;
        head.mode = CurveSegment::kUniform;
        head.start_time = 0.0f;
        head.end_time = kFarTime;
        l.ex_index = a.start_ex;
    } else {
        l.curve_segments = *a.inherited_segments;
        a.inherited_segments = nullptr;
        l.ex_index = 99;
    }
    if (!l.curve_nodes.empty()) l.curve_nodes[0].velocity = l.velocity;
    recompute_curve_nodes(l);

    l.spawn_grace_timer.set(30);
    if (a.spawn_sfx >= 0) push_sfx(a.spawn_sfx);
    l.graze_timer.reset();
}

// 0x438cb0：曲线激光的 Ex 解释——Ex 4 / 8 不直接改速度，而是追加一段新的运动律
void LaserManager::run_ex_curve(Laser& l) {
    BulletExStep* steps = l.curve.ex;
    while (l.ex_index < kExCount) {
        BulletExStep& st = steps[l.ex_index];
        if (ex_blocked(st, l.ex_flags) || (st.type & l.ex_flags) != 0) break;
        switch (st.type) {
            case kExInitialBoost:
                l.ex_flags |= kExInitialBoost;
                l.ex.boost_timer.set(0);
                break;
            case kExAccel:
            case kExAccelTurn: {
                auto start_segment = [&](CurveSegment& seg) {
                    const CurveSegment& prev = l.curve_segments[l.curve_segments.size() - 2];
                    eval_curve_head(prev, prev.end_time, seg.start_pos, seg.start_speed, seg.start_angle);
                    seg.start_pos.z = 0.0f;
                    seg.direction = polar(seg.start_angle, 1.0f);
                    seg.direction.z = 0.0f;
                };
                CurveSegment& seg = append_curve_segment(l, float(st.b));
                seg.mode = (st.type == kExAccel) ? CurveSegment::kAccelerate : CurveSegment::kAngularStep;
                start_segment(seg);
                seg.accel = st.r;
                seg.accel_angle = st.s;
                if (st.a < 0) {
                    seg.end_time = kFarTime;
                } else {
                    const float end = float(st.b) + float(st.a);
                    seg.end_time = end;
                    CurveSegment& tail = append_curve_segment(l, end);
                    tail.mode = CurveSegment::kUniform;
                    start_segment(tail);
                    tail.end_time = kFarTime;
                }
                break;
            }
            case kExStopTurn:
                l.ex_flags |= kExStopTurn;
                l.ex.turn_angle = st.r;
                l.ex.turn_speed = (st.s <= -999.0f) ? l.speed : st.s;
                l.ex.turn_timer.set(0);
                l.ex.turn_duration = st.a;
                l.ex.turn_max = st.b;
                l.ex.turn_count = 0;
                l.ex.turn_mode = st.c;
                break;
            case kExBounce:
                if (0 < st.a) {
                    l.ex_flags |= kExBounce;
                    l.ex.bounce_speed = (st.r < 0.0f) ? l.speed : st.r;
                    st.a = st.a - 1;
                    l.ex.bounce_max = st.a;
                    l.ex.bounce_count = 0;
                    l.ex.bounce_walls = u32(st.b);
                }
                break;
            case kExImmunity: l.no_cancel = st.a; break;
            case kExBoundsGuard:
                l.ex_flags |= kExBoundsGuard;
                l.ex.curve_hold_timer.set(st.a);
                l.ex.curve_hold_param = st.b;
                break;
            case kExChangeType:
                animations->init_from_template(*bullet_anm, l.vm_beam, bullet_type_info(st.a).script + st.b);
                l.vm_beam.root = nullptr;
                l.vm_beam.parent = nullptr;
                animations->run(l.vm_beam);
                break;
            case kExCancel: l.state = LaserState::Telegraph; break;
            case kExSfx: push_sfx(st.a, l.pos.x); break;
            case kExWrap:
                l.ex_flags |= kExWrap;
                l.ex.wrap_timer.set(st.a);
                break;
            case kExReshoot: ex_reshoot(l, steps); continue;
            case kExParam: l.id = st.a; break;
            case kExLoop: l.ex_index = st.a; continue;
            case kExBlend:
                if (st.a == 0) l.vm_beam.flags_lo &= 0xfffffe1fu;
                else l.vm_beam.flags_lo = (l.vm_beam.flags_lo & 0xfffffe3fu) | 0x20u;
                break;
            case 0x10000000u:  // 冻结 / 解冻采样点重算
                l.lifecycle = (l.lifecycle & ~kLaserCurveFrozen) | ((u32(st.a) << 4) & kLaserCurveFrozen);
                break;
            case kExWait:
                if (st.a >= 1) {
                    l.ex_flags |= kExWait;
                    l.ex.wait_timer.set(st.a);
                }
                break;
            default: break;
        }
        ++l.ex_index;
    }
}

// 0x439730：Ex 0x100 倒数期间屏蔽出界判定
int LaserManager::ex_curve_hold(Laser& l) {
    l.ex.curve_hold_timer.untick(game_speed);
    if (l.ex.curve_hold_timer.current < 1) { l.ex_flags ^= kExBoundsGuard; return 1; }
    return 0;
}

bool LaserManager::curve_in_bounds(const Laser& l) const {
    const float w = l.width;
    for (i32 i = 0; i < l.curve.node_count && i < i32(l.curve_nodes.size()); ++i) {
        const Vec3& p = l.curve_nodes[u32(i)].pos;
        if (kFieldLeft < p.x + w && p.x - w < kFieldRight && kFieldTop < p.y + w && p.y - w < kFieldBottom) return true;
    }
    return false;
}

// 0x4377d0
bool LaserManager::step_curve(Laser& l) {
    int retry;
    do {
        run_ex_curve(l);
        if (l.ex_flags == 0) break;
        retry = 0;
        if ((l.ex_flags & kExAccel) != 0) retry += ex_accelerate(l);
        if ((l.ex_flags & kExAccelTurn) != 0) retry += ex_angular_velocity(l);
        if ((l.ex_flags & kExStopTurn) != 0 && l.ex.turn_mode == 0) retry += ex_turn(l, l.curve.step_sfx);
        if ((l.ex_flags & kExBoundsGuard) != 0) retry += ex_curve_hold(l);
        run_ex_wait_and_immunity(l, &retry);
    } while (retry != 0);

    if ((l.lifecycle & kLaserCurveFrozen) == 0) recompute_curve_nodes(l);

    if (l.spawn_grace_timer.current < 1 && (l.ex_flags & kExBoundsGuard) == 0) {
        if (!curve_in_bounds(l)) return true;
    } else {
        l.spawn_grace_timer.untick(game_speed);
    }
    collide_player_curve(l, false);
    animations->run(l.vm_beam);
    animations->run(l.vm_origin);
    l.frame_timer.tick(game_speed);
    return false;
}

// 0x437cf0：逐节点线段判定（累计 16 像素后才开始），擦弹点取首个命中的节点
void LaserManager::collide_player_curve(Laser& l, bool force_graze) {
    const i32 n = l.curve.node_count;
    if (n >= 2 && i32(l.curve_nodes.size()) >= n) {
        float accumulated = 0.0f;
        bool graze_recorded = false;
        Vec3 graze_pos;
        for (i32 i = 0; i < n - 1; ++i) {
            const CurveNode& node = l.curve_nodes[u32(i)];
            const Vec3 half = polar(node.angle, node.speed * 0.5f);
            const Vec3 seg_origin{node.pos.x + half.x, node.pos.y + half.y, half.z + node.pos.z};
            accumulated = accumulated + node.speed;
            if (accumulated < 16.0f) continue;
            const int r = test_player_segment(seg_origin, node.angle, node.speed, l.width * 0.5f, force_graze);
            if (r == 1) {
                cancel_curve_rect(l, player->pos, Vec3{32.0f, 32.0f, 0.0f}, 0.0f, 0, true);
            } else if (r == 2 && !graze_recorded && l.graze_timer.current % 3 == 0) {
                graze_pos = Vec3{seg_origin.x, seg_origin.y, 0.0f};  // z 口径见 graze_at_player_foot
                graze_recorded = true;
            }
        }
        if (graze_recorded && bullets) bullets->graze(graze_pos);
    }
    l.graze_timer.tick(game_speed);
}

// =============================================================================================
// 占位激光

// 0x43a860：只复位 VM 并记录长度 / 朝向 / 句柄（0x43ac00 更新恒为存活，0x43ac10 不绘制）
void LaserManager::init_beam(Laser& l) {
    BeamLaserArgs& a = l.beam;
    l.pos = a.origin;
    l.length = a.length;
    l.angle = a.angle;
    l.bullet_color = a.color;
    l.state = LaserState::Telegraph;
    l.id = a.handle;
    l.width = 1.0f;
    l.curve_full_width = 0.0f;
    AnmManager::reset_vm(l.vm_beam);
    l.vm_beam.flags_hi = (l.vm_beam.flags_hi & 0xffff7fffu) | 0x4000u;
    l.vm_beam.flags_lo = (l.vm_beam.flags_lo & 0xfffffe3fu) | 0x20u;
}

// =============================================================================================
// 消弹

// 消弹特效：直线 / 照射激光按弹种分三档脚本；炸弹（矩形）变体把特效登记到 EffectManager
void LaserManager::spawn_cancel_effect(i32 bullet_type, i32 bullet_color, const Vec3& pos, bool register_in_effects,
                                       bool rect_thresholds) {
    i32 script = 0;
    if (!cancel_effect_script(bullet_type, bullet_color, rect_thresholds, script)) return;
    const u32 id = animations->spawn_effect(*bullet_anm, script, -1, pos, 0.0f);
    if (register_in_effects && effects != nullptr) effects->register_id(id);
}

i32 LaserManager::cancel_single(Laser& l, i32 item_mode, bool respect_immunity) {
    switch (l.kind) {
        case LaserKind::Line: return cancel_line_single(l, item_mode, respect_immunity);
        case LaserKind::Infinite: return cancel_infinite_single(l, item_mode, respect_immunity);
        case LaserKind::Curve: return cancel_curve_single(l, item_mode, respect_immunity);
        case LaserKind::Beam:  // 0x43ac40
            if (!respect_immunity) mark_for_destroy(l);
            return 0;
    }
    return 0;
}

i32 LaserManager::cancel_circle(Laser& l, const Vec3& center, float radius, i32 item_mode, bool respect_immunity) {
    switch (l.kind) {
        case LaserKind::Line: return cancel_line_circle(l, center, radius, item_mode, respect_immunity);
        case LaserKind::Infinite: return cancel_infinite_circle(l, center, radius, item_mode, respect_immunity);
        case LaserKind::Curve: return cancel_curve_circle(l, center, radius, item_mode, respect_immunity);
        case LaserKind::Beam: return 0;
    }
    return 0;
}

i32 LaserManager::cancel_rect_single(Laser& l, const Vec3& center, const Vec3& size, float rect_angle, i32 item_mode,
                                     bool respect_immunity) {
    switch (l.kind) {
        case LaserKind::Line: return cancel_line_rect(l, center, size, rect_angle, item_mode, respect_immunity);
        case LaserKind::Infinite: return cancel_infinite_rect(l, center, size, rect_angle, item_mode, respect_immunity);
        case LaserKind::Curve: return cancel_curve_rect(l, center, size, rect_angle, item_mode, respect_immunity);
        case LaserKind::Beam: return 0;
    }
    return 0;
}

// 0x431af0
void LaserManager::cancel_all(i32 item_mode) {
    visit_existing(lasers, [&](Laser& l) {
        if (l.state != LaserState::Dead) cancel_single(l, item_mode, false);
        return false;
    });
}

// 0x4319e0
void LaserManager::cancel_all_ignoring_immunity() {
    visit_existing(lasers, [&](Laser& l) {
        l.spawn_grace_timer.reset();
        l.no_cancel = 0;
        cancel_single(l, true, false);
        return false;
    });
}

// ECL 710
void LaserManager::cancel_handle(i32 id) {
    while (Laser* l = find_by_id(id)) {
        cancel_single(*l, false, false);
        l->id = 0;
    }
}

// 0x431a70
i32 LaserManager::cancel_radius(const Vec3& center, float radius, i32 item_mode, bool respect_immunity) {
    last_cancel_center = center;
    i32 total = 0;
    visit_existing(lasers, [&](Laser& l) {
        if (l.state != LaserState::Dead) total += cancel_circle(l, center, radius, item_mode, respect_immunity);
        return false;
    });
    return total;
}

// 0x431950
i32 LaserManager::cancel_rect(const Vec3& center, const Vec3& size, float rect_angle) {
    last_cancel_center = center;
    last_cancel_size = size;
    i32 total = 0;
    visit_existing(lasers, [&](Laser& l) {
        if (l.state != LaserState::Dead && (l.lifecycle & kLaserRanOnce) != 0)
            total += cancel_rect_single(l, center, size, rect_angle, 5, true);
        return false;
    });
    return total;
}

// ---------------------------------------------------------------------------------------------
// 直线激光消弹：沿激光每 16 像素取一个采样点（首点距起点 8 像素）

// 0x434cd0：整条消弹——每个采样点出特效并转化道具
i32 LaserManager::cancel_line_single(Laser& l, i32 item_mode, bool respect_immunity) {
    if (respect_immunity && l.no_cancel != 0) return 0;
    const Vec3 step8 = polar(l.angle, 8.0f);
    Vec3 sample{step8.x + l.pos.x, l.pos.y + step8.y, l.pos.z + 0.0f};
    const Vec3 step16{step8.x + step8.x, step8.y + step8.y, 0.0f};
    i32 count = 0;
    if (l.length <= 16.0f) { l.state = LaserState::Dead; return count; }
    float cursor = 8.0f;
    for (;;) {
        ++count;
        spawn_cancel_effect(l.bullet_type, l.line.color, sample, false, false);
        if (bullets) bullets->convert_cancel_item(sample, item_mode);
        cursor = cursor + 16.0f;
        sample.x = sample.x + step16.x;
        sample.y = step16.y + sample.y;
        if (l.length <= cursor + 8.0f) { l.state = LaserState::Dead; return count; }
    }
}

// 0x434730 / 0x433860 共用后半：按命中表切分——去掉前导命中段、首个命中前的部分留给本激光，
// 其后每段连续未命中（> 24 像素）派生为新的直线激光
void LaserManager::split_line_after_hits(Laser& l, const bool* hit, int sample_count, const Vec3& step16) {
    int hits = 0;
    for (int i = 0; i < sample_count; ++i) hits += hit[i] ? 1 : 0;
    if (hits == 0) return;
    if (sample_count <= hits) { mark_for_destroy(l); return; }
    const Vec3 original_pos = l.pos;
    int first = 0;
    while (first < sample_count && hit[first]) ++first;
    if (first != 0) {
        const float shift = float(first);
        l.pos.x = l.pos.x + shift * step16.x;
        l.pos.y = l.pos.y + step16.y * shift;
        l.pos.z = l.pos.z + shift * step16.z;
        const float shortened = l.length - shift * kSampleSpacing;
        l.length = shortened;
        if (shortened <= kMinFragmentLength) { mark_for_destroy(l); return; }
        l.line.target_length = shortened;
        l.start_drawn_len = shift * kSampleSpacing;
    }
    int kept = 0;
    for (int i = first; i < sample_count; ++i, ++kept) {
        if (!hit[i]) continue;
        const float old_length = l.length;
        const float new_length = float(kept) * kSampleSpacing;
        l.length = new_length;
        l.line.target_length = l.line.target_length - (old_length - new_length);
        if (new_length < kMinFragmentLength) mark_for_destroy(l);
        int cursor = i;
        while (cursor < sample_count) {
            if (hit[cursor]) { ++cursor; continue; }
            int run = 0;
            int end = cursor;
            while (end < sample_count && !hit[end]) { ++end; ++run; }
            LineLaserArgs child = l.line;
            child.target_length = float(run) * kSampleSpacing;
            child.initial_length = child.target_length;
            if (kMinFragmentLength < child.target_length) {
                const float offset = float(cursor);
                child.origin.x = offset * step16.x + original_pos.x;
                child.origin.y = step16.y * offset + original_pos.y;
                child.origin.z = offset * step16.z + original_pos.z;
                spawn_line(child);
            }
            cursor = end;
        }
        break;
    }
}

// 0x434730
i32 LaserManager::cancel_line_circle(Laser& l, const Vec3& center, float radius, i32 item_mode, bool respect_immunity) {
    if (respect_immunity && l.no_cancel != 0) return 0;
    bool hit[256] = {};
    const Vec3 step8 = polar(l.angle, 8.0f);
    Vec3 sample{l.pos.x + step8.x, l.pos.y + step8.y, 0.0f};
    const Vec3 step16{step8.x + step8.x, step8.y + step8.y, 0.0f};
    int n = 0;
    if (16.0f <= l.length) {
        float cursor = 8.0f;
        do {
            const float dx = center.x - sample.x;
            const float dy = center.y - sample.y;
            if (dx * dx + dy * dy <= radius * radius && n < 256) {
                hit[n] = true;
                if (bullets) bullets->convert_cancel_item(sample, item_mode);
                spawn_cancel_effect(l.bullet_type, l.line.color, sample, false, false);
            }
            ++n;
            sample.x = sample.x + step16.x;
            sample.y = sample.y + step16.y;
            cursor = cursor + 16.0f;
        } while (cursor + 8.0f <= l.length);
        split_line_after_hits(l, hit, n < 256 ? n : 256, step16);
    }
    return 0;
}

// 0x433860：矩形（可旋转）消弹——在矩形局部坐标系里沿激光采样
i32 LaserManager::cancel_line_rect(Laser& l, const Vec3& center, const Vec3& size, float rect_angle, i32 item_mode,
                                   bool respect_immunity) {
    if (respect_immunity && l.no_cancel != 0) return 0;
    bool hit[256] = {};
    const float dx = l.pos.x - center.x;
    const float dy = l.pos.y - center.y;
    const float sin_a = sin_f(-rect_angle);
    const float cos_a = cos_f(-rect_angle);
    float local_x = dx * cos_a - dy * sin_a;
    float local_y = dy * cos_a + dx * sin_a;
    const Vec3 local_step8 = polar(wrap_angle(l.angle - rect_angle), 8.0f);
    local_y = local_y + local_step8.y;
    local_x = local_step8.x + local_x;
    const Vec2 local_step16{local_step8.x + local_step8.x, local_step8.y + local_step8.y};
    const float half_w = size.x * 0.5f;
    const float half_h = size.y * 0.5f;
    const Vec3 step8 = polar(l.angle, 8.0f);
    Vec3 sample{l.pos.x + step8.x, l.pos.y + step8.y, 0.0f};
    const Vec3 step16{step8.x + step8.x, step8.y + step8.y, 0.0f};
    int n = 0;
    if (16.0f <= l.length) {
        float cursor = 8.0f;
        do {
            if (-half_w <= local_x && local_x <= half_w && -half_h <= local_y && local_y <= half_h && n < 256) {
                hit[n] = true;
                if (bullets) bullets->convert_cancel_item(sample, item_mode);
                spawn_cancel_effect(l.bullet_type, l.line.color, sample, true, true);
            }
            ++n;
            sample.x = sample.x + step16.x;
            sample.y = sample.y + step16.y;
            local_x = local_x + local_step16.x;
            local_y = local_y + local_step16.y;
            cursor = cursor + 16.0f;
        } while (cursor + 8.0f <= l.length);
        split_line_after_hits(l, hit, n < 256 ? n : 256, step16);
    }
    return 0;
}

// ---------------------------------------------------------------------------------------------
// 照射激光消弹

// 0x436c70：整条消弹——场内（±16）的采样点出特效并转化道具
i32 LaserManager::cancel_infinite_single(Laser& l, i32 item_mode, bool respect_immunity) {
    if (respect_immunity && l.no_cancel != 0) return 0;
    const Vec3 step8 = polar(l.angle, 8.0f);
    const Vec3 step16{step8.x + step8.x, step8.y + step8.y, 0.0f};
    Vec3 sample{step8.x + l.pos.x, l.pos.y + step8.y, l.pos.z + 0.0f};
    i32 count = 0;
    if (16.0f < l.length) {
        float cursor = 8.0f;
        do {
            ++count;
            if (point_near_field(sample.x, sample.y, 16.0f)) {
                spawn_cancel_effect(l.bullet_type, l.infinite.color, sample, false, false);
                if (bullets) bullets->convert_cancel_item(sample, item_mode);
            }
            sample.x = sample.x + step16.x;
            cursor = cursor + 16.0f;
            sample.y = sample.y + step16.y;
        } while (cursor + 8.0f < l.length);
    }
    l.state = LaserState::Dead;
    return count;
}

// 0x436670 / 0x435880 共用后半：命中段之后的每段连续未命中派生为直线激光（本激光长度缩到首个命中处）
void LaserManager::spawn_infinite_fragments(Laser& l, const bool* hit, int sample_count, const Vec3& step16,
                                            bool rect_variant) {
    int hits = 0;
    for (int i = 0; i < sample_count; ++i) hits += hit[i] ? 1 : 0;
    if (hits == 0) return;
    const Vec3 original_pos = l.pos;
    int first = 0;
    while (first < sample_count && hit[first]) ++first;
    int cursor;
    if (first != 0) {
        l.length = 0.0f;
        cursor = first;
    } else {
        int k = 0;
        while (k < sample_count && !hit[k]) ++k;
        if (k >= sample_count) return;
        l.length = float(k) * kSampleSpacing;
        cursor = k;
    }
    while (cursor < sample_count) {
        if (hit[cursor]) { ++cursor; continue; }
        int run = 0;
        int end = cursor;
        while (end < sample_count && !hit[end]) { ++end; ++run; }
        const float offset = float(cursor);
        const Vec3 origin{offset * step16.x + original_pos.x, step16.y * offset + original_pos.y,
                          offset * step16.z + original_pos.z};
        cursor = end;
        if (!rect_variant && !point_near_field(origin.x, origin.y, 32.0f)) continue;
        LineLaserArgs child;
        child.origin = origin;
        child.angle = l.angle;
        child.target_length = float(run) * kSampleSpacing;
        child.initial_length = child.target_length;
        child.max_extent = l.infinite.target_length - offset * kSampleSpacing;
        child.width = l.width;
        child.speed = 8.0f;
        child.type = l.infinite.type;
        child.color = l.infinite.color;
        if (rect_variant) {
            child.spawn_sfx = -1;
            child.step_sfx = -1;
            child.start_offset = 0.0f;
            child.flags = (l.infinite.flags >> 1) & 0x1u;
        } else {
            // 0x436670 对派生参数整块清零：音效号 0 也会入队
            child.spawn_sfx = 0;
            child.step_sfx = 0;
        }
        spawn_line(child);
    }
}

// 0x436670
i32 LaserManager::cancel_infinite_circle(Laser& l, const Vec3& center, float radius, i32 item_mode,
                                         bool respect_immunity) {
    if (respect_immunity && l.no_cancel != 0) return 0;
    bool hit[256] = {};
    const Vec3 step8 = polar(l.angle, 8.0f);
    Vec3 sample{l.pos.x + step8.x, l.pos.y + step8.y, 0.0f};
    const Vec3 step16{step8.x + step8.x, step8.y + step8.y, 0.0f};
    int n = 0;
    if (16.0f < l.length) {
        float cursor = 8.0f;
        do {
            const float dx = center.x - sample.x;
            const float dy = center.y - sample.y;
            if (dx * dx + dy * dy <= radius * radius && n < 256) {
                hit[n] = true;
                if (bullets) bullets->convert_cancel_item(sample, item_mode);
                if (point_near_field(sample.x, sample.y, 32.0f))
                    spawn_cancel_effect(l.bullet_type, l.infinite.color, sample, false, false);
            }
            ++n;
            sample.x = sample.x + step16.x;
            sample.y = sample.y + step16.y;
            cursor = cursor + 16.0f;
        } while (cursor + 8.0f < l.length);
        spawn_infinite_fragments(l, hit, n < 256 ? n : 256, step16, false);
    }
    return 0;
}

// 0x435880
i32 LaserManager::cancel_infinite_rect(Laser& l, const Vec3& center, const Vec3& size, float rect_angle, i32 item_mode,
                                       bool respect_immunity) {
    if (respect_immunity && l.no_cancel != 0) return 0;
    bool hit[256] = {};
    const float dx = l.pos.x - center.x;
    const float dy = l.pos.y - center.y;
    const float sin_a = sin_f(-rect_angle);
    const float cos_a = cos_f(-rect_angle);
    float local_x = dx * cos_a - dy * sin_a;
    float local_y = dy * cos_a + dx * sin_a;
    const Vec3 local_step8 = polar(wrap_angle(l.angle - rect_angle), 8.0f);
    local_x = local_step8.x + local_x;
    local_y = local_y + local_step8.y;
    const Vec2 local_step16{local_step8.x + local_step8.x, local_step8.y + local_step8.y};
    const float half_w = size.x * 0.5f;
    const float half_h = size.y * 0.5f;
    const Vec3 step8 = polar(l.angle, 8.0f);
    Vec3 sample{l.pos.x + step8.x, l.pos.y + step8.y, 0.0f};
    const Vec3 step16{step8.x + step8.x, step8.y + step8.y, 0.0f};
    int n = 0;
    if (16.0f <= l.length) {
        float cursor = 8.0f;
        do {
            if (-half_w <= local_x && local_x <= half_w && -half_h <= local_y && local_y <= half_h && n < 256) {
                hit[n] = true;
                if (bullets) bullets->convert_cancel_item(sample, item_mode);
                spawn_cancel_effect(l.bullet_type, l.infinite.color, sample, true, true);
            }
            ++n;
            sample.x = sample.x + step16.x;
            sample.y = sample.y + step16.y;
            local_x = local_x + local_step16.x;
            local_y = local_y + local_step16.y;
            cursor = cursor + 16.0f;
        } while (cursor + 8.0f <= l.length);
        spawn_infinite_fragments(l, hit, n < 256 ? n : 256, step16, true);
    }
    return 0;
}

// ---------------------------------------------------------------------------------------------
// 曲线激光消弹

// 0x43a620：整条消弹——每隔 3 个节点出一个消散特效（不转化道具）
i32 LaserManager::cancel_curve_single(Laser& l, i32 item_mode, bool respect_immunity) {
    (void)item_mode;
    if (respect_immunity && l.no_cancel != 0) return 0;
    for (i32 i = 0; i < l.curve.node_count && i < i32(l.curve_nodes.size()); ++i) {
        if (i % 3 != 0) continue;
        animations->spawn_effect(*bullet_anm, l.curve.color * 2 + 0xd1, -1, l.curve_nodes[u32(i)].pos, 0.0f);
    }
    l.state = LaserState::Dead;
    return 0;
}

// 丢弃最前面的 drop_count 个节点（节点数组左移、采样时刻回退、节点数减少）
void LaserManager::shift_curve_nodes(Laser& l, int drop_count) {
    const i32 n = l.curve.node_count;
    for (i32 k = 0; k + drop_count < n && k + drop_count < i32(l.curve_nodes.size()); ++k)
        l.curve_nodes[u32(k)] = l.curve_nodes[u32(k + drop_count)];
    Timer& t = l.frame_timer;
    if (t.speed_slot != 0) t.speed_slot = 0;
    t.previous = t.current;
    if (game_speed <= 0.99f || 1.01f <= game_speed) t.current_f = game_speed * float(-drop_count) + t.current_f;
    else t.current_f = t.current_f + float(-drop_count);
    t.current = i32(t.current_f);
    l.curve.node_count = n - drop_count;
}

// 0x43a2f0：圆形消弹——每 10 个节点取一个转化道具 / 出特效；只保留一段连续未命中节点
i32 LaserManager::cancel_curve_circle(Laser& l, const Vec3& center, float radius, i32 item_mode,
                                      bool respect_immunity) {
    if (respect_immunity && l.no_cancel != 0) return 0;
    const i32 n = l.curve.node_count;
    if (n <= 0 || i32(l.curve_nodes.size()) < n) return 0;
    bool hit[256] = {};
    int hits = 0;
    for (i32 i = 0; i < n && i < 256; ++i) {
        const Vec3& p = l.curve_nodes[u32(i)].pos;
        const float dx = center.x - p.x;
        const float dy = center.y - p.y;
        if (dx * dx + dy * dy <= radius * radius) {
            ++hits;
            hit[i] = true;
            if (i % 10 == 0) {
                if (bullets) bullets->convert_cancel_item(p, item_mode);
                animations->spawn_effect(*bullet_anm, l.curve.color * 2 + 0xd1, -1, p, 0.0f);
            }
        }
    }
    if (hits == 0) return 0;
    if (n <= hits) { mark_for_destroy(l); return 0; }
    int leading = 0;
    while (leading < n && hit[leading]) ++leading;
    if (leading != 0) { shift_curve_nodes(l, leading); return 0; }
    int first_hit = 0;
    while (first_hit < n && !hit[first_hit]) ++first_hit;
    if (first_hit >= n) return 0;
    for (int i = first_hit + 1; i < n; ++i) {
        if (hit[i]) continue;
        // 首段未命中与命中段一起丢弃，保留其后的部分
        for (i32 k = 0; k + i < n; ++k) l.curve_nodes[u32(k)] = l.curve_nodes[u32(k + i)];
        l.curve.node_count = n - i;
        return 0;
    }
    l.curve.node_count = first_hit;
    return 0;
}

// 0x4397d0：矩形消弹——前导命中回退采样时刻，其后每段 ≥ 4 个连续未命中节点派生为继承轨迹的子曲线激光
i32 LaserManager::cancel_curve_rect(Laser& l, const Vec3& center, const Vec3& size, float rect_angle, i32 item_mode,
                                    bool respect_immunity) {
    if (respect_immunity && l.no_cancel != 0) return 0;
    const i32 original_count = l.curve.node_count;
    if (original_count <= 0 || i32(l.curve_nodes.size()) < original_count) return 0;
    std::vector<bool> hit(u32(original_count), false);
    int hits = 0;
    const float half_w = size.x * 0.5f;
    const float half_h = size.y * 0.5f;
    for (i32 i = 0; i < original_count; ++i) {
        const Vec3& p = l.curve_nodes[u32(i)].pos;
        float local_x = p.x - center.x;
        float local_y = p.y - center.y;
        if (rect_angle != 0.0f) {
            const float sin_a = sin_f(-rect_angle);
            const float cos_a = cos_f(-rect_angle);
            const float rx = local_x * cos_a - local_y * sin_a;
            const float ry = local_y * cos_a + local_x * sin_a;
            local_x = rx;
            local_y = ry;
        }
        if (std::fabs(local_x) <= half_w && std::fabs(local_y) <= half_h) {
            ++hits;
            hit[u32(i)] = true;
            if (i % 10 == 0) {
                if (bullets) bullets->convert_cancel_item(p, item_mode);
                const u32 id = animations->spawn_effect(*bullet_anm, l.curve.color * 2 + 0xd1, -1, p, 0.0f);
                if (effects) effects->register_id(id);
            }
        }
    }
    if (hits == 0) return 0;
    if (original_count <= hits) { mark_for_destroy(l); return 0; }
    int leading = 0;
    while (leading < original_count && hit[u32(leading)]) ++leading;
    i32 n = original_count;
    if (leading != 0) {
        // 原版只左移命中表并回退采样时刻；节点在下一帧按时刻重算
        for (i32 k = 0; k + leading < n; ++k) hit[u32(k)] = hit[u32(k + leading)];
        Timer& t = l.frame_timer;
        if (t.speed_slot != 0) t.speed_slot = 0;
        t.previous = t.current;
        if (game_speed <= 0.99f || 1.01f <= game_speed) t.current_f = float(-leading) * game_speed + t.current_f;
        else t.current_f = t.current_f + float(-leading);
        t.current = i32(t.current_f);
        n = n - leading;
        l.curve.node_count = n;
        if (n < 4) { mark_for_destroy(l); return 0; }
    }
    int first_hit = 0;
    while (first_hit < n && !hit[u32(first_hit)]) ++first_hit;
    if (first_hit < n) {
        int cursor = first_hit;
        do {
            int run_start = cursor;
            while (run_start < n && hit[u32(run_start)]) ++run_start;
            if (original_count <= run_start) break;
            int run = 0;
            cursor = run_start;
            while (cursor < n && !hit[u32(cursor)]) { ++cursor; ++run; }
            if (run > 3) {
                // 0x4397d0：整包复制母激光参数后把 +0x344 生成音效写成 -1（子曲线不再播出现音）
                CurveLaserArgs child = l.curve;
                child.spawn_sfx = -1;
                child.step_sfx = -1;
                child.inherited_segments = &l.curve_segments;
                child.initial_time = l.frame_timer.current_f - float(run_start);
                child.node_count = run;
                spawn_curve(child);
            }
        } while (cursor < l.curve.node_count);
    }
    if (first_hit < 4) mark_for_destroy(l);
    else l.curve.node_count = first_hit;
    return 0;
}

}  // namespace th16
