// 标题场景（th15 TitleScene 裁剪：全部原版标题屏共享一份光标历史、动画注册表
// 与回调调度器；平台拥有文件/音频/GPU 工作并排队 destination）。
#pragma once

#include "TitleFrame.hpp"
#include "TitleMainMenu.hpp"
#include "TitlePlayerDataMenu.hpp"
#include "TitlePlayerDataView.hpp"
#include "TitleReplayMenu.hpp"
#include "TitleResultEntry.hpp"
#include "TitleResultView.hpp"
#include "TitleSelectionMenu.hpp"
#include "TitleOptionsMenu.hpp"
#include "TitleKeyConfigMenu.hpp"
#include "TitleMusicRoomMenu.hpp"
#include "TitleMusicRoomView.hpp"
#include "TitleManualMenu.hpp"
#include "TitleSpellPracticeMenu.hpp"
#include "TitleSpellPracticeView.hpp"
#include "AnmText.hpp"
#include "HudDrawData.hpp"

#include <memory>
#include <utility>

namespace th13 {

// 平台聚合：Replay 目录/导入/启动 + Option 音量/持久化 + 音楽室曲目/听过位
// + Manual help 查看器服务随场景平台一并注入（TitleReplayPlatform/
// TitleOptionPlatform/TitleMusicRoomPlatform/TitleManualPlatform 全默认实现，
// 空宿主不受影响；数据源见 r4/menu-extras/integration.md）。
struct TitleScenePlatform : TitleFramePlatform, TitleReplayPlatform, TitleOptionPlatform,
                            TitleMusicRoomPlatform, TitleManualPlatform, TitleMenuServices,
                            TitlePlayerDataPlatform, TitleSpellPracticePlatform,
                            HudDrawServices {
    virtual bool sound(i32) = 0;
    virtual bool start_spell_practice(const SpellPracticeCommand&) { return false; }
    virtual bool can_start_spell_practice(const SpellPracticeCommand&) const { return false; }
    virtual bool record_menu_views() const { return false; }
    virtual bool rasterize_title_text(AnmVm&, u32, const std::string&,
                                     AnmTextAlignment = AnmTextAlignment::Left) { return false; }
    // [通关结算] 0x454050/0x4549d0 宿主面（默认未接 = 不假成功）。前三个签名
    // 与 RunPausePlatform 现有 override 完全一致，ApplicationState 的同一组
    // override 同时满足两个虚槽。
    virtual bool save_replay_slot(i32, const std::string&) { return false; }
    virtual bool can_save_replay() const { return false; }
    virtual bool record_result_name(const std::string&) { return false; }
    // 全通结算（0x454050，先 DAT_004be81c=8 再插入）与对局内插入
    //（0x43f8f0，当前关）stage 字节不同：全通走此独立槽。
    virtual bool record_clear_result_name(const std::string&) { return false; }
    // 0x454050 state3 的 DAT_004be7c8（续关数）==0 门：true → 流程 0xf =
    // 0x4549d0（回放槽保存）；false → 回主菜单。默认 false = 宿主未接查询。
    virtual bool result_replay_save_available() const { return false; }
    // 0x454050 case0 的 score 块读取（10 行 + 当前插入预览，见
    // TitleResultView.hpp）：默认未接 = 不返回伪造记录；结算屏仍按空表显示/
    // 编辑（测试与无存档宿主）。
    virtual bool result_screen_data(TitleResultScreenData&) const { return false; }
};

class TitleScene final : private TitleFrameServices, private TitleMenuServices {
public:
    TitleState state;
    std::string error;

private:
    SessionState& progress;
    TitleSelectionSettings& settings;
    AnmManager& animations;
    TitleScenePlatform& host;
    FrameScheduler& scheduler;
    TitleSceneResources resources;
    TitleFrameInput input;
    TitleMainMenu main;
    TitleDifficultyMenu difficulty;
    TitleCharacterMenu character;
    TitleStageMenu stage;
    FrameCallback stage_text_draw{};
    TitleReplayMenu replay;
    TitleOptionVolumes option_volumes;  // 音量真值（宿主 FileStore 预填，菜单共享编辑）
    TitleOptionsMenu option;
    // キー配置屏（0x44fcc0）：手柄原始位图/五键存档由 Application 在
    // open_key_config 时经 connect_key_config 注入；未接线 = 默认失败门。
    TitleKeyConfigHost key_config_host;
    TitleKeyConfigMenu key_config;
    TitleMusicRoomMenu music_room;
    std::unique_ptr<TitleMusicRoomView> music_room_view;
    TitleManualMenu manual;
    // 通关结算屏（0x44c570 case 0xe/0xf → 0x454050/0x4549d0）。
    TitleResultEntry result;
    // PlayerData/SpellPractice 路由骨架：宿主数据平台由 TitleScenePlatform 聚合
    // 注入；菜单在每次进屏时新建（TitlePlayerDataMenu 收面把 substate 停在 3，
    // TitleSpellPracticeMenu 构造即固化三级目录，复用会带入上次浏览态）。
    std::unique_ptr<TitlePlayerDataMenu> player_data;
    std::unique_ptr<TitlePlayerDataView> player_data_view;
    std::unique_ptr<TitleSpellPracticeMenu> spell_practice;
    std::unique_ptr<TitleSpellPracticeView> spell_practice_view;
    i32 spell_menu_delay_ = 0;
    SpellPracticeInput delayed_spell_input_;
    std::string spell_practice_launch_status_;  // Host rejection keeps the selection available.
    SpellPracticeCommand pending_spell_;
    // [H-SP2] 对局返回级联暂存：restore_spell_practice_selection 反查槽位后
    // 由 update_spell_practice 建表时消费（DAT_004bb4c4/c8/cc 对应物）。
    SpellPracticeCommand spell_restore_{};
    bool spell_restore_pending_ = false;
    i32 spell_launch_age_ = -1;
    bool check(bool, const std::string&);
    // TitleFrameServices
    bool clear_title_overlay() override;
    bool queue_title_music(i32 code, i32 value, const std::string&) override;
    bool clear_current_wave() override;
    bool title_exit(i32) override;
    bool fade_out_title_music() override;
    bool update_title_menu(TitleScreen, const TitleFrameInput&) override;
    bool update_player_data(const TitleFrameInput&);
    bool update_spell_practice(const TitleFrameInput&);
    bool restore_practice_selection() override;
    bool restore_spell_practice_selection() override;
    bool draw_title_replay_texts() override;
    bool draw_title_music_room_texts() override;
    // TitleMenuServices
    bool sound(i32) override;
    bool begin_transition(u32&) override;
    bool start_game(i32) override;
    bool character_cleared(i32 character, i32 difficulty) override;
    bool extra_unlocked(i32 character) override;
    bool practice_stage_played(i32 character, i32 difficulty, i32 stage) override;

public:
    // Option 屏观测（row/volumes/error 采样；接线与测试用，不提供可变访问）。
    const TitleOptionsMenu& option_menu() const noexcept { return option; }
    // M1/T4 F1″：commit 期 persist_options 存档失败补记通道。原访问链三处
    // 封闭——option private（本类）、上访问器 const、TitleOptionsMenu::fail
    // private——ApplicationState::commit_pending_file_events 须经本 header-only
    // 非常量访问器才能对 public 的 error 做 set-if-empty（逐位复刻
    // TitleOptionsMenu.cpp:113 fail 的 `if (error.empty()) error = reason`
    // 语义）。零行为变化：只是放开一条只写 error 的通路。
    TitleOptionsMenu& option_menu() noexcept { return option; }
    // キー配置屏观测（row/bindings/error 采样）。
    const TitleKeyConfigMenu& key_config_menu() const noexcept { return key_config; }
    // 宿主接线（ApplicationState::open_key_config）：手柄存在/原始位图/当前
    // 五键/落盘服务。未调用时 TitleKeyConfigHost 默认 false/空，不假成功。
    void connect_key_config(std::function<bool()> present, std::function<u32()> buttons,
                            std::function<std::array<i16, 5>()> bindings,
                            std::function<bool(const std::array<i16, 5>&)> persist) {
        key_config_host.present = std::move(present);
        key_config_host.buttons = std::move(buttons);
        key_config_host.bindings = std::move(bindings);
        key_config_host.persist = std::move(persist);
    }
    // 音楽室观测（row/error 采样）。
    const TitleMusicRoomMenu& music_room_menu() const noexcept { return music_room; }
    // マニュアル观测（error 采样；wrapper≠真实 help viewer 完成）。
    const TitleManualMenu& manual_menu() const noexcept { return manual; }
    // PlayerData/SpellPractice 观测（路由接线与测试采样；空指针 = 尚未进屏）。
    const TitlePlayerDataMenu* player_data_menu() const noexcept { return player_data.get(); }
    const TitleSpellPracticeMenu* spell_practice_menu() const noexcept {
        return spell_practice.get();
    }
    // SpellPractice Launch：原版 0x4576c0 启动拍保留命令，但不接真实对局路由；
    // 非空状态 = 明确「未实现/未启动」，不是错误。
    bool spell_practice_launch_pending() const noexcept {
        return !spell_practice_launch_status_.empty();
    }
    const std::string& spell_practice_launch_status() const noexcept {
        return spell_practice_launch_status_;
    }
    TitleFrame frame;
    TitleScene(SessionState&, TitleSelectionSettings&, AnmManager&, TitleScenePlatform&,
               FrameScheduler&, TitleSceneResources = {});
    ~TitleScene();
    TitleScene(const TitleScene&) = delete;
    TitleScene& operator=(const TitleScene&) = delete;
    void controls(const TitleFrameInput& value) noexcept {
        input = value;
        frame.controls(value);
    }
};

}  // namespace th13
