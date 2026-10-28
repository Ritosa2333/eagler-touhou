// WASI 无头 harness：同一份 game 源码在 wasm32-wasip1 下独立运行，
// 供 CI 中的 oracle 对照（原版 Unicorn 轨迹 vs C++ 行为）调用。
// 编译方式见 portable/build-harness.mjs（WASI_SDK_PATH 需指向 wasi-sdk）。
// 用法：
//   th13-harness              旧有 RNG 冒烟（8 值输出）
//   th13-harness stage1 <资源目录> <事件输出路径> [帧数] [stage.ecl]
//     资源目录需含 st01.ecl 及其 ECLI/ANIM 依赖（artifacts/reference/raw）。
//   th13-harness replay <rpy 路径> <资源目录> <事件输出路径> [帧数] [stage 索引]
//     同源默认（r4/harness）：解码原版录像，走与网页一致的 RunScene 生产
//     tick（输入单实例 0x448200 相位 → RunScene::controls → scheduler.update
//     单调；bank 装载序/RNG 双流镜像 ApplicationState），事件经
//     StageBattle::open_event_sink 落 player/kill/boss_op 等，窗口自定
//     （Boss 第一非符 +300 帧或输入耗尽）。资源目录另需 plXXa.sht（§7.2）。
//     TH13_STATE_LOG=<guest 路径> 时另产对局状态流（th13-state-raw-v1，
//     逐消费帧采样=battle_oracle.py:1086 契约；字段契约见
//     oracle/state_compare.py；帧相位 -1 = 装载完成后首帧步进前）。
//     诊断开关：TH13_FREEZE_AT=<帧>（冻结门自证）、TH13_NO_ECL_SLOT_ANM=1
//     （ANM sync 负控）——不改评分字段。
//   th13-harness draws <资源目录> <绘制输出路径> [帧数=3600] [stage.ecl]
//   th13-harness draws-replay <rpy> <资源目录> <绘制输出路径> [帧数] [stage 索引]
//     语义绘制清单导出（draw-contract.md v1 移植侧，TH13_DRAW_CAPTURE 构建）：
//     无输入窗 3600 帧 / 回放窗（与 oracle/render_oracle.py 同窗同帧号）。
//     帧序镜像 web 调度：副池步进 → ECL/battle → 表现层 sync → 主池步进 →
//     逐层（0..32）+ 战斗直绘 4 pass（debug_pass_mask=4 仅敌机，对齐 oracle
//     窗口径：mgr 两池精灵；HUD/STD/自机不在窗）。帧 0 = 装载趟渲染（不步进）。
//   th13-harness title <资源目录> <绘制输出路径>     （窗A：主菜单→难度，600 帧）
//   th13-harness title-b <资源目录> <绘制输出路径>   （窗B：难度→角色→开局转场）
//     标题窗语义绘制导出（oracle/title_oracle.py 同窗同规则）：帧序镜像 web
//     Application 启动序——装载幕（prepare_loading 首渲染 = 帧 0，boot 180 帧
//     推进 = 帧 1..180）→ TitleScene 步（= 帧 181 起，帧内序 controls →
//     scheduler.update（标题@6/副池@9/主池@34）→ scheduler.draw（AnmDrawSchedule
//     31 层 + 标题垫片@66））。全局帧概念与 oracle 0x45d2f0 帧循环 1:1 对齐
//     （oracle f0=开场创建当帧 ops=0 ↔ port 帧 0=装载幕首渲染）。输入按冻结
//     时刻表注入（controls u32 单帧：Z=0x01/↑=0x10/↓=0x20/←=0x40/→=0x80；
//     窗A f380↓/f420↑/f470Z；窗B f470Z/f530↓/f590Z/f650→/f710Z）。导出后按
//     oracle cut_window 同规则截窗：首个 ops>0 帧重编号 0 起；窗A 固定 600 帧，
//     窗B 止于开局转场请求帧（start_game → Destination::Run，含该帧）。
#include <cstdlib>
#include <cstring>

#include <algorithm>
#include <cstdio>
#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "game/AnmDrawSchedule.hpp"
#include "game/AnmManager.hpp"
#include "game/AnmRenderer.hpp"
#include "game/AsciiFrame.hpp"
#include "game/AsciiText.hpp"
#include "game/DrawCapture.hpp"
#include "game/EclResource.hpp"
#include "game/GameInput.hpp"
#include "game/PlayfieldTargets.hpp"
#include "game/ReplayFile.hpp"
#include "game/ReplaySession.hpp"
#include "game/RunScene.hpp"
#include "game/Rng.hpp"
#include "game/ScreenViews.hpp"
#include "game/StageBattle.hpp"
#include "game/StagePresentation.hpp"
#include "game/StageSim.hpp"
#include "game/TitleScene.hpp"
#include "game/TitleFrame.hpp"

namespace {

using th13::i32;
using th13::u32;

int rng_smoke() {
    th13::Rng rng(1);  // MSVC CRT 默认 srand(1)
    std::printf("th13-harness rng");
    for (int i = 0; i < 8; i++) {
        std::printf(" %d", rng.next());
    }
    std::printf("\n");
    return 0;
}

int stage1(const char* resource_dir, const char* event_path, unsigned frames,
           const char* stage) {
    th13::StageSim sim;
    sim.anm_effects_noop = true;  // 事件契约：307/308 与 stage1 oracle NOOP 同源
    if (!sim.load(resource_dir, event_path, stage)) {
        std::printf("stage1 load failed: %s\n", sim.error().c_str());
        return 1;
    }
    // TH13_LIFE_PROBE=1：stage1 路径同款生命轨迹探针（与 StageBattle::step /
    // oracle stage1_oracle 同格式——draws 循环走 sim.step_frame 不经 battle，
    // 无输入窗对拍需在此出探针）。
    bool ok;
    if (std::getenv("TH13_LIFE_PROBE")) {
        ok = sim.start();
        for (unsigned f = 1; ok && f < frames && sim.error().empty(); ++f) {
            sim.sink.set_frame(f);
            if (!sim.step_frame()) break;
            for (const auto& enemy : sim.enemies)
                std::printf("LIFE f=%u e=%d life=%d pos=(%.9g,%.9g) hb=(%.9g,%.9g) "
                            "flags=%08x\n",
                            f, int(enemy->serial), int(enemy->life),
                            double(enemy->position[0]), double(enemy->position[1]),
                            double(enemy->hurtbox[0]), double(enemy->hurtbox[1]),
                            enemy->flags);
        }
        sim.sink.close();
        ok = ok && sim.error().empty() && sim.sink.ok();
    } else {
        ok = sim.run(frames);
    }
    std::printf("stage1 frames=%u enemies=%u events=%llu ok=%d\n", frames, sim.spawn_count(),
                static_cast<unsigned long long>(sim.event_count()), ok ? 1 : 0);
    if (!ok) {
        std::printf("stage1 error: %s\n", sim.error().empty() ? "event sink failure" : sim.error().c_str());
        return 1;
    }
    return 0;
}

// 仓库文件读取（与 StageSim.cpp 的 read_file 同源约定）。
bool read_whole_file(const std::string& path, std::vector<th13::u8>& out) {
    std::FILE* file = std::fopen(path.c_str(), "rb");
    if (!file) return false;
    std::fseek(file, 0, SEEK_END);
    const long size = std::ftell(file);
    std::fseek(file, 0, SEEK_SET);
    out.resize(size_t(size < 0 ? 0 : size));
    const size_t got = out.empty() ? 1 : std::fread(out.data(), 1, out.size(), file);
    std::fclose(file);
    return got == out.size();
}

// （旧 battle-only 回放路径与 ReplayResources/ReplayHost 已删——r4/harness
// 同源切换：replay 认证一律走 RunScene 生产 tick，见下方 replay_runscene。）

// ---- TH13_STATE_LOG：对局状态流（th13-state-raw-v1，r3 lockstep 门禁）----
// 字段/键序与 oracle/battle_oracle.py _write_state_sample 一一对应；帧相位：
// -1 = 装载完成后、首帧步进前（oracle 的 scene7 首帧/检测帧对应物），0 起
// 每帧步进后一行。port 只读既有公开状态（StageBattle/HudPlayerState/
// SessionState/HudScoreState），不改游戏代码。004be814 = DAT_004be814
//（hi-score 达成时的续关数，0x42a370:35-38 每帧写 be814=be7c8）由
// SessionState.high_score_extra 真映射（GameHudDraw.cpp:56-60 生产写点）；
// 无对应来源的字段原样输出 null（spell_id：0x4be7d4 符卡练习选择，port
// 无符卡练习模式；raw_enemy_manager：port 无 0x4c2188 ctl 完整镜像——契约
// 见 oracle/state_compare.py 模块 docstring，null 字段不进入裁决面）。
// player_position 与 oracle player+0x5B8/0x5BC 同基（game 坐标，非屏幕系）。
void state_keys(th13::u16 state, std::string& out) {
    // oracle REPLAY_INPUT_VK：位→VK；输出按 VK 升序（oracle sorted()）。
    static constexpr std::pair<th13::u16, int> kBits[] = {
        {0x01, 0x5A}, {0x02, 0x58}, {0x08, 0x10}, {0x10, 0x26},
        {0x20, 0x28}, {0x40, 0x25}, {0x80, 0x27}};
    std::vector<int> keys;
    for (const auto& bit : kBits)
        if (state & bit.first) keys.push_back(bit.second);
    std::sort(keys.begin(), keys.end());
    out.clear();
    for (size_t i = 0; i < keys.size(); i++) {
        out += i ? "," : "";
        out += std::to_string(keys[i]);
    }
}

void write_state_row(std::FILE* log, int frame, const th13::StageBattle& battle,
                     const th13::SessionState& progress,
                     const th13::HudPlayerState& player,
                     const th13::HudScoreState& score, th13::u16 state) {
    std::string keys;
    state_keys(state, keys);
    // %.9g = binary32 精确十进制往返（FLT_DECIMAL_DIG）。
    std::fprintf(log,
                 "{\"schema\":\"th13-state-raw-v1\",\"frame\":%d,"
                 "\"player_position\":[%.9g,%.9g],"
                 "\"power\":%d,\"power_step\":%d,\"spell_id\":null,"
                 "\"keys\":[%s],"
                 "\"raw_globals\":{\"004be7c0\":%d,\"004be7c8\":%d,"
                 "\"004be810\":%d,\"004be814\":%d},"
                 "\"raw_enemy_manager\":null}\n",
                 frame, double(battle.player_x), double(battle.player_y),
                 player.power, player.power_step, keys.c_str(),
                 score.score, progress.continues, progress.high_score,
                 progress.high_score_extra);
}


// ---- RunScene 生产 tick 回放（replay 模式唯一路径；2026-10-25 r4/harness）----
// 与网页同一机制：ApplicationState::begin_replay_run/begin_run 的 bank 装载序
// + RunScene::initialize + ApplicationState::step 的每帧序（输入单实例
// GameInput::update → RunScene::controls → scheduler.update()：副池@9 →
// RunScene::update@20（0x70 冻结门 → battle.step → dialog → scenery →
// presentation->sync → hud.update/update_score → post_flow）→ 主池@34）。
// 无 anm_effects_noop 特例（ecl_slot_anm/enemies_from_ecl 由 initialize 按
// 生产置位）。事件流经 StageBattle::open_event_sink（events_/set_frame 生产
// 事件点，player 行为帧内首条）；状态流逐消费帧采样（battle_oracle.py:1086
// 契约）。自证开关（仅诊断，不改评分字段）：TH13_FREEZE_AT=<帧>（该帧起
// scene_flags|=0x10：输入照常逐帧消费、战斗冻结、状态行照写值冻结）；
// TH13_NO_ECL_SLOT_ANM=1（负控：槽位 VM 不建，敌机 anm_slot_handle 恒 0，
// 与默认正控对照证明 ANM sync 真实执行）。
class RunSceneHost final : public th13::RunSceneServices {
public:
    std::string dir;
    th13::AnmManager* animations = nullptr;
    th13::DrawCapture* graphics = nullptr;
    th13::AsciiText* captions = nullptr;  // draws 模式接生产文字面；null=纯回放
    i32 destination = 0;  // 0 = 未请求（PostGameFlow 终局转发记账）
    bool game_over = false;
    bool hud_text(const th13::HudTextDraw& value) override {
        // ApplicationState::hud_text 同款（ApplicationTitle.cpp:9-11）。
        return captions ? captions->enqueue(value, 0, value.style.shadow) : true;
    }
    bool hud_sound(i32) override { return true; }
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool camera(th13::DrawCamera, bool) override { return true; }
    bool request_pause() override { return true; }
    bool request_game_over() override {
        game_over = true;
        return true;
    }
    bool request_destination(i32 raw) override {
        destination = raw;
        return true;  // 生产由 Application 接线换场景；harness 记账并收窗
    }
    // harness 独占无音频服务模拟：记录请求并显式返回 true，不改 RunSceneServices 生产默认值
    std::vector<std::tuple<i32, i32, std::string>> music_requests;
    bool queue_music(i32 code, i32 value, const std::string& name) override {
        music_requests.emplace_back(code, value, name);
        return true;
    }
    bool read_asset(const std::string& name, std::vector<th13::u8>& bytes,
                    bool required) override {
        if (read_whole_file(dir + "/" + name, bytes)) return true;
        bytes.clear();
        return !required;
    }
    th13::ZunGraphics& stage_graphics() override { return *graphics; }
    bool preload_scenery_animation(i32 bank, const std::string& name) override {
        std::vector<th13::u8> bytes;
        return animations && read_whole_file(dir + "/" + name, bytes) &&
               animations->load(bank, bytes.data(), th13::u32(bytes.size()));
    }
};

// 帧序与 bank 面镜像 ApplicationState（visual → AnmManager；game_random →
// RunScene → StageBattle/ItemManager；environment.game_rng 同拍 reseed——
// begin_replay_run 的畸变侧同款）。装载序 = web 完整启动链：text0/sig1/
// ascii2/effect8/title16/title_v17/help19（Application init）→ front3/
// stage4 合并/bullet5/player6（begin_run）→ scenery7（initialize 内经
// preload_scenery_animation）——模板 tick 共享 visual 流，序错即移流。
struct ReplayRunSession {
    th13::GameRandom visual;      // ApplicationState.visual（0x4dc31c 位）
    th13::GameRandom game_random; // ApplicationState.game_random（战斗流）
    th13::AnmEnvironment environment;
    th13::AnmManager animations{visual, environment};
    th13::DrawCapture capture;    // 无头语义面（draws 模式 open_output 记账）
    th13::AnmRenderer renderer{capture};
    th13::ScreenViews views{renderer, environment};
    th13::AsciiText captions{animations, environment};
    th13::FrameScheduler scheduler;
    th13::SessionState progress;
    th13::HudPlayerState player;
    th13::HudScoreState score;
    RunSceneHost host;
    std::unique_ptr<th13::RunScene> run;
    std::unique_ptr<th13::PlayfieldTargets> playfield;
    std::unique_ptr<th13::AnmDrawSchedule> draw_schedule;
    std::unique_ptr<th13::AsciiFrame> caption_frame;
    std::array<th13::FrameCallback, 2> animation_updates{};  // @9/@34（Application 同款）
    std::string error;
    bool fail(const std::string& why) {
        if (error.empty()) error = why;
        return false;
    }
    void configure_environment() {
        // web 对局期环境口径（ApplicationState::initialize battle 子集，
        // DrawsSession 同款已在 draws 窗对拍）。
        environment.screen_width = 640;
        environment.screen_height = 480;
        environment.raster_scale = 1;
        environment.resolution_scale = 1;
        environment.render_target_has_alpha = true;
        environment.screen_offsets = {32, 16, 320, 16};
        environment.playfield_origin = {32, 16};
        environment.camera_origin = {0, 0, 1000};
    }
    bool read_bank(const char* name, th13::i32 bank, th13::i32 identity_slot = -100) {
        std::vector<th13::u8> bytes;
        if (!read_whole_file(host.dir + "/" + name, bytes) ||
            !animations.load(bank, bytes.data(), th13::u32(bytes.size())))
            return fail(std::string("ANM load failed: ") + name + ": " + animations.error);
        // 纹理身份登记：原版 decomp+oracle 元组（Render r4/render/HANDOFF 三轮
        // 证据）text0/sig1/ascii2/front5/stage6/bullet7/effect8/player9/title16/
        // help19——标签=原版槽，内部 bank id 不动；未列族（scenery 等）留空
        // → DrawCapture 引用时显式 port:unregistered 标记，不静默。
        if (identity_slot != -100)
            if (auto* file = animations.resource(bank))
                capture.register_texture(file, 0, th13::u32(file->textures.size()),
                                         identity_slot);
        return true;
    }
    // begin_run 面装载（Application.cpp:400-451 同序同法）。
    bool load_run_banks(const th13::ReplayFile& replay_file) {
        static constexpr std::pair<const char*, th13::i32> init_banks[7] = {
            {"text.anm", 0},  {"sig.anm", 1},   {"ascii.anm", 2}, {"effect.anm", 8},
            {"title.anm", 16}, {"title_v.anm", 17}, {"help.anm", 19}};
        for (const auto& bank : init_banks) {
            if (!read_bank(bank.first, bank.second, bank.second)) return false;
            // ApplicationState::prepare_title 同款：ascii(2) 为兜底精灵源
            //（effect.anm 模板 32 等引用兜底面，无则装载期 tick 失败）。
            if (bank.second == 2)
                environment.fallback_sprite_resource = animations.resource(2);
        }
        if (!read_bank("front.anm", 3, 5)) return false;  // kFrontBank；原版槽 5
        // stage 合并 bank（Application.cpp:416-433：ECL ANIM 名单序 append；
        // 纹理身份 = 原版槽 6 整名单单标签——Render 三轮元组，内部 bank 4）。
        std::vector<th13::u8> ecl, bytes;
        char stage_ecl[16];
        std::snprintf(stage_ecl, sizeof stage_ecl, "st%02d.ecl", int(progress.stage));
        if (!read_whole_file(std::string(host.dir) + "/" + stage_ecl, ecl))
            return fail(std::string("Stage ECL missing: ") + stage_ecl);
        th13::EclResource head;
        if (!head.open(ecl.data(), th13::u32(ecl.size())) || head.animations.empty())
            return fail("Stage ECL ANIM list unavailable");
        th13::AnmResource merged;
        for (const auto& name : head.animations) {
            if (!read_whole_file(host.dir + "/" + name, bytes))
                return fail("Stage ANM missing: " + name);
            if (!merged.append(bytes.data(), th13::u32(bytes.size())))
                return fail("Stage ANM append failed: " + name);
        }
        if (!animations.adopt(4, std::move(merged)))
            return fail("Stage ANM bank failed: " + animations.error);  // kStageBank
        if (auto* file = animations.resource(4))
            capture.register_texture(file, 0, th13::u32(file->textures.size()), 6);
        if (!read_bank("bullet.anm", 5, 7)) return false;  // 原版槽 7
        if (!read_bank("astral.anm", 25, 25)) return false; // 00405530 / 00438ec0.
        char player_anm[16];
        std::snprintf(player_anm, sizeof player_anm, "pl%02d.anm", replay_file.character & 3);
        if (!read_bank(player_anm, 6, 9)) return false;  // kPlayerBank；原版槽 9
        return true;
    }
    bool register_animation_pools() {
        animation_updates[0].owner = animation_updates[1].owner = this;
        animation_updates[0].enabled = animation_updates[1].enabled = true;
        animation_updates[0].run = [](void* p) {
            return static_cast<ReplayRunSession*>(p)->animations.update(true)
                       ? th13::i32(th13::FrameAction::Continue)
                       : th13::i32(th13::FrameAction::Error);
        };
        animation_updates[1].run = [](void* p) {
            return static_cast<ReplayRunSession*>(p)->animations.update(false)
                       ? th13::i32(th13::FrameAction::Continue)
                       : th13::i32(th13::FrameAction::Error);
        };
        if (scheduler.add(animation_updates[0], th13::FramePass::Update, 9) < 0 ||
            scheduler.add(animation_updates[1], th13::FramePass::Update, 34) < 0)
            return fail("animation pool registration failed");
        return true;
    }
    // 恢复 → bank 面 → RunScene 构造/装载（replay 与 draws-replay 共用；
    // begin_replay_run→begin_run 同序）。event_path 非空时：initialize 前
    // open_event_sink（装载趟 frame-0 事件入流；load 不清已开 sink）+
    // initialize 后幂等重 afirm（Combat r4 幂等版：二次调用仅 set_frame，
    // 不重开不截断）。调用方随后自行接绘制栈/自证开关。
    bool assemble(const th13::ReplayFile& replay_file, u32 stage_index,
                  const th13::ReplayStage& stage, const char* event_path = nullptr) {
        host.animations = &animations;
        host.graphics = &capture;
        configure_environment();
        progress.stage = i32(stage.number);
        progress.difficulty = replay_file.rank & 3;
        progress.character = replay_file.character;
        th13::ReplayRestoreLog restore_log;
        std::string restore_error;
        const th13::ReplayRestoreTarget restore_target{&progress, &player, &score,
                                                       &game_random, &visual};
        if (!th13::ReplaySession::restore_stage(replay_file, stage_index, restore_target,
                                                restore_log, restore_error)) {
            error = "replay restore failed: " + restore_error;
            return false;
        }
        th13::ReplaySession::emit_log(restore_log);
        progress.stage = i32(stage.number);  // DAT_004be81c 位（restore 不写）
        environment.game_rng.reseed(restore_log.seed);
        if (!load_run_banks(replay_file)) return false;
        char std_name[16], sht_file[16];
        std::snprintf(std_name, sizeof std_name, "st%02d.std", int(stage.number));
        std::snprintf(sht_file, sizeof sht_file, "pl%02d%c.sht", replay_file.character,
                      replay_file.shot_type == 0 ? 'a' : 'b');
        run = std::make_unique<th13::RunScene>(
            progress, player, score, animations, renderer, environment, host, scheduler,
            th13::StagePresentation::Banks{4, 5, 6, 8}, 3, 2, 7, std_name, &game_random);
        run->battle_sht = sht_file;
        if (event_path && !run->battle.open_event_sink(event_path))
            return fail("event sink open failed: " + run->battle.error());
        if (!register_animation_pools()) return false;
        if (!run->initialize()) return fail(run->error);
        if (event_path && !run->battle.open_event_sink(event_path))  // 幂等重 afirm
            return fail("event sink reaffirm failed: " + run->battle.error());
        return true;
    }
};

int replay_runscene(const char* replay_path, const char* resource_dir,
                    const char* event_path, unsigned frames, unsigned stage_index) {
    std::vector<th13::u8> bytes;
    if (!read_whole_file(replay_path, bytes)) {
        std::printf("replay read failed: %s\n", replay_path);
        return 1;
    }
    th13::ReplayFile replay_file;
    if (!replay_file.decode(bytes.data(), bytes.size())) {
        std::printf("replay-runscene decode failed: %s\n", replay_file.error().c_str());
        return 1;
    }
    if (stage_index >= replay_file.stages.size()) {
        std::printf("replay stage index %u out of %u\n", stage_index,
                    unsigned(replay_file.stages.size()));
        return 1;
    }
    const th13::ReplayStage& stage = replay_file.stages[stage_index];
    std::printf("replay-runscene name=%s rank=%d chara=%d shot=%d stages=%u\n",
                replay_file.player_name.c_str(), replay_file.rank, replay_file.character,
                replay_file.shot_type, unsigned(replay_file.stages.size()));
    const auto session = new ReplayRunSession();  // 堆置（RunScene+管理器超栈）
    session->host.dir = resource_dir;
    if (!session->assemble(replay_file, stage_index, stage, event_path)) {
        std::printf("replay-runscene setup failed: %s\n",
                    session->error.empty() ? session->run->error.c_str() : session->error.c_str());
        return 1;
    }
    const bool no_ecl_slot_anm = std::getenv("TH13_NO_ECL_SLOT_ANM") != nullptr;
    if (no_ecl_slot_anm)  // 负控须在 initialize 置位后压回
        session->run->battle.sim.ecl_slot_anm = false;
    unsigned freeze_at = 0;
    if (const char* at = std::getenv("TH13_FREEZE_AT")) freeze_at = unsigned(std::atoi(at));

    std::FILE* state_log = nullptr;
    if (const char* state_path = std::getenv("TH13_STATE_LOG")) {
        state_log = std::fopen(state_path, "wb");
        if (!state_log) {
            std::printf("replay state log open failed: %s\n", state_path);
            return 1;
        }
        write_state_row(state_log, -1, session->run->battle, session->progress,
                        session->player, session->score, 0);
    }
    // 每帧序 = ApplicationState::step（run 分支）：先消费同一六字节输入
    //（00448200 每帧相位，冻结帧照常消费——原版输入层与战斗步进解耦），
    // controls 传同一实例，scheduler.update() 单调（@9 pool → RunScene::update
    // → @34 pool）。冻结门内 RunScene::update 早退：stage_frame 不进、
    // battle 不 step；输入照常消费（00448200 相位）。
    th13::ReplayInputCursor cursor(stage);
    th13::GameInput input;
    const u32 total = frames ? th13::u32(frames) : stage.frame_count;
    u32 consumed = 0, stepped = 0, frozen = 0;
    unsigned max_with_handles = 0;  // 槽位 VM 峰值（敌机死后 handle 归零，只看存活面）
    i32 boss_frame = -1, nonspell_frame = -1;
    bool boss_seen = false, nonspell_seen = false, terminal = false;
    u32 stop_after = 0xFFFFFFFFu;
    while (consumed < total && cursor.valid() && !terminal) {
        const th13::ReplayFrame frame = cursor.next();
        input.update(frame.state);
        session->run->controls(input);
        if (freeze_at && consumed + 1 >= freeze_at) session->progress.scene_flags |= 0x10u;
        const u32 frame_before = session->progress.stage_frame;
        if (session->scheduler.update() < 0) {
            std::printf("replay-runscene error at frame %u: %s\n", consumed,
                        session->run->error.empty() ? session->animations.error.c_str()
                                                    : session->run->error.c_str());
            if (state_log) std::fclose(state_log);
            session->run->battle.sim.sink.close();
            return 1;
        }
        ++consumed;
        // 采样相位 = oracle 真实契约（battle_oracle.py:1086-1091）：battle
        // 开始后每个消费帧一行，帧号 = 消费序（冻结帧 battle 不步进但行照
        // 写、值保持冻结态；oracle 另有 enemy_update_zero_frames 计数证明
        // 零步进帧在窗外如实存在）。绝不为匹配跳行。
        if (state_log)
            write_state_row(state_log, i32(consumed) - 1, session->run->battle,
                            session->progress, session->player, session->score,
                            frame.state);
        if (session->progress.stage_frame == frame_before) {
            ++frozen;  // 冻结帧：输入已消费，战斗未步进（0x42c550:263 门）
        } else {
            ++stepped;
            unsigned live_handles = 0;
            for (const auto& enemy : session->run->battle.sim.enemies)
                for (auto handle : enemy->anm_slot_handle)
                    if (handle) live_handles++;
            max_with_handles = std::max(max_with_handles, live_handles);
            const bool boss = session->run->battle.sim.boss() != nullptr;
            if (boss && !boss_seen) {
                boss_seen = true;
                boss_frame = i32(stepped) - 1;
            }
            if (session->run->battle.sim.boss_nonspell_frame >= 0 && !nonspell_seen) {
                nonspell_seen = true;
                nonspell_frame = session->run->battle.sim.boss_nonspell_frame;
                // 当显式指定 frames（如用户整关 11675）或环境变量 TH13_REPLAY_FULL=1 时，不强制截断；
                // 仅在默认旧短窗（frames==0 且未置 TH13_REPLAY_FULL）时保留 +300 截断
                if (!frames && !std::getenv("TH13_REPLAY_FULL")) {
                    stop_after = consumed + 300;  // 非符锚点 +300 帧（§7.3 同窗）
                }
            }
        }
        if (session->host.game_over || session->host.destination) terminal = true;
        if (consumed >= stop_after) break;
        if (consumed % 600 == 0)
            std::printf("replay-runscene frame=%u held=%04x x=%.1f y=%.1f stage_frame=%u "
                        "enemies=%u slow=%u\n",
                        consumed, frame.state, session->run->battle.player_x,
                        session->run->battle.player_y, session->progress.stage_frame,
                        unsigned(session->run->battle.sim.enemies.size()),
                        cursor.current_slow_rate());
    }
    if (state_log) std::fclose(state_log);
    // 事件落盘完整性：显式 close（堆置会话不依赖析构）+ 写失败即败。
    session->run->battle.sim.sink.close();
    const bool sink_ok = session->run->battle.sim.sink.ok();
    const unsigned long long events =
        static_cast<unsigned long long>(session->run->battle.sim.event_count());
    std::printf("replay-runscene done consumed=%u/%u stepped=%u frozen=%u events=%llu "
                "sink_ok=%d x=%.1f y=%.1f deaths=%d score=%d hi=%d boss_frame=%d "
                "nonspell_frame=%d game_over=%d destination=%d\n",
                consumed, unsigned(stage.frame_count), stepped, frozen, events,
                int(sink_ok), session->run->battle.player_x, session->run->battle.player_y,
                session->player.deaths, session->score.score, session->progress.high_score,
                boss_frame, nonspell_frame, int(session->host.game_over),
                int(session->host.destination));
    if (!sink_ok) {
        std::printf("replay-runscene FAIL event sink write error\n");
        return 1;
    }
    // ---- 自证（freeze 正控 / ANM sync 正负对照）----
    int failures = 0;
    if (freeze_at) {
        const u32 expect_frozen = total >= freeze_at ? total - freeze_at + 1 : 0;
        std::printf("SELFTEST-FREEZE freeze_at=%u consumed=%u stepped=%u frozen=%u "
                    "expect_frozen=%u rows_per_consumed_frame=1\n",
                    freeze_at, consumed, stepped, frozen, expect_frozen);
        if (consumed != total || frozen != expect_frozen || stepped != total - expect_frozen) {
            std::printf("SELFTEST-FREEZE FAIL\n");
            failures++;
        } else {
            std::printf("SELFTEST-FREEZE PASS\n");
        }
    }
    {
        // ANM sync 正控：表现层每帧 sync 清空 anm_effects（StagePresentation
        // :297），敌机槽位 VM 经 303/306 建立（存活期 handle != 0；死后归零，
        // 故断言用峰值）。负控（TH13_NO_ECL_SLOT_ANM=1）：ecl_slot_anm 压回
        // false → 全程无槽位 VM。
        const bool drained = session->run->battle.sim.anm_effects.empty();
        std::printf("SELFTEST-ANMSYNC ecl_slot_anm=%d effects_drained=%d "
                    "max_with_handles=%u\n",
                    int(session->run->battle.sim.ecl_slot_anm), int(drained),
                    max_with_handles);
        if (!drained) {
            std::printf("SELFTEST-ANMSYNC FAIL effects not drained\n");
            failures++;
        }
        if (no_ecl_slot_anm ? max_with_handles != 0 : max_with_handles == 0) {
            std::printf("SELFTEST-ANMSYNC FAIL slot handles\n");
            failures++;
        } else {
            std::printf("SELFTEST-ANMSYNC PASS\n");
        }
    }
    return failures ? 1 : 0;
}

// ---- 语义绘制清单导出（draw-contract.md v1 移植侧导出器驱动）----

// 装载序对齐 oracle 最小战斗集（draw-contract §0）+ web begin_run bank 面：
// effect.anm → bank 8（mgr 槽 8）、st01.ecl ANIM 名单合并 → bank 4（名单序
// 槽 10/11）、bullet.anm → bank 5（mgr 槽 7）、plXX.anm → bank 6（窗口外，
// 身份槽 -1 可见标记）。纹理身份 (槽, entry=文件内序号, 名) 供 tex 字段。
constexpr th13::i32 kDrawsStageBank = 4, kDrawsBulletBank = 5, kDrawsPlayerBank = 6,
                    kDrawsEffectBank = 8;

class DrawsProvider final : public th13::EclResourceProvider {
public:
    std::string dir;
    th13::StageSim* sim = nullptr;
    bool read(const std::string& name, std::vector<th13::u8>& data) override {
        return read_whole_file(dir + "/" + name, data);
    }
    bool animation(u32, const std::string& name) override {
        std::vector<th13::u8> probe;
        if (!read_whole_file(dir + "/" + name, probe)) return false;
        if (sim) sim->anim_list.push_back(name);
        return true;
    }
};

class DrawsHost final : public th13::StageBattleServices {
public:
    bool life_hud(i32, i32) override { return true; }
    bool bomb_hud(i32, i32) override { return true; }
    bool request_game_over() override {
        game_over = true;
        return true;
    }
    bool hud_sound(i32) override { return true; }
    // 敌机销毁回收（enemies_from_ecl 路径）：Session 建立后由各模式接线
    //（回收敌人槽位 VM；draws 窗内敌机会出界自杀，句柄不回收会滞留池内
    // 继续被层桶绘制，污染捕获面）。
    th13::AnmManager* retire_animations = nullptr;
    void enemy_retire(u32 serial) override {
        if (!retire_animations) return;
        for (const auto& enemy : session_sim->enemies)
            if (enemy->serial == serial) {
                for (auto& handle : enemy->anm_slot_handle)
                    if (handle) retire_animations->retire(handle);
                return;
            }
    }
    th13::StageSim* session_sim = nullptr;
    bool game_over = false;
};

struct DrawsSession {
    struct StageFileRange {
        th13::u32 first = 0, count = 0;
        th13::i32 slot = -1;
    };
    th13::DrawCapture capture;
    th13::AnmEnvironment environment;
    // 双流实证（oracle）：ANM 用全局 0x4dc31c（种子 0，装载 2 调 + 每神灵
    // 10 调）；ECL 的 [-9987] 族抽签在 f61 仍返回新流第 1 抽（-0.618 →
    // GirlRedA01 生成 x=-163.783264）——与 ANM 消费互不推进 ⟹ 两条独立
    // 种子 0 流（stage1 事件基线逐字节一致亦证 ECL 流无 ANM 污染）。
    // 曾误并为单流（轮 2）：f61 前恰好 ECL 零抽签故神灵仍匹配，f61 首抽
    // 即分歧（x=-154.101）。
    th13::GameRandom anm_random;  // ANM 侧流（原版 0x4dc31c 同构）
    th13::StageBattle battle;
    th13::AnmManager animations{anm_random, environment};
    th13::AnmRenderer renderer{capture};
    th13::ScreenViews views{renderer, environment};
    std::unique_ptr<th13::StagePresentation> presentation;
    std::vector<StageFileRange> stage_files;
    std::string error;

    bool fail(const std::string& why) {
        if (error.empty()) error = why;
        return false;
    }
    // web 对局期环境口径（ApplicationState::initialize 的 battle 子集）。
    void configure_environment() {
        environment.screen_width = 640;
        environment.screen_height = 480;
        environment.raster_scale = 1;
        environment.resolution_scale = 1;
        environment.render_target_has_alpha = true;
        environment.screen_offsets = {32, 16, 320, 16};
        environment.playfield_origin = {32, 16};
        // 0x43bbc0 战斗初始化后的相机/参考全局（oracle 同构）：变量 10018 =
        // 0x4dcb38(0)+0x4dcafc(1000.0)——神灵 z 链（ins_103 [10006]+=1000）对拍
        // 实证；10016/10017/10019-10021 均为 0。
        environment.camera_origin = {0, 0, 1000};
    }
    bool load_banks(DrawsProvider& provider, const char* player_anm);
    // 一帧渲染（draw-contract §0 帧序的移植侧镜像；ECL/battle 步进由调用方
    // 在本函数前完成）。
    bool render_frame(unsigned frame, th13::u32 held);
};

bool DrawsSession::load_banks(DrawsProvider& provider, const char* player_anm) {
    std::vector<th13::u8> bytes;
    // 装载序对齐 oracle 最小战斗集（draw-contract §0 / render_oracle run()）：
    // bullet（0x40d920 弹幕管理器先装）→ effect（mgr 槽 8）→ st01 ANIM 名单。
    // 模板 tick 共享 0x472c90 RNG，装载序错位会移动共享流（装载期 2 调的
    // 归属文件序即消费序）。ascii 不装（oracle 窗外资产；模板 tick 会抽签）；
    // 兜底精灵源 = bullet.anm（原版 game_main+0x19194 同源）。
    if (!read_whole_file(provider.dir + "/bullet.anm", bytes) ||
        !animations.load(kDrawsBulletBank, bytes.data(), th13::u32(bytes.size())))
        return fail("bullet.anm load failed: " + animations.error);
    if (auto* file = animations.resource(kDrawsBulletBank))
        capture.register_texture(file, 0, th13::u32(file->textures.size()), 7);
    environment.fallback_sprite_resource = animations.resource(kDrawsBulletBank);
    // effect.anm（mgr 槽 8；全局 ANM，oracle 0x46df40(EAX=8) 同位）。
    if (!read_whole_file(provider.dir + "/effect.anm", bytes) ||
        !animations.load(kDrawsEffectBank, bytes.data(), th13::u32(bytes.size())))
        return fail("effect.anm load failed: " + animations.error);
    if (auto* file = animations.resource(kDrawsEffectBank))
        capture.register_texture(file, 0, th13::u32(file->textures.size()), 8);
    if (!read_whole_file(provider.dir + "/astral.anm", bytes) ||
        !animations.load(25, bytes.data(), th13::u32(bytes.size())))
        return fail("astral.anm load failed: " + animations.error);
    if (auto* file = animations.resource(25))
        capture.register_texture(file, 0, th13::u32(file->textures.size()), 25);
    // st01.ecl ANIM 名单合并 bank（web begin_run 同式；名单序 = mgr 槽 10 起）。
    std::vector<th13::u8> ecl;
    if (!read_whole_file(provider.dir + "/st01.ecl", ecl))
        return fail("st01.ecl missing");
    th13::EclResource head;
    if (!head.open(ecl.data(), th13::u32(ecl.size())) || head.animations.empty())
        return fail("st01.ecl ANIM list unavailable");
    th13::AnmResource merged;
    for (const auto& name : head.animations) {
        if (!read_whole_file(provider.dir + "/" + name, bytes))
            return fail("stage ANM missing: " + name);
        th13::AnmResource single;
        if (!single.open(bytes.data(), th13::u32(bytes.size())))
            return fail("stage ANM invalid: " + name);
        StageFileRange range;
        range.first = th13::u32(merged.textures.size());
        range.count = th13::u32(single.textures.size());
        range.slot = th13::i32(10 + stage_files.size());
        if (!merged.append(bytes.data(), th13::u32(bytes.size())))
            return fail("stage ANM append failed: " + name);
        stage_files.push_back(range);
    }
    if (!animations.adopt(kDrawsStageBank, std::move(merged)))
        return fail("stage ANM bank failed: " + animations.error);
    if (auto* file = animations.resource(kDrawsStageBank))
        for (const auto& range : stage_files)
            capture.register_texture(file, range.first, range.count, range.slot);
    // plXX.anm（自机 bank：oracle 窗口外的表现资产；身份槽 -1 可见标记）。
    if (!read_whole_file(provider.dir + "/" + std::string(player_anm), bytes) ||
        !animations.load(kDrawsPlayerBank, bytes.data(), th13::u32(bytes.size())))
        return fail(std::string("player ANM load failed: ") + player_anm);
    if (auto* file = animations.resource(kDrawsPlayerBank))
        capture.register_texture(file, 0, th13::u32(file->textures.size()), -1);
    return true;
}

bool DrawsSession::render_frame(unsigned frame, th13::u32 held) {
    if (!error.empty()) return false;
    capture.begin_frame(frame);
    // web 调度序：副池步进（@9）→ 场景 update（ECL + 表现层 sync，@20）→
    // 主池步进（@34）→ Draw pass。ECL/battle 步进由调用方在本函数前完成。
    if (!animations.update(true)) return fail("fore pool update: " + animations.error);
    if (!presentation->sync(battle, held)) return fail("presentation sync: " + presentation->error());
    if (!animations.update(false)) return fail("main pool update: " + animations.error);
    // f159 取证（TH13_DUMPCHECK=帧号）：层 2 灵的插值通道字段
    //（world[12] = vm+0x3c；start/end/duration/timer 全量，位级 dump——
    // oracle 侧 trace_spawn 0x4692d0 watch 同字段对照）。
    if (const char* dump_at = std::getenv("TH13_DUMPCHECK")) {
        const unsigned at = unsigned(std::atoi(dump_at));
        if (frame == at) {
            const auto as_bits = [](float value) {
                return th13::u32(*reinterpret_cast<const th13::u32*>(&value));
            };
            for (th13::AnmVm* vm : animations.registry.layer(2)) {
                const auto& ch = vm->interpolators.position;
                const auto& col = vm->interpolators.color;
                const auto& al = vm->interpolators.alpha;
                std::printf(
                    "[f%u] s4aa=%d x3c=%.9g bits=%08x start=%08x,%08x,%08x "
                    "end=%08x,%08x,%08x dur=%08x mode=%d frac=%08x "
                    "color=%08x cdur=%d ctimer=%d alpha=%08x adur=%d "
                    "atimer=%d astart=%.9g aend=%.9g\n",
                    frame, vm->source_script,
                    double(vm->variables.position.x),
                    as_bits(vm->variables.position.x),
                    as_bits(ch.start[0]), as_bits(ch.start[1]),
                    as_bits(ch.start[2]), as_bits(ch.end[0]),
                    as_bits(ch.end[1]), as_bits(ch.end[2]),
                    as_bits(float(ch.duration)), ch.mode,
                    as_bits(ch.timer.fractional),
                    vm->visual.color, int(col.duration), col.timer.current,
                    vm->visual.color, int(al.duration), al.timer.current,
                    double(al.start[0]), double(al.end[0]));
            }
        }
    }
    if (!views.camera(th13::DrawCamera::Fullscreen, false))
        return fail("full-screen camera unavailable");
    // [f8 捕获面对齐] oracle 窗不含玩家/弹幕遍历绘制（draw-contract §4.2：
    // 0x40e4d0 族不在捕获面）。port 的 .sht 弹型脚本 spawn 的子弹入了
    // registry 层桶（真机走 IShot 独立遍历）——绘制时隐藏 player-bank
    // 池 VM 以镜像捕获面（行为仍 tick，仅 draws 捕获不画）。
    std::vector<th13::AnmVm*> hidden;
    for (th13::u32 layer = 0; layer < 33; layer++)
        for (auto* vm : animations.registry.layer(layer))
            if (vm && vm->resource == animations.resource(kDrawsPlayerBank) && (vm->visual.flags & 1u)) {
                vm->visual.flags &= ~1u;  // draw 门 = flags bit0（非 visible）
                hidden.push_back(vm);
            }
    for (th13::u32 layer = 0; layer < 33; layer++)  // th13 层域：主链 0-30 + 副链 31/32
        if (!renderer.draw_layer(animations.registry.layer(layer)))
            return fail("animation layer draw: " + renderer.error);
    for (auto* vm : hidden) vm->visual.flags |= 1u;
    for (th13::u32 pass = 0; pass < 4; pass++)
        if (!presentation->draw(pass)) return fail("presentation draw: " + presentation->error());
    capture.set_stale_layer(56);  // 原版层循环 0..56 走完后的 watch 残值（UP 层口径）
    renderer.flush();
    capture.clear_device_texture();  // 0x45d2f0:49 帧尾 SetTexture(0,0)（UG 缓存不失效）
    if (!capture.end_frame()) return fail("draws write: " + capture.error());
    return true;
}

int run_draws(const char* resource_dir, const char* draws_path, unsigned frames,
              const char* stage) {
    DrawsSession session;
    session.configure_environment();
    DrawsProvider provider;
    provider.dir = resource_dir;
    provider.sim = &session.battle.sim;  // sim.begin 装载契约：anim_list ≥ 2
    DrawsHost host;
    th13::SessionState progress;
    th13::HudPlayerState player;
    th13::HudScoreState score;
    th13::draw_capture = &session.capture;
    const std::string stage_file = stage ? stage : "st01.ecl";
    if (!session.battle.load(provider, stage_file.c_str(), 1 << 3, host, progress, player,
                             score, nullptr, nullptr)) {
        std::printf("draws battle load failed: %s\n", session.battle.error().c_str());
        return 1;
    }
    if (!session.load_banks(provider, "pl00.anm")) {
        std::printf("draws banks failed: %s\n", session.error.c_str());
        return 1;
    }
    session.presentation = std::make_unique<th13::StagePresentation>(
        session.animations, session.renderer, th13::StagePresentation::Banks{
                                                   kDrawsStageBank, kDrawsBulletBank,
                                                   kDrawsPlayerBank, kDrawsEffectBank},
        31);
    session.presentation->enemies_from_ecl = true;  // 敌机本体 VM 走 ECL 303/306
    session.battle.sim.ecl_slot_anm = true;
    host.retire_animations = &session.animations;
    host.session_sim = &session.battle.sim;
    th13::StagePresentation::debug_pass_mask = 4;  // 仅敌机（oracle 窗口径）
    // 无输入窗玩家口径：静态 (192,420)（oracle stage1 env 契约常量；不走
    // battle 自机模型——ECL 瞄准读 sim.player_x/y 默认值即此）。
    session.battle.player_x = 192.0f;
    session.battle.player_y = 420.0f;
    if (!session.capture.open_output(draws_path)) {
        std::printf("draws output failed: %s\n", session.capture.error().c_str());
        return 1;
    }
    if (!session.render_frame(0, 0)) {  // 帧 0 = 装载趟（root 构造内首趟已跑）
        std::printf("draws frame 0 failed: %s\n", session.error.c_str());
        return 1;
    }
    for (unsigned frame = 1; frame < frames; frame++) {
        if (!session.battle.sim.step_frame()) {
            std::printf("draws error at frame %u: %s\n", frame,
                        session.battle.sim.error().c_str());
            return 1;
        }
        if (!session.render_frame(frame, 0)) {
            std::printf("draws error at frame %u: %s\n", frame, session.error.c_str());
            return 1;
        }
        if (frame % 600 == 0)
            std::printf("draws frame=%u enemies=%u ops=%llu\n", frame,
                        unsigned(session.battle.sim.enemies.size()),
                        static_cast<unsigned long long>(
                            session.capture.quads() + session.capture.ups()));
    }
    session.capture.close_output();
    std::printf("port-draws frames=%llu quads=%llu ups=%llu ops=%llu zero=%u\n",
                static_cast<unsigned long long>(session.capture.frames_out()),
                static_cast<unsigned long long>(session.capture.quads()),
                static_cast<unsigned long long>(session.capture.ups()),
                static_cast<unsigned long long>(session.capture.quads() +
                                                session.capture.ups()),
                session.capture.zero_op_frames());
    return 0;
}

int run_draws_replay(const char* replay_path, const char* resource_dir, const char* draws_path,
                     unsigned frames, unsigned stage_index) {
    // 同源绘制回放（r4/harness）：与 replay 模式同一 ReplayRunSession/RunScene
    // 生产 tick，绘制面 = 生产 SceneDisplay 的游戏层全链（SceneDisplay.cpp:44-58
    // 的无头等价：AnmDrawSchedule 31 层 + PlayfieldTargets @R + RunScene 场景
    // 绘制回调 + AsciiFrame 文字面），scheduler.draw() 单调驱动；GPU present
    // 换 DrawCapture 记账。不隐藏任何有效 VM（旧契约的 player-bank 遮蔽与
    // debug_pass_mask=4 局部窗已移除——那是局部诊断面，不能认证严格绘制）。
    // 帧约：帧 0 = 装载趟渲染（不步进）；帧 f≥1 = 消费输入 f-1 → update →
    // draw（oracle render_oracle 同窗同帧号）。
    std::vector<th13::u8> bytes;
    if (!read_whole_file(replay_path, bytes)) {
        std::printf("draws-replay read failed: %s\n", replay_path);
        return 1;
    }
    th13::ReplayFile replay_file;
    if (!replay_file.decode(bytes.data(), bytes.size())) {
        std::printf("draws-replay decode failed: %s\n", replay_file.error().c_str());
        return 1;
    }
    if (stage_index >= replay_file.stages.size()) {
        std::printf("draws-replay stage index %u out of %u\n", stage_index,
                    unsigned(replay_file.stages.size()));
        return 1;
    }
    const th13::ReplayStage& stage = replay_file.stages[stage_index];
    std::printf("draws-replay-runscene name=%s rank=%d chara=%d shot=%d stages=%u\n",
                replay_file.player_name.c_str(), replay_file.rank, replay_file.character,
                replay_file.shot_type, unsigned(replay_file.stages.size()));
    const auto session = new ReplayRunSession();
    session->host.dir = resource_dir;
    session->host.captions = &session->captions;  // hud_text 接生产文字面
    if (!session->assemble(replay_file, stage_index, stage)) {
        std::printf("draws-replay setup failed: %s\n",
                    session->error.empty() ? session->run->error.c_str() : session->error.c_str());
        return 1;
    }
    th13::draw_capture = &session->capture;
    // 生产绘制栈（Application.cpp:455-458 + SceneDisplay.cpp:28-31 同件）：
    // views.configure + captions.initialize(2) + AnmDrawSchedule +
    // AsciiFrame + PlayfieldTargets.initialize(0)。
    session->views.configure(session->environment);
    if (!session->captions.initialize(2)) {
        std::printf("draws-replay captions failed: %s\n", session->captions.error.c_str());
        return 1;
    }
    session->draw_schedule = std::make_unique<th13::AnmDrawSchedule>(
        session->scheduler, session->animations, session->renderer, session->capture,
        session->host);
    if (!session->draw_schedule->error.empty()) {
        std::printf("draws-replay schedule failed: %s\n", session->draw_schedule->error.c_str());
        return 1;
    }
    session->caption_frame =
        std::make_unique<th13::AsciiFrame>(session->scheduler, session->captions,
                                           session->renderer, session->host);
    if (!session->caption_frame->error.empty()) {
        std::printf("draws-replay caption frame failed: %s\n",
                    session->caption_frame->error.c_str());
        return 1;
    }
    session->playfield = std::make_unique<th13::PlayfieldTargets>(
        session->scheduler, session->animations, session->renderer, session->capture,
        session->views);
    if (!session->playfield->initialize(0)) {
        std::printf("draws-replay playfield failed: %s\n", session->playfield->error.c_str());
        return 1;
    }
    if (!session->capture.open_output(draws_path)) {
        std::printf("draws-replay output failed: %s\n", session->capture.error().c_str());
        return 1;
    }
    const auto render_production_frame = [&](unsigned frame) -> bool {
        if (!session->error.empty()) return false;
        session->capture.begin_frame(frame);
        if (session->scheduler.draw() < 0) {
            session->fail("draw callbacks failed: " +
                          (!session->draw_schedule->error.empty()
                               ? session->draw_schedule->error
                               : !session->caption_frame->error.empty()
                                     ? session->caption_frame->error
                                     : session->run->error));
            return false;
        }
        session->renderer.flush();
        session->capture.clear_device_texture();  // 0x45d2f0:49 帧尾 SetTexture(0,0)
        return session->capture.end_frame();
    };
    const unsigned total = frames && frames < stage.frame_count ? frames
                                                               : unsigned(stage.frame_count);
    th13::ReplayInputCursor cursor(stage);
    th13::GameInput input;
    if (!render_production_frame(0)) {  // 帧 0 = 装载趟渲染（不步进）
        std::printf("draws-replay frame 0 failed: %s\n", session->error.c_str());
        return 1;
    }
    unsigned drawn = 0;
    bool terminal = false;
    for (unsigned frame = 1; frame <= total && !terminal; frame++) {
        if (!cursor.valid()) {
            std::printf("draws-replay input exhausted at frame %u (window %u)\n", frame, total);
            session->capture.close_output();
            return 1;
        }
        input.update(cursor.next().state);
        session->run->controls(input);
        if (session->scheduler.update() < 0) {
            std::printf("draws-replay update failed at frame %u: %s\n", frame,
                        session->run->error.empty() ? session->animations.error.c_str()
                                                    : session->run->error.c_str());
            session->capture.close_output();
            return 1;
        }
        if (session->host.game_over || session->host.destination) {
            // 终局照实：最后一帧仍渲染落盘后收窗报非零，不加命不截绿。
            render_production_frame(frame);
            session->capture.close_output();
            std::printf("draws-replay game over at frame %u (window %u) destination=%d\n",
                        frame, total, int(session->host.destination));
            return 1;
        }
        if (!render_production_frame(frame)) {
            std::printf("draws-replay render failed at frame %u: %s\n", frame,
                        session->error.empty() ? session->capture.error().c_str()
                                               : session->error.c_str());
            session->capture.close_output();
            return 1;
        }
        drawn = frame;
        if (frame % 600 == 0)
            std::printf("draws-replay frame=%u stage_frame=%u enemies=%u ops=%llu\n", frame,
                        session->progress.stage_frame,
                        unsigned(session->run->battle.sim.enemies.size()),
                        static_cast<unsigned long long>(session->capture.quads() +
                                                        session->capture.ups()));
    }
    session->capture.close_output();
    std::printf("port-draws-replay frames=%llu drawn=%u/%u quads=%llu ups=%llu ops=%llu "
                "zero=%u\n",
                static_cast<unsigned long long>(session->capture.frames_out()), drawn, total,
                static_cast<unsigned long long>(session->capture.quads()),
                static_cast<unsigned long long>(session->capture.ups()),
                static_cast<unsigned long long>(session->capture.quads() +
                                                session->capture.ups()),
                session->capture.zero_op_frames());
    return 0;
}

// ---- 标题窗语义绘制导出（title/title-b；oracle/title_oracle.py 同窗）----

// 标题期平台（无头）：音频/覆盖层全桩；destination 记账供窗B终点判定
// （title_probe.cpp 的 Host 同形 + start_game 观察口）。
class TitleHost final : public th13::TitleScenePlatform {
public:
    bool clear_title_overlay() override { return true; }
    bool queue_title_music(i32, i32, const std::string&) override { return true; }
    bool clear_current_wave() override { return true; }
    bool title_exit(i32 value) override {
        exit_destination = value;
        return true;
    }
    bool fade_out_title_music() override { return true; }
    bool sound(i32) override { return true; }
    bool begin_transition(u32& animation) override {
        animation = 0;
        return true;
    }
    bool start_game(i32 stage) override {
        requested_stage = stage;
        run_requested = true;  // Destination::Run 排定帧（ApplicationState::start_game 同语义）
        return true;
    }
    bool hud_text(const th13::HudTextDraw&) override { return true; }
    i32 exit_destination = -1;
    i32 requested_stage = 0;
    bool run_requested = false;
};

// 帧序与成员关系镜像 ApplicationState（装载幕隔离管理器 → 主管理器 7 bank →
// TitleScene + AnmDrawSchedule 同一 FrameScheduler）；析构走显式 teardown
// （~ApplicationState 的摘回调序），不依赖成员逆序。
// 注意：每个 AnmManager 内嵌 0x1fff 槽池（≈64KB），双管理器会话远超 wasm
// 默认栈——必须堆置（DrawsSession 单管理器恰好塞得下栈，本模式不行）。
struct TitleSession {
    th13::DrawCapture capture;
    th13::GameRandom loading_random;  // 装载幕隔离 RNG（ApplicationState::loading_random）
    th13::AnmEnvironment loading_environment;
    th13::AnmManager loading_animations{loading_random, loading_environment};
    th13::GameRandom anm_random;  // ANM 侧流（原版 0x4dc31c 种子 0 同构）
    th13::AnmEnvironment environment;
    th13::AnmManager animations{anm_random, environment};
    th13::AnmRenderer renderer{capture};
    th13::ScreenViews views{renderer, environment};
    th13::FrameScheduler scheduler;
    TitleHost host;
    th13::SessionState progress;
    th13::TitleSelectionSettings selection;
    std::unique_ptr<th13::AnmDrawSchedule> draw_schedule;
    std::unique_ptr<th13::TitleScene> title;
    std::array<th13::FrameCallback, 2> animation_updates{};
    th13::u32 loading_signature = 0;
    unsigned loading_frames = 0;
    std::string dir;
    std::string error;

    bool fail(const std::string& why) {
        if (error.empty()) error = why;
        return false;
    }
    bool read_bank(th13::AnmManager& manager, const char* name, th13::i32 bank) {
        std::vector<th13::u8> bytes;
        if (!read_whole_file(dir + "/" + name, bytes) ||
            !manager.load(bank, bytes.data(), th13::u32(bytes.size())))
            return fail(std::string("ANM load failed: ") + name + ": " + manager.error);
        if (auto* file = manager.resource(bank))
            capture.register_texture(file, 0, th13::u32(file->textures.size()), bank);
        return true;
    }
    // 装载幕（ApplicationState::prepare_loading + draw_loading(true) 镜像）：
    // sig+ascii 入隔离管理器、sig script0 建 VM、首渲染 = port 全局帧 0。
    bool prepare_loading() {
        loading_environment.resolution_scale = 1;
        loading_environment.raster_scale = 1;
        if (!read_bank(loading_animations, "sig.anm", 1) || !read_bank(loading_animations, "ascii.anm", 2) ||
            !read_bank(loading_animations, "text.anm", 0))  // [boot @R] 边条纹理身份（槽0 entry2/3）
            return false;
        loading_signature = loading_animations.create(1, 0);  // sig script0（两半拼 640）
        if (!loading_signature) return fail("loading signature create: " + loading_animations.error);
        // [窗A 修正镜像] sig 右半（script0 自 spawn 的 script1）层 = L26——与
        // Application.cpp draw_loading 同行（oracle boot 全程 f0-f178 实测 23/26；
        // dump ins_304(25) 的 +1 偏移，蓝图 §8.3）。
        if (auto* boot = loading_animations.registry.find(loading_signature))
            if (auto* right = loading_animations.registry.find_child_script(*boot, 1, 0))
                right->visual.layer = 26;
        loading_frames = 0;
        return render_loading_frame(0);
    }
    // 装载幕渲染（ApplicationState::render_loading 镜像；web 外壳品牌直绘
    // draw_startup_branding 为浏览器壳层物，不在语义清单，不镜像）。
    bool render_loading_frame(unsigned frame) {
        capture.begin_frame(frame);
        renderer.invalidate();
        // 帧首管线复位，但 textureFactor 保持（真机 TFACTOR 为跨帧持续的
        // 设备态：D3D 初始 0，仅 world_quad 族批设置写入——oracle f500 @R
        // 携带上帧残留 0x2aff0140 实证，见蓝图 §8.8）。
        const auto factor_kept = capture.pipeline().textureFactor;
        capture.pipeline() = touhou::graphics::PipelineState{};
        capture.pipeline().textureFactor = factor_kept;
        th13::ScreenViews loading_views{renderer, loading_environment};
        if (!loading_views.camera(th13::DrawCamera::Fullscreen, false))
            return fail("loading camera unavailable");
        // [boot @R] 装载幕帧发 4 条边条（oracle raw f1 起每帧恒定；raw f0 即
        // prepare 首渲染保持空帧——oracle f0 也无 op，两侧 prefix 均=1 对齐）。
        if (frame != 0) th13::emit_title_side_strips(loading_animations);
        // sig 幕止于 raw f179（oracle 实测：f179 末日、f180 空档帧仅 @R、
        // f181 背景入场——开场计数 0xb4 使能标题任务在 f181）。
        if (frame < 180)
            for (th13::u32 layer = 0; layer < 33; layer++)
                if (!renderer.draw_layer(loading_animations.registry.layer(layer)))
                    return fail("loading layer draw: " + renderer.error);
        renderer.flush();
        capture.clear_device_texture();  // 0x45d2f0:49 帧尾 SetTexture(0,0)（契约 §2.1）
        return capture.end_frame();
    }
    // boot 一帧（ApplicationState::advance_loading(1/60) 镜像：主池→副池各一趟）。
    bool advance_loading(unsigned frame) {
        if (!loading_animations.update(false) || !loading_animations.update(true))
            return fail("loading tick: " + loading_animations.error);
        // [boot tick 对齐] oracle raw f1（截窗 f0）sig alpha=76=n3：装载趟 tick#1
        // + raw f0 update walk tick#2（空帧不画）+ raw f1 tick#3。port 侧
        // prepare render0 即画出（n2），首个 advance 帧补一趟 tick 对齐 n3。
        if (frame == 1 && !loading_animations.update(false))
            return fail("loading tick: " + loading_animations.error);
        ++loading_frames;
        return render_loading_frame(frame);
    }
    // 标题期（ApplicationState::initialize 的 bank 面/回调面 + begin_title(0) 镜像；
    // web 在装载幕完成前即装载完毕 7 bank，本侧在 boot 180 帧后装载，装载期
    // 行为等价——7 bank 装载不触帧序，装载幕期间主管理器不被步进）。
    bool prepare_title() {
        environment.raster_scale = 1;
        environment.render_target_has_alpha = true;
        environment.screen_offsets = {32, 16, 320, 16};
        environment.playfield_origin = {32, 16};
        constexpr std::pair<const char*, th13::i32> banks[7] = {
            {"text.anm", 0},  {"sig.anm", 1},   {"ascii.anm", 2}, {"effect.anm", 8},
            {"title.anm", 16}, {"title_v.anm", 17}, {"help.anm", 19}};
        for (const auto& bank : banks) {
            if (!read_bank(animations, bank.first, bank.second)) return false;
            if (bank.second == 2) environment.fallback_sprite_resource = animations.resource(2);
        }
        // @R 身份自证（比对契约键 (0,2,"@R")/(0,3,"@R")）：text.anm(槽0) entry2/3
        // 为 640×480 离屏渲染目标标记（无纹素，契约只比身份与几何）——解析出的
        // 名即登记身份，read_bank 已随槽 0 全量入表；此处打印供导出报告核对。
        if (const auto* text = animations.resource(0)) {
            if (text->textures.size() < 4) return fail("text.anm @R entries missing");
            std::printf("title rt identities: (0,2,%s) (0,3,%s)\n",
                        text->textures[2].name.c_str(), text->textures[3].name.c_str());
            if (text->textures[2].name != "@R" || text->textures[3].name != "@R")
                return fail("text.anm entry2/3 not @R: contract identity mismatch");
        } else {
            return fail("text.anm bank unavailable");
        }
        animation_updates[0].owner = animation_updates[1].owner = this;
        animation_updates[0].enabled = animation_updates[1].enabled = true;
        animation_updates[0].run = [](void* p) {
            return static_cast<TitleSession*>(p)->animations.update(true)
                       ? th13::i32(th13::FrameAction::Continue)
                       : th13::i32(th13::FrameAction::Error);
        };
        animation_updates[1].run = [](void* p) {
            return static_cast<TitleSession*>(p)->animations.update(false)
                       ? th13::i32(th13::FrameAction::Continue)
                       : th13::i32(th13::FrameAction::Error);
        };
        if (scheduler.add(animation_updates[0], th13::FramePass::Update, 9) < 0 ||
            scheduler.add(animation_updates[1], th13::FramePass::Update, 34) < 0)
            return fail("animation callback registration failed");
        draw_schedule = std::make_unique<th13::AnmDrawSchedule>(scheduler, animations, renderer,
                                                               capture, views);
        if (!draw_schedule->error.empty()) return fail(draw_schedule->error);
        title = std::make_unique<th13::TitleScene>(progress, selection, animations, host,
                                                   scheduler, th13::TitleSceneResources{});
        if (!title->error.empty()) return fail(title->error);
        title->state.return_reason = 0;  // 冷启动 boot 序（begin_title(0)）
        return true;
    }
    // 标题一帧（ApplicationState::step 标题分支 + SceneDisplay::draw 镜像）。
    bool title_step(unsigned frame, th13::u32 held, th13::u32 pressed) {
        title->controls({held, pressed, 0, 0});
        if (scheduler.update() < 0)
            return fail(title->error.empty() ? (title->frame.error.empty() ? animations.error
                                                                           : title->frame.error)
                                             : title->error);
        capture.begin_frame(frame);
        if (scheduler.draw() < 0)
            return fail(!draw_schedule->error.empty() ? draw_schedule->error
                                                      : title->error.empty() ? "title draw failed"
                                                                             : title->error);
        renderer.flush();
        capture.clear_device_texture();
        return capture.end_frame();
    }
    // ~ApplicationState 的摘回调序（title → 动画回调 → display）。
    void teardown() {
        title.reset();
        for (auto& callback : animation_updates) scheduler.remove(callback);
        draw_schedule.reset();
        capture.close_output();
    }
};

// 与 oracle cut_window 同规则的截窗：首个 ops>0 帧重编号 0 起、固定窗长（窗A）；
// 行内 ops 判定走 DrawCapture 固定键序（"ops":[] 即空），帧号改写按同键序就位。
struct TitleWindowCut {
    unsigned prefix = 0, frames = 0, min_ops = 0;
    unsigned long long max_ops = 0, total_ops = 0;
    bool ok = false;
};

unsigned title_line_ops(const std::string& line) {
    unsigned ops = 0;
    for (size_t at = line.find("\"seq\":"); at != std::string::npos; at = line.find("\"seq\":", at + 1))
        ops++;
    return ops;
}

TitleWindowCut cut_title_window(const char* path, unsigned want, bool fixed_window) {
    TitleWindowCut out;
    std::vector<th13::u8> bytes;
    if (!read_whole_file(path, bytes)) return out;
    std::vector<std::string> lines;
    std::string current;
    for (th13::u8 c : bytes) {
        if (c == '\n') {
            if (!current.empty()) lines.push_back(current);
            current.clear();
        } else {
            current.push_back(char(c));
        }
    }
    if (!current.empty()) lines.push_back(current);
    const auto drawn = [](const std::string& line) {
        return line.find("\"ops\":[]") == std::string::npos;
    };
    size_t first = 0;
    while (first < lines.size() && !drawn(lines[first])) first++;
    if (first >= lines.size()) return out;
    size_t end = lines.size();
    if (fixed_window) {
        end = first + want;
        if (end > lines.size()) return out;
    } else {
        while (end > first && !drawn(lines[end - 1])) end--;  // 尾部 ops=0 回Trim（oracle 同则）
    }
    std::FILE* file = std::fopen(path, "wb");
    if (!file) return out;
    unsigned index = 0;
    for (size_t i = first; i < end; i++) {
        std::string& line = lines[i];
        const size_t at = line.find("\"frame\":");
        const size_t digits = line.find(',', at + 8);
        if (at == std::string::npos || digits == std::string::npos) {
            std::fclose(file);
            return out;
        }
        line.replace(at + 8, digits - (at + 8), std::to_string(index));
        const unsigned ops = title_line_ops(line);
        if (index == 0 || ops < out.min_ops) out.min_ops = ops;
        if (ops > out.max_ops) out.max_ops = ops;
        out.total_ops += ops;
        line.push_back('\n');
        std::fwrite(line.data(), 1, line.size(), file);
        index++;
    }
    std::fclose(file);
    out.prefix = unsigned(first);
    out.frames = index;
    out.ok = index > 0;
    return out;
}

// 两窗输入时刻表（真机全局帧 → controls u32 单帧置位；位布局 oracle 实测：
// Z=0x01 ↑=0x10 ↓=0x20 ←=0x40 →=0x80）。port 全局帧 = 渲染序号，与 oracle
// f0（开场创建当帧）1:1 对齐，注入帧号同刻表。
std::map<unsigned, th13::u32> title_input_script(bool window_b) {
    if (window_b)
        return {{470, 0x01}, {530, 0x20}, {590, 0x01}, {650, 0x80}, {710, 0x01}};
    return {{380, 0x20}, {420, 0x10}, {470, 0x01}};
}

int run_title(const char* resource_dir, const char* draws_path, bool window_b) {
    std::printf("title mode start dir=%s out=%s window=%c\n", resource_dir, draws_path,
                window_b ? 'b' : 'a');
    std::fflush(stdout);
    constexpr unsigned kBootFrames = 180;  // boot 装载幕（oracle g181 = 标题使能）
    // 堆置会话（双 AnmManager ≈128KB > wasm 栈；见 TitleSession 注）。
    const std::unique_ptr<TitleSession> session(new TitleSession());
    session->dir = resource_dir;
    th13::draw_capture = &session->capture;
    if (!session->capture.open_output(draws_path)) {
        std::printf("title output failed: %s\n", session->capture.error().c_str());
        return 1;
    }
    if (!session->prepare_loading()) {
        std::printf("title boot failed: %s\n", session->error.c_str());
        return 1;
    }
    std::printf("title boot prepared sig=%u\n", session->loading_signature);
    std::fflush(stdout);
    for (unsigned frame = 1; frame <= kBootFrames; frame++) {
        if (!session->advance_loading(frame)) {
            std::printf("title boot failed at frame %u: %s\n", frame, session->error.c_str());
            return 1;
        }
        if (frame % 60 == 0) {
            std::printf("title boot frame=%u ops=%llu\n", frame,
                        static_cast<unsigned long long>(session->capture.quads() +
                                                        session->capture.ups()));
            std::fflush(stdout);
        }
    }
    if (!session->prepare_title()) {
        std::printf("title load failed: %s\n", session->error.c_str());
        return 1;
    }
    std::printf("title scene ready\n");
    std::fflush(stdout);
    const std::map<unsigned, th13::u32> script = title_input_script(window_b);
    // 窗A：多跑 10 帧余量（prefix 0..10 内截 600 帧不短窗）；窗B：跑到开局
    // 转场请求帧（含该帧），900 帧兜底（到不了 = 如实报错）。
    const unsigned first_step = kBootFrames + 1;
    const unsigned cap = window_b ? 900 : first_step + 429;
    unsigned transition_frame = 0;
    for (unsigned frame = first_step; frame < cap; frame++) {
        const auto at = script.find(frame);
        const th13::u32 bits = at == script.end() ? 0 : at->second;
        if (!session->title_step(frame, bits, bits)) {
            std::printf("title error at frame %u: %s\n", frame, session->error.c_str());
            return 1;
        }
        if (window_b && session->host.run_requested) {
            transition_frame = frame;
            break;
        }
        if (frame % 60 == 0) {
            std::printf("title frame=%u screen=%d substate=%d ops=%llu\n", frame,
                        i32(session->title->state.screen), session->title->state.substate,
                        static_cast<unsigned long long>(session->capture.quads() +
                                                        session->capture.ups()));
            std::fflush(stdout);
        }
    }
    if (window_b && transition_frame == 0) {
        std::printf("title-b: no game start request within %u frames\n", cap);
        return 1;
    }
    const unsigned long long quads = session->capture.quads(), ups = session->capture.ups();
    const unsigned long long raw_frames = session->capture.frames_out();
    session->teardown();
    const TitleWindowCut cut =
        cut_title_window(draws_path, window_b ? 0 : 600, !window_b);
    if (!cut.ok) {
        std::printf("title window cut failed (raw frames=%llu)\n", raw_frames);
        return 1;
    }
    std::printf("port-title-draws window=%c prefix=%u frames=%u min_ops=%u max_ops=%llu "
                "total_ops=%llu transition=%u stage=%d quads=%llu ups=%llu raw=%llu\n",
                window_b ? 'b' : 'a', cut.prefix, cut.frames, cut.min_ops, cut.max_ops,
                cut.total_ops, transition_frame, session->host.requested_stage, quads, ups,
                raw_frames);
    return 0;
}

}  // namespace

int main(int argc, char** argv) {
    if (argc >= 4 && std::strcmp(argv[1], "stage1") == 0)
        // stage1 <资源目录> <输出> [frames] [stage.ecl]
        return stage1(argv[2], argv[3], argc >= 5 ? unsigned(std::atoi(argv[4])) : 3600u,
                      argc >= 6 ? argv[5] : nullptr);
    if (argc >= 5 && std::strcmp(argv[1], "replay") == 0)
        // replay <rpy> <资源目录> <事件输出> [帧数] [stage 索引]
        // 同源默认（r4/harness）：RunScene 生产 tick 唯一路径，事件经
        // StageBattle::open_event_sink 产出，无 battle-only 绕路。
        return replay_runscene(argv[2], argv[3], argv[4],
                               argc >= 6 ? unsigned(std::atoi(argv[5])) : 0u,
                               argc >= 7 ? unsigned(std::atoi(argv[6])) : 0u);
    if (argc >= 4 && std::strcmp(argv[1], "draws") == 0)
        // draws <资源目录> <绘制输出> [帧数] [stage.ecl]
        return run_draws(argv[2], argv[3], argc >= 5 ? unsigned(std::atoi(argv[4])) : 3600u,
                         argc >= 6 ? argv[5] : nullptr);
    if (argc >= 5 && std::strcmp(argv[1], "draws-replay") == 0)
        // draws-replay <rpy> <资源目录> <绘制输出> [帧数] [stage 索引]
        return run_draws_replay(argv[2], argv[3], argv[4],
                                argc >= 6 ? unsigned(std::atoi(argv[5])) : 0u,
                                argc >= 7 ? unsigned(std::atoi(argv[6])) : 0u);
    if (argc >= 4 && (std::strcmp(argv[1], "title") == 0 || std::strcmp(argv[1], "title-b") == 0))
        // title/title-b <资源目录> <绘制输出>（窗长内置：A=600、B=转场请求帧）
        return run_title(argv[2], argv[3], std::strcmp(argv[1], "title-b") == 0);
    return rng_smoke();
}
