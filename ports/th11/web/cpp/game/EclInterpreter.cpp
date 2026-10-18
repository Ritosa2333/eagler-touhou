#include "EclOwner.hpp"
#include <cmath>
namespace th11 {
namespace {
enum class Op:u16 {
    Nop=0,Delete=1,Return=10,Call=11,Jump=12,JumpIfZero=13,JumpIfNonzero=14,
    Spawn=15,SpawnNamed=16,CancelThread=17,SetThreadFlag=18,ClearThreadFlag=19,SetThreadState=20,CancelSecondary=21,Diagnostic=30,
    EnterFrame=40,LeaveFrame=41,PushInt=42,StoreInt=43,PushFloat=44,StoreFloat=45,
    AddInt=50,AddFloat=51,SubtractInt=52,SubtractFloat=53,MultiplyInt=54,MultiplyFloat=55,DivideInt=56,DivideFloat=57,
    Remainder=58,EqualInt=59,EqualFloat=60,NotEqualInt=61,NotEqualFloat=62,LessInt=63,LessFloat=64,
    LessEqualInt=65,LessEqualFloat=66,GreaterInt=67,GreaterFloat=68,GreaterEqualInt=69,GreaterEqualFloat=70,
    NotInt=71,NotFloat=72,LogicalOr=73,LogicalAnd=74,Xor=75,Or=76,And=77,PostDecrement=78,Sin=79,Cos=80,
    Polar=81,NormalizeAngle=82,Wait=83,NegateInt=84,NegateFloatBits=85,LengthSquared=86,AngleTo=87,Sqrt=88
};
i32 pop_integer(EclStack& s){u32 b;EclValueType t;if(!s.pop(b,t))__builtin_trap();return t==EclValueType::Float?truncate_int(float_bits(b)):signed_bits(b);}
float pop_float(EclStack& s){u32 b;EclValueType t;if(!s.pop(b,t))__builtin_trap();return t==EclValueType::Integer?float(signed_bits(b)):float_bits(b);}
void push_integer(EclStack& s,i32 v){s.push(EclValueType::Integer,u32(v));}
void push_float(EclStack& s,float v){s.push(EclValueType::Float,float_bits(v));}
template<class T>T& destination(T* p){if(!p)__builtin_trap();return *p;}
const EclInstruction* advance(const EclInstruction* p,u32 n){return reinterpret_cast<const EclInstruction*>(reinterpret_cast<const u8*>(p)+n);}
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
}
// 0x45b860. Original ECL bytecode is interpreted directly by hand-authored C++.
i32 EclContext::update(float elapsed,EclServices& services){
    if(!instruction)return -1;
    while(double(instruction->time)<=double(time)){
        if(instruction->difficulty&u8(difficulty)){
            const auto op=Op(instruction->opcode);
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
            case Op::PushInt:push_integer(stack,integer_argument(0,services));break;
            case Op::PushFloat:push_float(stack,float_argument(0,services));break;
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
            case Op::NegateFloatBits:stack.push(EclValueType::Float,0u-float_bits(pop_float(stack)));break;
            case Op::PostDecrement:{const i32 value=integer_argument(0,services);destination(integer_reference(0,services))=wrapping_add(value,-1);push_integer(stack,value);break;}
            case Op::Sin:case Op::Cos:case Op::Sqrt:{const double value=pop_float(stack);push_float(stack,float(op==Op::Sin?std::sin(value):op==Op::Cos?std::cos(value):std::sqrt(value)));break;}
            case Op::Polar:{const float length=float_argument(3,services),radians=normalize(float_argument(2,services));const float x=float(std::cos(double(radians))*length),y=float(std::sin(double(radians))*length);destination(float_reference(0,services))=x;destination(float_reference(1,services))=y;break;}
            case Op::NormalizeAngle:{const float value=normalize(float_argument(0,services));destination(float_reference(0,services))=value;break;}
            case Op::Wait:time=float(double(time)-integer_argument(0,services));break;
            case Op::LengthSquared:{const double x=float(float_argument(1,services)),y=float(float_argument(2,services));destination(float_reference(0,services))=float(y*y+x*x);break;}
            case Op::AngleTo:{const double x=float_argument(2,services),y=float_argument(3,services);const float dx=float(x-float_argument(0,services)),dy=float(y-float_argument(1,services));destination(float_reference(0,services))=float(std::atan2(double(dy),double(dx)));break;}
            default:{const i32 result=services.command(*this);if(result==-1)return 0;if(result==-2)return -2;break;}
            }
        }
        instruction=advance(instruction,instruction->length);
    }
    time=float(double(time)+elapsed);return 0;
}
}
