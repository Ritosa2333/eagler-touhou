#include "bullet.hpp"
#include "../item_system/item.hpp"
namespace th20::source::bullet::unrecovered {
void item_spawn_004c45b0(void* p,const sprite::Vec3& position,std::int32_t count,std::int32_t type){item::spawn_many(*static_cast<item::ItemInf*>(p),position,count,type);}
void item_spawn_004c3c90(void* p,std::int32_t type,const sprite::Vec3& position,std::int32_t color,float angle,float speed,std::int32_t delay,std::int32_t extra,std::int32_t sound){item::spawn(*static_cast<item::ItemInf*>(p),type,position,static_cast<std::uint32_t>(color),angle,speed,delay,static_cast<std::uint32_t>(extra),sound);}
}
