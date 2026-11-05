#pragma once
#include "EnemyState.hpp"
namespace th15 {
struct ItemSpawnRequest {i32 type=0;Vec3 position{};float angle=-1.57079637050628662109375f,speed=2.2000000476837158203125f;};
struct ItemSpawnHost {virtual ~ItemSpawnHost()=default;virtual bool spawn_item(const ItemSpawnRequest&)=0;};
bool drop_enemy_items(EnemyState&,Rng&,ItemSpawnHost&);
// ECL 509 / native 424ea0 emits the primary item before the scatter, then clears it.
bool drop_enemy_items_now(EnemyState&,Rng&,ItemSpawnHost&);
}
