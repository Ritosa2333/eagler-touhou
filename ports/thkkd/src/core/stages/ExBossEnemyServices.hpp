#pragma once
#include <cstdint>
namespace kaikeidou {
struct ExBossEnemyServices {
    virtual ~ExBossEnemyServices()=default;
    virtual void kill_enemy(std::uint32_t index)=0;
};
}
