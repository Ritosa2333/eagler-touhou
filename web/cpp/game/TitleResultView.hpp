// 标题战后结算屏（0x454050 名前录入 / 0x4549d0 回放保存）的宿主数据面。
// 记录 DTO 直接复用 PlayerData 的 TitlePlayerDataScore（0x43d6b0/0x453db0
// 同族 score 块字段），避免再造一套字段语义。
#pragma once

#include "TitlePlayerDataMenu.hpp"

#include <array>

namespace th13 {

// 0x454050 case0 的 score 块读取结果 + 当前成绩插入预览。
// 语义（ApplicationState::result_screen_data）：
//   records = files.ordinary_records[character].ranking[difficulty] 的 10 行；
//   若本次成绩入榜，宿主已按 FileStore ties-before-old 规则把它预插入
//   records[rank]（name 为空，timestamp/stage/continues 已填），但未落盘；
//   rank = 当前成绩行 0..9，-1 = 未入榜（原版 +0x59ac=1 锁定态）。
// 绘制侧不得再次插入或改写 records；END 仍由 record_result_name 落盘。
struct TitleResultScreenData {
    i32 character = 0;
    i32 difficulty = 0;
    i32 rank = -1;
    TitlePlayerDataScore current;
    std::array<TitlePlayerDataScore, 10> records{};
};

}  // namespace th13
