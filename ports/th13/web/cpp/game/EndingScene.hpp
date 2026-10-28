// th13 通关 Ending 场景（原版 0x416570 家族，ending.cpp）。
//
// 原版链路（artifacts/verification/s6-full-20261028/report.md Part B）：
//   0x42c550 0x4000 路径 → DAT_004dcc24=0xf → 0x43c1c0 case0xf（0x42c500 战斗
//   拆卸内含 0x42bfa0→0x449e50 scoreth13.dat 落盘）→ 0x416570 场景对象 →
//   0x416130 初始化（结局索引/落档位/e##.msg 装载/滚屏对象）→ 每帧
//   0x416610 → 0x416a10 结局脚本解释器 → 脚本 END → DAT_004dcc24=0x10 →
//   0x43c1c0 case0x10（0x4165f0→0x4163b0 拆卸：ANM 槽 20-23 释放/msg/任务）
//   → 目的地 4 + DAT_004dc670=3（标题直入菜单态 10）。
//
// msg 记录格式（parse_ending_msg.py 实测 e01..e12/staff）：
//   [u16 行号][u8 op][u8 len][payload len 字节]，推进 = len+4（0x416a10:0x4170e0）。
//   行计数（滚屏对象 +0x1c，插值器 B 的目标值）在「下一记录行号 > 当前行」时
//   每帧 +1（速度 1.0；速度 ∈(0.99,1.01) 走 +1 路径，否则 acc += speed 取整，
//   0x4170fa-0x4171dc——Ending 期 0x4c0a28 恒 1.0，快进由 op5/6 的 %6 节拍承担）。
//   opcode 跳转表 0x4171e8（capstone 复核）：
//     0 END→返 -1（0x416610 置 0x10） / 1,2 空转 / 3 文本页行 / 4 清 5 文本 VM /
//     5,6 文本行等待（可跳过；6 另清页计数 +0x78 与 DAT_004dc318）/
//     7 e##.anm 异步装载（槽 0x14+idx=20..23） / 8 建 ANM VM /
//     9 颜色 +0x7c / 10 音乐 cue / 11 音乐淡出 / 12 接下一段 msg /
//     13,14 覆盖窗 0x45fa60(0/5,param) / 15,16,17 难度门建 VM（4be7c4==1/2/3）。
#pragma once

#include "GameInput.hpp"
#include "SessionState.hpp"
#include "Types.hpp"

#include <array>
#include <string>
#include <vector>

namespace th13 {

// 原版 score 存档缓冲（DAT_004c22cc）相对偏移 0x1b26a..0x1b276 的 13 字节
// 结局区视图。idx 与结局 msg 表 0x4bba20 同序（0=staff.msg，1..12=e01..e12）。
// 生产侧由 ApplicationState 提供内存镜像；FileStore 序列化属后续登记缺口
// （原版也在内存置位、最晚下一次 0x449e50 才落盘）。
class EndingSaveBytes {
public:
    virtual ~EndingSaveBytes() = default;
    virtual u8 read(i32 index) const = 0;
    virtual void write(i32 index, u8 value) = 0;
};

// 结局索引公式（0x416130:0x41623a-0x41629a，反汇编逐指令复核）：
//   base = DAT_004be7bc(shot) + DAT_004be7b8(char)；
//   续关数 DAT_004be7c8 != 0 → idx = base+9（坏结局 e10 段起步，实际 9..12）；
//   否则 idx = 2*base+1；且 难度(4be7c4)>=1 && 好结局资格(4be83c)>=3 &&
//   save[2*base+0x1b26b]&1（标准好结局已见）→ idx += 1（真结局变体）。
// th13 每角色单机体（plXXa.sht 单档）→ DAT_004be7bc 恒 0，base = character。
i32 ending_index_for(const SessionState& progress, const EndingSaveBytes& save);

// Ending 平台服务（对照 PostGamePlatform/MsgPlayerPlatform 的宿主面）。
struct EndingPlatform {
    virtual ~EndingPlatform() = default;
    // 0x4724c0 按名读 th13.dat 资产（e##.msg/staff.msg/e##.anm）。
    // 签名与 RunSceneServices::read_asset 同型（宿主单实现双接口）。
    virtual bool read_asset(const std::string& name, std::vector<u8>& bytes,
                            bool required = true) = 0;
    // DAT_004c22cc 存档缓冲视图（结局字节区）。
    virtual EndingSaveBytes* save_bytes() = 0;
    // 0x416610 END → DAT_004dcc24 = 0x10（练习位 0x2000 时为 2——Ending 期不达）。
    // 签名与 TitleScenePlatform::request_destination 同型（宿主单实现双接口）。
    virtual bool request_destination(i32 raw) = 0;
    // op10：0x43adc0(0,"bgm/th13_18") 排队 "<name>.wav"（fmt 名表 th13_18.wav）
    // + 0x43ae40(0, th13_18?0xf:0x10) 播预载槽 0。port 映射同 play_track：
    // queue_music(1,0,name) 后 queue_music(2,0,"")；第二参 0xf/0x10 原版语义
    // 未定位（fade/循环族），生产暂不区分。
    virtual bool queue_music(i32 code, i32 value, const std::string& name) = 0;
    // 43ae40 marks score data's music-room index when playback is requested.
    virtual bool record_music_unlocked(i32 /*index*/) { return true; }
    // op11：0x43af70(3.0f) 音乐淡出（生产 code 5，3 秒 = 180 帧）。
    virtual bool fade_out_music(i32 frames) = 0;
    // op5/6 计声放行：0x460bc0(0)（同 MsgPlayer opcode 11 的对话推进音约定）。
    virtual bool advance_sound() = 0;
    // ---- 切片 4 挂点：ANM（默认成功 = 无图形依赖的行为切片）----
    // op7：e##.anm 装载到 ANM 槽 0x14+idx（20..23）。原版异步 + 0x473600 回调
    // 0x417230；生产同步装载（B8 落点建议），装载帧差分另行登记。
    virtual bool load_ending_anm(i32 /*slot*/, const std::string& /*name*/) { return true; }
    // op8/15/16/17：替换指定 VM 槽并从 bank 起脚本（0x416ce8）。
    virtual bool cue_ending_anm(i32 /*vm_slot*/, i32 bank_slot, i32 script) {
        return cue_ending_anm(bank_slot, script);
    }
    virtual bool cue_ending_anm(i32 /*bank_slot*/, i32 /*script*/) { return true; }
    // 0x4163b0 拆卸：ANM 槽 20..23 反装载（0x46e620）+ VM 回收。
    virtual void release_ending_anm(i32 /*slot*/) {}
    // ---- 切片 5 挂点：op13/op14 覆盖窗 0x45fa60(kind,param,0,0,0)，优先级 0x4b ----
    virtual bool open_ending_window(i32 /*kind*/, i32 /*param*/) { return true; }
    // ---- 切片 3 挂点：文本页（5 VM 近似；0x46e8f0/0x46fc30 通道）----
    virtual bool ending_page_reset() { return true; }  // op3 页首：5 VM 重建 + 行 0
    virtual bool ending_page_line(i32 row, u32 color, const std::string& cp932) = 0;
    virtual bool ending_page_clear() { return true; }  // op4：5 VM 清（脚本 3）
    virtual void release_ending_page() {}
};

class EndingScene {
public:
    EndingScene(SessionState& progress, EndingPlatform& platform);

    // 0x416570→0x416130：结局索引、落档位、msg 装载、滚屏对象初始化。
    // 失败（msg 读不到 = 原版 return -1 族）置 error 返回 false。
    bool initialize();
    // 每帧 0x416610 → 0x416a10（END 时经 request_destination(0x10) 请求出口）。
    bool update(const GameInput& input);
    // 0x4165f0→0x4163b0 拆卸（ANM 槽释放 + 文本面收尾）。
    void teardown();

    bool finished() const { return finished_; }   // END 已请求 0x10
    i32 ending_index() const { return ending_index_; }
    u32 scene_flags() const { return scene_flags_; }  // 0x4c2184+0x20（跳过许可门）
    i32 line() const { return line_.target; }         // 滚屏行计数（+0x1c）
    i32 frames() const { return frames_; }            // +0x24 帧计数
    const std::string& message_name() const { return message_name_; }

    std::string error;

private:
    bool fail(const std::string& why) {
        if (error.empty()) error = why.empty() ? "Ending scene failed" : why;
        return false;
    }
    // 0x4016c0 插值器置位：current=param-1、target=param、acc=(float)param。
    struct Interp {
        i32 current = -999999, target = 0;
        float acc = 0;
    };
    static void interp_set(Interp& it, i32 param);
    // 0x4732c0：current=target；速度 1.0（0x4c0a28）时 acc+=delta 否则
    // acc+=speed*delta；target=截断(acc)。
    static void interp_tick(Interp& it, float delta);
    // 0x4170fa 滚屏推进（行计数插值器 B）。
    void advance_line();

    struct Record {
        u32 line = 0;
        u8 op = 0, length = 0;
        const u8* payload = nullptr;  // 指向 msg_bytes_ 内部
    };
    bool record_at(u32 offset, Record& out) const;
    i32 payload_i32(const Record& r, u32 index) const;
    std::string payload_cstring(const Record& r, u32 byte_offset = 0) const;

    // op 处理器返回 false = 本帧等待（不推进记录）；true = 已处理（推进）。
    bool op_page_line(const Record& r);      // op3（0x416a57）
    bool op_page_clear();                    // op4（0x416b73）
    bool op_text_wait(const GameInput& input, const Record& r,
                      bool page_break);      // op5/6（0x416b9b/0x416c4d）
    bool op_anm_load(const Record& r);       // op7（0x417144）
    bool op_anm_vm(const Record& r);         // op8/15/16/17（0x416ce8 族）
    bool op_music(const Record& r);          // op10（0x416f82）
    bool op_reload_next(const Record& r);    // op12（0x416ff4）：true=成功

    SessionState& progress_;
    EndingPlatform& platform_;

    // ---- 0x4c2184 场景对象面 ----
    i32 ending_index_ = 0;
    u32 scene_flags_ = 0;   // bit0=该结局首见、bit1=好结局全未见过（0x416130 置位）
    i32 frames_ = 0;        // +0x24

    // ---- 0xf0 滚屏对象面 ----
    std::vector<u8> msg_bytes_;
    std::string message_name_;  // 当前 msg 名（0x4bba20[idx] / op12 载入名）
    u32 record_ = 0;            // +0x54 当前记录偏移
    Interp line_;               // +0x18/+0x1c/+0x20：滚屏行
    Interp text_wait_;          // +0x2c/+0x30/+0x34：文本行等待
    u32 state_flags_ = 1;       // +0x74：bit0 op11 清、bit1 op12 置、bit2 装载中
    u32 color_ = 0x00ffffff;    // +0x7c
    i32 page_lines_ = 0;        // +0x78 页内行计数（op6 清零）
    std::array<std::string, 5> page_text_;  // 近似渲染缓冲（原版 5 文本 VM）

    bool finished_ = false;
};

}  // namespace th13
