#include "EnemyCallbacks.hpp"
#include "AnmVisualState.hpp"
#include <cmath>
namespace th17 {
bool enemy_update_rule(EnemyState& enemy,BulletManager& bullets,Rng& random){
    if(enemy.update_rule==EnemyUpdateRule::none)return true;
    if(enemy.update_rule!=EnemyUpdateRule::redirect_bullets)return false;
    constexpr float pi=3.1415927410125732421875f;
    float angle=float(double(random.signed_unit())*double(pi));
    const i32 first=wrapping_mul(enemy.integer_registers[1],100),last=wrapping_add(first,100);
    for(u32 slot=bullets.first_active();slot!=BulletManager::none;slot=bullets.next_active(slot)){
        auto& bullet=*bullets.state(slot);if(bullet.step_limit<first||bullet.step_limit>=last)continue;
        bullet.transform_index=8;auto& transform=bullet.transforms[8];
        transform.floats[0]=float(enemy.float_registers[1]+float(std::cos(double(angle))*double(enemy.float_registers[0])));
        transform.floats[1]=float(enemy.float_registers[2]+float(std::sin(double(angle))*double(enemy.float_registers[0])));
        transform.integers[0]=enemy.integer_registers[0];transform.integers[1]=9;transform.type=0x20000;transform.active=0;
        angle=normalize_angle(float(angle+enemy.float_registers[3]));
    }return true;
}
}
