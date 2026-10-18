#include "laser.hpp"
#include "../bomb_system/character_environment.hpp"
#include "../gameplay/loading_dependencies.hpp"
namespace th20::source::bomb::unrecovered {
void cancel_rectangle_004c9db0(void* p,const sprite::Vec3& center,const sprite::Vec3& size,float angle,std::int32_t mode,std::int32_t kind){static_cast<laser::Controller*>(p)->cancel_rectangle(center,size,angle,mode,kind);}
void cancel_circle_004caad0(void* p,const sprite::Vec3& center,float radius,std::int32_t mode,std::int32_t kind){static_cast<laser::Controller*>(p)->cancel_circle(center,radius,mode,kind);}
}
namespace th20::source::gameplay::unrecovered {runtime::CallbackOwner* create_004d7e40(std::int32_t index){return laser::create_controller(index);}}
