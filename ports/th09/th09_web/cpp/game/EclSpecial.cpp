#include "EclSpecial.hpp"
#include <cmath>
namespace th09 {
bool EclSpecial::redirect_bullets(EclVm& vm){
    if(!vm.values.world||!vm.values.field)return false;
    for(u32 other=0;other<2;++other){
        const i32 side=other?1-vm.values.field->side:vm.values.field->side;
        u32 count=0;Bullet* bullets=actions.bullets(side,count);if(!bullets)return false;
        for(u32 n=0;n<count;++n){
            auto& b=bullets[n];if(b.state==0||b.state==6)continue;
            if(b.animation_height<=16.f&&(!other||b.base_sprite<0x157))actions.set_sprite(b,b.base_sprite+15);
            b.speed=b.acceleration=b.speed_delta=0;
            b.direction=float(vm.values.world->random.signed_unit())*3.1415927410125732f;
            b.angular_velocity=b.turn_delta=0;b.velocity={};b.clear_extras();
            u32 slot=0;
            if(other&&!(n&1)&&b.animation_height==16.f){b.sprite=0;auto& ex=b.set_extra(slot++,0x4000);ex.integer_a=0;ex.integer_b=15;}
            auto& wait=b.set_extra(slot++,0x20000);wait.integer_a=60;
            auto& accelerate=b.set_extra(slot,0x10);accelerate.integer_a=60;
            accelerate.float_a=vm.context().locals.floats[0];accelerate.float_b=-999.f;
        }
    }
    actions.play_sound(0x34,0);return true;
}
bool EclSpecial::execute(EclVm& vm,i32 callback,const EclInstruction& instruction){
    if(!vm.values.field)return false;
    auto& values=vm.values;const i32 destination=1-values.field->side;
    const auto queue=[&](i32 kind,const Vec3& position,const Vec3* extra=nullptr){actions.queue_attack(kind,destination,position,extra);};
    switch(callback){
    case 0:queue(1,values.position);break;
    case 1:{
        if(!values.opponent)return false;const u32 flag=vm.raw(instruction,1);
        values.field->flags=(values.field->flags&~1u)|(flag&1);values.opponent->flags=(values.opponent->flags&~1u)|(flag&1);
        if(flag)actions.play_sound(0x34,0);break;
    }
    case 2:return redirect_bullets(vm);
    case 3:queue(7,values.position);break;
    case 4:queue(9,values.position);break;
    case 5:queue(8,values.position);break;
    case 6:case 7:case 8:{const auto& local=vm.context().locals;
        queue(callback+4,{local.extra[0],local.extra[1],callback==6?0.f:local.floats[0]});break;
    }
    case 9:{
        for(float angle:{1.5707963705062866f,1.2566370964050293f,1.884955644607544f}){
            Vec3 extra{float(std::cos(double(angle))*64.0),float(std::sin(double(angle))*64.0),angle};
            extra.x+=values.position.x;extra.y+=values.position.y;queue(22,values.position,&extra);
        }break;
    }
    case 10:queue(23,values.position);break;
    case 11:queue(24,values.position);break;
    default:return false;
    }
    return !vm.invalid;
}
}
