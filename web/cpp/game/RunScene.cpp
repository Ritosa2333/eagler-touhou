#include "RunScene.hpp"

#include <algorithm>
#include <cstdio>

namespace th13 {

namespace {
// 场景回调优先级（th13 原版调度位待锚定；先按 th15 同族布局取值）：
// 更新 20（标题 6 与暂停 11 之间），HUD 文本 76（动画层 75 之后、字幕 78 之前）。
// 战斗直绘穿插 AnmDrawSchedule 各层：自机 23（00441e5a→00443df0）/
// 自机弹 20（原版 layer9；不能在 priority14 的背景合成之前绘制）/
// 敌机 19（层 8 同拍）/敌弹 43（层 20 之后，注册晚于动画层回调故同值仍靠后）。
// STD 舞台背景两组均在动画层 0（优先级 5）之前：背景组 3 / 前景组 4
// （th15 StageDrawFrame 3/6 的 th13 排位：层 0 前置约束下压缩到 3/4）。
constexpr i32 kRunUpdatePriority = 20;
constexpr i32 kRunTextPriority = 76;
constexpr std::array<i32, 5> kStageDrawPriorities = {20, 23, 19, 43, 29};
constexpr std::array<i32, 2> kSceneryDrawPriorities = {2, 5};
}  // namespace

RunScene::RunScene(SessionState& p, HudPlayerState& player, HudScoreState& score,
                   AnmManager& manager, AnmRenderer& scene_view, AnmEnvironment& e,
                   RunSceneServices& h, FrameScheduler& f, StagePresentation::Banks banks,
                   i32 front, i32 ascii, i32 scenery_bank, const std::string& std_name,
                   GameRandom* shared_random, GameHud* shared_hud,
                   StagePlayerState* shared_player)
    : progress(p),
      animations(manager),
      scene_renderer(scene_view),
      environment(e),
      host(h),
      scheduler(f),
      player_state(player),
      score_state(score),
      owned_hud(shared_hud ? nullptr :
          std::make_unique<GameHud>(animations, p, player, score, front, ascii)),
      hud(shared_hud ? *shared_hud : *owned_hud),
      battle(shared_random, shared_player),
      presentation(std::make_unique<StagePresentation>(animations, scene_renderer, banks,
                                                       kPlayerShotSprite)),
      msg_presentation(std::make_unique<MsgPresentation>(
          animations, MsgPresentation::Banks{banks.player, banks.stage})),
      scenery_bank(scenery_bank),
      scenery_std(std_name),
      post_flow(p, player, score, *this) {
    spell_presentation = std::make_unique<SpellPresentation>(
        animations, SpellPresentation::Banks{banks.stage, p.stage});
    age.set(0);
    callbacks[0].owner = callbacks[1].owner = this;
    callbacks[0].enabled = callbacks[1].enabled = true;
    callbacks[0].run = [](void* p) {
        return static_cast<RunScene*>(p)->update() ? i32(FrameAction::Continue)
                                                   : i32(FrameAction::Error);
    };
    callbacks[1].run = [](void* p) {
        return static_cast<RunScene*>(p)->draw() ? i32(FrameAction::Continue)
                                                 : i32(FrameAction::Error);
    };
    if (scheduler.add(callbacks[0], FramePass::Update, kRunUpdatePriority) < 0)
        fail("Run scene callback registration failed");
    if (scheduler.add(callbacks[1], FramePass::Draw, kRunTextPriority) < 0)
        fail("Run scene text callback registration failed");
    // 405530 registers405d60 at draw priority0x33; enqueue before ASCII's
    // screen-space draw78, independently from HUD visibility and text order.
    callbacks[2].owner = this;
    callbacks[2].enabled = true;
    callbacks[2].run = [](void* p) {
        auto& scene = *static_cast<RunScene*>(p);
        if (scene.hud.draw_trance_countdown(scene.host)) return i32(FrameAction::Continue);
        scene.fail(scene.hud.error);
        return i32(FrameAction::Error);
    };
    if (scheduler.add(callbacks[2], FramePass::Draw, 51) < 0)
        fail("Trance countdown callback registration failed");
    // 42fa30 registers laser update22/draw29. Birth ANM must be connected
    // before battle.load runs the stage root's constructor ECL.
    laser_presentation = std::make_unique<LaserPresentation>(manager, scene_view,
                                                            battle.sim, banks.bullet);
    battle.sim.defer_laser_update = true;
    battle.sim.laser_sound = [this](i32 id) { return host.hud_sound(id); };
    battle.sim.enemy_sound = [this](i32 id) { return host.hud_sound(id); };
    battle.sim.time_rate_changed = [this](float rate) { animations.rate = rate; };
    battle.sim.screen_shake = [this](i32 duration, i32 start, i32 end) {
        return start_shake(duration,start,end);
    };
    environment.stage_difficulty=p.difficulty;
    battle.sim.scene_effect=[this,banks](i32 kind,const Vec3& position){
        // 4bba80+6*18: effect bank0 -> effect.anm, script136, native401730.
        // 4bba80+1*18: kind1 script0 native401000；位置=敌机VM+0x44（op334 显式），
        // 经 translation 通道交 spawn_effect（415650 语义读后清零）。
        if(kind!=0&&kind!=1&&kind!=2&&kind!=3&&kind!=5&&kind!=6)return fail("Scene effect kind "+std::to_string(kind)+" not implemented");
        if(kind==0||kind==1||kind==2||kind==3||kind==5){
            const u32 handle=animations.create(banks.effect,0,-1,0,position);
            auto* vm=animations.registry.find(handle);
            return vm&&animations.spawn_effect(*vm,kind);
        }
        const u32 handle=animations.create(banks.effect,136,-1,0);
        auto* vm=animations.registry.find(handle);
        return vm&&animations.spawn_effect(*vm,kind);
    };
    battle.sim.enemy_hit_sound = [this](i32 id) { return host.hud_sound(id); };
    battle.sim.enemy_fog=[this](StageEnemy& enemy,bool reset){return presentation->enemy_fog(enemy,reset);};
    battle.sim.enemy_death_effect = [this](i32 file,i32 script,const Vec3& pos,float angle) {
        return presentation->enemy_death(file,script,pos,angle);
    };
    battle.sim.bullet_cancel_effect = [this](i32 script,const Vec3& pos,const Vec3& displacement) {
        return presentation->bullet_cancel(script,pos,displacement);
    };
    battle.sim.stationary_cancel_effect = [this](i32 script,const Vec3& pos) {
        return presentation->bullet_cancel(script,pos,{},false);
    };
    battle.sim.player_effect = [this](i32 script,const Vec3& pos,i32 layer) {
        const u32 h= presentation->player_effect(script,pos,layer);
        return h!=0;
    };
    battle.sim.enemy_animation = [this](StageEnemy& enemy, i32 slot) -> AnmVm* {
        // Mutations must see preceding303/306 births in this ECL turn.
        if (!presentation->flush_ecl_animations(battle, &enemy)) {
            battle.sim.error_ = presentation->error();
            return nullptr;
        }
        auto* vm = animations.registry.find(enemy.anm_slot_handle[slot]);
        if (!vm) enemy.anm_slot_handle[slot] = 0;
        return vm;
    };
    battle.sim.enemy_animation_interrupt=[this](StageEnemy& enemy,i32 slot,i32 label) {
        if(!presentation->flush_ecl_animations(battle,&enemy)) {
            battle.sim.error_=presentation->error();return false;
        }
        if(!animations.interrupt(enemy.anm_slot_handle[slot],label,false)) {
            battle.sim.error_=animations.error;return false;
        }
        return true;
    };
    laser_update.owner = this;
    laser_update.enabled = true;
    laser_update.run = [](void* owner) -> i32 {
        return static_cast<RunScene*>(owner)->update_lasers() ? 1 : 5;
    };
    if (scheduler.add(laser_update, FramePass::Update, 22) < 0)
        fail("Laser update registration failed");
    for (auto& callback : spell_callbacks) {
        callback.owner = this;
        callback.enabled = true;
    }
    spell_callbacks[0].run = [](void* owner) {
        return static_cast<RunScene*>(owner)->update_spell() ? 1 : 5;
    };
    spell_callbacks[1].run = [](void* owner) {
        return static_cast<RunScene*>(owner)->draw_spell() ? 1 : 5;
    };
    // 004129b0 registers 00413250 at Update 26 and 00413260 at Draw 10.
    if (scheduler.add(spell_callbacks[0], FramePass::Update, 26) < 0 ||
        scheduler.add(spell_callbacks[1], FramePass::Draw, 10) < 0)
        fail("Spell callback registration failed");
    scenery_update.owner = this;
    scenery_update.enabled = true;
    scenery_update.run = [](void* owner) -> i32 {
        return static_cast<RunScene*>(owner)->update_scenery() ? 1 : 5;
    };
    if (scheduler.add(scenery_update, FramePass::Update, 13) < 0)
        fail("Scenery update registration failed");
    for (u32 index = 0; index < scenery_draws.size(); index++) {
        auto& draw = scenery_draws[index];
        draw.owner = this;
        draw.pass = index;
        draw.callback.owner = &draw;
        draw.callback.enabled = true;
        draw.callback.run = [](void* p) -> i32 {
            auto* draw = static_cast<StageDraw*>(p);
            return draw->owner->draw_scenery(draw->pass) ? i32(FrameAction::Continue)
                                                         : i32(FrameAction::Error);
        };
        if (scheduler.add(draw.callback, FramePass::Draw, kSceneryDrawPriorities[index]) < 0)
            fail("Scenery draw callback registration failed");
    }
}

RunScene::~RunScene() {
    battle.sim.time_rate_changed = {};
    battle.sim.screen_shake = {};
    battle.sim.stage_fog = {};
    battle.sim.stage_script_interrupt = {};
    for (auto& shake : shakes) scheduler.remove(shake->callback);
    battle.sim.enemy_fog={};
    battle.sim.enemy_sound = {};battle.sim.enemy_hit_sound = {};battle.sim.enemy_death_effect = {};battle.sim.player_effect = {};
    battle.sim.bullet_animation = {};
    battle.sim.scene_effect = {};
    battle.sim.enemy_animation = {};
    battle.sim.enemy_animation_interrupt = {};
    scheduler.remove(laser_update);
    scheduler.remove(scenery_update);
    laser_presentation.reset(); // detach callbacks while battle.sim is alive
    battle.sim.laser_sound = {};
    for (auto& callback : callbacks) scheduler.remove(callback);
    for (auto& callback : spell_callbacks) scheduler.remove(callback);
    if (spell_presentation) spell_presentation->release();
    for (auto& draw : stage_draws) scheduler.remove(draw.callback);
    for (auto& draw : scenery_draws) scheduler.remove(draw.callback);
    host.dialogue_end();  // 0x4282b0 四文本 VM 随对话对象销毁
    if (msg_presentation) msg_presentation->release_all();  // 对话立绘/名牌 VM 归还
    if (presentation) presentation->release();
}

bool RunScene::fail(const std::string& why) {
    if (error.empty()) error = why.empty() ? "Run scene unavailable" : why;
    return false;
}

bool RunScene::initialize(const HudEntryContext& hud_context) {
    // 42c550's stage activation starts with42ad40. This only finishes the
    // clear presentation; loader preparation itself must retain its clock.
    if (!hud.finish_stage_clear()) return fail(hud.error);
    if (!error.empty()) return false;
    battle.sim.bullet_animation = [this](StageBullet& bullet, bool birth) {
        if (presentation->tick_bullet(bullet, birth)) return true;
        battle.sim.error_ = presentation->error();
        return false;
    };
    battle.item_manager.spirit_animation_alive = [this](u32 slot, u32 serial) {
        return presentation->spirit_animation_alive(slot, serial);
    };
    battle.sim.spell_event = [this](const SpellEvent& event, const StageEnemy& enemy) {
        // Consume each ECL event immediately; begin/end in one tick cannot be lost.
        if (event.kind == SpellEventKind::Begin) hud.boss_hud.spell_started();
        if (!host.record_spell_event(event)) return fail("Spell record persistence failed");
        if (!spell_presentation->consume(event, enemy.position,
                [this](i32 id) { return host.hud_sound(id); }))
            return fail(spell_presentation->error);
        if (event.kind == SpellEventKind::Begin &&
            !host.rasterize_spell_name(*spell_presentation))
            return fail("Spell name rasterization failed");
        return true;
    };
    // 战斗装载：ECL 交互模式 + 首帧（root 创建趟）。难度位 = 1 << 难度序号
    // （rank 掩码 1111LHNE；契约默认 0x08 = Lunatic）。
    RunSceneAssets provider;
    provider.host = &host;
    provider.sim = &battle.sim;
    provider.anim_list = &battle.sim.anim_list;
    // 关卡 ECL 名（stage 表 0x4bb4f0 行 +0x8；换关 0xc 后 Application 已按
    // DAT_004be81c 装载 st0N.ecl/std，此处取同表同款——非首关不再误装 st01）。
    char stage_ecl[16];
    std::snprintf(stage_ecl, sizeof stage_ecl, "st%02d.ecl", int(progress.stage));
    const u8 rank_mask = ecl_difficulty_mask(progress.difficulty);
    if (!rank_mask) return fail("Invalid ECL difficulty");
    if (!battle.load(provider, stage_ecl, rank_mask, host, progress,
                     player_state, score_state, nullptr,
                     battle_sht.empty() ? nullptr : battle_sht.c_str()))
        return fail(battle.error().empty() ? "Stage battle load failed" : battle.error());
    // 敌机句柄化默认路径（2026-10-23 基座治理整改）：ECL 303/306 建池 VM
    // 入 registry（0x41fe80/case3 同构——oracle 极小环境路径），表现层手工
    // 敌机段跳过（enemies_from_ecl），敌机销毁回收由 retire_sink 钩承担。
    // 逃生开关：sim.ecl_slot_anm=false 回退双绘制旧路径。
    battle.sim.ecl_slot_anm = true;
    presentation->enemies_from_ecl = true;
    battle.sim.retire_sink = [this](u32 serial) {
        presentation->retire_enemy_fog(serial);
        // 0x418e00 销毁支路的 ANM 面：回收该敌机全部槽位 VM
        //（0x470110 语义：retire 树 + 句柄归零）。敌机此刻仍在 sim.enemies
        //（erase 前调用），按 serial 查槽位句柄。
        for (const auto& enemy : battle.sim.enemies)
            if (enemy->serial == serial) {
                for (u32& handle : enemy->anm_slot_handle)
                    if (handle && !animations.retire(handle))
                        fail("Enemy retire animation: " + animations.error);
                return;
            }
    };
    // STD 舞台背景装载：stXX.std 解析 → 引用 ANM（st01wl.anm）入 bank → 图元
    // VM 池绑定（模板复制 + 首 tick）。
    std::vector<u8> bytes;
    if (!host.read_asset(scenery_std, bytes)) return fail("Stage STD missing: " + scenery_std);
    if (!scenery) scenery = std::make_unique<StageScenery>();
    if (!scenery->ready()) {
    if (!scenery->file.open(bytes.data(), u32(bytes.size())))
        return fail("Stage STD invalid: " + scenery->file.error);
    if (!host.preload_scenery_animation(scenery_bank, scenery->file.animation_name))
        return fail("Stage scenery ANM unavailable: " + scenery->file.animation_name);
    if (!scenery->initialize(animations, scenery_bank))
        return fail(scenery->error.empty() ? "Stage scenery init failed" : scenery->error);
    }
    battle.sim.stage_fog = [this](i32 duration, i32 mode, u32 color,
                                   float near_distance, float far_distance) {
        auto& state = scenery->script.state;
        state.fog.start = state.camera.fog;
        state.fog.end.set(color, near_distance, far_distance);
        state.fog.duration = duration;
        state.fog.mode = mode;
        state.fog.timer.set(0);
    };
    // ECL 630 → 0x409100：STD 相机脚本 ins_16 标号中断（游标跳标号后首
    // 指令、计时三槽设为目标 time——StageScript::interrupt）。
    battle.sim.stage_script_interrupt = [this](i32 id) {
        return scenery->script.interrupt(id);
    };
    for (u32 index = 0; index < stage_draws.size(); index++) {
        auto& draw = stage_draws[index];
        draw.owner = this;
        draw.pass = index;
        draw.callback.owner = &draw;
        draw.callback.enabled = true;
        draw.callback.run = [](void* p) -> i32 {
            auto* draw = static_cast<StageDraw*>(p);
            return draw->owner->draw_stage(draw->pass) ? i32(FrameAction::Continue)
                                                       : i32(FrameAction::Error);
        };
        if (scheduler.add(draw.callback, FramePass::Draw, kStageDrawPriorities[index]) < 0)
            return fail("Stage draw callback registration failed");
    }
    // S3：对话资产（st0Na-d.msg 按角色四份——stage 表 +0x14..+0x20 以
    // be7bc+be7b8 选份，0x424fd0:27）。缺失时降级为对话缺席（519 即过、
    // 事件契约行为不变）；浏览器侧资产清单补 st0N[a-d].msg 属接线 TODO。
    char msg_name[16];
    std::snprintf(msg_name, sizeof msg_name, "st%02d%c.msg", int(progress.stage),
                  'a' + (progress.character & 3));
    std::vector<u8> msg_bytes;
    if (host.read_asset(msg_name, msg_bytes, false)) {
        std::string msg_error;
        if (dialog_file.open(msg_bytes.data(), u32(msg_bytes.size()), msg_error)) {
            dialog_ready = true;
            battle.sim.dialog_host = this;  // ins_518/519 接口位
        } else {
            return fail("Stage msg invalid: " + std::string(msg_name) + ": " + msg_error);
        }
    }
    // 对话立绘 bank/脚本绑定（stage 表 0x4bb4f0 行 + ECL ANIM 名单全局号换算）：
    // 只在 msg 资产存在时绑定；失败即场景错误（不静默降级成无立绘对话）。
    if (dialog_ready && !msg_presentation->bind_stage_scripts(
                            int(progress.stage), battle.sim.anim_list,
                            [this](const std::string& name, std::vector<u8>& bytes) {
                                return host.read_asset(name, bytes);
                            }))
        return fail("Dialog presentation bind: " + msg_presentation->error);
    hud.prepare_stage_assets();
    if (!hud.initialize(hud_context)) return fail(hud.error);
    return true;
}

bool RunScene::update_spell() {
    if (transition_wait) return true;
    if (!error.empty()) return false;
    if (environment.paused || (progress.scene_flags & 0x70u)) return true;
    if (!battle.sim.spell.step(battle.bomb_timer > 0, &progress.rate))
        return fail(battle.sim.spell.error);
    battle.sim.spell.update_visibility(battle.player_y);
    const auto* boss = battle.sim.boss();
    spell_presentation->sync(battle.sim.spell.state(), boss ? boss->position : nullptr);
    return true;
}

bool RunScene::update_lasers() {
    if (transition_wait) return true;
    if (!error.empty()) return false;
    if (environment.paused || (progress.scene_flags & 0x70u)) return true;
    battle.sim.advance_lasers();
    if (!battle.sim.error().empty())
        return fail(laser_presentation->error.empty() ? battle.sim.error()
                                                     : laser_presentation->error);
    return true;
}

bool RunScene::draw_spell() {
    if (!error.empty()) return false;
    i32 captures = 0, attempts = 0;
    host.spell_counts(battle.sim.spell.state().id, captures, attempts);
    if (!spell_presentation->draw(host, captures, attempts))
        return fail(spell_presentation->error);
    if (trance_active(progress)) battle.sim.spell.trance_active(battle.bomb_timer > 0);
    spell_presentation->sync(battle.sim.spell.state());
    return true;
}

bool RunScene::start_shake(i32 duration, i32 start, i32 end) {
    auto shake = std::make_unique<ShakeCallback>(*this,duration,start,end);
    auto& callback = shake->callback;
    callback.owner = shake.get();
    callback.enabled = true;
    callback.run = [](void* owner) -> i32 {
        auto& shake = *static_cast<ShakeCallback*>(owner);
        auto& scene = shake.scene;
        // 42c550 returns3 before Update75 during pause/overlays. The port
        // keeps UI callbacks running, so apply that gate to this effect.
        if (scene.environment.paused || (scene.progress.scene_flags & 0x70u)) return 1;
        return shake.effect.advance(scene.animations.random_source(),scene.progress.rate,
                                    scene.environment.screen_shake_offset) ? 1 : 7;
    };
    callback.cleanup = [](void* owner) -> i32 {
        auto& shake = *static_cast<ShakeCallback*>(owner);
        shake.scene.scheduler.remove(shake.callback);
        return 0;
    };
    if (scheduler.add(callback,FramePass::Update,75) < 0) return false;
    shakes.push_back(std::move(shake));
    return true;
}

bool RunScene::update() {
    shakes.erase(std::remove_if(shakes.begin(),shakes.end(),[this](const auto& shake) {
        return !scheduler.contains(shake->callback);
    }),shakes.end());
    if (!error.empty()) return false;
    // 42c550 ->42a370 precedes the player/enemy/item managers. HUD score
    // convergence consumes the previous update's score, including while the
    // 10/20/40 overlay flags freeze gameplay. Bit4 is the loader gate.
    if (!(progress.scene_flags & 4u)) hud.update_score();
    if (environment.paused) return true;  // 暂停覆盖层冻结对局（th15 environment.paused 同机制）
    // 对局内覆盖层三旗冻结（0x42c550:263：GameInf+0x60 的 0x10/0x20/0x40 任一
    // 非零 → 对局 update 返 3 不推帧；0x4000 通关演出不在门内照常推帧）。
    if (progress.scene_flags & 0x70u) return true;
    const GameInput* effective_input = input_frame;
    if (playback_input) {
        effective_input = playback_input();
        if (!effective_input) return true;
    }
    static const GameInput no_input{};
    const GameInput& selected_input = effective_input ? *effective_input : no_input;
    if (record_input) record_input(selected_input);
    if (transition_wait) {
        if (!battle.step_transition(selected_input)) return fail(battle.error());
        if (!presentation->sync(battle, selected_input.held)) return fail(presentation->error());
        hud.tick_stage_clear();
        HudFrameContext frame;
        // 0x4265a0 的灵界槽左下淡化在过渡帧也必须运行；玩家位置仍是
        // DAT_004c22c4+0x5b8/+0x5bc 的游戏坐标。
        const Vec3 player_position{battle.player_x, battle.player_y, 0};
        frame.player_position = &player_position;
        hud.update(frame, host);
        return hud.error.empty() || fail(hud.error);
    }
    progress.stage_frame++;
    age.tick(&progress.rate);
    // One effective gameplay snapshot: live controls or the recorded input
    // selected above (448200 -> 414f30). Recorded previous/edge history is
    // preserved while physical input operates the pause menu. EOF consumes
    // no gameplay frame. Direct tests without controls use zero input.
    static const GameInput kNoInput{};
    const GameInput& input = effective_input ? *effective_input : kNoInput;
    if (!battle.step(input)) return fail(battle.error());
    // 4265a0 advances HUD+184 before it calls the dialogue interpreter.
    // MSG21 may create the clear event below: its birth frame stays at0.
    // ECL's direct -2 clear, on the other hand, already happened above.
    hud.tick_stage_clear();
    // 对话解释器（0x4265a0 每帧驱动：ECL 步进后同帧推进；Z 边沿 = bit0）。
    // 推进键面（0x428ae0:122-129）：Z 持续位 0x1、Ctrl 0x200（快进）、Enter
    // 0x80000 与 opcode 11 放行共用；持续帧数 = GameInput.duration（0x42b3f0
    // 的 0x4e49f0+0x194+key*4 同构）。
    if (dialog.active()) {
        MsgAdvance advance;
        advance.z_pressed = (input.pressed & 0x01u) != 0;
        advance.held = input.held;
        advance.pressed = input.pressed;
        advance.z_frames = input.duration[0];
        advance.ctrl_frames = input.duration[9];
        dialog.step(advance);
    }
    // 原版 0x428ae0 在 update 内把 opcode 17 正文写进 text.anm 的四条 VM/
    // 动态纹理；提交必须早于 AnmDrawSchedule 的层 26（Draw 优先级 59）。
    if (dialog.active()) {
        const auto& state = dialog.state();
        const u32 rows = u32(std::min<size_t>(state.lines_meta.size(), 2));
        for (u32 row = 0; row < rows; row++) {
            const MsgLine& line = state.lines_meta[row];
            if (!host.dialogue_line(i32(row), line, state.box_x, state.box_y))
                return fail("Dialogue line " + std::to_string(row) + " submission failed");
        }
        if (!rows && dialogue_rows_ != 0) host.dialogue_clear();
        dialogue_rows_ = i32(rows);
    } else if (dialogue_rows_ != 0) {
        host.dialogue_clear();
        dialogue_rows_ = 0;
    }

    if (!presentation->sync(battle, input.held)) return fail(presentation->error());
    // Boss HUD 每帧视图装配（HudFrameContext.boss；0x4265a0 驱动面，RunScene
    // 装配契约）：可证字段接实值，未锚定缺口保持默认并登记，不伪造。
    BossFrameInput boss_view;
    boss_view.boss = battle.sim.boss();           // mgr+0x5c 槽 0
    boss_view.dialog_active = dialog.active();    // ctx+0x198 对话进行中
    boss_view.player_x = battle.player_x;         // DAT_004c22c4+0x5b8 世界位
    boss_view.player_y = battle.player_y;         // +0x5bc
    boss_view.boss_life =
        boss_view.boss ? i32(boss_view.boss->life) : 0;  // +0x5138 int 口径
    boss_view.trance_mode = battle.sim.spell.state().active(); // 4275xx: spell flags bit0
    boss_view.boss_leaving=battle.sim.boss_leaving;
    boss_view.life_initial=boss_view.boss?boss_view.boss->spell_base_life:0;
    boss_view.timer_seconds=battle.sim.boss_timer_seconds;
    boss_view.timer_centiseconds=battle.sim.boss_timer_centiseconds;
    boss_view.remaining_stars=battle.sim.boss_remaining_stars;
    constexpr i32 name_rows[7][2]={{-1,0},{-1,1},{2,3},{-1,4},{5,6},{-1,7},{8,9}};
    if (progress.stage >= 1 && progress.stage <= 7) {
        const i32 name = name_rows[progress.stage-1][progress.chapter >= 5 ? 1 : 0];
        boss_view.name_script = name >= 0 ? 155 + name : -1;
    }
    for(size_t i=0;i<4;++i) {
        boss_view.markers[i].ratio=battle.sim.boss_markers[0][i].ratio;
        boss_view.markers[i].frame=battle.sim.boss_markers[0][i].color;
    }
    if(!boss_view.boss)battle.sim.boss_markers[0]={}; // 4265a0 clears vacant slot ratios
    HudFrameContext frame;
    // 0x4265a0:85-96 灵界槽左下淡化/恢复需要玩家游戏坐标。
    const Vec3 player_position{battle.player_x, battle.player_y, 0};
    frame.player_position = &player_position;
    frame.dialogue = boss_view.dialog_active;
    frame.boss = &boss_view; // a vacant boss slot must also retire its HUD
    hud.update(frame, host);
    // 覆盖层/通关演出推进（0x43dbc0 行为子集 + GameInf 0x4000 路径）。
    post_flow.update();
    if (!post_flow.error.empty()) return fail(post_flow.error);
    return hud.error.empty() || fail(hud.error);
}

void RunScene::open_dialog(i32 entry) {
    // 00428820 -2 ends Spell Practice; timeout uses the game-over branch.
    if (entry == -2) {
        if (battle.sim.spell.state().flags & kSpellTimedOut) {
            post_flow.on_game_over();
            if (!host.request_game_over()) fail("Spell Practice game-over request failed");
        } else {
            msg_stage_clear();
        }
        return;
    }
    if (entry == -1 || entry == -3) {
        if (!host.queue_music(2, entry == -1 ? 1 : 0, ""))
            fail("ECL music queue failed");
        return;
    }
    // 0x428820 else 分支：已有对话先销毁（+0x198 换手），再开新 entry。
    if (!dialog_ready) return;
    host.dialogue_end();  // 旧对话对象的四个文本 VM（0x4282b0）先回收
    dialogue_rows_ = 0;
    if (msg_presentation) msg_presentation->release_all();
    std::string dialog_error;
    if (!dialog.open(dialog_file, u32(entry), *this, dialog_error))
        fail("Dialog entry " + std::to_string(entry) + ": " + dialog_error);
    else {
        // 4282b0's constructor cancels projectiles and clears enemies before
        // 428820 ends Trance. ECL518 repeats its own clear after returning;
        // marked enemies remain linked and consume their death RNG twice.
        battle.sim.clear_bullets();
        battle.sim.cancel_lasers(false);
        if (!battle.sim.clear_enemies()) fail(battle.sim.error());
        else if (!battle.end_trance()) fail(battle.error());
    }
}

void RunScene::msg_bgm(i32 opcode) {
    // 0x428ae0 jump table（0x429c7c）逐 handler 实证分派——旧「BGM 族」注释
    // 仅对 19 成立（p58-dialogue 纠正）：
    //   2（0x429335）右立绘：entry<2 或无 Boss 对 = 中Boss 对（stage 表
    //     +0x6c/+0x70），否则 Boss 对（+0x64/+0x68）；
    //   19（0x4299df → 0x43ae40(1, stage+0x2c)）Boss 曲：播关首预载槽 1
    //     （0x42b990:215 载入；Application.begin_run 侧预载）；
    //   20（0x429a94）名牌 VM：stage 表 +0x54/+0x58 缺席用 +0x5c/+0x60；
    //   31（0x42948c）右立绘 Boss 对无条件。
    if (!msg_presentation) return;
    switch (opcode) {
    case 2:
        if (!msg_presentation->side_portrait(dialog.state().entry))
            fail("Dialog side portrait: " + msg_presentation->error);
        break;
    case 19:
        if (!host.queue_music(2, 1, "")) fail("Boss music queue failed");
        break;
    case 20:
        if (!msg_presentation->nameplate())
            fail("Dialog nameplate: " + msg_presentation->error);
        break;
    case 31:
        if (!msg_presentation->boss_side_portrait())
            fail("Dialog boss portrait: " + msg_presentation->error);
        break;
    default:
        break;
    }
}

void RunScene::msg_portrait_load() {
    // opcode 1（0x429285）：自机立绘根 VM——玩家 plXX bank 脚本表
    // 0x4c0aa0[be7b8]（EXE 实读 = 29,34,30,28）。
    if (msg_presentation && !msg_presentation->player_portrait(progress.character & 3))
        fail("Dialog player portrait: " + msg_presentation->error);
}

void RunScene::msg_text_anchor_y(float value) {
    if (!host.dialogue_anchor_y(value)) fail("Dialogue text anchor update failed");
}

void RunScene::msg_stage_clear() {
    // opcode 21 → 0x42cd40 关终了（Boss 后对话 entry 尾的通关触发链入口）。
    if (!host.record_stage_clear()) {
        fail("Stage clear record persistence failed");
        return;
    }
    if ((progress.run_mode_flags & 0x30u) == 0x20u && progress.practice_mode == 0 &&
        !host.record_spell_practice_score(score_state.score)) {
        fail("Spell Practice score persistence failed");
        return;
    }
    if ((progress.run_mode_flags & 0x30u) != 0x20u && !hud.begin_stage_clear()) {
        fail(hud.error);
        return;
    }
    post_flow.on_stage_clear();
    if (!post_flow.error.empty()) fail(post_flow.error);
}

// ——追加可视化通知钩子（0x428ae0 逐 handler 证据；见 MsgPresentation.hpp）——

void RunScene::msg_portrait_interrupt(i32 opcode, i32 label) {
    // 13/14（0x4298d4/0x4298e8）：label = arg+0x11 的 0x46fca0 立即中断——
    // 表情切换；13 打左（自机立绘）、14 打右（Boss 立绘）。
    if (!msg_presentation) return;
    const bool ok = opcode == 0x0d ? msg_presentation->interrupt_player(label)
                                   : msg_presentation->interrupt_side(label);
    if (!ok) fail("Dialog portrait interrupt: " + msg_presentation->error);
}

void RunScene::msg_speaker_switch(i32 opcode) {
    // 7（0x429641）：左 label2 亮、右 label3 暗；8（0x4296ee）：左 3、右 2。
    if (!msg_presentation) return;
    const bool ok = opcode == 0x08 ? msg_presentation->interrupt_player(3) &&
                                         msg_presentation->interrupt_side(2)
                                   : msg_presentation->interrupt_side(3) &&
                                         msg_presentation->interrupt_player(2);
    if (!ok) fail("Dialog speaker switch: " + msg_presentation->error);
}

void RunScene::msg_release(i32 opcode) {
    // 4/23（0x4295cf/0x4295a9）左槽；5/24（0x4295e9/0x4295bc）右槽含名牌；
    // 6/18（0x42960c/0x429250）名牌 + 文本区（port 单名牌槽 + dialog 行块）。
    if (!msg_presentation) return;
    switch (opcode) {
    case 0x04:
    case 0x17:
        msg_presentation->release_player();
        break;
    case 0x05:
    case 0x18:
        msg_presentation->release_side();
        break;
    case 0x06:
        msg_presentation->release_nameplates();
        // 0x42960c：文本 VM label1（8 帧 alpha 淡出后 op1 回收），
        // 0x46fc30 不清 texture0 像素。
        if (!host.dialogue_fade(1)) fail("Dialog text fade (opcode 6) failed");
        break;
    case 0x12:
        msg_presentation->release_nameplates();
        // 0x429250：文本 VM label3（3 帧 alpha 淡出后循环保留 VM/像素）。
        if (!host.dialogue_fade(3)) fail("Dialog text fade (opcode 18) failed");
        break;
    default:
        break;
    }
    if (!msg_presentation->error.empty())
        fail("Dialog release: " + msg_presentation->error);
}

void RunScene::msg_music_fade(i32 opcode, float seconds) {
    // 429bf5: Stage6 uses4aed90=2, other stages4aea74=8;27 reads its f32.
    const i32 duration = opcode == 22 ? (progress.stage == 6 ? 2 : 8) : truncate_int(seconds);
    if (!host.queue_music(5, duration, "")) fail("Dialog music fade queue failed");
}

void RunScene::msg_advance_blip() {
    // opcode 11 计声放行（0x460bc0(ctx,0)）：原版 voice 0 提示音。
    if (!host.hud_sound(0)) fail("Dialog advance sound failed");
}

void RunScene::msg_dialog_end() {
    // entry 终止（0x4265a0 当帧销毁）：全部对话 VM 释放（原版对话对象销毁
    // 连带 +0x40/+0x44/+0x5c 槽位回收）。
    host.dialogue_end();
    dialogue_rows_ = 0;
    if (msg_presentation) {
        msg_presentation->release_all();
        if (!msg_presentation->error.empty())
            fail("Dialog end release: " + msg_presentation->error);
    }
}

bool RunScene::draw_stage(u32 pass) {
    if (transition_wait && pass >= 2) return true;
    if (!error.empty()) return false;
    // Original game sprites are centered in the offscreen @R surface. The
    // supervisor's text84 pass later samples128..512 to screen32..416.
    // Do not translate either these vertices or their full-screen camera.
    if (!host.camera(DrawCamera::Fullscreen, false)) return fail("Stage camera unavailable");
    if (pass == 4)
        return laser_presentation->draw() || fail(laser_presentation->error);
    if (!presentation->draw(pass)) return fail(presentation->error());
    return true;
}

bool RunScene::prepare_transition(i32 bank, const std::string& name) {
    transition_wait = true;
    scenery_started = false;
    // 414180 destroys the linked enemy list without death routines/drops.
    for (auto& enemy : battle.sim.enemies)
        if (battle.sim.retire_sink) battle.sim.retire_sink(enemy->serial);
    battle.sim.enemies.clear();
    battle.item_manager.reset_items();
    presentation->release_stage();
    previous_scenery = std::move(scenery);
    previous_scenery_bank = scenery_bank;
    scenery_bank = bank;
    scenery_std = name;
    scenery = std::make_unique<StageScenery>();
    std::vector<u8> bytes;
    if (!host.read_asset(name, bytes) || !scenery->file.open(bytes.data(), u32(bytes.size())))
        return fail("Next stage STD load failed");
    if (!host.preload_scenery_animation(bank, scenery->file.animation_name) ||
        !scenery->initialize(animations, bank)) return fail("Next stage scenery load failed");
    return true;
}

bool RunScene::start_transition() {
    if (!hud.finish_stage_clear()) return fail(hud.error);
    if (previous_scenery) previous_scenery->begin_exit();
    if (scenery) scenery->begin_enter();
    scenery_started = true;
    return host.open_game_fade(false, 30, true) || fail("Stage fade-out unavailable");
}

void RunScene::preserve_presentation_from(RunScene& old) {
    presentation = std::move(old.presentation);
    scenery = std::move(old.scenery);
    previous_scenery = std::move(old.previous_scenery);
    previous_scenery_bank = old.previous_scenery_bank;
    scenery_bank = old.scenery_bank;
}

bool RunScene::update_scenery() {
    if (environment.paused || (progress.scene_flags & 0x70u)) return true;
    if (previous_scenery && previous_scenery->retired()) {
        previous_scenery.reset();
        animations.unload(previous_scenery_bank);
        previous_scenery_bank = -1;
    }
    if (scenery_started && scenery && !scenery->update(animations, environment, progress.rate))
        return fail(scenery->error);
    if (previous_scenery && !previous_scenery->update(animations, environment, progress.rate))
        return fail(previous_scenery->error);
    return true;
}

bool RunScene::draw_scenery(u32 pass) {
    if (!error.empty()) return false;
    auto& graphics = host.stage_graphics();
    auto& renderer = scene_renderer;
    const GraphicsViewport viewport{116, 2, 408, 476, 0, 1};
    // Equal-priority callbacks insert newest first: new2, old2, new5, old5.
    for (auto* scene : {scenery_started ? scenery.get() : nullptr, previous_scenery.get()}) {
        if (!scene || !scene->ready() || scene->retired()) continue;
        if (pass == 0 && scene->draw_camera_enabled()) {
            renderer.invalidate();
            graphics.pipeline().fogMode = touhou::graphics::FogMode::Linear;
            graphics.set_depth_test(true);
            graphics.set_depth_compare(touhou::graphics::Compare::LessEqual);
            graphics.set_depth_mask(true);
            if (!graphics.clear_target(scene->draw_clear_color(), &viewport))
                return fail("Scenery clear failed");
        }
        if (pass == 0) scene->finish_draw_pass(2);
        const bool saved_tint_enabled = renderer.tint_enabled;
        const u32 saved_tint = renderer.tint;
        if (pass == 0) {
            renderer.tint = scene->take_draw_tint();
            renderer.tint_enabled = (renderer.tint >> 24) != 0;
        }
        if (!scene->draw(animations, renderer, graphics, pass == 0 ? 0 : 8,
                         pass == 0 ? 7 : 11, viewport,
                         {float(viewport.width), float(viewport.height)}, environment.screen_shake_offset))
            return fail(scene->error);
        renderer.flush();
        renderer.tint_enabled = saved_tint_enabled;
        renderer.tint = saved_tint;
        if (pass == 1) scene->finish_draw_pass(5);
        if (scene->fade_in_requested()) {
            scene->clear_fade_in_request();
            if (!host.open_game_fade(true, 30, true)) return fail("Stage fade-in unavailable");
        }
    }
    if (pass == 1) {
        renderer.invalidate();
        graphics.set_fog(false);
        graphics.set_depth_compare(touhou::graphics::Compare::Always);
        graphics.set_depth_mask(false);
        if (!host.camera(DrawCamera::Fullscreen, false)) return fail("Scenery camera restoration failed");
    }
    return true;
}

bool RunScene::draw() {
    if (!error.empty()) return false;
    hud.draw(HudDrawContext{}, host);
    // 对话正文已在 update 内写入 text.anm texture0；四条文本 VM 由
    // AnmDrawSchedule 层 26（Draw 优先级 59）生产绘制，故 draw 不补画文本。
    return hud.error.empty() || fail(hud.error);
}

}  // namespace th13
