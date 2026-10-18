#include "AnmEnvironment.hpp"
#include "GameMath.hpp"
namespace th10 {
namespace {
void seek(Timer& timer,u32& flags,i32 frame,const float* rate){
    if(!(flags&1)){timer.reset();timer.rate=rate;flags|=1;}
    timer.previous=wrapping_add(frame,-1);timer.current=frame;timer.fractional=Extended::from_int(frame).to_float();
}
template<class T,unsigned N> void restart(Interpolator<T,N>& value,const float* rate){seek(value.timer,value.flags,0,rate);}
void set_color(RgbInterpolator& value,i32 duration,u8 mode,u32 from,u32 to,const float* rate){
    value.duration=duration;value.mode=static_cast<InterpolationMode>(mode);
    for(unsigned axis=0;axis<3;++axis){value.start[axis]=(from>>(axis*8))&255;value.end[axis]=(to>>(axis*8))&255;value.initial_tangent[axis]=value.final_tangent[axis]=0;}
    restart(value,rate);
}
void set_alpha(AlphaInterpolator& value,i32 duration,u8 mode,u8 from,u8 to,const float* rate,bool clear_tangents){
    value.duration=duration;value.mode=static_cast<InterpolationMode>(mode);value.start[0]=from;value.end[0]=to;
    if(clear_tangents)value.initial_tangent[0]=value.final_tangent[0]=0;
    restart(value,rate);
}
u32 rgb_bits(Rgb value){return (static_cast<u32>(value.blue)&255)|((static_cast<u32>(value.green)&255)<<8)|((static_cast<u32>(value.red)&255)<<16);}
i32 wrap_subtract(i32 a,i32 b){const u32 bits=static_cast<u32>(a)-static_cast<u32>(b);i32 result;std::memcpy(&result,&bits,4);return result;}
i32 calculate_integer(i32 left,i32 right,u32 operation){
    if(operation==0)return right;
    if(operation==1)return wrapping_add(left,right);
    if(operation==2)return wrap_subtract(left,right);
    if(operation==3){const u32 bits=static_cast<u32>(left)*static_cast<u32>(right);i32 result;std::memcpy(&result,&bits,4);return result;}
    if(!right||(left==INT32_MIN&&right==-1))__builtin_trap();
    return operation==4?left/right:left%right;
}
Extended calculate_float(Extended left,Extended right,u32 operation){
    switch(operation){case 0:return right;case 1:return right+left;case 2:return left-right;case 3:return right*left;case 4:return left/right;default:return remainder(left,right);}
}
bool interrupt(AnmVm& vm,const float* rate){
    AnmInstruction* fallback=nullptr;auto* marker=vm.script_begin;
    for(;marker->opcode!=-1;marker=marker->next()){
        if(marker->opcode==64){
            const i32 id=marker->argument<i32>(0);
            if(id==vm.pending_interrupt)break;
            if(id==-1)fallback=marker;
        }
    }
    vm.pending_interrupt=0;vm.flags&=~0x1000u;
    if(marker->opcode!=64)marker=fallback;
    if(!marker)return false;
    vm.saved_timer=vm.script_timer;vm.saved_timer_flags=vm.script_timer_flags;vm.saved_instruction=vm.instruction;
    seek(vm.script_timer,vm.script_timer_flags,marker->time,rate);vm.instruction=marker->next();vm.flags|=1;
    return true;
}
void animate(AnmVm& vm,AnmEnvironment& env){
    const auto rate=number(*env.rate);
    float* rotations[]={&vm.rotation.x,&vm.rotation.y,&vm.rotation.z};
    const float velocities[]={vm.angular_velocity.x,vm.angular_velocity.y,vm.angular_velocity.z};
    for(unsigned axis=0;axis<3;++axis)if(velocities[axis]!=0){
        *rotations[axis]=add_angle(*rotations[axis],(rate*number(velocities[axis])).to_float()).to_float();vm.flags|=4;
    }
    if(vm.scale_velocity.y!=0){vm.scale.y=(rate*number(vm.scale_velocity.y)+number(vm.scale.y)).to_float();vm.flags|=8;}
    if(vm.scale_velocity.x!=0){vm.scale.x=(rate*number(vm.scale_velocity.x)+number(vm.scale.x)).to_float();vm.flags|=12;}
    auto scroll=[&](float value,float velocity){
        auto next=rate*number(velocity)+number(value);
        if(number(1.0f)<next||number(1.0f)==next)next=next-number(1.0f);
        else if(next<number(0.0f))next=next+number(1.0f);
        return next.to_float();
    };
    vm.uv_offset.x=scroll(vm.uv_offset.x,vm.uv_velocity.x);vm.uv_offset.y=scroll(vm.uv_offset.y,vm.uv_velocity.y);
    if(vm.flags&0x2000){
        vm.position.x=Scalar::add(env.camera_delta->x,vm.position.x);
        vm.position.y=Scalar::add(env.camera_delta->y,vm.position.y);
        vm.position.z=Scalar::add(env.camera_delta->z,vm.position.z);
    }
    if(vm.position_interpolation.duration){const auto next=sample(vm.position_interpolation,env.rate);if(vm.flags&0x100)vm.child_position=next;else vm.script_position=next;}
    if(vm.color_interpolation.duration)vm.color=(vm.color&0xff000000)|rgb_bits(sample(vm.color_interpolation,env.rate));
    if(vm.alpha_interpolation.duration)vm.color=(vm.color&0xffffff)|(static_cast<u32>(sample(vm.alpha_interpolation,env.rate))<<24);
    if(vm.scale_interpolation.duration){vm.scale=sample(vm.scale_interpolation,env.rate);vm.flags|=8;}
    if(vm.rotation_interpolation.duration){vm.rotation=sample(vm.rotation_interpolation,env.rate);vm.flags|=4;}
    if(vm.color2_interpolation.duration)vm.secondary_color=(vm.secondary_color&0xff000000)|rgb_bits(sample(vm.color2_interpolation,env.rate));
    if(vm.alpha2_interpolation.duration)vm.secondary_color=(vm.secondary_color&0xffffff)|(static_cast<u32>(sample(vm.alpha2_interpolation,env.rate))<<24);
    if((vm.flags&0x3c00000)==0x2400000)vm.update_ring_geometry();
    vm.script_timer.tick();
}
}
// 0x43ee30. The ANM language drives sprites independently from ECL threads.
i32 AnmVm::update(AnmEnvironment& env){
    if(!instruction)return 1;
    if(flags&0x20000)return 0;
    struct RateScope{float* rate;float previous;~RateScope(){*rate=previous;}} scope{env.rate,*env.rate};
    if(flags&0x20000000)*env.rate=1;
    if(pending_interrupt&&!interrupt(*this,env.rate)){script_timer.advance(-1);animate(*this,env);return 0;}
    while(instruction->time<=script_timer.current){
        auto* current=instruction;
        auto integer=[&](u32 index){const auto value=current->argument<i32>(index);return current->is_reference(index)?integer_variable(value):value;};
        auto floating=[&](u32 index){const float value=current->argument<float>(index);return current->is_reference(index)?float_variable(value,env):number(value);};
        auto int_destination=[&](u32 index){return integer_reference(index,current->references,reinterpret_cast<i32*>(reinterpret_cast<u8*>(current)+8+4*index));};
        auto float_destination=[&](u32 index){return float_reference(index,current->references,reinterpret_cast<float*>(reinterpret_cast<u8*>(current)+8+4*index));};
        auto jump=[&](u32 offset,i32 time){seek(script_timer,script_timer_flags,time,env.rate);instruction=reinterpret_cast<AnmInstruction*>(reinterpret_cast<u8*>(script_begin)+offset);};
        auto field=[&](u32 mask,u32 shift,u32 value){flags=(flags&~mask)|((value<<shift)&mask);};
        const i32 op=current->opcode;
        if(op>=6&&op<=27){
            const bool three_arguments=op>=18;
            const u32 operation=three_arguments?(op-18)/2+1:(op-6)/2;
            if(!(op&1)){
                const i32 first=integer(1);
                const i32 second=three_arguments?integer(2):0;
                auto* out=int_destination(0);
                *out=calculate_integer(three_arguments?first:*out,three_arguments?second:first,operation);
            }else{
                Extended left,right;
                if(three_arguments&&operation==5){right=number(floating(2).to_float());left=number(floating(1).to_float());}
                else if(three_arguments){left=number(floating(1).to_float());right=number(floating(2).to_float());}
                else{right=number(floating(1).to_float());if(operation==5)left=number(floating(0).to_float());}
                auto* out=float_destination(0);
                if(!three_arguments&&operation!=5)left=number(*out);
                *out=calculate_float(left,right,operation).to_float();
            }
        }else if(op>=28&&op<=39){
            const u32 comparison=(op-28)/2;bool take=false;
            if(op&1){
                const auto left=number(floating(0).to_float()),right=floating(1);
                switch(comparison){
                case 0:take=left==right;break;case 1:take=!(left==right);break;
                case 2:take=!((right<left)||(right==left));break;
                case 3:take=(left<right)||(left==right);break;
                case 4:take=!((left<right)||(left==right));break;
                case 5:take=(right<left)||(right==left);break;
                }
            }else{
                const i32 left=integer(0),right=integer(1);
                switch(comparison){case 0:take=left==right;break;case 1:take=left!=right;break;case 2:take=left<right;break;case 3:take=left<=right;break;case 4:take=left>right;break;case 5:take=left>=right;break;}
            }
            if(take){jump(current->argument<u32>(2),current->argument<i32>(3));continue;}
        }else switch(op){
        case -1:case 1:flags&=~1u;[[fallthrough]];
        case 2:instruction=nullptr;return 1;
        case 3:flags|=1;env.bind_sprite(*this,integer(0));sprite_frame=script_timer.current;break;
        case 4:jump(current->argument<u32>(0),current->argument<i32>(1));continue;
        case 5:{auto* counter=int_destination(0);*counter=wrapping_add(*counter,-1);if(integer(0)>0){jump(current->argument<u32>(1),current->argument<i32>(2));continue;}break;}
        case 40:{const i32 limit=integer(1);const u32 value=env.random(*this).bounded(static_cast<u32>(limit));*int_destination(0)=static_cast<i32>(value);break;}
        case 41:{const auto maximum=floating(1);const float value=(env.random(*this).unit()*maximum).to_float();*float_destination(0)=value;break;}
        case 42:case 43:case 44:case 45:case 46:{
            const auto value=number(floating(1).to_float());auto* out=float_destination(0);Extended result;
            if(op==42)result=sine(value);else if(op==43)result=cosine(value);else if(op==44)result=tangent(value);
            else if(op==45)result=arccosine(value);else result=angle_to(value,number(1.0f));
            *out=result.to_float();break;
        }
        case 47:{const float value=floating(0).to_float();*float_destination(0)=add_angle(value,0).to_float();break;}
        case 48:{const float z=floating(2).to_float(),y=floating(1).to_float(),x=floating(0).to_float();(flags&0x100?child_position:script_position)={x,y,z};break;}
        case 49:rotation.x=floating(0).to_float();rotation.y=floating(1).to_float();rotation.z=floating(2).to_float();flags|=4;break;
        case 50:scale.x=floating(0).to_float();scale.y=floating(1).to_float();flags|=8;break;
        case 51:color=(color&0xffffff)|(static_cast<u32>(integer(0))<<24);break;
        case 52:case 76:{
            u32& destination=op==52?color:secondary_color;
            const u32 red=static_cast<u32>(integer(0))&255;destination=(destination&~0xff0000u)|(red<<16);
            const u32 green=static_cast<u32>(integer(1))&255;destination=(destination&~0xff00u)|(green<<8);
            const u32 blue=static_cast<u32>(integer(2))&255;destination=(destination&~255u)|blue;break;
        }
        case 53:angular_velocity.x=floating(0).to_float();angular_velocity.y=floating(1).to_float();angular_velocity.z=floating(2).to_float();flags|=4;break;
        case 54:scale_velocity.x=floating(0).to_float();scale_velocity.y=floating(1).to_float();break;
        case 55:{const i32 duration=integer(1);set_alpha(alpha_interpolation,duration,0,color>>24,current->argument<u8>(0),env.rate,true);break;}
        case 56:{
            auto& value=position_interpolation;value.duration=integer(0);
            std::memcpy(value.initial_tangent,env.default_tangent,12);std::memcpy(value.final_tangent,env.default_tangent,12);
            value.mode=static_cast<InterpolationMode>(current->argument<i32>(1));const auto from=flags&0x100?child_position:script_position;std::memcpy(value.start,&from,12);
            const float z=floating(4).to_float(),y=floating(3).to_float(),x=floating(2).to_float();value.end[0]=x;value.end[1]=y;value.end[2]=z;restart(value,env.rate);break;
        }
        case 57:case 78:{
            const u32 from=op==57?color:secondary_color;
            const u32 blue=static_cast<u32>(integer(4))&255,green=static_cast<u32>(integer(3))&255,red=static_cast<u32>(integer(2))&255;
            const i32 duration=integer(0);set_color(op==57?color_interpolation:color2_interpolation,duration,current->argument<u8>(1),from,blue|(green<<8)|(red<<16),env.rate);break;
        }
        case 58:case 79:{
            const u8 to=static_cast<u8>(integer(2));const i32 duration=integer(0);
            set_alpha(op==58?alpha_interpolation:alpha2_interpolation,duration,current->argument<u8>(1),(op==58?color:secondary_color)>>24,to,env.rate,op==58);break;
        }
        case 59:{
            const float z=floating(4).to_float(),y=floating(3).to_float(),x=floating(2).to_float();auto& value=rotation_interpolation;
            value.duration=integer(0);std::memcpy(value.initial_tangent,env.default_tangent,12);std::memcpy(value.final_tangent,env.default_tangent,12);
            value.mode=static_cast<InterpolationMode>(current->argument<i32>(1));std::memcpy(value.start,&rotation,12);value.end[0]=x;value.end[1]=y;value.end[2]=z;restart(value,env.rate);flags|=4;break;
        }
        case 60:{
            const float y=floating(3).to_float(),x=floating(2).to_float();auto& value=scale_interpolation;
            value.duration=integer(0);value.mode=static_cast<InterpolationMode>(current->argument<u8>(1));std::memcpy(value.start,&scale,8);value.end[0]=x;value.end[1]=y;restart(value,env.rate);flags|=8;break;
        }
        case 61:flags=(flags^0x200)|8;scale.x=Scalar::mul(scale.x,-1.0f);break;
        case 62:flags=(flags^0x400)|8;scale.y=Scalar::mul(scale.y,-1.0f);break;
        case 63:case 69:
            if(op==69)flags&=~1u;
            if(pending_interrupt){if(interrupt(*this,env.rate))continue;}else flags|=0x1000;
            script_timer.advance(-1);animate(*this,env);return 0;
        case 65:{const u32 value=current->argument<u32>(0);field(0xc0000,18,value&65535);field(0x300000,20,value>>16);break;}
        case 66:field(0x30,4,current->argument<u32>(0));break;
        case 67:field(0x3c00000,22,current->argument<u32>(0));if((flags&0x3c00000)==0x2800000)env.change_draw_mode(*this);break;
        case 68:owner_tag=current->argument<u8>(0);break;
        case 70:uv_velocity.x=floating(0).to_float();break;case 71:uv_velocity.y=floating(0).to_float();break;
        case 72:field(1,0,current->argument<u32>(0));break;case 73:field(0x800,11,current->argument<u32>(0));break;
        case 74:field(0x2000,13,current->argument<u32>(0));break;
        case 75:script_timer.advance(Extended::from_int(wrap_subtract(0,integer(0))).to_float());break;
        case 77:secondary_color=(secondary_color&0xffffff)|(static_cast<u32>(integer(0))<<24);break;
        case 80:field(0x8000,15,current->argument<u8>(0));break;
        case 81:script_timer=saved_timer;script_timer_flags=saved_timer_flags;instruction=saved_instruction;continue;
        case 82:field(0x8000000,27,current->argument<u8>(0));break;
        case 83:script_position=position;position={0,0,0};break;
        case 84:flags=(flags&0xfe7fffff)|0x2400000;geometry=env.allocate_geometry(static_cast<u32>(integer(0))*0x38);break;
        case 85:field(0x10000000,28,current->argument<u8>(0));break;
        case 86:field(0x20000000,29,integer(0));break;
        case 87:field(0x40000000,30,current->argument<u8>(0));break;
        case 89:field(0x80000000,31,current->argument<u32>(0));break;
        case 88:case 90:case 91:case 92:{
            auto* child=env.spawn_child(*this,integer(0),op);
            if(!child)__builtin_trap();
            child->child_node.insert_after(child_node);
            child->child_position=script_position;child->position=position;break;
        }
        default:break; // Includes nop (0), interrupt labels (64), and original unknown-op behavior.
        }
        instruction=current->next();
    }
    animate(*this,env);return 0;
}
}
