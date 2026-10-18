#include "BulletState.hpp"
#include "AnmVisualState.hpp"
#include "VectorMath.hpp"
#include <cmath>
namespace th15 {
void BulletState::velocity(float speed,float angle)noexcept{motion.velocity.x=float(std::cos(double(angle))*double(speed));motion.velocity.y=float(std::sin(double(angle))*double(speed));}
bool BulletState::update_boost(float rate)noexcept{
    if(boost.current>=17){transform_flags^=1;return true;}
    velocity(float(float(5.f-float(float(boost.fractional*5.f)*.0625f))+motion.speed),motion.angle);boost.tick(&rate);return false;
}
bool BulletState::update_acceleration(float rate,bool maintain_speed)noexcept{
    auto& effect=maintain_speed?approach:acceleration;const u32 flag=maintain_speed?0x200000u:4u;
    if(effect.duration<=effect.timer.current){transform_flags&=~flag;return true;}
    motion.speed=float(float(effect.speed*rate)+motion.speed);
    motion.velocity={float(motion.velocity.x+float(effect.vector.x*rate)),float(motion.velocity.y+float(effect.vector.y*rate)),float(motion.velocity.z+float(effect.vector.z*rate))};
    if(double(std::fabs(motion.velocity.x))>.0001||double(std::fabs(motion.velocity.y))>.0001){motion.angle=normalize_angle(float(std::atan2(double(motion.velocity.y),double(motion.velocity.x))));if(!maintain_speed)motion.speed=float(std::sqrt(double(float(float(motion.velocity.x*motion.velocity.x)+float(motion.velocity.y*motion.velocity.y)))));}
    effect.timer.tick(&rate);return false;
}
bool BulletState::update_angular(float rate)noexcept{
    if(angular.duration<=angular.timer.current){transform_flags&=~8u;return true;}
    motion.angle=normalize_angle(normalize_angle(float(float(angular.angle*rate)+motion.angle)));motion.speed=float(float(angular.speed*rate)+motion.speed);velocity(motion.speed,motion.angle);angular.timer.tick(&rate);return false;
}
bool BulletState::update_drift(float rate)noexcept{
    if(drift.duration<=drift.timer.current){transform_flags&=0xfffffff6u;return true;}
    motion.position={float(motion.position.x+float(drift.velocity.x*rate)),float(motion.position.y+float(drift.velocity.y*rate)),float(motion.position.z+float(drift.velocity.z*rate))};drift.timer.set(0);return false;
}
bool BulletState::reflect_edge(u32 edge,const Vec2& override)noexcept{
    const float width=override.x<=0?reflection.bounds.x:override.x,height=override.y<=0?reflection.bounds.y:override.y;float boundary=0;
    if(edge==4){boundary=float(width*-.5f);if(boundary<motion.position.x||boundary==motion.position.x)return false;}
    else if(edge==8){boundary=float(width*.5f);if(motion.position.x<boundary)return false;}
    else if(edge==1){boundary=float(224.f-float(height*.5f));if(boundary<motion.position.y||boundary==motion.position.y)return false;}
    else{boundary=float(float(height*.5f)+224.f);if(motion.position.y<boundary)return false;}
    if(!(reflection.sides&16)){
        if(edge==4||edge==8){float angle=normalize_angle(float(-motion.angle-3.1415927410125732421875f));motion.angle=normalize_angle(normalize_angle(float(angle+0.f)));motion.position.x=edge==4?float(-width-motion.position.x):float(width-motion.position.x);}
        else{motion.angle=normalize_angle(-motion.angle);motion.position.y=edge==1?float(float(448.f-height)-motion.position.y):float(float(height+448.f)-motion.position.y);}
    }return true;
}
bool BulletState::update_reflection(const Vec2& override,BulletSoundHost* sounds){
    bool outside=false;
    if(override.x<=0){const float half_width=float(reflection.bounds.x*.5f),half_height=float(reflection.bounds.y*.5f);outside=float(motion.position.x+0.f)<=-half_width||half_width<=float(motion.position.x-0.f)||float(motion.position.y+0.f)<=float(224.f-half_height)||float(half_height+224.f)<=float(motion.position.y-0.f);if(!outside)return false;}
    if(override.x>0){const float half_width=float(override.x*.5f),half_height=float(override.y*.5f);if(-half_width<float(motion.position.x+0.f)&&float(motion.position.x-0.f)<half_width&&float(224.f-half_height)<float(motion.position.y+0.f)&&float(motion.position.y-0.f)<float(half_height+224.f))return false;}
    bool reflected=false;for(u32 edge:{1u,2u,8u,4u})if((reflection.sides&edge)&&reflect_edge(edge,override))reflected=true;
    if(-990.f<reflection.speed)motion.speed=reflection.speed;velocity(motion.speed,motion.angle);
    if(reflected){reflection.count=wrapping_add(reflection.count,1);if(transform_sound>=0&&sounds)sounds->play(transform_sound);}
    if(reflection.count<reflection.limit)return false;transform_flags&=~64u;return true;
}
bool BulletState::update_wrap(const Vec2& size,BulletSoundHost* sounds){
    const float half_width=float(size.x*.5f),half_height=float(size.y*.5f);
    if(-192.f<float(motion.position.x+half_width)&&float(motion.position.x-half_width)<192.f&&0.f<float(motion.position.y+half_height)&&float(motion.position.y-half_height)<448.f)return false;
    if((wrapping.sides&1)&&motion.position.y<0)motion.position.y=float(float(size.y+448.f)+motion.position.y);
    else if((wrapping.sides&2)&&448.f<motion.position.y)motion.position.y=float(motion.position.y-float(size.y+448.f));
    else if((wrapping.sides&4)&&motion.position.x<-192.f)motion.position.x=float(float(size.x+384.f)+motion.position.x);
    else if((wrapping.sides&8)&&192.f<motion.position.x)motion.position.x=float(motion.position.x-float(size.x+384.f));else return false;
    wrapping.count=wrapping_add(wrapping.count,1);if(transform_sound>=0&&sounds)sounds->play(transform_sound);
    if(wrapping.count<wrapping.limit)return false;transform_flags^=0x1000;return true;
}
bool BulletState::update_position_curve(float rate)noexcept{
    auto& curve=position_curve;
    if(curve.duration<=curve.timer.current){transform_flags&=~0x20000u;motion.position=curve.target;motion.speed=curve.speed;velocity(motion.speed,motion.angle);motion.velocity.z=0;return true;}
    if(curve.timer.current==0)curve.interpolation.start={motion.position.x,motion.position.y,motion.position.z};const auto out=curve.interpolation.step(rate);
    motion.velocity={float(out[0]-motion.position.x),float(out[1]-motion.position.y),float(out[2]-motion.position.z)};
    if(double(std::fabs(motion.velocity.x))>.0001||double(std::fabs(motion.velocity.y))>.0001)motion.angle=normalize_angle(float(std::atan2(double(motion.velocity.y),double(motion.velocity.x))));motion.velocity.z=0;curve.timer.tick(&rate);return false;
}
bool BulletState::update_turn(float rate,const Vec3& player,BulletSoundHost* sounds){
    float speed=motion.speed;
    if(turn.timer.current<turn.duration)speed=float(motion.speed-float(float(turn.timer.fractional*motion.speed)/float(turn.duration)));
    else{
        if(transform_sound>=0&&sounds)sounds->play(transform_sound);turn.count=wrapping_add(turn.count,1);
        switch(turn.mode){
            case 0:case 5:motion.angle=normalize_angle(float(motion.angle+turn.angle));break;
            case 1:case 6:motion.angle=normalize_angle(float(bullet_aim(motion.position,player)+turn.angle));break;
            case 2:motion.angle=normalize_angle(float(bullet_aim(saved.position,player)+turn.angle));break;
            case 3:motion.angle=normalize_angle(float(saved.angle+turn.angle));break;
            case 4:motion.angle=normalize_angle(turn.angle);break;
            default:break;
        }
        speed=motion.speed=turn.speed;turn.timer.set(0);
        if(turn.limit<=turn.count){velocity(speed,motion.angle);transform_flags&=~16u;return true;}
    }
    velocity(speed,motion.angle);turn.timer.tick(&rate);return false;
}
bool BulletState::update_wait(float rate,const Vec2& size)noexcept{
    wait.decrement(&rate);
    const float half_width=float(size.x*.5f),half_height=float(size.y*.5f);
    const bool inside=-192.f<float(motion.position.x+half_width)&&float(motion.position.x-half_width)<192.f&&0.f<float(motion.position.y+half_height)&&float(motion.position.y-half_height)<448.f;
    if(wait_outside&&!inside){
        const Vec2 direction{float(std::cos(double(motion.angle))),float(std::sin(double(motion.angle)))};
        const float left=float(float(float(-384.f-size.x)*.5f)-motion.position.x),right=float(float(float(size.x+384.f)*.5f)-motion.position.x);
        const float top=float(float(float(float(-448.f-size.y)*.5f)+224.f)-motion.position.y),bottom=float(float(float(float(size.y+448.f)*.5f)+224.f)-motion.position.y);
        float clockwise=-999.f,counterclockwise=-999.f;
        for(const Vec2 corner:{Vec2{left,top},Vec2{right,top},Vec2{left,bottom},Vec2{right,bottom}}){
            const auto ray=normalize_vector(corner);
            const float cross=float(float(direction.x*ray.y)-float(direction.y*ray.x)),dot=float(float(direction.y*ray.y)+float(direction.x*ray.x));
            if(cross<=0.f&&dot>clockwise&&dot>=0.f)clockwise=dot;
            if(cross>=0.f&&dot>counterclockwise&&dot>=0.f)counterclockwise=dot;
        }
        if(clockwise<-998.f||counterclockwise<-998.f){transform_flags^=0x100;return true;}
    }
    if(wait.current>0)return false;transform_flags^=0x100;return true;
}
}
