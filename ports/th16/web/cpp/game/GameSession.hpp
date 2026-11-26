// th16 游戏会话——不依赖 SDL/GL 的逻辑层：档案 + ANM 管理器 + Supervisor + Chain + 标题菜单。
// Application（浏览器）与 tests/harness（WASI/本机）共用同一份时序，保证无头对拍与上屏一致。
//
// 原版对应：0x43ba40 注册主更新链 → 0x43b3d0 每帧泵 → 0x43ce10 场景切换；
// 启动幕 0x43adc0/0x43b300/0x43b290；标题 0x44aee0/0x44abc0/0x44ac70/0x44b5f0；
// 过关换关 0x43ce10 case 0xc → 0x42d780（0x42d200 bit1 路径）→ 0x42d700 + 0x42cb60（DAT_004c17d0 == 0）。
#pragma once
#include "AnmGeometry.hpp"
#include "AnmManager.hpp"
#include "AnmRenderer.hpp"
#include "AnmResource.hpp"
#include "Archive.hpp"
#include "Ascii.hpp"
#include "Chain.hpp"
#include "Effect.hpp"
#include "EndingScene.hpp"
#include "ScreenEffect.hpp"
#include "Music.hpp"
#include "Sound.hpp"
#include "Spellcard.hpp"
#include "Input.hpp"
#include "Supervisor.hpp"
#include "GameGlobals.hpp"
#include "GameManager.hpp"
#include "GameMenu.hpp"
#include "FrameTimer.hpp"
#include "Replay.hpp"
#include "ReplayFile.hpp"
#include "ReplayGlobals.hpp"
#include "ReplayRecording.hpp"
#include "ScoreFile.hpp"
#include "ScreenCompositor.hpp"
#include "TextPaint.hpp"
#include "TitleScene.hpp"

#include <functional>
#include <memory>
#include <string>
#include <vector>

namespace th16 {

class GameSession {
public:
    // 资源装入后的回调（浏览器端建纹理；无头端空）
    using OnResourceLoaded = std::function<bool(AnmResource&)>;

    // Supervisor 先于 animations：AnmManager 的 game_speed 引用 Supervisor::game_speed（原版同一个全局量 DAT_004a5788）
    Supervisor supervisor;
    AnmManager animations{supervisor.game_speed};
    AsciiManager ascii;           // DAT_004a6d98（0x407cb0：启动期创建，更新链 4，绘制链 0x51 / 0x35 / 0x42）
    EffectManager effects;        // DAT_004a6db8（0x418a30：启动期创建，更新链 0x1f）
    Spellcard spellcard;          // DAT_004a6db0（0x4178a0：进游戏时创建；敌机 / 自机被弹共用其标志）
    ScreenShakeOffset screen_shake;  // DAT_004c1788 / DAT_004c1530：ECL 517 画面震动当前偏移
    std::vector<std::shared_ptr<ScreenFade>> screen_fades;  // 存活的淡入淡出 ScreenEffect（绘制链 0x54 全屏色块；渲染侧读）
    std::vector<TextIntoTextureRequest> text_into_texture;  // 0x46d990 / 0x46dab0：逻辑帧入队，宿主绘制前刷入纹理
    ChainManager chains;
    GameGlobals globals;
    TitleScene title;
    EndingScene ending;            // 场景 0xf：DAT_004a6dbc
    GameManager game;              // 场景 7：DAT_004a6dd4
    GameMenu menu;                 // DAT_004a6ef4（0x43e5a0：新开局时与各管理器一起建，换关沿用，离开游戏时销毁）
    bool menu_live = false;
    bool ending_live = false;
    ScoreFile scores;              // DAT_004a6f0c
    u32 config_flags = 0;          // DAT_004c1800（bit4 失焦自动暂停 / bit13 回标题改为退出）
    StageCamera stage_camera;      // DAT_004c168c：当前运行中关卡每帧抄出的全局相机（ANM 变量 10016..10021）
    bool game_live = false;
    bool quit_requested = false;   // 场景 2/3：原版退出进程；宿主据此停循环
    std::vector<TitleScene::Event> title_events;  // 离开标题前的事件归档（harness 断言用）
    std::string failure;
    u32 injected_input = 0;   // 外部注入 held 位（与宿主键盘 OR）
    u32 host_input = 0;       // 宿主键盘采样
    u8 keyboard_state[256] = {};   // 宿主键盘快照（DIK 下标，bit7 = 按下）：0x402130
    u32 pad_buttons = 0;           // 0x401c30 手柄按钮快照（bit i = 按钮 i 按下）；0x4018e0 按 GameGlobals::pad_buttons 合成位
    InputState game_input;    // DAT_004a52c8：Replay 记录链（优先级 0x10，0x447fd0）每帧抄 held 低 16 位并做边沿
    // 逻辑侧屏幕几何（DAT_004d9d34 窗口缩放 / DAT_004d9d50,54 场地偏移 / DAT_004d9d48,4c）：自机弹出界判定 0x468c00 用；
    // 原版由 Supervisor 绘制链 0x43b3d0 每帧写为 640/2、(480-448)/2，这里取同值常量
    AnmGeometry geometry;
    // 绘制侧：AnmManager 绘制族（0x468490）与 Supervisor 画面合成；renderer.device 为空时绘制链只推进状态不出图
    AnmRenderer renderer{animations, geometry};
    ScreenCompositor compositor;
    // 挂上设备后绘制链才真正出图（Application / harness game-draw）；可为空
    void attach_device(RenderDevice* device) { renderer.device = device; }
    SoundEffects sound;                // 原版 SoundManager 音效部分：请求队列 / 声部状态（宿主通过 sound.output 出声）
    MusicManager music;                // 原版 SoundManager 乐曲部分：命令队列 / thbgm.fmt（宿主通过 music.output 串流）
    // 0x45e330：乐曲命令推进一步 + 音效队列消费，返回队头命令 kind（0 = 空）；drain_sound 为 do … while (ret != 0)
    i32 process_sound();
    void drain_sound();
    void unload_bgm();
    SoundRequests sfx_events;          // 本帧 0x45e150 / 0x45e1f0 音效登记（全部管理器，调用序；harness 对拍取 ids）
    std::vector<i32> player_sfx_events; // 其中由 Player 代码段（0x440000..0x449000：更新/被弹/擦弹）登记的子集
    bool run_draw_chains = true;  // harness 对拍 oracle（不跑 0x4015a0）时关掉
    // 0x45a8a0 一帧：更新链 → BeginScene → 绘制链 → 出批 → EndScene；run_draw_chains 为假时跳过绘制链
    void draw_frame();
    // harness 提供 enemies 时间表时置 false，由桩保持 PlayerWorld.enemy_count（对齐旧桩语义）
    bool sync_enemy_count = true;
    OnResourceLoaded on_loaded;

    GameSession();   // 0x43ba40 / 0x46a3a0：登记常驻绘制链
    bool open(const u8* archive_bytes, u32 size);
    bool begin_boot();                 // 0x43adc0：ascii + sig，注册启动幕绘制链，请求 Title
    // 跳过启动幕/标题直接进入场景 7（harness）：装启动 bank 后按 globals（stage/shot/season/difficulty）装载关卡
    bool start_game_direct(i32 replay_mode = 0);
    // 0x448c10：装入回放文件；随后 enter_game(1) / start_game_direct(1) 走回放输入链
    bool load_replay(const u8* bytes, u32 size);
    bool load_replay_file(const char* path);
    // 0x43ce10 case 0xd / 0x447760(1)：按回放取开局关卡（stage_choice < 0 取首段）与用户块中的机体 / 难度 / 季节
    void apply_replay_start(i32 stage_choice);
    // 标题 demo 目录（内含 demo/demo1.rpy…）；空则跳过自动装载；亦为 th16_XX.rpy 写出根目录
    std::string demo_directory;
    // 回放槽与成绩文件的读写根目录；空则同 demo_directory（浏览器宿主指向 IDBFS 挂载点）
    std::string save_directory;
    const std::string& replay_directory() const { return save_directory.empty() ? demo_directory : save_directory; }
    // 写盘成功后通知宿主（浏览器 IDBFS 回写）
    std::function<void()> on_saved;
    // 0x448400：把本局记录写入 replay_directory()/replay/th16_XX.rpy（或根目录回退）
    bool save_replay_slot(int slot, const char* player_name);
    void restamp_replay_globals() { if (game.mode != 1) replay_recording_.restamp_globals(globals); }
    // scoreth16.dat 路径；空则不读不写（harness 默认）。启动幕 0x4497e0 读、0x42d200 / 0x43afe0 经 0x449a00 写
    std::string score_path;
    bool save_scores();   // 0x449a00
    // th16.cfg 路径；空则不读不写（harness 默认）。0x43c050 开机读+校验+回写；退出（场景 Exit）再写
    std::string config_path;
    bool load_config();   // 返回文件是否合法；不合法 / 缺失时 globals 已回配置默认值
    bool save_config();
    double play_clock_origin = 0.0;   // DAT_004a5c00：上次计时时刻（秒）
    void accumulate_play_time();      // 0x42dbc0
    void restart_play_clock();        // 0x45b130 → DAT_004a5c00（0x42cb60 末 / 0x43f6a0 / 0x43f740 / 0x43f790）
    // 宿主墙钟（秒）。<0 = harness/oracle 口径：跳过 0x4262f0 / 真实 0x417bc0
    double wall_clock_seconds = -1;
    i32 skip_frames = 0;                 // DAT_004c12c4：本逻辑帧额外计数（跳帧）
    FrameTimer frame_timer;              // DAT_004a6dc8
    // 0x43b290：AsciiManager 的更新链 4 与绘制链 0x51 / 0x35 / 0x42 在启动幕装载完成时一起打开
    void enable_ascii_chains();
    // 0x43ce10 case 0xa / 0xb / 0xe / 0x13：GameMenu 重开 / Extra 续关——0x42d200 按请求决定快速重开（bit0）或整体销毁，
    // 再以 stage_copy 新开局
    bool restart_game(SceneId reason, i32 replay_mode);
    bool logic_step();                 // 一个逻辑帧（输入 → 切换 → 链 → ANM tick）
    const Archive& archive() const { return archive_; }
    Archive& archive() { return archive_; }
    AnmResource* resource(const char* name);

private:
    Archive archive_;
    std::vector<std::unique_ptr<AnmResource>> resources_;  // 地址稳定：AnmManager 持有 resource 指针；逐关累加不设上限
    std::vector<std::string> resource_names_;
    ChainEntry* boot_draw_chain_ = nullptr;
    ChainEntry* ascii_update_chain_ = nullptr;        // 4（0x408070）
    ChainEntry* ascii_draw_chains_[3] = {};           // 0x51 / 0x35 / 0x42（0x408090 / 0x4080a0 / 0x4080b0）
    ChainEntry* effect_update_chain_ = nullptr;
    ChainEntry* title_update_chain_ = nullptr;
    ChainEntry* ending_update_chain_ = nullptr;       // 0x23（0x4191f0 → LAB_004197a0）
    ChainEntry* ending_draw_chain_ = nullptr;         // 0x43（0x4191f0 → LAB_004197b0）
    ChainEntry* game_update_chain_ = nullptr;
    ChainEntry* replay_record_chain_ = nullptr;       // 0x10（0x448e20 → 0x447fd0 记录）
    ChainEntry* replay_playback_chain_ = nullptr;     // 0x22（0x448e40：回放中按住射击则 Restart 整条更新链快进）
    ChainEntry* replay_draw_chain_ = nullptr;         // 0x47（0x448e90 → 0x4482f0 回放 FPS 文字）
    ChainEntry* global_fps_draw_chain_ = nullptr;     // 0x4b（0x426490 → 0x4263e0：全局 "%2.1ffps"）
    i32 replay_ff_counter_ = 0;                      // ReplayManager+0x20c：快进 Restart 计数（%8）
    u8 replay_fps_byte_ = 60;                        // ReplayManager+0x208：最近测得帧率（记录链每 30 帧写）
    float global_fps_ = 60.0f;                       // FrameTimer+0x30（0x4262f0 测速；无墙钟时固定 60）
    ReplayFile replay_file_;                         // 0x448c10 展开后的回放
    ReplayPlaybackCursor replay_cursor_;             // 当前关卡段读指针（+0xc8 / +0xd8）
    i32 replay_spell_time_(i32 index) const;
    i32 replay_next_stage_index_ = -1;               // 换关 0x449030(1) 已备好、待 0x448eb0 才切过去的段（+0x214 之前不动）
    u16 replay_stage_seed_ = 0;                      // 记录：0x447760 / 0x449030 装载时抄进新段头 +2 的 DAT_004a6d88
    bool replay_file_loaded_ = false;
    ReplayRecording replay_recording_;               // 非回放局 0x447fd0 缓冲；0x448400 经 ReplayStore 落盘
    std::unique_ptr<ReplayStore> replay_store_;       // 目录型 th16_XX.rpy
    ChainEntry* player_update_chain_ = nullptr;
    ChainEntry* player_draw_chain_ = nullptr;
    ChainEntry* enemy_update_chain_ = nullptr;
    ChainEntry* bullet_update_chain_ = nullptr;
    ChainEntry* bullet_draw_chain_ = nullptr;
    ChainEntry* item_update_chain_ = nullptr;
    ChainEntry* item_draw_chain_ = nullptr;
    ChainEntry* item_draw_season_chain_ = nullptr;
    ChainEntry* laser_update_chain_ = nullptr;
    ChainEntry* laser_draw_chain_ = nullptr;
    ChainEntry* spellcard_update_chain_ = nullptr;
    ChainEntry* popup_update_chain_ = nullptr;        // 0x14（0x44a440 → 0x449ea0 得分飘字）
    ChainEntry* popup_draw_chain_ = nullptr;          // 0x2f（0x44a450 → 0x44a000）
    ChainEntry* bomb_update_chain_ = nullptr;         // 0x18（0x40dd00 主炸弹）
    ChainEntry* release_update_chain_ = nullptr;      // 0x19（0x40dd00 季节解放）
    ChainEntry* bomb_draw_chain_ = nullptr;           // 0x28（0x40da70）
    ChainEntry* release_draw_chain_ = nullptr;        // 0x29
    ChainEntry* gui_update_chain_ = nullptr;
    ChainEntry* gui_draw_chains_[2]{};                // 0x33（0x426b00 +0x8：空回调）/ 0x30（0x428e70 HUD 文字）
    ChainEntry* spellcard_draw_chain_ = nullptr;      // 0xc（0x417d70 符卡奖励 / 取得数文字）
    ChainEntry* menu_update_chain_ = nullptr;         // 10（0x43e720 → 0x43e5f0）
    ChainEntry* menu_draw_chain_ = nullptr;           // 0x4a（0x43ef10 → 0x43edc0）
    ChainEntry* ui_tick_chain_ = nullptr;             // 9（0x46e740 → 0x46e490：UI 表 VM 推进）
    ChainEntry* world_tick_chain_ = nullptr;          // 0x21（0x46e710 → 0x46e340：世界表 VM 推进）
    std::vector<ChainEntry*> shake_chains_;  // 0x45d150(1) 实例（链 0x13，自删）
    std::vector<ChainEntry*> fade_chains_;   // 0x45d150(0/2/3/5) 实例（链 0x13，自删）

    bool load_anm(const char* name, AnmResource*& out);
    bool load_boot_banks();            // 0x43adc0 / 0x407cb0 / 0x43ad20 / 0x418790 的同步装载部分
    bool create_player();              // 0x42cb60 新游戏段：0x441c60 + 火力/量表初值 + 0x4440e0
    bool enter_title_assets();
    bool activate_title_menu();
    void leave_title();
    void mark_stage_reached();  // 0x42cb60 +0x519d
    bool enter_game(i32 replay_mode = 0);  // 0x43ce10 case 7 → 0x42d700(回放标志) + 0x42cb60（同步装载）
    bool bind_loaded_replay_();            // 0x448c10 成功后：选段、记 RNG 种子、复位游标
    const ReplayStageSegment* current_replay_segment_() const;   // 回放中的当前段（段头完整时）
    void replay_stage_start_();            // 0x448eb0 段头读回（回放）/ 写入（录制）
    bool next_stage();     // 0x43ce10 case 0xc：0x42d780 → 0x42d200（bit1）；0x42d700 + 0x42cb60 换关装载
    void leave_game();     // 0x42d200
    bool enter_ending(bool extra);  // 0x43ce10 case 0xf / 0x10 → 0x419640；Extra 直接回标题
    void leave_ending();

    // 0x42cb60 子步骤：0x409db0 Stage（STD + 背景 ANM 槽 3 + (stage & 1)，链 0x11）/ 0x41b340 ECL（链 0x1a）/
    // 0x426c10 Gui 关卡部分（logo 槽 6 + 机体 msg）/ 0x4178a0 Spellcard 绑定
    bool create_stage();
    bool create_enemies();
    void spawn_loading_vm(const Vec3& pos);   // 0x41a390 / 0x42d200："Loading" VM（不存在时才建）
    // 0x40d890：ECL 之后、Spellcard 之前建主炸弹 + 季节解放，挂更新链 0x18 / 0x19 与绘制链 0x28 / 0x29
    void create_bombs();
    // 0x40da90（0x42d200 无条件调用）
    void destroy_bombs();
    // 敌机伤害修正和道具吸引在各自的更新链前读取炸弹 / 解放状态。
    void sync_bomb_state();
    // 炸弹代码段登记的音效 / Gui 通知 / 画面震动（0x45d150 kind 1 与 kind 8）
    void harvest_bomb_events();
    // 0x45d150(8, amplitude, attack, sustain, decay) + 0x45cf10：炸弹震动包络（链 0x13，visual_rng）
    void create_screen_shake_envelope(const BombShakeRequest& request);
    bool load_gui_stage_assets(AnmFile*& logo, std::vector<u8>& msg_bytes);
    void bind_spellcard();
    // 0x43e5a0 + 0x43e3f0：GameMenu 上下文绑定与更新链 10
    void create_menu();
    void harvest_menu_events();
    void register_game_chains();   // 0x42cb60 末：GameManager 链 0xf
    void unload_stage_anm(StageManager& stage);  // 0x4099a0：背景 ANM 槽回收（无 tick）
    // 0x4097c0 建链 / 0x40add0 启用 / 0x4099a0 摘链：更新链 0x11 与绘制链 3、6（换关后上一关 DAT_004a6d9c 的链继续运行）
    void register_stage_chains(StageManager& stage);
    void enable_stage_chains(StageManager& stage);
    void remove_stage_chains(StageManager& stage);
    void harvest_player_sfx();
    // 0x45d150(1, …)：登记一条链 0x13 的画面震动（ECL 517 在敌机链内请求，下一帧起消耗 game_rng）
    void create_screen_shake(const EnemyManager::ShakeRequest& request);
    void clear_screen_shake();
    // 0x45d150(kind, duration, color, …)：登记一条链 0x13 的淡入淡出（0x45c900），DAT_004c0f40 非零时自删
    void create_screen_fade(i32 kind, i32 duration, u32 color);
    void sample_input();
    void register_layer_draw_chains();   // 0x46a3a0：AnmManager 各层绘制链（含带相机切换的 0x46ddd0 / 0x46e030 族）
};

}  // namespace th16
