// 对局场景（Stage1 可玩化：HUD + StageBattle/StagePresentation + STD 舞台背景 +
// 帧钟 + Esc/失焦暂停请求）。ECL/ANM 数据驱动见 StageBattle/StagePresentation，
// 背景由 stXX.std/stXXwl.anm 驱动见 StageScenery；状态机合同见
// docs/plans/ui-base.md §四。
#pragma once

#include "GameHud.hpp"
#include "HudDrawData.hpp"
#include "HudFrameData.hpp"
#include "MsgPlayer.hpp"
#include "MsgPresentation.hpp"
#include "PostGameFlow.hpp"
#include "SessionState.hpp"
#include "StageBattle.hpp"
#include "StagePresentation.hpp"
#include "StageScenery.hpp"
#include "ScreenShake.hpp"
#include "FrameScheduler.hpp"
#include "AnmDrawSchedule.hpp"
#include "ScreenViews.hpp"
#include "SpellPresentation.hpp"
#include "LaserPresentation.hpp"

namespace th13 {

struct RunSceneServices : HudDrawServices, HudFrameServices, StageBattleServices,
                          AnmDrawServices {
    // HudFrameServices 与 StageBattleServices 同名同参声明 hud_sound（两侧
    // 语义同为「宿主播放 HUD 提示音」），多基类名查找二义。此处纯覆写合一：
    // 单一最终覆写（ApplicationState::hud_sound）同时覆盖两基类槽位。
    bool hud_sound(i32 id) override = 0;
    virtual bool request_pause() = 0;
    virtual bool request_practice_end() { return false; }
    virtual bool record_spell_event(const SpellEvent&) { return true; }
    virtual bool record_spell_practice_score(i32) { return true; }
    virtual bool record_stage_clear() { return true; }
    virtual bool request_stage6_fade() { return false; }
    virtual bool request_extra_results() { return false; }
    virtual bool open_game_fade(bool, i32, bool) { return false; }
    virtual bool dialogue_anchor_y(float) { return true; }
    virtual bool rasterize_spell_name(SpellPresentation&) { return false; }
    virtual void spell_counts(i32, i32& captures, i32& attempts) const {
        captures = attempts = 0;
    }
    // 对局终了链路的场景请求（raw = DAT_004dcc24 真值：0xc 下一关 / 0xf 结局 /
    // 4 标题 / 0xa 重试…全表见 results-flow.md §1）→ Application 接线
    //（PostGamePlatform 经 RunScene 转发到本接口）。
    virtual bool request_destination(i32 raw) {
        (void)raw;
        return false;
    }
    // 交互装载：ECL/ANM 字节（经宿主资源通道，浏览器侧 /assets）。
    virtual bool read_asset(const std::string& name, std::vector<u8>& bytes, bool required = true) = 0;
    // STD 舞台背景支撑：图形设备 / 背景 ANM bank 装载（宿主侧 preload 一体）。
    virtual ZunGraphics& stage_graphics() = 0;
    virtual bool preload_scenery_animation(i32 bank, const std::string& name) = 0;
    // 对话音乐面（0x43ae40/0x43af70 → AudioDevice 曲命令）：code 2 = 播预载槽
    // （value = 槽号），code 5 = 淡出（value = 秒）。默认 false = 宿主无音频面
    //（产品宿主 ApplicationState 覆写；无头测试宿主不受影响）。
    virtual bool queue_music(i32 code, i32 value, const std::string& name) {
        (void)code;
        (void)value;
        (void)name;
        return false;
    }
    // 对话文本渲染服务：将保留 CP932 原文与 ruby 元数据的 MsgLine 提交给宿主
    virtual bool dialogue_line(i32 row, const MsgLine& line, float box_x, float box_y) {
        (void)row;
        (void)line;
        (void)box_x;
        (void)box_y;
        return true;
    }
    // opcode 6/18 的文本释放（0x42960c label1 / 0x429250 label3）：
    // 只 retire 对话框根 VM 并对四条文本 VM 发 label，不清 texture0 像素。
    virtual bool dialogue_fade(i32 label) {
        (void)label;
        return true;
    }
    // rows==0 兜底（原版 opcode 6 语义）：淡出保留像素。
    virtual void dialogue_clear() {}
    // entry 销毁/重开（0x4282b0 对象回收）：retire 四条文本 VM。
    virtual void dialogue_end() {}
};

// 交互装载供给：read_asset → EclResourceProvider（ECL 递归 include + ANIM 名单）。
class RunSceneAssets final : public EclResourceProvider {
public:
    RunSceneServices* host = nullptr;
    StageSim* sim = nullptr;
    std::vector<std::string>* anim_list = nullptr;
    bool read(const std::string& name, std::vector<u8>& data) override {
        return host && host->read_asset(name, data);
    }
    bool animation(u32 slot, const std::string& name) override {
        // 装载核验（0x4795b0 注册序的行为子集）：ANM 必须可读；slot 从 10 起
        // 记名——ECL 302 引用的文件槽 = 2 + 名单序（st01：2=enemy、3=st01enm）。
        std::vector<u8> probe;
        if (!host || !host->read_asset(name, probe)) {
            if (sim) sim->error_ = "missing ANM dependency: " + name;
            return false;
        }
        (void)slot;
        if (anim_list) anim_list->push_back(name);
        return true;
    }
};

class RunScene final : private StageDialogHost, private MsgHost, private PostGamePlatform {
    SessionState& progress;
    AnmManager& animations;
    AnmRenderer& scene_renderer;
    AnmEnvironment& environment;
    RunSceneServices& host;
    FrameScheduler& scheduler;
    HudPlayerState& player_state;
    HudScoreState& score_state;
    std::array<FrameCallback, 3> callbacks;
    std::array<FrameCallback, 2> spell_callbacks{};
    FrameCallback laser_update{};
    struct ShakeCallback {
        RunScene& scene;
        ScreenShake effect;
        FrameCallback callback;
        ShakeCallback(RunScene& owner, i32 duration, i32 start, i32 end)
            : scene(owner), effect(duration,start,end) {}
    };
    std::vector<std::unique_ptr<ShakeCallback>> shakes;
    bool start_shake(i32 duration, i32 start, i32 end);
    std::unique_ptr<LaserPresentation> laser_presentation;
    bool update_lasers();
    std::unique_ptr<SpellPresentation> spell_presentation;
    bool update_spell();
    bool draw_spell();
    struct StageDraw {
        FrameCallback callback;
        RunScene* owner = nullptr;
        u32 pass = 0;
    };
    std::array<StageDraw, 5> stage_draws;  // 自机/自机弹/敌机/敌弹/激光直绘
    std::array<StageDraw, 2> scenery_draws;  // STD 背景（背景组/前景组）
    i32 scenery_bank = -1;
    std::string scenery_std;
    FrameCallback scenery_update{};
    bool update_scenery();
    // 对局输入（0x4e49f0 单一全局输入对象的 port 等价）：唯一 GameInput 实例
    // 由 BrowserRuntime 采样并按 00448200/00414f30 相位每游戏帧 update 一次，
    // 经 Application::step 传入本场景（controls 存指针）；本类不自持第二
    // GameInput 实例（task 卡 stage1-replay-runtime 禁止 P3）。update 前未
    // controls() 过则消费零输入（仅测试直调路径）。
    const GameInput* input_frame = nullptr;
    // S3 对局终了链路：msg 播放器（Boss 前后对话）+ 对局内覆盖层骨架。
    MsgFile dialog_file;
    MsgPlayer dialog;
    bool dialog_ready = false;  // msg 资产缺失时降级为对话缺席（519 即过）
    i32 dialogue_rows_ = 0;     // 上一 update 已提交的正文行数（0 = 无文本）
    // 对话立绘/名牌 VM 面（0x428ae0 opcode 1/2/20/31 可视指令；见 MsgPresentation）。
    std::unique_ptr<MsgPresentation> msg_presentation;
    PostGameFlow post_flow;
    // The HUD survives a stage transition (0042bfa0's run_flags bit1 path).
    // Direct scene users may still give this scene its own HUD.
    std::unique_ptr<GameHud> owned_hud;
    bool fail(const std::string&);

public:
    // 自机弹精灵（plXX.anm 内 16×16 御札帧；.sht 弹型表接入前的可玩化近似）。
    static constexpr i32 kPlayerShotSprite = 31;
    GameHud& hud;
    // 共享 game RNG 注入（0x4dc324 位）：ECL/弹幕/自机弹出膛/道具掉落同源；
    // 缺省空 = StageBattle 自持（tests 无头路径不变）。
    StageBattle battle;
    std::unique_ptr<StagePresentation> presentation;
    std::unique_ptr<StageScenery> scenery;  // STD 舞台背景（stXX.std 驱动）
    std::unique_ptr<StageScenery> previous_scenery;
    i32 previous_scenery_bank = -1;
    bool transition_wait = false, scenery_started = true;
    bool prepare_transition(i32 bank, const std::string& name);
    bool start_transition();
    void preserve_presentation_from(RunScene& old);
    std::function<void(const GameInput&)> record_input;
    std::string battle_sht;  // 移速源 .sht（回放会话注入；空 = 模型默认 pl02a 定标）
    u32 input_held = 0, input_pressed = 0, input_repeated = 0;
    Timer age{0, 0, 0, 0, 0};
    std::string error;
    RunScene(SessionState&, HudPlayerState&, HudScoreState&, AnmManager&, AnmRenderer&,
             AnmEnvironment&, RunSceneServices&, FrameScheduler&,
             StagePresentation::Banks banks, i32 front_bank, i32 ascii_bank,
             i32 scenery_bank, const std::string& std_name,
             GameRandom* shared_random = nullptr, GameHud* shared_hud = nullptr,
             StagePlayerState* shared_player = nullptr);
    ~RunScene();
    RunScene(const RunScene&) = delete;
    RunScene& operator=(const RunScene&) = delete;
    void controls(const GameInput& input) { input_frame = &input; }
    // Replay consumption belongs behind the gameplay freeze gate. Null
    // means the stream ended and its end overlay owns this update.
    std::function<const GameInput*()> playback_input;
    bool initialize(const HudEntryContext& = {});
    bool update();
    bool draw();
    bool draw_stage(u32 pass);  // 战斗直绘（initialize 注册的 4 个 Draw 回调）
    bool draw_scenery(u32 pass);  // STD 背景（层 0-7 背景组 / 8-11 前景组）
    void request_pause() { host.request_pause(); }
    void request_game_over() { host.request_game_over(); }
    void open_dialog(i32 entry) override;
    bool dialog_active() const override { return dialog.active(); }
    bool dialog_wait_released() const override { return dialog.ecl_wait_released(); }

private:
    // MsgHost（msg opcode 回调；21 → 0x42cd40 关终了）
    void msg_bgm(i32 opcode) override;         // 2/19/20/31 分派（0x429c7c 语义）
    void msg_portrait_load() override;         // opcode 1：自机立绘根 VM
    void msg_text_anchor_y(float value) override;
    void msg_stage_clear() override;           // → PostGameFlow::on_stage_clear
    // 追加可视化通知钩子（MsgHost 默认空实现 → 本类按 0x428ae0 证据落 ANM 面）
    void msg_portrait_interrupt(i32 opcode, i32 label) override;  // 13/14 表情切换
    void msg_speaker_switch(i32 opcode) override;                 // 7/8 亮暗切换
    void msg_release(i32 opcode) override;                        // 4/5/6/18/23/24
    void msg_music_fade(i32 opcode, float seconds) override;                     // 22/27 曲淡出
    void msg_advance_blip() override;                             // 0x460bc0(0)
    void msg_dialog_end() override;                               // 终止 → 全释放
    // PostGamePlatform
    void request_destination(i32 raw) override {
        // 0x42cd40 的 supervisor 投递：失败即入场景错误（dcc24 请求不能静默丢）。
        if (!host.request_destination(raw))
            fail("Post-game destination request failed: " + std::to_string(raw));
    }
    bool request_practice_end() override {
        if (!host.request_practice_end()) {
            fail("Practice end request failed");
            return false;
        }
        return true;
    }
    bool request_stage6_fade() override { return host.request_stage6_fade(); }
    bool request_extra_results() override { return host.request_extra_results(); }
    void bgm(i32 id) override { msg_bgm(id); }
};

}  // namespace th13
