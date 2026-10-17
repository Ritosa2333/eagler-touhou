#include "bullet.hpp"
#include "../bomb_system/character_environment.hpp"
#include "../gameplay/loading_dependencies.hpp"
namespace th20::source::bomb::unrecovered {
void cancel_rectangle_0047cc90(void* p,const sprite::Vec3& position,const sprite::Vec3& size,float angle,std::int32_t mode,std::int32_t kind){bullet::cancel_rectangle(*static_cast<bullet::Controller*>(p),position,size,angle,mode,static_cast<std::uint32_t>(kind));}
void cancel_circle_0047cf60(void* p,const sprite::Vec3& position,float radius,std::int32_t mode,std::int32_t limit,std::int32_t kind){bullet::cancel_circle(*static_cast<bullet::Controller*>(p),position,radius,mode,limit,static_cast<std::uint32_t>(kind));}
}
namespace th20::source::gameplay::unrecovered {runtime::CallbackOwner* create_00486630(std::int32_t index){return bullet::create_controller(index);}}
