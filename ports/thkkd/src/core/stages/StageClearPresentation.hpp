// 恢复613344..61335E的绘制侧计时写入；逻辑侧660860另有一次递减。
#pragma once
#include "core/shared/ObjectRecord.hpp"
namespace kaikeidou {
inline void update_stage_clear_presentation(ObjectRecord<240>& player) {
    if(player.word(0xe8)!=0)player.word(0xe8,player.word(0xe8)-1u);
}
}
