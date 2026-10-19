#pragma once
#include "enemy_entity.hpp"
namespace th20::source::gameplay {
enum class EnemyClearKind {ordinary,group,no_effect,exclude_special};
class EnemyCleanupServices {
public:
    virtual ~EnemyCleanupServices()=default;
    virtual void defeat(Enemy&)=0;
    virtual void delete_animation(std::uint32_t&)=0;
    virtual const float* timer_rate()=0;
};
void clear_enemy_group(EnemyController&,EnemyClearKind,int group,EnemyCleanupServices&); //4a4810/4a4a20/4a4c20/4a4e30
EnemyCleanupServices& enemy_cleanup_services();
inline void clear_enemy_group(EnemyController& c,EnemyClearKind kind=EnemyClearKind::ordinary,int group=0){clear_enemy_group(c,kind,group,enemy_cleanup_services());}
}
