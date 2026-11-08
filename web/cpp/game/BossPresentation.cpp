// BossPresentation 实现：0x4265a0 boss 段的逐条移植（行号对照见头注释）。
// VM 写入走 AnmRegistry 句柄（不旁路池调度）；无脚本的星位/位标 VM 与
// 原版空白模板 VM 同构（0x403800 复制的池模板无脚本——状态位持有者）。
#include "BossPresentation.hpp"

#include <cmath>
#include <algorithm>

namespace th13 {

namespace {
// 原版 VM 字段位（00462450.c/0046a700.c 消费位）：
constexpr u32 kFlagVisible = 0x2;    // VM+0x18 bit1（0x4265a0:102-112 显隐位）
constexpr u32 kFlagSelected = 0x1;   // VM+0x18 bit0（ins_300 精灵选中位）
constexpr float kPi = 3.1415927410125732421875f;
constexpr float kTwoPi = 6.283185482025146484375f;
}  // namespace

bool BossPresentation::initialize() {
    if (digit_tens_ && digit_ones_) return true;
    // 0x425170:181-287：ascii 池模板 2/3 → ctx+0x98/+0x9c，初始隐藏
    //（VM+0x18 &= ~2、VM+0x1c &= ~0x40000 对应 render_flags 位清）。
    digit_tens_ = animations_.create(banks_.ascii, kTimerTensScript, -1, 0);
    digit_ones_ = animations_.create(banks_.ascii, kTimerOnesScript, -1, 0);
    if (!digit_tens_ || !digit_ones_) {
        fail("Boss timer digit animation unavailable");
        return false;
    }
    for (u32 handle : {digit_tens_, digit_ones_}) {
        if (auto* vm = animations_.registry.find(handle)) {
            vm->visual.flags &= ~kFlagVisible;
            vm->visual.render_flags &= ~0x40000u;
        }
    }
    return true;
}

void BossPresentation::release() {
    retire_ring();
    if (name_handle_) animations_.retire(name_handle_);
    for(auto& handle:remaining_star_handles_)if(handle){animations_.retire(handle);handle=0;}
    for (auto& handle : star_handles_) {
        if (handle && !animations_.retire(handle)) fail("Boss star retire failed");
        handle = 0;
    }
    stars_created_ = false;
    for (u32 handle : {digit_tens_, digit_ones_, marker_handle_}) {
        if (handle && !animations_.retire(handle)) fail("Boss VM retire failed");
    }
    digit_tens_ = digit_ones_ = marker_handle_ = 0;
    gate_ = false;
    displayed_ratio_ = target_ratio_ = 0;
    timer_displayed_ = -1;
    warn_state_ = 0;
    escalate_state_ = 0;
}

bool BossPresentation::create_ring() {
    // 槽 240/241/242 → front.anm 240/241/242（0x426b06/0x426bad/0x426c66）。
    if (ring_created_) return true;
    ring_arc_ = animations_.create(banks_.front, kRingArcScript, -1, 0);
    if (!ring_arc_) {
        fail("Boss ring arc animation unavailable");
        return false;
    }
    for (i32 i = 0; i < 2; i++) {
        ring_overlays_[u32(i)] = animations_.create(banks_.front, kRingOverlayScripts[i], -1, 0);
        if (!ring_overlays_[u32(i)]) {
            fail("Boss ring overlay animation unavailable");
            return false;
        }
    }
    for (u32 handle : {ring_arc_, ring_overlays_[0], ring_overlays_[1]})
        if (auto* vm = animations_.registry.find(handle))
            vm->visual.render_flags &= ~0xc000u;
    ring_created_ = true;
    return true;
}

void BossPresentation::retire_ring() {
    for (u32 handle : {ring_arc_, ring_overlays_[0], ring_overlays_[1]}) {
        if (handle && !animations_.retire(handle)) fail("Boss ring retire failed");
    }
    ring_arc_ = ring_overlays_[0] = ring_overlays_[1] = 0;
    ring_created_ = false;
}

bool BossPresentation::create_stars() {
    // 槽 243×4 → front.anm 243（0x426d13/0x426dba/0x426e61/0x426f1a）。
    if (stars_created_) return true;
    for (auto& handle : star_handles_) {
        handle = animations_.create(banks_.front, kRingStarScript, -1, 0);
        if (!handle) {
            fail("Boss star animation unavailable");
            return false;
        }
        if (auto* vm = animations_.registry.find(handle))
            vm->visual.render_flags &= ~0xc000u;
    }
    stars_created_ = true;
    return true;
}

bool BossPresentation::ensure_marker() {
    if (marker_handle_) return true;
    // 425d90 loads front.anm script111 (sprite81, layer25), not an
    // unbound VM. Its labels7..10 animate the phase-HP warning flash.
    marker_handle_=animations_.create(banks_.front,111,-1,0);
    if(!marker_handle_){fail("Boss marker animation unavailable");return false;}

    return true;
}

void BossPresentation::spell_started() {
    // 00412890 consumes label2 immediately, before the later HUD update.
    for (u32 handle : {digit_tens_, digit_ones_})
        if (!animations_.interrupt(handle, 2, true)) fail("Boss timer spell interrupt failed");
}

void BossPresentation::spell_ended() {
    // 00412900（0x413a60 符卡结束开头调用，先于 0x42a430 横幅创建，捕获/
    // 失败/超时同路径）：对 ctx+0x98/+0x9c 两条数字 VM 写 pending=3 并立即
    // 0x462450——与 0x412890（label2）对称的 immediate 消费。ascii.anm
    // script2/3 label3 = ins_407(60帧, mode9, →(208/220,23))：把符卡期
    // 下移到 51 的数字组拉回 23。缺失此接线时首卡后数字组恒停 y=51
    //（数字锚 51-7=44，quad 顶 43，即"整组低约 16px"残余，round-21 BUG2）。
    for (u32 handle : {digit_tens_, digit_ones_})
        if (!animations_.interrupt(handle, 3, true)) fail("Boss timer spell-end interrupt failed");
}

float BossPresentation::timer_text_x() const noexcept {
    const auto* vm = animations_.registry.find(digit_tens_);
    return (vm ? vm->variables.position.x : 208.f) + 16.f;
}

float BossPresentation::timer_text_y() const noexcept {
    const auto* vm = animations_.registry.find(digit_tens_);
    return (vm ? vm->variables.position.y : 39.f) - 7.f;
}

void BossPresentation::interrupt_digits(i32 label) {
    // 0x42b510/0x42b530 同式（label 4=复原、5=移开）。
    for (u32 handle : {digit_tens_, digit_ones_}) {
        if (auto* vm = animations_.registry.find(handle)) vm->pending_interrupt = label;
    }
}

u32 BossPresentation::timer_digit_color() const {
    auto* vm = const_cast<AnmManager&>(animations_).registry.find(digit_tens_);
    return vm ? vm->visual.color : 0xffffffffu;
}

bool BossPresentation::marker_visible() const noexcept {
    const auto* vm=animations_.registry.find(marker_handle_);
    return vm&&(vm->visual.flags&3u)==3u;
}

bool BossPresentation::update(const BossFrameInput& input,
                              const std::function<bool(i32)>& sound) {
    const StageEnemy* boss = input.boss;
    if (!boss && name_handle_ && !animations_.retire(name_handle_)) {
        fail("Boss name retirement failed");
        return false;
    }
    // 0x4265a0:98-107 总门（boss HUD 是否激活）：mgr+0x5c 槽占用、
    // (mgr+0x7c&1)==0 未离场、ctx+0x198==0 无对话、ctx+0x1a0>=0 有计时。
    // 槽占用（而非实体存活）是判据：终符捕获（514→BossDead）后槽由
    // 512(-1) 约 +60 帧才清，期间计时器保持显示（native-s3 trace：boss
    // 实体保留 61 帧）——保留此语义，不回归「终符后计时消失」修复。
    // 该门只管两数字 VM：环/覆盖条/阶段星由 0x4265a0:180/225-226 的
    // 独立判据驱动（见下 ring_gate），不受计时/符卡门约束。
    const bool gate = input.boss_slot_present && !input.boss_leaving &&
                      !input.dialog_active && input.timer_seconds >= 0;
    if (gate != gate_) {
        gate_ = gate;
        // 0x4265a0:102-112：数字 VM 显隐位（VM+0x18 bit1）。
        for (u32 handle : {digit_tens_, digit_ones_}) {
            if (auto* vm = animations_.registry.find(handle)) {
                if (gate)
                    vm->visual.flags |= kFlagVisible | kFlagSelected;
                else
                    vm->visual.flags &= ~kFlagVisible;
            }
        }
    }
    // 4265a0+15c: ten remaining-card stars are independent of timer/ring gates.
    for(size_t i=0;i<remaining_star_handles_.size();++i) {
        auto& handle=remaining_star_handles_[i];
        if(i<size_t(std::max(0,input.remaining_stars))) {
            if(!handle)handle=animations_.create(banks_.front,68+i32(i),-1,0);
            if(!handle){fail("Boss remaining-star animation unavailable");return false;}
        } else if(handle) {
            if(!animations_.interrupt(handle,1,false)){fail("Boss remaining-star exit failed");return false;}
            handle=0;
        }
    }
    if(gate) {
    // ---- 计时秒数与警告（0x4265a0:147-177）----
    timer_seconds_ = input.timer_seconds;
    timer_frames_ = input.timer_frames;
    timer_centiseconds_=input.timer_centiseconds;
    const i32 seconds = input.timer_seconds;
    if (seconds != timer_displayed_) {
        if(seconds<timer_displayed_) {
            // 4267db / 426808: every decremented second, labels9/8, sounds12/11.
            if(seconds<10) {
                warn_state_=seconds<5?2:1;
                interrupt_digits(seconds<5?9:8);
                if(sound&&!sound(seconds<5?12:11))fail("Boss timer sound service failed");
            }
        } else {
            interrupt_digits(7); // 42683b: reset/increase label7
            warn_state_=0;
        }
        // 数字精灵 = 270 + 数位（0x461a60 同式：sprite 直选 + uv/矩阵重算）。
        if (auto* vm = animations_.registry.find(digit_tens_))
            if (!vm->select_sprite(kTimerDigitSpriteBase + seconds / 10))
                fail("Boss timer tens sprite unavailable");
        if (auto* vm = animations_.registry.find(digit_ones_))
            if (!vm->select_sprite(kTimerDigitSpriteBase + seconds % 10))
                fail("Boss timer ones sprite unavailable");
        timer_displayed_ = seconds;
    }

    // ---- 玩家贴顶移动态机（0x4265a0:109-145，迟滞 128/160）----
    const u32 move = (move_state_ >> 9) & 3;
    if (move == 0) {
        if (input.player_y < 128.f) {
            move_state_ = (move_state_ & ~0x400u) | 0x200u;
            interrupt_digits(5);  // 0x42b530：移开
        }
    } else if (move == 1) {
        if (input.player_y > 160.f) {
            interrupt_digits(4);  // 0x42b510：复原
            move_state_ &= ~0x600u;
        }
    }

    } // timer gate
    if(!update_marker(input))return false;
    // 0x4265a0:180 / :225-226：环组外层门只看 (mgr+0x7c&1)==0，逐槽条件
    // +0x5138<100000 / (+0x521c&0x31)==0 / +0x51bc<1 / ctx+0x198==0；
    // 两数字 VM 的槽占用/计时门（:98-107）与环无关——原版血环在 boss
    // 非符卡阶段同样显示（boss_hud_test
    // boss_ring_is_independent_of_timer_and_empty_boss_retires_it 锁定）。
    // 「道中无符卡误显蓝槽 / 同屏 2 条」尚未定案：0x4265a0:192-213 的
    // 空槽逐帧回收已由下方 if(!ring_gate) 实现，早先追加的换关 retire
    // 因依据（0x42bfa0.c:104-139）被否证而撤回。此处不新增计时/槽占用
    // 判据，避免 boss 非符卡阶段血环消失的回归；位置/样式待捕获裁决。
    const bool ring_gate = boss && !input.boss_leaving && !input.dialog_active &&
        boss->life < 100000.f && !(boss->flags & 0x31u) &&
        boss->invulnerable < 1;
    if(!ring_gate) {
        retire_ring();
        for(auto& handle:star_handles_)if(handle){animations_.retire(handle);handle=0;}
        stars_created_=false;
        if(!boss){displayed_ratio_=target_ratio_=0;}
        return error.empty();
    }
    if (input.name_script >= 0 && !name_handle_) {
        name_handle_ = animations_.create(banks_.front, input.name_script);
        if (!name_handle_) { fail("Boss name animation unavailable"); return false; }
    }
    // ---- 血环比值（0x4265a0:225-239）----
    if (input.life_initial > 0 && input.boss_life >= 0) {
        target_ratio_ = float(input.boss_life) / float(input.life_initial);
        if (target_ratio_ > 1.f) target_ratio_ = 1.f;
    }
    // displayed：目标更小直落、否则每帧 +0.025 逼近（0x4265a0:230-239）。
    if (displayed_ratio_ > target_ratio_)
        displayed_ratio_ = target_ratio_;
    else
        displayed_ratio_ = float(displayed_ratio_ + 0.025f) > target_ratio_
                               ? target_ratio_
                               : displayed_ratio_ + 0.025f;

    // ---- 环根与跟随（0x4265a0:416-447 + 0x426f9e-0x427091 写入点）----
    const bool life_ratio_ready = input.life_initial > 0;
    if (life_ratio_ready) {
        if (!create_ring()) return false;
        // 驱动：VM+0x18|=4、rotation.x = -displayed*2π（0x426fcc-0x426fd8）。
        if (auto* vm = animations_.registry.find(ring_arc_)) {
            vm->visual.flags |= 0x4u;
            vm->variables.rotation.x = -displayed_ratio_ * kTwoPi;
            // 三条带 offset(+0x574..) = boss 世界位（0x426fdb-0x42704f）。
            vm->visual.translation = {boss->position[0], boss->position[1],
                                      boss->position[2]};
        }
        for (u32 handle : ring_overlays_)
            if (auto* vm = animations_.registry.find(handle))
                vm->visual.translation = {boss->position[0], boss->position[1],
                                          boss->position[2]};
    } else if (ring_created_) {
        retire_ring();
    }

    // ---- 阶段星（0x4265a0:448-482）----
    if (life_ratio_ready) {
        if (!stars_created_ && !create_stars()) return false;
        for (i32 i = 0; i < 4; i++) write_star(star_handles_[u32(i)], input, i);
    } else if (stars_created_) {
        for (auto& handle : star_handles_) {
            if (handle && !animations_.retire(handle)) fail("Boss star retire failed");
            handle = 0;
        }
        stars_created_ = false;
    }

    // 42721b/427296 send labels 2/3 to all seven existing VMs; the
    // scripts animate the transition, so their handles and clocks survive.
    if (life_ratio_ready) {
        const float dx = boss->position[0] - input.player_x;
        const float dy = boss->position[1] - input.player_y;
        const float distance_squared = dx * dx + dy * dy;
        const i32 label = !proximity_hidden_ && distance_squared < 6400.f ? 3 :
                          proximity_hidden_ && distance_squared >= 9216.f ? 2 : 0;
        if (label) {
            for (u32 handle : {ring_arc_, ring_overlays_[0], ring_overlays_[1],
                               star_handles_[0], star_handles_[1], star_handles_[2],
                               star_handles_[3]})
                if (!animations_.interrupt(handle, label, false)) {
                    fail("Boss proximity interrupt failed");
                    return false;
                }
            proximity_hidden_ = label == 3;
        }
    }


    return true;
}

bool BossPresentation::update_marker(const BossFrameInput& input) {
    const auto* boss=input.boss;
    if(!boss||(boss->flags&0x21u)) {
        if(auto* vm=animations_.registry.find(marker_handle_))vm->visual.flags&=~kFlagVisible;
        return true;
    }
    if (!ensure_marker()) return false;
    if (auto* vm = animations_.registry.find(marker_handle_)) {
        vm->visual.flags |= kFlagVisible | kFlagSelected;
        // offset = (boss_x + 32 + 192, 480)（0x4265a0:717-719）。
        vm->visual.translation = {boss->position[0] + 32.f + 192.f, 480.f, 0.f};
        // 字节 +0x52b = '@' + round(|dx|*191/64)；|dx|>=64 → 0xff、横向出界 → 0。
        // Native vm+52b is alpha; retain the observed value for diagnostics.
        const float dx = std::fabs(boss->position[0] - input.player_x);
        i32 glyph = 0xff;
        if (dx < 64.f) glyph = '@' + i32(std::nearbyint(dx * 191.f * 0.015625f));
        if (boss->position[0] < -192.f || boss->position[0] > 192.f) glyph = 0;
        marker_glyph_ = glyph;
        vm->visual.color=(vm->visual.color&0xffffffu)|(u32(glyph)<<24);
        // 4265a0 reads remaining phase HP (+5140), not remaining time.
        const i32 life_left=boss->phase_life_remaining;
        const i32 t1 = input.trance_mode ? 2000 : 700;
        const i32 t2 = input.trance_mode ? 1000 : 400;
        const i32 t3 = input.trance_mode ? 400 : 200;
        if (escalate_state_ == 0 && life_left < t1) {
            vm->pending_interrupt = 7;
            escalate_state_ = 1;
        } else if (escalate_state_ == 1 && life_left < t2) {
            vm->pending_interrupt = 8;
            escalate_state_ = 2;
        } else if (escalate_state_ == 2 && life_left < t3) {
            vm->pending_interrupt=9;
            escalate_state_ = 3;
        } else if(escalate_state_==3&&life_left>t3){vm->pending_interrupt=10;escalate_state_=0;}
    }
    return true;
}

void BossPresentation::write_star(u32 handle, const BossFrameInput& input, i32 index) {
    auto* vm = animations_.registry.find(handle);
    if (!vm) return;
    const float angle = input.markers[u32(index)].ratio;
    // 原版两段隐藏判据（0x4270da-0x42719d，objdump 原件见
    // artifacts/pr159-fix-20261104/blue-marker/）：
    //   1) 0x4270da flds ratio; 0x4270dc fld %st(1)（ST1 = 0x42706b fldz 的
    //      0.0）; fucompp; test ah,0x44; jnp 0x42719d —— ratio == 0 即隐藏。
    //   2) 0x4270eb flds ratio; flds displayed(+0x1ac); fcompp; test ah,0x41;
    //      jnp 0x42719d —— displayed <= ratio 隐藏。
    // ECL527 全 stage 只写 index 0（其余槽保持 0），缺 (1) 时 4 个零比例
    // 标记会叠在同一位置（用户截图 145616 的左侧蓝团）。上轮以「ratio=0
    // 合法」为由删掉 angle==0 与本条反汇编冲突，本节以直调/反汇编为准。
    if (angle == 0.f || angle == displayed_ratio_ || displayed_ratio_ < angle) {
        vm->visual.flags &= ~kFlagVisible;
        return;
    }
    vm->visual.flags |= kFlagVisible | kFlagSelected;
    // 0x427100-0x427139：bit1|bit2 置、rotation.z = normalize(−π − a*2π)
    //（0x4ae880=−π、0x4ae850=2π、0x472f30 归一）。
    const float t = normalize_angle(float(-double(kPi) - double(angle) * double(kTwoPi)));
    vm->variables.rotation.z = t;
    // 0x42713c-0x427193：位置 = boss_world + R(t)·(0, 56, 0)。
    // 调用点 0x427135 `push %ecx` 使 ESP 相对循环基址 −4；0x42713c
    // `lea 0xa0(%esp)` 因此指向循环基址 +0x9c，即 {0.0, 56.0}（0x9c 来自
    // 0x42706b/0x427071 的 fldz/fst，56.0 由 0x4aef18 经 0x42708a 存入
    // 0xa0）。0x472fc0 直调（Unicorn，输入/输出见同目录 probe-out.txt）是
    // 标准旋转：R(t)·(x,y) = (x·cos−y·sin, x·sin+y·cos)，故 x 分量 =
    // −56·sin t、y 分量 = +56·cos t（0x472fc0 输出 pair 由 edi=+0x38 写入，
    // 0x42714f/0x427171 再 fadds boss 世界位 +0x44/+0x48）。旧式
    // boss+56·(cos t, sin t) 少了一次 90° 旋转（截图左侧蓝标即该偏差）。
    const StageEnemy* boss = input.boss;
    const float sine = std::sin(t);
    const float cosine = std::cos(t);
    vm->visual.translation = {
        float(boss->position[0] - kStarOrbitRadius * sine),
        float(boss->position[1] + kStarOrbitRadius * cosine),
        boss->position[2]};
}

}  // namespace th13
