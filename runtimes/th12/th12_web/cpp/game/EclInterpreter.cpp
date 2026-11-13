#include "EclOwner.hpp"
#include "EclRoutes.hpp"
#include "Trace.hpp"
#include <cstdio>
#include <cmath>
namespace th11 {
// The command the interpreter was running, so an argument-stack underflow can say which
// opcode asked for more arguments than its instruction carried (EnemyScript.cpp).
extern u32 g_ecl_last_opcode;
inline void report_ecl_underflow() {
    static bool reported=false;
    if(reported)return;
    reported=true;
    std::printf("[ecl] argument stack underflow in command 0x%x\n",g_ecl_last_opcode);
    std::fflush(stdout);
}
namespace {
enum class Op:u16 {
    Nop=0,Delete=1,Return=10,Call=11,Jump=12,JumpIfZero=13,JumpIfNonzero=14,
    Spawn=15,SpawnNamed=16,CancelThread=17,SetThreadFlag=18,ClearThreadFlag=19,SetThreadState=20,CancelSecondary=21,Diagnostic=30,
    EnterFrame=40,LeaveFrame=41,PushInt=42,StoreInt=43,PushFloat=44,StoreFloat=45,
    AddInt=50,AddFloat=51,SubtractInt=52,SubtractFloat=53,MultiplyInt=54,MultiplyFloat=55,DivideInt=56,DivideFloat=57,
    Remainder=58,EqualInt=59,EqualFloat=60,NotEqualInt=61,NotEqualFloat=62,LessInt=63,LessFloat=64,
    LessEqualInt=65,LessEqualFloat=66,GreaterInt=67,GreaterFloat=68,GreaterEqualInt=69,GreaterEqualFloat=70,
    NotInt=71,NotFloat=72,LogicalOr=73,LogicalAnd=74,Xor=75,Or=76,And=77,PostDecrement=78,Sin=79,Cos=80,
    Polar=81,NormalizeAngle=82,Wait=83,NegateInt=84,NegateFloatBits=85,LengthSquared=86,AngleTo=87,Sqrt=88,
    WaitFloat=0xff00,Length=0xff01,RandomPolar=0xff02,ScalarInterpolation=0xff03
};
i32 pop_integer(EclStack& s){u32 b;EclValueType t;if(!s.pop(b,t)){report_ecl_underflow();return 0;}return t==EclValueType::Float?truncate_int(float_bits(b)):signed_bits(b);}
float pop_float(EclStack& s){u32 b;EclValueType t;if(!s.pop(b,t)){report_ecl_underflow();return 0.f;}return t==EclValueType::Integer?float(signed_bits(b)):float_bits(b);}
void push_integer(EclStack& s,i32 v){s.push(EclValueType::Integer,u32(v));}
void push_float(EclStack& s,float v){s.push(EclValueType::Float,float_bits(v));}
template<class T>T& destination(T* p){if(!p)__builtin_trap();return *p;}
// Jump operands are signed relative offsets; a backwards jump must subtract
// bytes directly, rather than form an overflowing unsigned pointer first.
const EclInstruction* advance(const EclInstruction* p,u32 n){return reinterpret_cast<const EclInstruction*>(reinterpret_cast<const u8*>(p)+signed_bits(n));}
float normalize(float value){constexpr double pi=3.1415927410125732421875,tau=6.283185482025146484375;u32 loops=0;while(value>pi){value=float(double(value)-tau);if(loops++>32)break;}while(value<-pi){value=float(double(value)+tau);if(loops++>32)break;}return value;}
void diagnostic(EclContext& context,EclServices& services){
    const char* cursor=reinterpret_cast<const char*>(context.instruction)+20;u32 index=1,descriptor=0;
    auto* scratch=static_cast<char*>(services.allocate(1024));if(!scratch)__builtin_trap();scratch[0]=0;
    while(const char* marker=std::strchr(cursor,'%')){
        if(marker[1]=='d'||marker[1]=='f'){
            const auto* instruction=context.instruction;const u32 length=instruction->argument();
            const auto* args=reinterpret_cast<const u8*>(instruction)+16;const char type=args[length+4+descriptor];
            const u32 raw=instruction->argument(length/4+2+descriptor/4);
            if(instruction->references&(1u<<(index&31))){if(type=='f'||type=='g')context.resolve_float(float_bits(raw),services);else context.resolve_integer(signed_bits(raw),services);}
            ++index;descriptor+=8;
        }
        cursor=marker+2;
    }
    services.release(scratch);
}
#if PORT_TITLE == 16
// Own4171c0 scalar sampler; its current-value store and SSE association matter.
float scalar_sample(EclContext::ScalarSlot& slot,const float* rate){
    auto& v=slot.value;const auto mode=v.mode;
    if(v.duration>0){v.timer.tick();if(v.timer.current>=v.duration){v.timer.set(v.duration,rate);v.duration=0;return mode==InterpolationMode::Velocity||mode==InterpolationMode::Acceleration?v.start[0]:v.end[0];}}
    if(!v.duration)return mode==InterpolationMode::Velocity||mode==InterpolationMode::Acceleration?v.start[0]:v.end[0];
    if(mode==InterpolationMode::Velocity)slot.current=v.start[0]=v.start[0]+v.end[0];
    else if(mode==InterpolationMode::Acceleration){slot.current=v.start[0]=v.start[0]+v.final_tangent[0];v.final_tangent[0]+=v.end[0];}
    else if(mode==InterpolationMode::Hermite){
        const float t=v.timer.fractional/float(v.duration),r=1.f-t,m=t-1.f,twice=t+t;
        const float a=((m*m)*(twice+1.f))*v.start[0],b=((t*t)*(3.f-twice))*v.end[0],c=((r*r)*t)*v.initial_tangent[0],d=((m*t)*t)*v.final_tangent[0];
        slot.current=((b+a)+c)+d;
    }else{const float delta=v.end[0]-v.start[0];slot.current=easing(v.timer.fractional,v.duration,mode)*delta+v.start[0];}
    return slot.current;
}
#endif
}
// 0x45b860. Original ECL bytecode is interpreted directly by hand-authored C++.
i32 EclContext::update(float elapsed,EclServices& services){
    if(!instruction)return -1;
#if PORT_TITLE == 16
    th16_scalar_rate=services.scalar_rate();
#endif
    while((kPortTitle==14||kPortTitle==16)?float(instruction->time)<=time:double(instruction->time)<=double(time)){
        if(instruction->difficulty&u8(difficulty)){
            // 2000+opcode so a trap inside the script pinpoints the instruction.
            trace_point=2000+i32(instruction->opcode);
            // The titles renumber the bytecode too; the row in EclRoutes.hpp carries the
            // measured ones (TH13's 23 is TH12's 83).
            const auto op=Op(ecl_vm_opcode(instruction->opcode));
            switch(op){
            case Op::Nop:break;
            case Op::Delete:instruction=nullptr;return -1;
            case Op::Return:{stack.leave_frame();if(!stack.top){instruction=nullptr;return -1;}u32 b;if(stack.pop_raw(b))instruction=reinterpret_cast<const EclInstruction*>(uintptr_t(b));if(stack.pop_raw(b))time=float_bits(b);if(!instruction)return -1;break;}
            case Op::Call:if(call_subroutine(*this,*this,0,services))return -1;continue;
            case Op::Jump:case Op::JumpIfZero:case Op::JumpIfNonzero:{const bool jump=op==Op::Jump?true:(pop_integer(stack)==0)==(op==Op::JumpIfZero);if(jump){time=float(instruction->argument<i32>(1));instruction=advance(instruction,instruction->argument());continue;}break;}
            case Op::Spawn:owner->spawn_thread(-1,0,services);break;
            case Op::SpawnNamed:{i32 id=instruction->argument<i32>((instruction->argument()+4)/4);if(instruction->references&2)id=resolve_integer(id,services);owner->spawn_thread(id,1,services);break;}
            case Op::CancelThread:case Op::SetThreadFlag:case Op::ClearThreadFlag:case Op::SetThreadState:{auto* node=owner->find_thread(integer_argument(0,services));if(node){auto& c=*node->value;if(op==Op::CancelThread)c.instruction=nullptr;if(op==Op::SetThreadFlag)c.flags|=1;if(op==Op::ClearThreadFlag)c.flags&=~1u;if(op==Op::SetThreadState)c.state=u32(integer_argument(1,services));}break;}
            case Op::CancelSecondary:owner->cancel_secondary_threads();break;
            case Op::Diagnostic:diagnostic(*this,services);break;
            case Op::EnterFrame:stack.enter_frame(integer_argument(0,services));break;
            case Op::LeaveFrame:stack.leave_frame();break;
            case Op::PushInt:{const i32 raw=instruction->argument<i32>();push_integer(stack,(kPortTitle==14||kPortTitle==16)&&(instruction->references&1)&&raw>=-100&&raw<=-1?pop_integer(stack):integer_argument(0,services));break;}
            case Op::PushFloat:{const float raw=instruction->argument<float>();push_float(stack,(kPortTitle==14||kPortTitle==16)&&(instruction->references&1)&&raw>=-100&&raw<=-1?pop_float(stack):float_argument(0,services));break;}
            case Op::StoreInt:{auto* out=integer_reference(0,services);destination(out)=pop_integer(stack);break;}
            case Op::StoreFloat:{auto* out=float_reference(0,services);destination(out)=pop_float(stack);break;}
            case Op::AddInt:case Op::SubtractInt:case Op::MultiplyInt:case Op::DivideInt:case Op::Remainder:
            case Op::EqualInt:case Op::NotEqualInt:case Op::LessInt:case Op::LessEqualInt:case Op::GreaterInt:case Op::GreaterEqualInt:
            case Op::LogicalOr:case Op::LogicalAnd:case Op::Xor:case Op::Or:case Op::And:{
                const i32 right=pop_integer(stack),left=pop_integer(stack);i32 value=0;
                switch(op){
                case Op::AddInt:value=wrapping_add(left,right);break;case Op::SubtractInt:value=wrapping_sub(left,right);break;
                case Op::MultiplyInt:value=signed_bits(u32(left)*u32(right));break;
                case Op::DivideInt:case Op::Remainder:if(!right||(left==INT32_MIN&&right==-1))__builtin_trap();value=op==Op::DivideInt?left/right:left%right;break;
                case Op::EqualInt:value=left==right;break;case Op::NotEqualInt:value=left!=right;break;
                case Op::LessInt:value=left<right;break;case Op::LessEqualInt:value=left<=right;break;case Op::GreaterInt:value=left>right;break;case Op::GreaterEqualInt:value=left>=right;break;
                case Op::LogicalOr:value=left||right;break;case Op::LogicalAnd:value=left&&right;break;case Op::Xor:value=left^right;break;case Op::Or:value=left|right;break;case Op::And:value=left&right;break;default:__builtin_unreachable();
                }push_integer(stack,value);break;
            }
            case Op::AddFloat:case Op::SubtractFloat:case Op::MultiplyFloat:case Op::DivideFloat:{const double right=pop_float(stack),left=pop_float(stack);const double value=op==Op::AddFloat?left+right:op==Op::SubtractFloat?left-right:op==Op::MultiplyFloat?left*right:left/right;push_float(stack,float(value));break;}
            case Op::EqualFloat:case Op::NotEqualFloat:case Op::LessFloat:case Op::LessEqualFloat:case Op::GreaterFloat:case Op::GreaterEqualFloat:{const float right=pop_float(stack),left=pop_float(stack);bool value=false;if(op==Op::EqualFloat)value=left==right;else if(op==Op::NotEqualFloat)value=left!=right;else if(op==Op::LessFloat)value=left<right;else if(op==Op::LessEqualFloat)value=left<=right;else if(op==Op::GreaterFloat)value=left>right;else value=left>=right;push_integer(stack,value);break;}
            case Op::NotInt:push_integer(stack,pop_integer(stack)==0);break;
            case Op::NotFloat:push_integer(stack,pop_float(stack)==0);break;
            case Op::NegateInt:push_integer(stack,signed_bits(0u-u32(pop_integer(stack))));break;
            case Op::NegateFloatBits:{const u32 bits=float_bits(pop_float(stack));stack.push(EclValueType::Float,(kPortTitle==14||kPortTitle==16)?bits^0x80000000u:0u-bits);break;}
            case Op::PostDecrement:{const i32 value=integer_argument(0,services);destination(integer_reference(0,services))=wrapping_add(value,-1);push_integer(stack,value);break;}
            case Op::Sin:case Op::Cos:case Op::Sqrt:{const double value=pop_float(stack);push_float(stack,float(op==Op::Sin?std::sin(value):op==Op::Cos?std::cos(value):std::sqrt(value)));break;}
            case Op::Polar:{const float length=float_argument(3,services),radians=normalize(float_argument(2,services));const float x=float(std::cos(double(radians))*length),y=float(std::sin(double(radians))*length);destination(float_reference(0,services))=x;destination(float_reference(1,services))=y;break;}
            case Op::RandomPolar:{
                // TH14 483b3a uses two 4034b0 signed draws, then 484630.
                const float from=float_argument(2,services),to=float_argument(3,services);
                const float angle=float(float(services.floating(-9987))*3.1415927410125732f);
                const float draw=float(services.floating(-9987)),difference=to-from;
                const float radius=float(draw*difference)+from;
                const float x=float(std::cos(double(angle))*radius),y=float(std::sin(double(angle))*radius);
                destination(float_reference(0,services))=x;destination(float_reference(1,services))=y;break;
            }
            case Op::NormalizeAngle:{const float value=normalize(float_argument(0,services));destination(float_reference(0,services))=value;break;}
            case Op::Wait:{const i32 n=integer_argument(0,services);time=(kPortTitle==14||kPortTitle==16)?time-float(n):float(double(time)-n);break;}
            case Op::WaitFloat:time-=float(float_argument(0,services));break;
            case Op::Length:case Op::LengthSquared:{const float x=float(float_argument(1,services)),y=float(float_argument(2,services));const float sum=(kPortTitle==14||kPortTitle==16)?float(y*y)+float(x*x):float(double(y)*y+double(x)*x);destination(float_reference(0,services))=op==Op::Length?float(std::sqrt(double(sum))):sum;break;}
#if PORT_TITLE == 16
            case Op::ScalarInterpolation:{
                const i32 index=integer_argument(0,services);if(u32(index)>=8)return -2;auto& slot=th16_scalars[index];slot.target=instruction;
                scalar_sample(slot,&th16_scalar_rate);auto& v=slot.value;
                v.duration=integer_argument(2,services);v.mode=InterpolationMode(integer_argument(3,services));v.start[0]=float(float_argument(4,services));v.end[0]=float(float_argument(5,services));
                auto* out=float_reference(1,services);if(!out)return -2;*out=v.start[0];
                v.initial_tangent[0]=instruction->opcode==92?float(float_argument(6,services)):0.f;v.final_tangent[0]=instruction->opcode==92?float(float_argument(7,services)):0.f;
                v.timer.set(0,&th16_scalar_rate);break;
            }
#endif
            case Op::AngleTo:{
                float dx,dy;
                if constexpr((kPortTitle==14||kPortTitle==16)){const float x1=float_argument(1,services),y1=float_argument(2,services),x2=float_argument(3,services),y2=float_argument(4,services);dx=x2-x1;dy=y2-y1;}
                else{const double x=float_argument(2,services),y=float_argument(3,services);dx=float(x-float_argument(0,services));dy=float(y-float_argument(1,services));}
                destination(float_reference(0,services))=float(std::atan2(double(dy),double(dx)));break;
            }
            default:{const i32 result=services.command(*this);if(result==-1){
#if PORT_TITLE == 16
                goto finish_th16_scalars;
#else
                return 0;
#endif
            }if(result==-2)return -2;break;}
            }
            if constexpr((kPortTitle==14||kPortTitle==16)){
                // 483c0c performs argument cleanup; stack operators use 483c1f.
                const u16 raw=instruction->opcode;
                if(!((raw>=42&&raw<=45)||(raw>=50&&raw<=80)||raw==83||raw==84||raw==88)){
                    const u32 bytes=instruction->stack_adjustment&255;
                    if(bytes>u32(stack.top))return -2;stack.top-=i32(bytes);
                }
            }
        }
        instruction=advance(instruction,instruction->length);
    }
    time=float(double(time)+elapsed);
#if PORT_TITLE == 16
finish_th16_scalars:
    {
        const auto* resume_instruction=instruction;
        for(auto& slot:th16_scalars)if(slot.value.duration){
            if(!slot.target)return -2;instruction=slot.target;float* out=float_reference(1,services);instruction=resume_instruction;
            if(!out)return -2;*out=scalar_sample(slot,&th16_scalar_rate);
        }
    }
#endif
    return 0;
}
}
