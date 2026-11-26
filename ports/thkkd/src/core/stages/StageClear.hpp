// 恢复5E2B80..5E2C61；奖励及地址位移按32位回绕，保留存盘前的状态写入。
#pragma once
#include "core/stages/StageClearContext.hpp"
#include "core/frontend/SaveData.hpp"
#include "core/shared/ObjectRecord.hpp"
namespace kaikeidou {
inline void award_stage_clear(ObjectRecord<240>& player,SaveData& savedata,const StageClearContext& context,SaveIO& io){
    player.word(0xec,(context.stage+player.word(0x14)+1u)*1000000u);
    player.word(0x10,player.word(0x10)+player.word(0xec));player.word(0xe8,180);
    if(context.practice_mode!=0)return;
    const auto progress_index=(context.difficulty*63u+context.character*21u+context.weapon*7u+context.stage)&0x3fffffffu;
    auto& progress=savedata.progress.at(progress_index);
    if(progress<=1u||(progress&0x80000000u))progress=2u-static_cast<std::uint32_t>(player.word(0x24)!=0);
    const auto score_index=(context.difficulty*9u+context.character*3u+context.weapon)&0x3fffffffu;
    savedata.scores.at(score_index)=player.word(0x10);
    store_scores_and_settings(savedata,io);
}
}
