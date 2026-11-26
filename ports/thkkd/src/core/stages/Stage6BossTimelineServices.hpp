// 六面关底已存在的死亡解析、背景暂停和音乐动作边界。
#pragma once
#include "core/stages/StageBossServices.hpp"
namespace kaikeidou {
struct Stage6BossTimelineServices : StageBossServices {
 virtual void resolve_enemy_death(std::uint32_t index)=0;
 virtual void pause_background()=0;
 virtual void music(std::uint32_t track,std::uint32_t mode)=0;
};
}
