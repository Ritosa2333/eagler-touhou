#include "GameManager.hpp"

#include "AnmManager.hpp"
#include "GameGlobals.hpp"
#include "ScoreFile.hpp"

namespace th16 {

bool GameManager::first_frame(AnmManager& manager, GameGlobals& globals) {
    // 0x42dc50
    if (gui_live) gui.clear_stage_bonus();  // 0x42c1b0
    if ((flags & kLoadFailed) != 0) {
        globals.scene_request = 2;
        return false;
    }
    if (stage_live && stage) {
        // 0x40add0：启用关卡链、建面 VM
        if (on_stage_started) on_stage_started();
        stage->start(manager);
    }
    if (previous_stage) {
        // 0x40c0d0：ScreenEffect kind 2（0x1e 帧、色 0）+ 上一关进入结束（bit1）；0x40c210：新关淡入（bit2，0x3c 帧）
        if (on_screen_fade) on_screen_fade(2, 0x1e, 0, 0x0a);
        previous_stage->begin_end(0x1e, 2);
        if (stage_live && stage) stage->begin_end(0x3c, 4);
        flags |= kRetry;
        if (gui_live) gui.dismiss_notice();   // 上一关末尾弹出的通知（符卡奖励等）中断 1 收起
        return true;
    }
    reset_for_stage(manager, globals);
    // 0x42dc50 末：非回放（bit4-5 ≠ 0x20）且非演示时 0x43c3f0(0, 关卡曲)；
    // 随后中断 AsciiManager+0x19244（th16 内无人写入，恒 0）、+0x19248 Loading VM（reset_for_stage 已做）
    // 与 DAT_004c12a0 标题过场效果 → 1（面板展开，露出场地）
    if ((globals.flags & 0x30u) != 0x20u && (globals.flags & 0x40u) == 0) start_stage_bgm();
    manager.interrupt(globals.transition_effect_id, 1);
    return true;
}

void GameManager::start_stage_bgm() {
    // 0x43c3f0：0x45ed00(2, 槽 0) 播放并置成绩文件 +0x19fa6 + 曲目（DAT_004a6f18 +0x28）
    if (music != nullptr) music->play_slot(0);
    if (scores != nullptr && entry != nullptr) scores->mark_bgm_heard(entry->bgm_track);
}

void GameManager::deferred_init(AnmManager& manager, GameGlobals& globals) {
    // 0x42dee0
    if ((flags & kRetry) == 0) return;
    reset_for_stage(manager, globals);
    // 0x45ed00(3 / 4) 停曲 → 0x45ed00(2) 重播关卡曲，并无条件置 +0x19fa6 + 关卡曲已听过
    if (music != nullptr) music->stop_track();
    start_stage_bgm();
    timer.reset();
}

void GameManager::reset_for_stage(AnmManager& manager, GameGlobals& globals) {
    // 0x42dc50 / 0x42dee0 共用段
    flags &= ~kRetry;
    if (bullets_live) bullets->reset();                // 0x411b70
    if (player_live) player.reset(manager, globals);   // 0x441740
    if (items_live) items->reset();                    // 0x4184a0
    if (enemies_live) enemies.reset();                 // 0x4185b0
    if (lasers_live) lasers->reset();                  // 0x42cb00
    globals.game_frame = 0;
    globals.stage_frame = 0;
    // 0x448eb0：Replay 链启用、0x449120 游戏输入状态清零，随后段头自机坐标 / 快照的读回或写入
    if (game_input != nullptr) *game_input = InputState{};
    if (on_replay_stage_start) on_replay_stage_start();
    if (enemies_live) {
        if (!enemies.spawn_main()) {
            // 生成失败时置装载失败位；上层会在 failure 中看到 enemies.error
            flags |= kLoadFailed;
        }
        // enemy_count 由敌人更新链同步；harness 桩可覆盖，避免此处抢写
    }
    if (gui_live) gui.init_hud(manager.file(AnmManager::kAsciiSlot));   // 0x426d70
    chains_suspended = false;  // 各管理器链 |= 2
    if (player_live) player.setup_options(manager, globals);  // 0x4440e0
    // 0x43c3f0 BGM 播放、AsciiManager +0x19244 中断、DAT_004c12a0 中断：Sound / 渲染阶段补
    if (loading_vm != 0) {
        manager.interrupt(loading_vm, 1);
        loading_vm = 0;
    }
}

bool GameManager::update(AnmManager& manager, GameGlobals& globals, float game_speed) {
    // 0x42d7b0
    if ((flags & kStageClear) != 0) {
        if ((flags & kMenuOpen) != 0) return false;
        ++end_counter;
        if (end_counter == 0xb4 && on_screen_fade) on_screen_fade(5, 200, 0, 0x54);
        // 倒计时过 0x17b：非回放 → 结局场景 0xf（Extra 难度 → 0x10）；回放 → 0x43f240 回标题。
        // 每帧都会重复登记，直到主循环消费场景请求
        if (end_counter > 0x17b) {
            if (mode == 0) {
                // 0x42d8da：退出标志也适用于正篇和 Extra 通关。
                const bool quit_app = config_flags != nullptr && (*config_flags & 0x2000u) != 0;
                globals.scene_request = quit_app ? 2 : (globals.difficulty == 4 ? 0x10 : 0xf);
            } else if (on_return_to_title) {
                on_return_to_title();
            }
        }
    }
    // 过关奖励展示期间（Gui bit8、计时 < 0x78）整帧跳过（含帧计数与计时）
    if (gui_live && gui.clear_bonus_pending() && (flags & kMenuOpen) == 0) return true;
    if (timer.current == 0) {
        if (!first_frame(manager, globals)) return true;
    } else if (timer.current == 0x1e) {
        deferred_init(manager, globals);
    }
    if (previous_stage && (previous_stage->flags & 8u) != 0) {
        // 0x4099a0：析构上一关（面 VM 已随 ANM 槽卸载）
        if (on_previous_stage_destroyed) on_previous_stage_destroyed(*previous_stage);
        previous_stage.reset();
    }
    if (timer.current == 5) globals.sound_thread_state = 2;   // DAT_004d9d90：声音线程进入“游戏中”
    if ((flags & kLoading) != 0) {
        flags |= kFirstUpdate;
        return true;
    }
    if ((globals.flags & 0x40u) != 0) {
        // 演示回放：宿主键 0x80103 或结束位 0x70 → 回标题；第 0xf00 帧起 60 帧淡出，0xf3c 帧回标题
        const i32 title_scene = config_flags != nullptr && (*config_flags & 0x2000u) != 0 ? 2 : 4;
        if ((system_input != nullptr && (system_input->held & 0x80103u) != 0) || (flags & kEndMask) != 0)
            globals.scene_request = title_scene;
        if (timer.current == 0xf00) {
            // 0x42da06：演示淡出排在绘制链 0x33，先于后续标题过渡层。
            if (on_screen_fade) on_screen_fade(5, 0x3c, 0, 0x33);
        } else if (timer.current == 0xf3c) {
            globals.scene_request = title_scene;
        }
    }
    if (gui_live) gui.roll_score();  // 0x42bc10
    if ((flags & kEndMask) != 0) return false;
    if ((flags & kStageTransition) != 0) {
        if (transition_frame == 0) {
            // 首帧：0x43c440 停曲（非 Extra 关），DAT_004c12a0 过场效果中断 1
            if (globals.stage < 0x2b && music != nullptr) music->stop_track();
            manager.interrupt(globals.transition_effect_id, 1);
        }
        ++transition_frame;
        if (transition_frame < transition_length) {
            if (transition_frame > 1) return false;   // 原版返回 3
        } else {
            start_stage_bgm();
            if (on_transition_finished) on_transition_finished();
            flags &= ~kStageTransition;
            transition_frame = 0;
        }
    }
    if (on_frame_advanced) on_frame_advanced();   // 0x45f980 ×4：画面合成 VM
    ++globals.game_frame;
    ++globals.stage_frame;
    if (globals.post_load_frames != 0) --globals.post_load_frames;
    timer.tick(game_speed);
    return true;
}

}  // namespace th16
