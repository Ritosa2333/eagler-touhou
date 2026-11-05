#include "EclProgram.hpp"
#include "GameMath.hpp"
namespace th10 {
namespace {
enum class Op : u16 {
    Nop=0,Delete=1,Return=10,Call=11,Jump=12,JumpIfZero=13,JumpIfNonzero=14,
    Spawn=15,SpawnNamed=16,CancelThread=17,SetThreadFlag=18,ClearThreadFlag=19,
    SetThreadState=20,CancelSecondary=21,Diagnostic=30,
    EnterFrame=40,LeaveFrame=41,PushInt=42,StoreInt=43,PushFloat=44,StoreFloat=45,
    AddInt=50,AddFloat=51,SubtractInt=52,SubtractFloat=53,MultiplyInt=54,MultiplyFloat=55,
    DivideInt=56,DivideFloat=57,Remainder=58,EqualInt=59,EqualFloat=60,
    NotEqualInt=61,NotEqualFloat=62,LessInt=63,LessFloat=64,LessEqualInt=65,LessEqualFloat=66,
    GreaterInt=67,GreaterFloat=68,GreaterEqualInt=69,GreaterEqualFloat=70,
    NotInt=71,NotFloat=72,LogicalOr=73,LogicalAnd=74,Xor=75,Or=76,And=77,
    PostDecrement=78,Sin=79,Cos=80,Polar=81,NormalizeAngle=82,Wait=83,
    NegateInt=84,NegateFloatBits=85,LengthSquared=86,AngleTo=87
};
i32 integer_bits(u32 value){i32 result;std::memcpy(&result,&value,4);return result;}
float float_bits(u32 value){float result;std::memcpy(&result,&value,4);return result;}
u32 bits(float value){u32 result;std::memcpy(&result,&value,4);return result;}
i32 pop_integer(EclStack& stack){
    u32 value=0;EclValueType type;
    // Invalid bytecode otherwise reads uninitialized native stack memory.
    if(!stack.pop_value(value,type))__builtin_trap();
    return type==EclValueType::Float?number(float_bits(value)).truncate_int():integer_bits(value);
}
Extended pop_float(EclStack& stack,bool round=true){
    u32 value=0;EclValueType type;if(!stack.pop_value(value,type))__builtin_trap();
    auto result=type==EclValueType::Integer?Extended::from_int(integer_bits(value)):number(float_bits(value));
    return round?number(result.to_float()):result;
}
void push_integer(EclStack& stack,i32 value){stack.push(EclValueType::Integer,&value,4);}
void push_float(EclStack& stack,Extended value){const float result=value.to_float();stack.push(EclValueType::Float,&result,4);}
template<class T> T& destination(T* pointer){if(!pointer)__builtin_trap();return *pointer;}
EclInstruction* advance(EclInstruction* instruction,u32 bytes){
    return reinterpret_cast<EclInstruction*>(reinterpret_cast<u8*>(instruction)+bytes);
}
void diagnostic(EclContext& context,EclServices& services){
    // Retail code consumes formatting arguments but produces no visible text.
    const char* cursor=reinterpret_cast<const char*>(context.instruction)+20;
    auto* scratch=static_cast<char*>(services.allocate(1024));if(!scratch)__builtin_trap();scratch[0]=0;
    u32 index=1,descriptor=0;
    while(const char* marker=std::strchr(cursor,'%')){
        if(marker[1]=='d'||marker[1]=='f'){
            const auto* instruction=context.instruction;
            const u32 length=instruction->argument(0);
            const auto* args=reinterpret_cast<const u8*>(instruction)+16;
            const char type=args[length+4+descriptor];
            const u32 raw=instruction->argument(length/4+2+descriptor/4);
            if(type=='f'||type=='g')context.resolve_float(index,float_bits(raw),services);
            else context.resolve_integer(index,integer_bits(raw),services);
            ++index;descriptor+=8;
        }
        cursor=marker+2;
    }
    services.release(scratch);
}
}
// 0x44e1a0. This executes the game's ECL bytecode, not original x86 code.
// Game-specific commands dispatch through the original owner's service interface.
i32 EclContext::update(float elapsed,EclServices& services) {
    if(!instruction)return -1;
    while(Extended::from_int(instruction->time)<number(time)||Extended::from_int(instruction->time)==number(time)){
        if(instruction->difficulty&difficulty){
            const Op op=static_cast<Op>(instruction->opcode);
            switch(op){
            case Op::Nop:break;
            case Op::Delete:instruction=nullptr;return -1;
            case Op::Return:
                stack.leave_frame();
                if(!stack.top){instruction=nullptr;return -1;}
                stack.pop(EclValueType::Untyped,&instruction,4);
                stack.pop(EclValueType::Untyped,&time,4);
                if(!instruction)return -1;
                break;
            case Op::Call:
                if(call_subroutine(*this,*this,0,services))return -1;
                continue;
            case Op::Jump:case Op::JumpIfZero:case Op::JumpIfNonzero:{
                const bool jump=op==Op::Jump?true:(pop_integer(stack)==0)==(op==Op::JumpIfZero);
                if(jump){
                    time=Extended::from_int(integer_bits(instruction->argument(1))).to_float();
                    instruction=advance(instruction,instruction->argument(0));continue;
                }
                break;
            }
            case Op::Spawn:owner->spawn_thread(-1,0,services);break;
            case Op::SpawnNamed:{
                const u32 index=(instruction->argument(0)+4)/4;
                const i32 id=resolve_integer(1,integer_bits(instruction->argument(index)),services);
                owner->spawn_thread(id,1,services);break;
            }
            case Op::CancelThread:case Op::SetThreadFlag:case Op::ClearThreadFlag:case Op::SetThreadState:{
                auto* thread=owner->find_thread(integer_argument(0,services));
                if(thread){
                    auto& context=*thread->value;
                    if(op==Op::CancelThread)context.instruction=nullptr;
                    if(op==Op::SetThreadFlag)context.flags|=1;
                    if(op==Op::ClearThreadFlag)context.flags&=~1u;
                    if(op==Op::SetThreadState)context.state_1018=integer_argument(1,services);
                }
                break;
            }
            case Op::CancelSecondary:owner->cancel_secondary_threads();break;
            case Op::Diagnostic:diagnostic(*this,services);break;
            case Op::EnterFrame:stack.enter_frame(integer_argument(0,services));break;
            case Op::LeaveFrame:stack.leave_frame();break;
            case Op::PushInt:push_integer(stack,integer_argument(0,services));break;
            case Op::StoreInt:{auto* out=integer_reference(0,services);destination(out)=pop_integer(stack);break;}
            case Op::PushFloat:push_float(stack,float_argument(0,services));break;
            case Op::StoreFloat:{auto* out=float_reference(0,services);destination(out)=pop_float(stack).to_float();break;}
            case Op::AddInt:case Op::SubtractInt:case Op::MultiplyInt:case Op::DivideInt:case Op::Remainder:
            case Op::EqualInt:case Op::NotEqualInt:case Op::LessInt:case Op::LessEqualInt:case Op::GreaterInt:case Op::GreaterEqualInt:
            case Op::LogicalOr:case Op::LogicalAnd:case Op::Xor:case Op::Or:case Op::And:{
                const i32 right=pop_integer(stack),left=pop_integer(stack);i32 value=0;
                switch(op){
                case Op::AddInt:value=wrapping_add(left,right);break;
                case Op::SubtractInt:value=integer_bits(static_cast<u32>(left)-static_cast<u32>(right));break;
                case Op::MultiplyInt:value=integer_bits(static_cast<u32>(left)*static_cast<u32>(right));break;
                case Op::DivideInt:case Op::Remainder:
                    if(!right||(left==INT32_MIN&&right==-1))__builtin_trap();
                    value=op==Op::DivideInt?left/right:left%right;break;
                case Op::EqualInt:value=left==right;break;case Op::NotEqualInt:value=left!=right;break;
                case Op::LessInt:value=left<right;break;case Op::LessEqualInt:value=left<=right;break;
                case Op::GreaterInt:value=left>right;break;case Op::GreaterEqualInt:value=left>=right;break;
                case Op::LogicalOr:value=left||right;break;case Op::LogicalAnd:value=left&&right;break;
                case Op::Xor:value=left^right;break;case Op::Or:value=left|right;break;case Op::And:value=left&right;break;
                default:__builtin_unreachable();
                }
                push_integer(stack,value);break;
            }
            case Op::AddFloat:case Op::SubtractFloat:case Op::MultiplyFloat:case Op::DivideFloat:{
                const auto right=pop_float(stack),left=pop_float(stack,false);Extended value;
                if(op==Op::AddFloat)value=left+right;else if(op==Op::SubtractFloat)value=left-right;
                else if(op==Op::MultiplyFloat)value=left*right;else value=left/right;
                push_float(stack,value);break;
            }
            case Op::EqualFloat:case Op::NotEqualFloat:case Op::LessFloat:case Op::LessEqualFloat:case Op::GreaterFloat:case Op::GreaterEqualFloat:{
                const auto right=pop_float(stack),left=pop_float(stack);bool value;
                if(op==Op::EqualFloat)value=left==right;else if(op==Op::NotEqualFloat)value=!(left==right);
                else if(op==Op::LessFloat)value=left<right;else if(op==Op::LessEqualFloat)value=(left<right)||(left==right);
                else if(op==Op::GreaterFloat)value=right<left;else value=(right<left)||(left==right);
                push_integer(stack,value);break;
            }
            case Op::NotInt:push_integer(stack,pop_integer(stack)==0);break;
            case Op::NotFloat:push_integer(stack,pop_float(stack)==number(0.0f));break;
            case Op::NegateInt:push_integer(stack,integer_bits(0u-static_cast<u32>(pop_integer(stack))));break;
            case Op::NegateFloatBits:{
                // The shipped instruction negates the 32-bit representation,
                // including after integer conversion; it does not flip its sign bit.
                const u32 value=0u-bits(pop_float(stack).to_float());
                stack.push(EclValueType::Float,&value,4);break;
            }
            case Op::PostDecrement:{
                const i32 value=integer_argument(0,services);
                destination(integer_reference(0,services))=wrapping_add(value,-1);
                push_integer(stack,value);break;
            }
            case Op::Sin:case Op::Cos:{
                const auto radians=pop_float(stack);push_float(stack,op==Op::Sin?sine(radians):cosine(radians));break;
            }
            case Op::Polar:{
                const float length=float_argument(3,services).to_float();
                const float radians=normalize_angle(float_argument(2,services).to_float()).to_float();
                const Vec2 value=polar(radians,length);
                destination(float_reference(0,services))=value.x;
                destination(float_reference(1,services))=value.y;break;
            }
            case Op::NormalizeAngle:{
                auto* out=float_reference(0,services);
                destination(out)=normalize_angle(float_argument(0,services).to_float()).to_float();break;
            }
            case Op::Wait:time=(number(time)-Extended::from_int(integer_argument(0,services))).to_float();break;
            case Op::LengthSquared:{
                const auto x=number(float_argument(1,services).to_float()),y=number(float_argument(2,services).to_float());
                destination(float_reference(0,services))=(y*y+x*x).to_float();break;
            }
            case Op::AngleTo:{
                const auto x=number(float_argument(2,services).to_float());
                const auto dx=number((x-float_argument(0,services)).to_float());
                const auto y=number(float_argument(3,services).to_float());
                const auto dy=y-float_argument(1,services);
                const float angle=angle_to(dy,dx).to_float();
                destination(float_reference(0,services))=angle;break;
            }
            default:if(services.command(*this)==-1)return 0;break;
            }
        }
        instruction=advance(instruction,instruction->length);
    }
#ifdef TH_ENABLE_THPRAC
    // F4 time lock (0x44fb9f): freeze the ECL sub-time float for the stages the
    // upstream EHOOK selects. The service owns the stage/enemy-flag condition.
    if(!services.hold_time(*this,elapsed))time=Scalar::add(elapsed,time);
#else
    time=Scalar::add(elapsed,time);
#endif
    return 0;
}
}
