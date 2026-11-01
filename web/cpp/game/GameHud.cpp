// GameHud 生命周期：建句柄、interrupt 布局（与 th15 GameHud.cpp 逐行同构；
// 脚本号按 th13 front.anm dump 锁定：残机 script30–37、灵击 script38–45）。
#include "GameHud.hpp"

namespace th13 {

namespace {
// sig.anm = 常驻 ANM 管理器槽 1（Application kBanks 表；原版 0x439730 在
// 全局帧 0 以槽 1 打开 sig.anm，0x4398e0 注册的开场任务 0x439ce0 随即创建
// 其 script0，左半 512×480 + op500 子 script1 右半 128×480 拼满 640 宽）。
constexpr i32 kOpeningSignatureBank = 1;
// 相位对齐：原版创建趟 = 模板预跑态（色插值 1/10、timer1）再 tick 一次，
// 落 2/10、timer2；端口 create() 从指令 0 重跑 t=-1 的 ins_408，落 1/10、
// timer1（落后一步）。加速启动在全局帧 0 建、战斗帧 0 = 全局帧 4，中间
// 全局帧 1..3 各 tick 一次。四步单 VM tick 同时补「创建差一步」与「1..3」。
constexpr u32 kOpeningSignaturePhaseTicks = 4;
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

GameHud::~GameHud() { release_opening_signature(); }

void GameHud::release_opening_signature() {
    // 0x46f690/0x46fea0：标 render_flags|=0x20 后由下一趟链扫描回收；槽 1
    // 是常驻 bank（release_run 不 unload/retire 它），不主动回收会一直画到标题。
    if (opening_signature) animations.retire(opening_signature);
    opening_signature = 0;
    opening_signature_right = 0;
}

bool GameHud::begin_opening_signature() {
    // 原版只有加速战斗入口保留开场签名条：0x439730/0x4398e0/0x439ce0 的开场
    // 任务在全局帧 0 建 VM，而标题线程尾段 0x44c0d0 在开场计数 >= 0xB4(180)
    // 时经 0x46e620 卸载槽 1——正常「开场→标题→进关」没有这两条。端口里等价
    // 的加速入口是回放/调试直入（Application begin_replay_stage 置
    // practice_mode=1；关卡练习在 run_mode_flags 置 bit4=0x10、符卡练习置
    // bit5=0x20，二者都经过标题，不得创建）。
    if (progress.practice_mode == 0 || (progress.run_mode_flags & 0x30u) != 0) return true;
    // UI 夹具只装 front/ascii 合成 bank；生产路径 sig.anm 是常驻槽 1。
    if (!animations.resource(kOpeningSignatureBank)) return true;
    release_opening_signature();
    // 0x439ce0：模板 0 拷贝 + 0x462450 初始 tick + 0x46f3f0 主链尾插（ordering 0），
    // 无 layer 参数、无平移（layer 23/位置由 script0 的 ins_304/ins_400 写入）。
    const u32 root = create(kOpeningSignatureBank, 0, 0);
    if (!root) return false;
    opening_signature = root;
    auto* vm = animations.registry.find(root);
    if (!vm) {
        release_opening_signature();
        return check(false);
    }
    // script0 的 ins_500 在创建 tick 内经 0x46f990 生成右半 script1。
    auto* right = animations.registry.find_child_script(*vm, 1, 0);
    if (!right) {
        release_opening_signature();
        error = "sig.anm script1 child VM missing";
        return false;
    }
    // 原版 VM 层是 25（script1 ins_304 0x19）；捕获里的 26 是 0x43b900 在层 26
    // 遍历期冲批的提交层。DrawCapture 记 VM 层，设 26 才复刻基线 op 层
    //（与 Application::draw_loading 的既有 26 约定一致）。
    right->visual.layer = 26;
    opening_signature_right = animations.registry.handle(*right);
    // 单 VM tick（tick_instance）只推进这两条 VM，不碰本帧之前已建好的战斗
    // VM（自机/背景/特效在 RunScene::initialize 更早处创建）。
    for (u32 i = 0; i < kOpeningSignaturePhaseTicks; i++) {
        if (animations.tick_instance(*vm) < 0 || animations.tick_instance(*right) < 0) {
            release_opening_signature();
            return check(false);
        }
    }
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
    // 42adc0 copies script0 from the loaded stage logo bank; the animation
    // owns its lifetime. Transition destination 8 and mode bit 0x40 skip it.
    if (scene_destination == 8 || (progress.run_mode_flags & 0x40u)) return true;
    if (stage_logo_bank < 0) {
        error = "Stage logo animation bank unavailable";
        return false;
    }
    return create(stage_logo_bank, 0, 0) != 0;
}

bool GameHud::stage_logo_title(i32 scene_destination) {
    // 0x425170:291-349 (block D): at stage entry the retail creates the stage
    // logo VM from template index 1 of the stXXlogo.anm file object
    // (00424FD0: mov eax,6 -> 0046DF40 slot 6; 0042586c mov edi,[edi+0x14c];
    // 00425929 mov esi,[edi+0x10c]; add esi,0x5a4 -> rep movsd). That template
    // is script 1, whose first tick sets layer 20/sprite 5 (00462450 0x130/0x12c)
    // and op400 (512,464): the front/logo/stXXlogo.png quad drawn from frame 0.
    // Gate 00425841/0042584e/0042585b: DAT_004dcc24 != 8,
    // !(DAT_004be830 & 0x40), (DAT_004be830 & 0x30) != 0x20 (not spell practice).
    if (scene_destination == 8 || (progress.run_mode_flags & 0x40u) ||
        (progress.run_mode_flags & 0x30u) == 0x20u)
        return true;
    // The run scene injects the bank before initialize(); the UI test harness
    // intentionally has no logo bank, so absence here is not an error.
    if (stage_logo_bank < 0) return true;
    return create(stage_logo_bank, 1, 0) != 0;
}

bool GameHud::stage_music_logo() {
    // MSG opcode19: 429a24 creates script2 in the same stage-logo bank.
    if (stage_logo_bank < 0) {
        error = "Stage logo animation bank unavailable";
        return false;
    }
    return create(stage_logo_bank, 2, 0) != 0;
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
    // 开场签名条先于 0x425170 的 HUD VM 创建（原版全局帧 0 建，早于进关），
    // 保证同层（26）链表序与原版一致。
    if (!begin_opening_signature()) return false;
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
    // 0x425170:291-349 block D: stage-entry logo (stXXlogo.anm script 1).
    // Native order 0x425829/0x42583c (life/bomb interrupts) precedes the
    // 0x425841 gate; life/bombs only interrupt existing VMs, but keep the
    // creation sequence identical to retail.
    // Distinct from ECL554's 0x42adc0 (script 0 banner, fires ~730 ticks in).
    if (!stage_logo_title(context.scene_destination)) return false;
    // 425170:509-512 creates front.anm script67 only for a fresh Stage 1
    // ordinary run with no continues. The script owns its children and the
    // full 198-tick presentation lifecycle; keep no HUD-local timer/handle.
    if (progress.stage == 1 && progress.practice_mode == 0 && progress.continues == 0 &&
        !create(front, 67, 0))
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
// 175: scale.x=uv_scale.x=(g%200)/200、offset.x=floor(g/200)*16；174 原版不写。
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
        // 42b1dc multiplies the retained quotient, not the fractional ST0
        // value stored to scale above (42b16a leaves the quotient on x87).
        partial->visual.translation.x = float(value / kTranceSegment) * 16.f;
        partial->visual.flags |= 0x18;
    }
    return true;
}

}  // namespace th13
