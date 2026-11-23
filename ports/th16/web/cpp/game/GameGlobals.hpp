// th16 全局游戏配置——原版散落的 .data 全局（初值按 th16.exe .data 节读取）。
#pragma once
#include "Types.hpp"

namespace th16 {

struct GameGlobals {
    static constexpr u32 kModeMask = 0x30;  // DAT_004a5bec bit4-5：0 普通 / 1 练习 / 2 符卡练习（0x449190）

    i32 stage = 0;               // DAT_004a5790
    i32 stage_copy = 0;          // DAT_004a5794
    i32 shot = 0;                // DAT_004a57a4 机体（0 灵梦 1 琪露诺 2 文 3 魔理沙）
    i32 shot_aux = 0;            // DAT_004a57a8
    i32 season = 0;              // DAT_004a57ac 子季节（0 春 1 夏 2 秋 3 冬 4 土用=Extra）
    i32 difficulty = 1;          // DAT_004a57b4（初值 1 = Normal；4 = Extra）
    i32 replay_slot = 0;         // DAT_004a57c8
    u32 flags = 0;               // DAT_004a5bec（bit4-5 模式；bit6 demo 返回；bit7）
    i32 last_difficulty = 1;     // DAT_0049f274
    i32 last_shot = 0;           // DAT_004a6f24
    i32 last_practice_stage = -1;// DAT_004a2974
    // DAT_004a5bf8：练习关卡确认时（0x450ef0）按住的数字键 1..9（0x402130 快照，首个命中），0 = 未按；
    // 0x42cb60 练习初始残机 = 非零 ? 其值-1 : 9
    i32 practice_lives = 0;
    i32 demo_saved_difficulty = 0;// DAT_004a59c0：idle demo 触发时保存，回标题还原
    i32 spell_return_group = -1; // DAT_004a2970：符卡练习返回时还原的分组（0x4553d0 读后置 -1）
    i32 spell_return_list = -1;  // DAT_0049f2dc：…列表槽（0x455900）
    i32 spell_return_entry = -1; // DAT_0049f2d8：…条目（0x456a20）
    i32 title_origin = 0;        // DAT_004a6f1c：进入标题的来源（0 首次 1 正常 2 Replay 结束 4 练习返回 5 符卡练习返回）
    i32 stage_table = 0;         // DAT_004a6f18：0 = &DAT_004a23a4 普通关卡表，1 = &DAT_004a289c Extra 表（按季节表时另记）
    i32 scene_request = -1;      // DAT_004c17c4：Supervisor 场景请求（7 = Game）
    bool new_game = false;       // DAT_004c17d0：0x43ce10 case 7 进游戏时置 1、case 0xc 换关时清 0（HUD 面板 / 难度牌只在新开局建）
    i32 game_frame = 0;          // DAT_004a579c：本局帧计数（0x42d7b0 每帧 +1，0x42dc50 清零）
    i32 stage_frame = 0;         // DAT_004a57a0：本关帧计数

    // 机体资源（0x42cb60 装载段 / 0x440fb0 置）
    i32 power = 0;               // DAT_004a57e4
    i32 power_max = 0;           // DAT_004a57e8 = 每级火力 × 最大等级
    i32 power_per_level = 0;     // DAT_004a57ec
    i32 lives = 2;               // DAT_004a57f4
    i32 life_fragments = 0;      // DAT_004a57f8
    i32 bombs = 3;               // DAT_004a5800
    i32 bomb_fragments = 0;      // DAT_004a5804
    i32 season_gauge = 0;        // DAT_004a5808：子季节量表（0x4440e0 以阈值表决定子机数）
    i32 season_gauge_max = 0;    // DAT_004a580c = season_threshold[7]
    i32 season_step[8] = {};     // DAT_004a5810：0x43deb0 参数
    i32 season_threshold[8] = {};// DAT_004a5838：0x43deb0 前缀和；DAT_004a583c.. = [1..6] 为子机数阈值

    // 难度秩与擦弹（0x444cf0 累加，上限 99999999）
    i32 rank = 0;                // DAT_004a57bc：ECL -9960 / 弹幕 617–622 按秩选参
    i32 graze = 0;               // DAT_004a57c0：本局擦弹数
    i32 graze_total = 0;         // DAT_004a57c4：累计擦弹（含续关）
    i32 miss_count = 0;          // DAT_004a57cc：被弹次数（0x443cd0 +1，上限 999999）

    // 得分（0x43e080 累加，上限 999999999）与分数 extend（0x43ddd0 阈值表 DAT_00491880 / Extra DAT_004917c4）
    i32 score = 0;               // DAT_004a57b0
    i32 continues = 0;           // DAT_004a57b8：续关次数（0x43f980 续关时 +1，上限 9）
    i32 continues_left = 0;      // DAT_004a5bfc：剩余续关数（0x42cb60 按难度表 DAT_00492278 {5,5,5,5,0}；续关 -1；0 时菜单禁用“续关”）
    i32 sound_thread_state = 0;  // DAT_004d9d90：声音线程状态（GameManager 计时第 5 帧置 2；菜单打开时存旧值并置 0 / 1）
    // DAT_004c12a0：标题 → 游戏过场效果（EffectManager 种类 0，0x418ba0）的 VM id；标题各入口生成后中断 7（合拢），
    // 0x42dc50 首帧末 / 0x42d7b0 换关过场首帧中断 1（面板切 7..10 展开，全部停止后 0x407330 返回 1 删除）
    u32 transition_effect_id = 0;
    // th16.cfg 配置块 DAT_004c12a4（布局见 GameConfig.hpp）；DAT_004c12c6 / +1（Option 0x44c570）BGM / SE 音量，
    // Default → 0x5064 且 +2 清零
    u8 window_mode = 0;   // DAT_004c12c0（0x459830：窗口样式 0x15 / 0x17）
    u8 bgm_mode = 1;      // DAT_004c12c1（0x45e330：== 1 才做 WAV 串流；0x45de30 非零才建 SoundManager 播放）
    u8 se_enabled = 1;    // DAT_004c12c2（0x45e330：非零才处理音效队列）
    u8 resolution = 5;    // DAT_004c12c3（0x459830 写进 DAT_004d9d1c bit2..5；0x407cb0 % 3 选字体表）
    u8 frame_skip = 0;    // DAT_004c12c4（0x4262f0：帧计数每帧 +(frame_skip+1)）
    u8 config_c5 = 2;     // DAT_004c12c5（仅校验 <3，无读者）
    u8 bgm_volume = 100;
    u8 se_volume = 80;
    u8 option_flags = 0;  // DAT_004c12c8
    u8 config_c9 = 2;     // DAT_004c12c9（0x45b330 / 0x45b7d0：与 2 / 3 比较的窗口刷新模式）
    // DAT_004c12cc：输入相关配置；bit8(0x100) 由 0x43c050 恒置；bit9(0x200)=自动低速（射击且火力≥10 时记录链并上 Shift）
    u32 input_config = 0x100;
    i32 window_x = i32(0x80000000);  // DAT_004c12d0（0x459830 退出时保存窗口位置；0x80000000 = 默认）
    i32 window_y = i32(0x80000000);  // DAT_004c12d4
    // 手柄按钮表 DAT_004a52e4（10×i16，与配置块 +4 互抄；exe 初值 {0,1,2,5,-1,-1,-1,-1,-1,3}）
    // 0x4018e0 读的下标：0 Shot(bit0) / 1 Bomb(bit1) / 2 Slow(bit3) / 3 Pause(bit8) / 9 Release(bit11)；其余无读者
    static constexpr int kPadButtonCount = 10;
    static constexpr i16 kPadButtonDefaults[kPadButtonCount] = {0, 1, 2, 5, -1, -1, -1, -1, -1, 3};
    i16 pad_buttons[kPadButtonCount] = {0, 1, 2, 5, -1, -1, -1, -1, -1, 3};
    // 0x4018e0：手柄按钮位图（bit i = 按钮 i 按下）按表合成输入位，叠加到键盘位上
    u32 pad_input_bits(u32 buttons) const {
        auto down = [&](int slot) { return pad_buttons[slot] >= 0 && (buttons >> (pad_buttons[slot] & 0x1f) & 1u) != 0; };
        u32 bits = 0;
        if (down(0)) bits |= 1u;
        if (down(1)) bits |= 2u;
        if (down(3)) bits |= 0x100u;
        if (down(2)) bits |= 8u;
        if (down(9)) bits |= 0x800u;
        return bits;
    }
    // 0x42cb60 新开局自成绩文件取本机体 × 难度的最高分（无成绩文件时 0x4497e0 默认 100000）与其续关数；
    // 0x42bc10 每帧以 Gui 显示分越过时更新并置 flags bit2；换关 0x42cb60 另取 max(最高分, 当前分)
    i32 hiscore = 100000;        // DAT_004a59b8
    i32 hiscore_continues = 0;   // DAT_004a59bc
    // DAT_004c0f40：0x42d200 置 1、0x42cb60 末清 0、0x42d7b0 每帧递减；非零时 ScreenEffect 淡入淡出（0x45c900）暂停
    i32 post_load_frames = 0;
    i32 extend_index = 0;        // DAT_004a57fc：下一个分数 extend 阈值下标
    i32 spell_id = 0;            // DAT_004a5798：当前符卡 / 阶段编号（op 524）
    i32 enemy_bonus_pending = 0; // DAT_004a5880：op 569 登记的敌机击破奖励
    i32 enemy_bonus = 0;         // DAT_004a5884：op 570 / 0x41d520 / 0x424f00 / 0x425010 结算的奖励

    // 得点值与道具拾取统计（0x42cb60 初值；ItemManager 0x430100 / 0x430620 / 0x430dc0 / 0x4303a0 改写）
    i32 point_value = 0;         // DAT_004a57d8：得点道具当前价值（×100 存）
    i32 point_value_initial = 0; // DAT_004a57dc = DAT_004922a8[difficulty] × 100
    i32 point_value_max = 0;     // DAT_004a57e0 = DAT_00492290[difficulty] × 100
    i32 point_items = 0;         // DAT_004a57d4：拾取的得点道具数
    i32 collect_sum = 0;         // DAT_004a5860：自动收点累计分值
    i32 collect_count = 0;       // DAT_004a5868：自动收点累计个数
    i32 collect_indicator = 0;   // DAT_004a586c：有非季节道具被吸向自机时置 8（Gui 收点提示）
    Vec3 collect_pos;            // DAT_004a5870：最近一次自动收点时的自机坐标
    i32 bomb_fragment_items = 0; // DAT_004a587c：以种类 0xf 生成的炸弹碎片数

    // 外部推力（DAT_004d9dc4 / +4 / DAT_004d9dcc）：0x442560 每帧抄进 Player，0x441cf0 叠加到移动
    float push_x = 0, push_y = 0, push_z = 0;

    // 0x43deb0：季节阶梯 i 置 value 并重算前缀和（偶数项与奇数项分别累加后再加当前项）
    void set_season_step(int i, i32 value) {
        season_step[i] = value;
        i32 even = 0, odd = 0, last = 0, k = 0;
        if (i + 1 > 1) {
            for (; k < i; k += 2) { even += season_step[k]; odd += season_step[k + 1]; }
        }
        if (k < i + 1) last = season_step[k];
        season_threshold[i] = odd + even + last;
    }
    // 0x4440e0 / 0x40dda0：DAT_004a583c..DAT_004a5850（threshold[1..6]）中 ≤ 量表的个数
    i32 season_option_count() const {
        i32 n = 0;
        for (int i = 1; i <= 6; ++i) { if (season_gauge < season_threshold[i]) break; ++n; }
        return n;
    }

    // 0x43df20：季节量表在当前子机级内的进度 0..1（HUD 量表条长 = 进度 × 100）；满 6 级后恒为 1
    float season_gauge_fraction() const {
        const i32 level = season_option_count();
        if (level >= 6) return 1.0f;
        return float(season_gauge - season_threshold[level]) / float(season_step[level + 1]);
    }

    // 0x43ddd0：当前分数 extend 阈值（Extra 用另一张表；表尾 1e9 哨兵）
    i32 extend_threshold() const {
        static constexpr i32 kNormal[12] = {500000, 1000000, 2000000, 4000000, 7000000, 10000000,
                                            15000000, 25000000, 50000000, 100000000, 1000000000, 4794780};
        static constexpr i32 kExtra[12] = {1000000, 2000000, 4000000, 6000000, 8000000, 10000000,
                                           1000000000, 1084227584, 1092616192, 1101004800, 1106247680, 1109393408};
        const int i = extend_index < 0 ? 0 : extend_index > 11 ? 11 : extend_index;
        return difficulty == 4 ? kExtra[i] : kNormal[i];
    }
    // 0x42e590：新开局（0x42cb60 DAT_004c17d0 段）清本局资源——得点道具数、火力、炸弹 3 / 碎片 0、残机碎片、
    // extend 下标、自动收点统计、炸弹碎片道具数、被弹次数。同一关重开（DAT_004a5bec bit0）沿用全局区，故必须显式归零。
    // （Gui 存在时另调 0x42c390 刷新炸弹显示；新开局时 Gui 尚未创建，由 Gui::init 自行取值）
    void reset_run_resources() {
        point_items = 0;
        power = 0;
        power_per_level = 1;
        bombs = 3;
        bomb_fragments = 0;
        life_fragments = 0;
        extend_index = 0;
        collect_sum = 0;
        collect_count = 0;
        collect_indicator = 0;
        bomb_fragment_items = 0;
        miss_count = 0;
    }

    // 0x43e080：score += value / 10，逐级越过阈值时残机 +1（上限 8；每级都推进阈值下标）。
    // 每次 extend 都在残机已 +1 之后立刻调一次 on_extend()——原版此处是 Gui 0x42c280 残机显示、音效 0x11、
    // Gui 0x42bcf0(0, 4) 弹出（见 Gui::celebrate_extend）；返回 extend 次数
    template <class OnExtend>
    i32 add_score(i32 value, OnExtend&& on_extend) {
        score += value / 10;
        i32 extends = 0;
        while (score >= extend_threshold()) {
            if (lives < 8) {
                lives = lives + 1 > 8 ? 8 : lives + 1;
                ++extends;
                on_extend();
            }
            ++extend_index;
        }
        if (score > 999999999) score = 999999999;
        return extends;
    }
    i32 add_score(i32 value) { return add_score(value, [] {}); }

    // 0x449190：设置模式位；若原模式不是 2 则 +0x38（DAT_004a57c8）置 -1
    void set_mode(i32 mode) {
        if ((flags & kModeMask) != 0x20) replay_slot = -1;
        flags = (((u32(mode) << 4) ^ flags) & kModeMask) ^ flags;
    }
    i32 mode() const { return i32((flags & kModeMask) >> 4); }
};

}  // namespace th16
