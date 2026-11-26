// 关底时间轴的对话、结算和资源切换边界；状态写入由恢复后的时间轴保持原顺序。
#pragma once
#include "core/entities/BossStageContext.hpp"

namespace kaikeidou {
struct StageBossServices : BossStageServices {
    virtual void dialog(std::uint32_t mode) = 0;
    virtual void award_stage_clear() = 0;
    virtual void change_stage_resources(std::uint32_t stage) = 0;
    virtual void restore_stage_music(std::uint32_t mode) = 0;
};
}
