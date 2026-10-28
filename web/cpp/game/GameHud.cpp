// GameHud 生命周期：建句柄、interrupt 布局（与 th15 GameHud.cpp 逐行同构；
// 脚本号按 th13 front.anm dump 锁定：残机 script30–37、灵击 script38–45）。
#include "GameHud.hpp"

namespace th13 {

namespace {
// fresh stage ctx 由 0x4264f0 的 _memset(...,0,0x2b4) 清零，bit0 从 0 开始。
constexpr u32 kTranceGaugeDimmed = 1u;
}  // namespace

bool GameHud::check(bool ok) {
    if (!ok && error.empty())
        error = animations.error.empty() ? "HUD animation unavailable" : animations.error;
    return ok;
}

u32 GameHud::create(i32 bank, i32 script, u32 ordering) {
    const u32 handle = animations.create(bank, script, -1, ordering);
    check(handle != 0);
    return handle;
}

u32 GameHud::widget(i32 script) {
    const u32 handle = create(front, script, 0);
    if (auto* vm = animations.registry.find(handle)) vm->visual.render_flags &= ~0xc000u;
    return handle;
}

bool GameHud::direct_interrupt(u32 handle, i32 label) {
    auto* vm = animations.registry.find(handle);
    if (!vm) return check(false);
    vm->pending_interrupt = label;
    return true;
}

bool GameHud::life(i32 lives, i32 pieces) {
    if (!life_icons[0]) return true;
    if (lives > 8 || lives < 0) {
        error = "HUD life values outside original limits";
        return false;
    }
    for (i32 i = 0; i < lives; i++)
        if (!direct_interrupt(life_icons[u32(i)], 2)) return false;
    if (lives > 7) return true;
    // 碎片档位 = (pieces*5)/threshold+7（0x42ae90:34-40，threshold =
    // 0x4bb994[DAT_004be7fc]）；script30 标签 7..12 为 6 档，不是固定 5 片。
    const i32 threshold = life_piece_threshold(progress.life_piece_threshold_index);
    if (!direct_interrupt(life_icons[u32(lives)], (pieces * 5) / threshold + 7)) return false;
    for (i32 i = lives + 1; i < 8; i++)
        if (!direct_interrupt(life_icons[u32(i)], 3)) return false;
    return true;
}

bool GameHud::bombs(i32 stock, i32 pieces) {
    if (!bomb_icons[0]) return true;
    if (stock < 0 || stock > 8) {
        error = "HUD bomb stock outside original limits";
        return false;
    }
    for (i32 i = 0; i < stock; i++)
        if (!direct_interrupt(bomb_icons[u32(i)], 2)) return false;
    if (stock > 7) return true;
    // 0x42af60:38：label = (pieces*6)/8+7（灵击分母固定 8）。
    if (!direct_interrupt(bomb_icons[u32(stock)], (pieces * 6) / 8 + 7)) return false;
    for (i32 i = stock + 1; i < 8; i++)
        if (!direct_interrupt(bomb_icons[u32(i)], 3)) return false;
    return true;
}

bool GameHud::stage_logo(i32 scene_destination) {
    // Stage 场景资产（st01logo 等）随 Stage 分期接入；UI 基座仅认非转场入口。
    return scene_destination == 8;
}

bool GameHud::begin_stage_clear() {
    // 42abf0 uses the main chain, with zero translation/anmArg and the
    // template's coordinate flags intact (unlike widget()). create() uses
    // the same template -> initial tick -> submit path as this constructor.
    clear_banner = create(front, 115, 0);
    if (!clear_banner) return false;
    flags |= 0x100u;
    clear_age.set(0);
    return true;
}

bool GameHud::finish_stage_clear() {
    // 42ad40 sends pending label1 rather than destroying the live VM.
    // Missing/expired handles are accepted by46fc30 as well.
    if (!check(animations.interrupt(clear_banner, 1, false))) return false;
    flags &= ~0x100u;
    clear_age.set(0);
    return true;
}

bool GameHud::initialize(const HudEntryContext& context) {
    if (!error.empty()) return false;
    if (!root && (root = widget(0)) == 0) return false;
    if (!life_icons[0]) {
        for (u32 i = 0; i < 8; i++) {
            life_icons[i] = create(front, i32(i) + 29, 0);
            auto* vm = animations.registry.find(life_icons[i]);
            if (!vm) return false;
            vm->visual.render_flags &= ~0xc000u;
        }
        for (u32 i = 0; i < 8; i++) {
            bomb_icons[i] = create(front, i32(i) + 37, 0);
            auto* vm = animations.registry.find(bomb_icons[i]);
            if (!vm) return false;
            vm->visual.render_flags &= ~0xc000u;
        }
    }
    // Boss 计时数字 VM（0x425170:181-287：ascii 池模板 2/3，初始隐藏）。
    // ascii bank 缺失 = 资源/接线错误，显式失败（不吞错）。
    if (!boss_hud.initialize()) {
        error = boss_hud.error;
        return false;
    }
    if (!life(player.extra_lives, player.life_pieces) || !bombs(player.bombs, player.bomb_pieces))
        return false;
    // 00425170 starts the temporary difficulty notice through label3;
    // its script moves away from the FPS corner and terminates after164 ticks.
    if (progress.difficulty < 0 || progress.difficulty > 5) {
        error = "HUD difficulty outside front animation range";
        return false;
    }
    // 43c1c0 sets DAT_004dcc30 for fresh entries/retries and clears it for
    // destination0xc.425170 always creates the stage's difficulty label;
    // its notice and gauge creation are conditional on that fresh flag.
    // Overwriting a HUD handle does not retire the preceding label VM.
    if (context.new_run) {
        difficulty_notice = widget(79 + progress.difficulty);
        if (!difficulty_notice || !animations.interrupt(difficulty_notice, 3, false)) return false;
    }
    difficulty_label = widget(85 + progress.difficulty);
    if (!difficulty_label || !animations.interrupt(difficulty_notice, 3, false)) return false;
    if (context.new_run) {
        // 425170's final creation is171, followed by42b030. Its children
        // 172–175 are created by the script and survive along with the HUD.
        gauge_main = widget(171);
        if (!gauge_main) return false;
        // fresh stage ctx（0x4264f0 memset）bit0 从 0 开始；仅此路径清 dim。
        flags &= ~kTranceGaugeDimmed;
        trance_skull_label = 0;
        if (!update_trance()) return false;
    }
    return true;
}

bool GameHud::reset_for_retry() {
    prepare_stage_assets();
    return life(player.extra_lives, player.life_pieces) && bombs(player.bombs, player.bomb_pieces);
}

// 0x42b030 值→显示（每帧幂等）：只读 trance_value(SessionState)
//（DAT_004be808 灵界计，旧登记名白点/ending_points），不持第二份计量。
// 172: 标签 2 (>=200) / 3；173: scale.x=uv_scale.x=floor(g/200)/3；
// 175: scale.x=uv_scale.x=(g%200)/200、offset.x=frac*16；174 原版不写。
bool GameHud::update_trance() {
    if (!gauge_main) return true;
    auto* root = animations.registry.find(gauge_main);
    if (!root) {
        // 0x42b030:16-22/39：查找失败即清句柄，不保留悬挂 u32。
        gauge_main = 0;
        trance_skull_label = 0;
        return true;
    }
    i32 value = trance_value(progress);
    if (value < 0) value = 0;
    if (value > kTranceMax) value = kTranceMax;
    if (auto* skull = animations.registry.find_child_script(*root, 172, 0)) {
        const i32 label = value >= kTranceSegment ? 2 : 3;
        // pending_interrupt 每 tick 被解释器清零；仅在目标档变化时重发，
        // 避免每帧重入标签 2 把循环动画复位（原版 +0x4a0 仅在值变时写）。
        if (trance_skull_label != label) {
            skull->pending_interrupt = label;
            trance_skull_label = label;
        }
    }
    if (auto* filled = animations.registry.find_child_script(*root, 173, 0)) {
        const float completed = float(value / kTranceSegment) / 3.f;
        filled->visual.scale.x = completed;     // VM+0x60（ins_402 scale.x）
        filled->visual.uv_scale.x = completed;  // VM+0x78（ins_429 uv_scale.x）
        // 原版 0x42b030 同时 `or $0x18, VM+0x18`；+0x18=flags，0x8=scale 脏、
        // 0x10=uv_scale 脏（0x462450 case0x192/0x1ad），本作对应 visual.flags。
        filled->visual.flags |= 0x18;
    }
    if (auto* partial = animations.registry.find_child_script(*root, 175, 0)) {
        const float fraction = float(value % kTranceSegment) / float(kTranceSegment);
        partial->visual.scale.x = fraction;
        partial->visual.uv_scale.x = fraction;
        partial->visual.translation.x = fraction * 16.f;  // VM+0x574 offset.x
        partial->visual.flags |= 0x18;
    }
    return true;
}

}  // namespace th13
