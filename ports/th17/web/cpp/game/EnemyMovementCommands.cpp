#include "EnemyMovementCommands.hpp"
#include "AnmVisualState.hpp"
#include <cmath>
namespace th17 {
namespace {
void scalar_curve(ScalarInterpolation& curve,i32 duration,i32 mode,float start,float end){curve.start[0]=start;curve.end[0]=end;curve.control1=curve.control2={};curve.duration=duration;curve.mode=mode;curve.timer.set(0);}
void shape_curve(Vec2Interpolation& curve,i32 duration,i32 mode,std::array<float,2> start,std::array<float,2> end){curve.start=start;curve.end=end;curve.control1=curve.control2={};curve.duration=duration;curve.mode=mode;curve.timer.set(0);}
float mirror_angle(float value){return normalize_angle(float(1.57079637050628662109375f-normalize_angle(float(value-1.57079637050628662109375f))));}
void restart_motion(MotionState& motion){if((motion.flags&15)==4)motion.origin=motion.position;motion.advance();}
}
int EnemyMovementCommands::execute(EclContext& context,u16 opcode){
    if(opcode==302||opcode==303||opcode==306||opcode==313||opcode==316||opcode==317||opcode==318||opcode==319||opcode==320||opcode==322||(opcode>=325&&opcode<=333)||opcode==335||opcode==336||opcode==550||opcode==552)return visuals.execute(context,opcode);
    const auto* instruction=context.instruction;if(!instruction){context.error="Enemy instruction unavailable";return -2;}bool ok=true;
    auto arg=[&](u32 index){float value=0;if(20+index*4>instruction->length){context.error="Truncated enemy movement arguments";ok=false;}else if(!context.floating(index,instruction->argument<float>(index),value))ok=false;return value;};
    auto integer_arg=[&](u32 index){i32 value=0;if(20+index*4>instruction->length){context.error="Truncated enemy movement arguments";ok=false;}else if(!context.integer(index,instruction->argument<i32>(index),value))ok=false;return value;};
    switch(opcode){
        case 400:case 402:{auto& target=opcode==400?enemy.absolute:enemy.relative;const float x=arg(0),y=arg(1);if(!ok)return -2;if(double(x)>-999999)target.position.x=x;if(double(y)>-999999)target.position.y=y;target.flags&=~15u;enemy.motion.position={float(enemy.relative.position.x+enemy.absolute.position.x),float(enemy.relative.position.y+enemy.absolute.position.y),float(enemy.relative.position.z+enemy.absolute.position.z)};enemy.recompose();break;}
        case 416:case 417:{auto& target=opcode==416?enemy.absolute:enemy.relative;const float x=arg(0),y=arg(1),z=arg(2);if(!ok)return -2;target.position={float(x+target.position.x),float(y+target.position.y),float(z+target.position.z)};enemy.recompose();break;}
        case 401:case 403:case 436:case 437:case 434:case 435:case 438:case 439:{
            const bool absolute=opcode==401||opcode==436||opcode==434||opcode==438,axes=opcode==434||opcode==435||opcode==438||opcode==439,delta=opcode>=436;auto& motion=absolute?enemy.absolute:enemy.relative;auto& interpolation=absolute?enemy.absolute_position:enemy.relative_position;float x=arg(axes?3:2),y=arg(axes?4:3);const i32 duration=integer_arg(0);if(!ok)return -2;if(duration<=0){interpolation.duration=0;break;}
            if(delta){x=enemy.flags&0x80000?float(motion.position.x-x):float(motion.position.x+x);y=float(motion.position.y+y);}interpolation.duration=integer_arg(0);interpolation.control1=interpolation.control2={};if(axes){interpolation.flags|=1;interpolation.axis_modes[0]=integer_arg(1);interpolation.axis_modes[1]=integer_arg(2);}else{interpolation.mode=integer_arg(1);interpolation.flags&=~1u;}interpolation.start={motion.position.x,motion.position.y,motion.position.z};interpolation.end={double(x)>-999999?x:motion.position.x,double(y)>-999999?y:motion.position.y,0};interpolation.timer.set(0);motion.flags&=~15u;break;
        }
        case 425:case 426:{auto& motion=opcode==425?enemy.absolute:enemy.relative;auto& interpolation=opcode==425?enemy.absolute_position:enemy.relative_position;const float x=arg(3),y=arg(4),c1x=arg(1),c1y=arg(2),c2x=arg(5),c2y=arg(6);const i32 duration=integer_arg(0);if(!ok)return -2;interpolation.begin(duration,8,{motion.position.x,motion.position.y,motion.position.z},{double(x)>-999999?x:motion.position.x,double(y)>-999999?y:motion.position.y,0});interpolation.control1={c1x,c1y,0};interpolation.control2={c2x,c2y,0};motion.flags&=~15u;break;}
        case 404:case 406:case 428:case 430:{auto& target=opcode==404||opcode==428?enemy.absolute:enemy.relative;float angle=arg(0);const float speed=arg(1);if(!ok)return -2;if(double(angle)>-999999){if((enemy.flags&0x80000)&&(opcode==404||opcode==406)){angle=normalize_angle(float(angle-1.57079637050628662109375f));angle=normalize_angle(float(1.57079637050628662109375f-angle));}target.set_angle(angle);}if(double(speed)>-999999)target.speed=speed;target.flags&=~15u;break;}
        case 405:case 407:case 429:case 431:case 441:case 443:{
            const bool only_angle=opcode==441||opcode==443,absolute=opcode==405||opcode==429||opcode==441;auto& motion=absolute?enemy.absolute:enemy.relative;auto& angle_curve=absolute?enemy.absolute_angle:enemy.relative_angle;auto& speed_curve=absolute?enemy.absolute_speed:enemy.relative_speed;
            float angle=arg(2),speed=only_angle?0:arg(3);const i32 duration=integer_arg(0);if(!ok)return -2;if(duration<=0){angle_curve.duration=0;if(!only_angle)speed_curve.duration=0;break;}
            const i32 mode=integer_arg(1);const bool mirrored=(enemy.flags&0x80000)&&(opcode==405||opcode==407||only_angle);
            if(mode==7){if(double(angle)>-999999){if(mirrored)angle=-angle;}else angle=0;if(!only_angle&&!(double(speed)>-999999))speed=0;}
            else{if(double(angle)>-999999){if(mirrored)angle=mirror_angle(angle);}else angle=motion.angle;if(!only_angle&&!(double(speed)>-999999))speed=motion.speed;}
            float start=motion.angle;if(only_angle){float delta=float(angle-start);if(delta>3.1415927410125732421875f)delta=float(angle-float(start+6.283185482025146484375f));else if(float(start-angle)>3.1415927410125732421875f)delta=float(angle-float(start-6.283185482025146484375f));angle=float(start+delta);}else if(!(std::fabs(float(start-angle))<3.1415927410125732421875f)){if(angle>start)start=float(start+6.283185482025146484375f);else angle=float(angle+6.283185482025146484375f);}
            scalar_curve(angle_curve,integer_arg(0),mode,start,angle);if(!only_angle)scalar_curve(speed_curve,integer_arg(0),mode,motion.speed,speed);motion.flags&=~15u;break;
        }
        case 440:case 442:{auto& motion=opcode==440?enemy.absolute:enemy.relative;float angle=arg(0);if(!ok)return -2;if(enemy.flags&0x80000)angle=mirror_angle(angle);motion.set_angle(angle);motion.flags&=~15u;break;}
        case 444:case 446:{auto& motion=opcode==444?enemy.absolute:enemy.relative;const float speed=arg(0);if(!ok)return -2;if(double(speed)>-999999)motion.speed=speed;motion.flags&=~15u;break;}
        case 445:case 447:{auto& motion=opcode==445?enemy.absolute:enemy.relative;auto& curve=opcode==445?enemy.absolute_speed:enemy.relative_speed;float speed=arg(2);const i32 duration=integer_arg(0);if(!ok)return -2;if(duration<=0){curve.duration=0;break;}const i32 mode=integer_arg(1);if(!ok)return -2;if(!(double(speed)>-999999))speed=mode==7?0:motion.speed;scalar_curve(curve,integer_arg(0),mode,motion.speed,speed);if(!ok)return -2;motion.flags&=~15u;break;}
        case 412:case 413:{
            if(!world.random){context.error="Enemy random steering state unavailable";return -2;}
            auto& random=*world.random;constexpr float pi=3.1415927410125732421875f,half_pi=1.57079637050628662109375f;float angle;
            const float x=enemy.motion.position.x,quarter_width=float(enemy.bound_size.x*.25f);
            if(float(enemy.bound_center.x-quarter_width)>x)angle=float(float(random.signed_unit()*pi)/3.f);
            else if(x>float(enemy.bound_center.x+quarter_width))angle=normalize_angle(float(float(float(random.signed_unit()*pi)/3.f)+pi));
            else{angle=float(float(random.signed_unit()*pi)*.25f);const u16 word=random.next16();if(world.player_position.x>x?word%3==0:word%3!=0)angle=float(angle+pi);}
            const float quarter_height=float(enemy.bound_size.y*.25f),y=enemy.motion.position.y;
            if(float(enemy.bound_center.y-quarter_height)>y)angle=std::fabs(angle);else if(y>float(enemy.bound_center.y+quarter_height))angle=-std::fabs(angle);
            if(std::fabs(float(angle+half_pi))<.05000000074505806f)angle=angle<-half_pi?-1.6207963228225708f:-1.5207964181900024f;
            else if(double(std::fabs(float(angle-half_pi)))<.05)angle=angle<half_pi?1.5207964181900024f:1.6207963228225708f;
            const i32 mode=integer_arg(1);const float speed=arg(2);const i32 duration=integer_arg(0);if(!ok)return -2;
            auto& motion=opcode==412?enemy.absolute:enemy.relative;auto& angle_curve=opcode==412?enemy.absolute_angle:enemy.relative_angle;auto& speed_curve=opcode==412?enemy.absolute_speed:enemy.relative_speed;
            scalar_curve(angle_curve,duration,mode,angle,angle);const i32 speed_duration=integer_arg(0);if(!ok)return -2;
            // The original writes the angle easing twice and retains the speed
            // interpolator's previous easing mode. Preserve that side effect.
            scalar_curve(speed_curve,speed_duration,speed_curve.mode,speed,0);motion.flags&=~15u;break;
        }
        case 408:case 410:case 420:case 422:{
            auto& motion=opcode==408||opcode==420?enemy.absolute:enemy.relative;const float angle=arg(0),speed=arg(1),radius=arg(2),velocity=arg(3),axis=opcode>=420?arg(4):0,scale=opcode>=420?arg(5):0;if(!ok)return -2;
            if((motion.flags&15)!=2)motion.velocity=motion.position;if(double(angle)>-999999)motion.set_angle(angle);if(double(speed)>-999999)motion.speed=speed;if(double(radius)>-999999)motion.radius=radius;if(double(velocity)>-999999)motion.angular_velocity=velocity;
            if(opcode>=420){if(double(axis)>-999999)motion.axis_angle=normalize_angle(normalize_angle(axis));if(double(scale)>-999999)motion.axis_scale=scale;motion.flags=(motion.flags&0xfffffff3u)|3;}else motion.flags=(motion.flags&0xfffffff2u)|2;
            restart_motion(motion);enemy.recompose();break;
        }
        case 409:case 411:case 421:case 423:{
            const bool absolute=opcode==409||opcode==421,ellipse=opcode>=421;auto& motion=absolute?enemy.absolute:enemy.relative;auto& speed_curve=absolute?enemy.absolute_speed:enemy.relative_speed;auto& shape=absolute?enemy.absolute_shape:enemy.relative_shape;auto& axis_curve=absolute?enemy.absolute_wave:enemy.relative_wave;
            float speed=arg(2),radius=arg(3),velocity=arg(4),axis=ellipse?arg(5):0,scale=ellipse?arg(6):0;
            if(!(double(speed)>-999999))speed=motion.speed;if(!(double(radius)>-999999))radius=motion.radius;if(!(double(velocity)>-999999))velocity=motion.angular_velocity;if(ellipse){if(!(double(axis)>-999999))axis=motion.axis_angle;if(!(double(scale)>-999999))scale=motion.axis_scale;}
            const i32 duration=integer_arg(0),mode=integer_arg(1);if(!ok)return -2;if(duration<=0){speed_curve.duration=shape.duration=0;if(ellipse)axis_curve.duration=0;break;}
            scalar_curve(speed_curve,duration,mode,motion.speed,speed);shape_curve(shape,duration,mode,{motion.radius,motion.angular_velocity},{radius,velocity});if(ellipse)shape_curve(axis_curve,duration,mode,{motion.axis_angle,motion.axis_scale},{axis,scale});
            if(ellipse){motion.velocity=motion.position;motion.flags=(motion.flags&0xfffffff3u)|3;}else motion.flags=(motion.flags&0xfffffff2u)|2;restart_motion(motion);enemy.recompose();break;
        }
        case 432:case 433:{const i32 id=integer_arg(0);if(!ok)return -2;auto* target=world.lookup(u32(id));if(!target){context.error="Enemy movement target unavailable";return -2;}(opcode==432?enemy.absolute:enemy.relative).position=target->motion.position;break;}
        case 414:case 415:if(!world.boss){context.error="Enemy movement boss target unavailable";return -2;}else(opcode==414?enemy.absolute:enemy.relative).position=world.boss->motion.position;break;
        case 424:{i32 value;if(instruction->length<20||!context.integer(0,instruction->argument<i32>(0),value)){context.error="Enemy mirror argument unavailable";return -2;}enemy.flags=(enemy.flags&~0x80000u)|(u32(value)<<19&0x80000u);break;}
        case 504:{const float x=arg(0),y=arg(1),width=arg(2),height=arg(3);if(!ok)return -2;enemy.flags|=0x20000;enemy.bound_center={x,y};enemy.bound_size={width,height};break;}
        case 505:enemy.flags&=~0x20000u;break;
        case 427:enemy.absolute.position={float(enemy.absolute.position.x+enemy.relative.position.x),float(enemy.absolute.position.y+enemy.relative.position.y),float(enemy.absolute.position.z+enemy.relative.position.z)};enemy.relative.position={};enemy.absolute.speed=enemy.relative.speed=0;enemy.absolute.velocity=enemy.relative.velocity={};enemy.absolute.flags&=~15u;enemy.relative.flags&=~15u;enemy.absolute_position.duration=enemy.relative_position.duration=0;enemy.absolute_angle.duration=enemy.relative_angle.duration=enemy.absolute_speed.duration=enemy.relative_speed.duration=0;enemy.absolute_shape.duration=enemy.relative_shape.duration=enemy.absolute_wave.duration=enemy.relative_wave.duration=0;break;
        default:context.error="Unimplemented TH17 enemy ECL opcode "+std::to_string(opcode);return -2;
    }
    return ok?0:-2;
}
}
