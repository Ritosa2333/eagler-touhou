// 通关后标题结算屏（原版标题 supervisor 0x44c570 case 0xe/0xf）：
//   0xe → 0x454050（成绩名前录入/结算）：
//     case0：建面（背景 root89、标题条 script107、角色条 143+character、
//            难度条 147+difficulty；difficulty=4 → 151 EXTRA）+ 读榜位/默认名；
//     case1：>6 帧进编辑；case2：91 字符网格名字编辑（0x81/0x80/0x7f =
//     SPACE/DEL/END 原版特殊字形，最多 8 字）；case3：收面——续关数==0 →
//     0xf = 0x4549d0（回放保存），否则回主菜单。
//   0xf → 0x4549d0（回放槽保存）：
//     case0 建 script108（“SAVE REPLAY” 字框）+ 背景 root89，并逐槽查询
//     th13_%.2d.rpy（1..25）；case2 选槽（25 行列表，Z 进本屏名字编辑）；
//     case3 8 字名字编辑（0x5a=END 才写文件）；case4 >5 帧收面回主菜单。
// 绘制坐标/颜色/格式串为原版 .data 实读（0x454690/0x454f90）：
//   成绩 10 行 (48,160) 行距 18；名字编辑行 (84,160+18*rank)；
//   回放槽列表 (58,80) 行距 15；槽编辑选中行/名字行 (112,240)；
//   91 格 (212,360)，列 +18 / 行 +16；选中黄 0xffffff00、未选灰 0xff808080。
#pragma once

#include "TitleState.hpp"
#include "TitleResultView.hpp"
#include "HudDrawData.hpp"

#include <string>

namespace th13 {

class AnmManager;
struct TitleScenePlatform;

class TitleResultEntry {
    TitleState& state;
    TitleScenePlatform& host;
    AnmManager& animations;
    i32 title_bank;
    bool data_loaded_ = false;
    TitleResultScreenData data_;
    bool fail(const std::string& reason);
    bool create_visual(i32 script, u32& handle);  // 0x44bb80
    bool open_name_screen();                       // 0x454050 case0 建面
    bool open_slot_screen();                       // 0x4549d0 case0 建面
    void retire_name_visuals();                    // 0x454050 case3 收面
    void retire_slot_visuals();                    // 0x4549d0 case4 收面
    bool return_main();                            // 0x454050 case3 流程 1（0x454658 重选标题曲）
    bool name_editor(u32 pressed, u32 repeated);   // 0x454050 case2
    bool slot_selector(u32 pressed, u32 repeated); // 0x4549d0 case2
    bool slot_name_editor(u32 pressed, u32 repeated);  // 0x4549d0 case3
    bool edit_name(u32 pressed, u32 repeated, bool save_to_replay);
    bool draw_score_rows(HudDrawServices&, const TitleResultScreenData&,
                         const std::string& current_name);
    bool draw_name_editor(HudDrawServices&, float name_x, float name_y);
    bool draw_slot_list(HudDrawServices&);
    bool draw_slot_selected(HudDrawServices&);
    std::string slot_row_text(i32 index) const;
    static std::string score_row_text(const TitlePlayerDataScore&, i32 ordinal,
                                      const std::string& fallback_name);
public:
    std::string error;
    TitleResultEntry(TitleState& state_, TitleScenePlatform& host_, AnmManager& animations_,
                     i32 title_bank_ = 16)
        : state(state_), host(host_), animations(animations_), title_bank(title_bank_) {}
    bool update(u32 pressed, u32 repeated);
    // 屏内文本走标题现有 Draw 通道（TitleScene stage_text_draw → HudDrawServices
    // → AsciiText 队列）；背景/字框/角色/难度走 title.anm 原版脚本。
    bool draw_texts(HudDrawServices&);
};

}  // namespace th13
