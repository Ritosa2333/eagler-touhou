// 命中回调可切换Boss阶段，下一组早苗B子机按原地址重读A7EA98。
#pragma once
#include "core/player/PlayerShotHit.hpp"
namespace kaikeidou {
struct PlayerShotHitLiveModeServices final:PlayerShotHitServices {
    PlayerShotHitServices& target;
    PlayerShotHitContext& context;
    const std::uint32_t& boss_state;
    PlayerShotHitLiveModeServices(PlayerShotHitServices& next,PlayerShotHitContext& hit,const std::uint32_t& state)
        :target(next),context(hit),boss_state(state){}
    void damage(std::uint32_t slot,std::uint32_t amount)override{
        target.damage(slot,amount);
        context.mode_a7ea98=boss_state;
    }
};
}
