#include "AnmVm.hpp"
#include <cmath>
namespace th11 {
namespace {
float add_angle(float a,float b){
    float value=float(double(a)+b);u32 loops=0;
    constexpr double pi=3.1415927410125732421875,tau=6.283185482025146484375;
    while(value>pi){value=float(double(value)-tau);if(loops++>32)break;}
    while(value<-pi){value=float(double(value)+tau);if(loops++>32)break;}
    return value;
}
void rgb_start(RgbInterpolator& v,i32 duration,u8 mode,u32 from,u32 to,const float* rate){
    v.duration=duration;v.mode=InterpolationMode(mode);
    for(u32 i=0;i<3;++i){v.start[i]=(from>>(i*8))&255;v.end[i]=(to>>(i*8))&255;v.initial_tangent[i]=v.final_tangent[i]=0;}
    v.timer.set(0,rate);
}
void alpha_start(AlphaInterpolator& v,i32 duration,u8 mode,u8 from,u8 to,const float* rate,bool clear){
    v.duration=duration;v.mode=InterpolationMode(mode);v.start[0]=from;v.end[0]=to;
    if(clear)v.initial_tangent[0]=v.final_tangent[0]=0;v.timer.set(0,rate);
}
u32 rgb_bits(Rgb v){return (u32(v.blue)&255)|((u32(v.green)&255)<<8)|((u32(v.red)&255)<<16);}
bool interrupt(AnmVm& vm,const float* rate){
    auto* marker=vm.script_begin;AnmInstruction* fallback=nullptr;
    for(;marker->opcode!=-1;marker=reinterpret_cast<AnmInstruction*>(reinterpret_cast<u8*>(marker)+marker->length)){
        if(marker->opcode==64){const i32 id=marker->argument<i32>(0);if(id==vm.pending_interrupt)break;if(id==-1)fallback=marker;}
    }
    vm.pending_interrupt=0;vm.flags&=~0x1000u;if(marker->opcode!=64)marker=fallback;if(!marker)return false;
    vm.saved_timer=vm.timer;vm.saved_instruction=vm.instruction;vm.timer.set(marker->time,rate);
    vm.instruction=reinterpret_cast<AnmInstruction*>(reinterpret_cast<u8*>(marker)+marker->length);vm.flags|=1;return true;
}
bool animate(AnmVm& vm,AnmEnvironment& env){
    float* axes[]={&vm.rotation.x,&vm.rotation.y,&vm.rotation.z};const float speeds[]={vm.angular_velocity.x,vm.angular_velocity.y,vm.angular_velocity.z};
    for(u32 i=0;i<3;++i)if(speeds[i]!=0){*axes[i]=add_angle(*axes[i],float(double(env.rate)*speeds[i]));vm.flags|=4;}
    if(vm.scale_velocity.y!=0){vm.scale.y=float(double(env.rate)*vm.scale_velocity.y+vm.scale.y);vm.flags|=8;}
    if(vm.scale_velocity.x!=0){const float delta=float(double(env.rate)*vm.scale_velocity.x);vm.scale.x=float(double(delta)+vm.scale.x);vm.flags|=12;}
    auto scroll=[&](float pos,float speed){float next=float(double(env.rate)*speed+pos);if(next>=1)next=float(double(next)-1);else if(next<0)next=float(double(next)+1);return next;};
    vm.uv_offset.x=scroll(vm.uv_offset.x,vm.scroll_velocity.x);vm.uv_offset.y=scroll(vm.uv_offset.y,vm.scroll_velocity.y);
    if(vm.flags&0x2000){vm.position.x=float(double(vm.position.x)+env.camera_delta.x);vm.position.y=float(double(vm.position.y)+env.camera_delta.y);vm.position.z=float(double(vm.position.z)+env.camera_delta.z);}
    if(vm.flags2&4)if(!env.screen_uv(vm))return false;
    if(vm.position_interpolation.duration)(vm.flags&0x100?vm.child_position:vm.script_position)=sample(vm.position_interpolation,&env.rate);
    if(vm.color_interpolation.duration)vm.color=(vm.color&0xff000000)|rgb_bits(sample(vm.color_interpolation,&env.rate));
    if(vm.alpha_interpolation.duration)vm.color=(vm.color&0xffffff)|(u32(sample(vm.alpha_interpolation,&env.rate))<<24);
    if(vm.scale_interpolation.duration){vm.scale=sample(vm.scale_interpolation,&env.rate);vm.flags|=8;}
    if(vm.rotation_interpolation.duration){vm.rotation=sample(vm.rotation_interpolation,&env.rate);vm.flags|=4;}
    if(vm.color2_interpolation.duration)vm.secondary_color=(vm.secondary_color&0xff000000)|rgb_bits(sample(vm.color2_interpolation,&env.rate));
    if(vm.alpha2_interpolation.duration)vm.secondary_color=(vm.secondary_color&0xffffff)|(u32(sample(vm.alpha2_interpolation,&env.rate))<<24);
    if(vm.scroll_x_interpolation.duration)vm.scroll_velocity.x=sample(vm.scroll_x_interpolation,&env.rate);
    if(vm.scroll_y_interpolation.duration)vm.scroll_velocity.y=sample(vm.scroll_y_interpolation,&env.rate);
    const u32 mode=(vm.flags>>22)&15;if((mode==9||mode==13)&&!env.update_geometry(vm))return false;
    return true;
}
i32 calculate(i32 a,i32 b,u32 op){
    if(op==0)return b;if(op==1)return wrapping_add(a,b);if(op==2)return wrapping_sub(a,b);if(op==3)return signed_bits(u32(a)*u32(b));
    if(b==0||(a==INT32_MIN&&b==-1))__builtin_trap();return op==4?a/b:a%b;
}
double calculate(double a,double b,u32 op){switch(op){case 0:return b;case 1:return a+b;case 2:return a-b;case 3:return a*b;case 4:return a/b;default:return std::fmod(a,b);}}
}
// 0x44b4b0. The instruction set shares older opcodes but has TH11-specific
// scrolling interpolation, screen UVs, child spawning and partial-ring modes.
int AnmVm::update(AnmEnvironment& env){
    if(!instruction)return 1;if(flags&0x20000)return 0;
    struct RateScope{float& value;float previous;~RateScope(){value=previous;}} guard{env.rate,env.rate};
    if(flags&0x20000000)env.rate=1;
    auto finish=[&](){if(!animate(*this,env))return -1;if(after_update&&after_update(*this))return 1;timer.tick();return 0;};
    if(pending_interrupt&&!interrupt(*this,&env.rate)){timer.advance(-1);return finish();}
    for(u32 budget=0;instruction->time<=timer.current;++budget){
        if(budget==100000)return -1;
        auto* current=instruction;const i32 op=current->opcode;
        auto integer=[&](u32 n){const auto v=current->argument<i32>(n);return current->references&(1u<<n)?integer_value(v,env):v;};
        auto floating=[&](u32 n)->double{const auto v=current->argument<float>(n);return current->references&(1u<<n)?float_value(v,env):v;};
        auto int_dest=[&](u32 n){auto* p=reinterpret_cast<i32*>(reinterpret_cast<u8*>(current)+8+n*4);return current->references&(1u<<n)?integer_destination(p):p;};
        auto float_dest=[&](u32 n){auto* p=reinterpret_cast<float*>(reinterpret_cast<u8*>(current)+8+n*4);return current->references&(1u<<n)?float_destination(p):p;};
        auto jump=[&](u32 off,i32 time){timer.set(time,&env.rate);instruction=reinterpret_cast<AnmInstruction*>(reinterpret_cast<u8*>(script_begin)+off);};
        auto field=[&](u32 mask,u32 shift,u32 value){flags=(flags&~mask)|((value<<shift)&mask);};
        if(op>=6&&op<=27){
            const bool three=op>=18;const u32 operation=three?(op-18)/2+1:(op-6)/2;
            if(!(op&1)){const i32 a=integer(1),b=three?integer(2):0;auto* out=int_dest(0);*out=calculate(three?a:*out,three?b:a,operation);}
            else {double a=0,b;if(three&&operation==5){b=float(floating(2));a=float(floating(1));}else if(three){a=float(floating(1));b=float(floating(2));}else{b=float(floating(1));if(operation==5)a=float(floating(0));}auto* out=float_dest(0);if(!three&&operation!=5)a=*out;*out=float(calculate(a,b,operation));}
        }else if(op>=28&&op<=39){
            const double a=op&1?double(float(floating(0))):integer(0),b=op&1?floating(1):integer(1);bool take=false;
            switch((op-28)/2){case 0:take=a==b;break;case 1:take=a!=b;break;case 2:take=a<b;break;case 3:take=a<=b;break;case 4:take=a>b;break;case 5:take=a>=b;break;}
            if(take){jump(current->argument<u32>(2),current->argument<i32>(3));continue;}
        }else switch(op){
        case -1:case 1:flags&=~1u;[[fallthrough]];
        case 2:instruction=nullptr;return 1;
        case 3:{flags|=1;const i32 value=integer(0);if(!bind_sprite(sprite_callback?sprite_callback(*this,value):value))return -1;sprite_frame=timer.current;break;}
        case 4:jump(current->argument<u32>(0),current->argument<i32>(1));continue;
        case 5:{auto* counter=int_dest(0);*counter=wrapping_add(*counter,-1);if(integer(0)>0){jump(current->argument<u32>(1),current->argument<i32>(2));continue;}break;}
        case 40:{const u32 bound=u32(integer(1));const i32 v=bound?signed_bits(env.random(*this).next32()%bound):0;*int_dest(0)=v;break;}
        case 41:{const float max=float(floating(1));const float value=float(double(env.random(*this).unit())*max);*float_dest(0)=value;break;}
        case 42:case 43:case 44:case 45:case 46:{const double v=float(floating(1));double result=op==42?std::sin(v):op==43?std::cos(v):op==44?std::tan(v):op==45?std::acos(v):std::atan(v);*float_dest(0)=float(result);break;}
        case 47:{const float v=float(floating(0));*float_dest(0)=add_angle(v,0);break;}
        case 48:{const float z=float(floating(2)),y=float(floating(1)),x=float(floating(0));(flags&0x100?child_position:script_position)={x,y,z};break;}
        case 49:rotation.x=float(floating(0));rotation.y=float(floating(1));rotation.z=float(floating(2));flags|=4;break;
        case 50:scale.x=float(floating(0));scale.y=float(floating(1));flags|=8;break;
        case 51:color=(color&0xffffff)|(u32(integer(0))<<24);break;
        case 52:case 76:{u32& c=op==52?color:secondary_color;c=(c&~0xff0000u)|((u32(integer(0))&255)<<16);c=(c&~0xff00u)|((u32(integer(1))&255)<<8);c=(c&~255u)|(u32(integer(2))&255);break;}
        case 53:angular_velocity.x=float(floating(0));angular_velocity.y=float(floating(1));angular_velocity.z=float(floating(2));flags|=4;break;
        case 54:scale_velocity.x=float(floating(0));scale_velocity.y=float(floating(1));break;
        case 55:{const i32 duration=integer(1);alpha_start(alpha_interpolation,duration,0,color>>24,current->argument<u8>(0),&env.rate,true);break;}
        case 56:{auto& v=position_interpolation;v.duration=integer(0);std::memcpy(v.initial_tangent,&env.default_tangent,12);std::memcpy(v.final_tangent,&env.default_tangent,12);v.mode=InterpolationMode(current->argument<i32>(1));const auto from=flags&0x100?child_position:script_position;std::memcpy(v.start,&from,12);const float z=float(floating(4)),y=float(floating(3)),x=float(floating(2));v.end[0]=x;v.end[1]=y;v.end[2]=z;v.timer.set(0,&env.rate);break;}
        case 57:case 78:{const u32 from=op==57?color:secondary_color;const u32 b=u32(integer(4))&255,g=u32(integer(3))&255,r=u32(integer(2))&255;const i32 duration=integer(0);rgb_start(op==57?color_interpolation:color2_interpolation,duration,current->argument<u8>(1),from,b|(g<<8)|(r<<16),&env.rate);break;}
        case 58:case 79:{const u8 to=u8(integer(2));const i32 duration=integer(0);alpha_start(op==58?alpha_interpolation:alpha2_interpolation,duration,current->argument<u8>(1),(op==58?color:secondary_color)>>24,to,&env.rate,op==58);break;}
        case 59:{const float z=float(floating(4)),y=float(floating(3)),x=float(floating(2));auto& v=rotation_interpolation;v.duration=integer(0);v.mode=InterpolationMode(current->argument<i32>(1));std::memcpy(v.initial_tangent,&env.default_tangent,12);std::memcpy(v.final_tangent,&env.default_tangent,12);std::memcpy(v.start,&rotation,12);v.end[0]=x;v.end[1]=y;v.end[2]=z;v.timer.set(0,&env.rate);flags|=4;break;}
        case 60:{const float y=float(floating(3)),x=float(floating(2));auto& v=scale_interpolation;v.duration=integer(0);v.mode=InterpolationMode(current->argument<u8>(1));std::memcpy(v.start,&scale,8);v.end[0]=x;v.end[1]=y;v.timer.set(0,&env.rate);flags|=8;break;}
        case 61:flags=(flags^0x200)|8;scale.x=-scale.x;break;
        case 62:flags=(flags^0x400)|8;scale.y=-scale.y;break;
        case 63:case 69:if(op==69)flags&=~1u;if(pending_interrupt){if(interrupt(*this,&env.rate))continue;}else flags|=0x1000;timer.advance(-1);return finish();
        case 65:{const u32 v=current->argument<u32>(0);field(0xc0000,18,v&65535);field(0x300000,20,v>>16);break;}
        case 66:field(0x70,4,current->argument<u32>(0));break;
        case 67:field(0x3c00000,22,current->argument<u32>(0));if((flags&0x3c00000)==0x2800000&&!env.change_draw_mode(*this))return -1;break;
        case 68:layer=current->argument<u8>(0);break;
        case 70:scroll_velocity.x=float(floating(0));break;
        case 71:scroll_velocity.y=float(floating(0));break;
        case 72:field(1,0,current->argument<u32>(0));break;
        case 73:field(0x800,11,current->argument<u32>(0));break;
        case 74:field(0x2000,13,current->argument<u32>(0));break;
        case 75:timer.advance(float(wrapping_sub(0,integer(0))));break;
        case 77:secondary_color=(secondary_color&0xffffff)|(u32(integer(0))<<24);break;
        case 80:field(0x8000,15,current->argument<u8>(0));break;
        case 81:timer=saved_timer;instruction=saved_instruction;continue;
        case 82:field(0x8000000,27,current->argument<u8>(0));break;
        case 83:script_position=position;position={};break;
        case 84:case 101:flags=op==84?(flags&0xfe7fffff)|0x2400000:(flags&0xff7fffff)|0x3400000;if(!env.allocate_geometry(*this,u32(integer(0))*0x38))return -1;break;
        case 85:field(0x10000000,28,current->argument<u8>(0));break;
        case 86:field(0x20000000,29,integer(0));break;
        case 87:field(0x40000000,30,current->argument<u8>(0));break;
        case 89:field(0x80000000,31,current->argument<u32>(0));break;
        case 88:case 90:case 91:case 92:case 95:case 96:case 97:{
            auto* v=env.spawn(*this,i16(integer(0)),op);if(!v)return -1;
            if(op!=95&&op!=97){v->child.next=child.next;if(child.next)child.next->previous=&v->child;child.next=&v->child;v->child.previous=&child;}
            if(op==96||op==97){v->child_position.x=float(floating(1));v->child_position.y=float(floating(2));}else v->child_position=script_position;v->position=position;break;
        }
        case 93:case 94:{const float to=float(floating(2));const i32 duration=integer(0);auto& v=op==93?scroll_x_interpolation:scroll_y_interpolation;v.duration=duration;v.mode=InterpolationMode(current->argument<i32>(1));v.start[0]=op==93?scroll_velocity.x:scroll_velocity.y;v.end[0]=to;v.initial_tangent[0]=v.final_tangent[0]=0;v.timer.set(0,&env.rate);break;}
        case 98:if(!env.screen_uv(*this))return -1;break;
        case 99:flags2=(flags2&~4u)|((u32(integer(0))<<2)&4);break;
        case 100:{const float ax=float(floating(1)),ay=float(floating(2)),az=float(floating(3)),bx=float(floating(7)),by=float(floating(8)),bz=float(floating(9));auto& v=position_interpolation;v.duration=integer(0);v.mode=InterpolationMode::Hermite;v.initial_tangent[0]=ax;v.initial_tangent[1]=ay;v.initial_tangent[2]=az;v.final_tangent[0]=bx;v.final_tangent[1]=by;v.final_tangent[2]=bz;const auto from=flags&0x100?child_position:script_position;std::memcpy(v.start,&from,12);const float z=float(floating(6)),y=float(floating(5)),x=float(floating(4));v.end[0]=x;v.end[1]=y;v.end[2]=z;v.timer.set(0,&env.rate);break;}
        case 102:{flags|=1;const i32 first=integer(0);const u32 range=u32(integer(1));if(!range)return -1;const i32 value=wrapping_add(first,signed_bits(env.script_rng.next32()%range));if(!bind_sprite(sprite_callback?sprite_callback(*this,value):value))return -1;sprite_frame=timer.current;break;}
        default:break;
        }
        instruction=reinterpret_cast<AnmInstruction*>(reinterpret_cast<u8*>(current)+current->length);
    }
    return finish();
}
}
