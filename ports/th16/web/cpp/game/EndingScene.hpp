// th16 结局——原版 DAT_004a6dbc（0x419640 创建 / 0x4191f0 装载 / 更新 0x4196c0 / 剧本 0x4199f0）。
//
// 锚点：
//   0x4191f0  登记链 0x23/0x43；选 e0N.msg；成绩 +0x19f96；ascii Loading 0x11
//   0x4197c0  text.anm 0x2e..0x32 行 VM
//   0x4199f0  剧本：op0 结束、op3 写字、op5/6 等待、op7 装 e0N.anm（槽=arg+0x14）、
//            op8 放精灵、op9 颜色、op10 BGM、op11 淡出、op12 staffN.msg（按难度）…
//   0x4196c0  每帧调 0x4199f0；结束 → 场景 4/2；快进 Restart(6)
#pragma once
#include "AnmVm.hpp"
#include "Input.hpp"
#include "Music.hpp"
#include "TextPaint.hpp"
#include "Types.hpp"

#include <functional>
#include <string>
#include <vector>

namespace th16 {

struct ScoreFile;
class AnmManager;
struct AnmFile;

struct EndingScene {
    static constexpr i32 kSkipFrames = 0x78;
    static constexpr i32 kAutoFrames = 0x708;
    static constexpr const char* kMsgNames[8] = {
        "e01.msg", "e02.msg", "e03.msg", "e04.msg", "e05.msg", "e06.msg", "e07.msg", "e08.msg",
    };
    static constexpr const char* kStaffNames[4] = {
        "staff1.msg", "staff2.msg", "staff3.msg", "staff4.msg",
    };

    enum class ScriptResult : i32 { Running = 0, Finished = -1 };

    bool live = false;
    i32 timer = 0;                 // Ending 外壳 +0x20（0x4196c0 每帧 +1）
    i32 ending_index = 0;          // +0x18
    i32 difficulty = 1;            // DAT_004a57b4：op12 选 staffN
    u32 flags = 0;                 // 外壳 +0x1c（bit0 首次本结局、bit1 首次任意结局）
    bool finished = false;
    bool smoke_skip = false;       // 仅 harness 显式启用；实际游戏始终执行完整剧本
    std::vector<u8> msg_bytes;
    std::string msg_name;

    // —— 剧本对象（0x4199f0 的 param_1）——
    u32 line_vms[5] = {};          // +0x40
    const u8* script_pc = nullptr; // +0x54
    Timer script_timer;            // +0x14..+0x28：脚本时刻（current = +0x1c）
    Timer wait_timer;              // +0x2c
    u32 script_flags = 1;          // +0x74：bit0 活动；bit1 staff；bit2 装载中
    i32 lines_written = 0;         // +0x78
    u32 text_color = 0xffffff;     // +0x7c
    AnmFile* ending_anms[4] = {};  // +0x80：op7 装入的 ANM（下标 = 剧本 arg）
    u32 anm_vms[8] = {};           // +0x90：staff 用到槽 0..5
    i32 pending_anm_slot = -1;     // +0xec / op7 槽
    std::string pending_anm_name;  // +0x70 指向的名字副本
    std::string last_text;         // 最近一次 op3 解密结果（宿主/测试）
    std::string last_bgm;          // 最近一次 op10
    ScoreFile* scores = nullptr;   // DAT_004a6f0c（begin 时记下）
    MusicManager* music = nullptr;   // op 10 换曲（0x43c370 + 0x43c3f0）/ op 11 淡出 3 秒
    const float* game_speed = nullptr;   // DAT_004a5788
    i32 text_paint_count = 0;      // 入队 0x46d990 次数（测试）

    AnmManager* animations = nullptr;
    std::vector<TextIntoTextureRequest>* text_paints = nullptr;

    // 宿主：装 ending ANM → 返回槽内 AnmFile*（失败 nullptr）
    std::function<AnmFile*(int slot, const char* name)> load_ending_anm;
    // 宿主：在 ending ANM 上 spawn 脚本（op8）；返回 VM id
    std::function<u32(AnmFile* file, i32 script, i32 slot_index)> spawn_ending_anm;
    std::function<void(u32 vm)> kill_ending_anm;
    // 宿主：读 archive 条目（op12 staffN.msg）
    std::function<bool(const char* name, std::vector<u8>& out)> read_archive;
    // 宿主：0x45d150 淡入淡出（op 0xd → kind0 / op 0xe → kind5）
    std::function<void(i32 kind, i32 duration, u32 color)> create_fade;
    std::vector<const char*> events;

    void begin(i32 shot, i32 shot_aux, i32 continues, ScoreFile* scores, i32 difficulty);
    bool load_msg(const u8* bytes, u32 size, const char* name);
    void spawn_line_vms(AnmManager& anm, AnmFile* text_anm);
    void destroy_line_vms(AnmManager& anm);
    // 0x4199f0 一拍；返回 Finished 时 0x4196c0 请求回标题
    ScriptResult step_script(const InputState& input);
    // 0x4196c0 + 跳过/超时门控（烟雾测试口径：>0x78 确认结束）
    bool update(const InputState& input);
    // 0x41973f：重跑更新链，直到下一次职员表绘制。
    bool restart_staff_update(const InputState& input) const;
    bool finish_animation_load();
    void end();

private:
    static std::string decrypt_text(const u8* src);
    void advance_pc();
    void paint_line(u32 vm_id, const char* text, i32 font, u32 color);
    bool switch_to_staff();
};

}  // namespace th16
