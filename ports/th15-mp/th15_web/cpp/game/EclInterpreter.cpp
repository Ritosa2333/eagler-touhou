#include "EclContext.hpp"
#include "AnmVisualState.hpp"
#include <cmath>
namespace th15 {
namespace {enum class Op:u16{
    Nop=0,Delete=1,Return=10,Call=11,Jump=12,JumpIfZero=13,JumpIfNonzero=14,Spawn=15,SpawnIdentified=16,TerminateThread=17,PauseThread=18,ResumeThread=19,SetThreadState=20,TerminateChildren=21,WaitInteger=23,WaitFloat=24,
    EnterFrame=40,LeaveFrame=41,PushInt=42,StoreInt=43,PushFloat=44,StoreFloat=45,
    AddInt=50,AddFloat=51,SubtractInt=52,SubtractFloat=53,MultiplyInt=54,MultiplyFloat=55,DivideInt=56,DivideFloat=57,Remainder=58,
    EqualInt=59,EqualFloat=60,NotEqualInt=61,NotEqualFloat=62,LessInt=63,LessFloat=64,LessEqualInt=65,LessEqualFloat=66,GreaterInt=67,GreaterFloat=68,GreaterEqualInt=69,GreaterEqualFloat=70,
    NotInt=71,NotFloat=72,LogicalOr=73,LogicalAnd=74,Xor=75,Or=76,And=77,PostDecrement=78,Sin=79,Cos=80,Polar=81,NormalizeAngle=82,NegateInt=83,NegateFloat=84,LengthSquared=85,Length=86,AngleTo=87,Sqrt=88,AngleDifference=89,RotateVector=90,Interpolate=91,InterpolateHermite=92,RandomPolar=93
};}
// Executes TH15 bytecode in game-side C++. Unsupported game-specific commands
// are reported rather than treated as NOPs. Thread/call and enemy commands are
// separate integration work and must be installed before gameplay is enabled.
int EclContext::step(float elapsed){
    if(!code||routine<0||instruction_offset<0)return -1;
    auto fail=[&](const char* message){error=message;return -2;};
    auto finish=[&](){const auto* saved=instruction;for(u32 slot=0;slot<interpolators.size();slot++)if(interpolators[slot].duration){instruction=interpolation_targets[slot];if(!instruction)return fail("ECL interpolation target unavailable");auto* destination=float_destination(1);if(!destination)return -2;*destination=interpolators[slot].step(current_interpolation_rate())[0];}instruction=saved;return 0;};
    for(u32 steps=0;steps<100000;steps++){
        if(code_size<16||instruction_offset<0||u32(instruction_offset)>code_size-16)return fail("ECL instruction outside routine");
        instruction=reinterpret_cast<const EclInstruction*>(code+instruction_offset);
        if(float(instruction->time)>time){time=float(time+elapsed);return finish();}
        u32 length=instruction->length;if(length<16||length%4||length>code_size-u32(instruction_offset))return fail("Invalid ECL instruction length");
        const Op op=Op(instruction->opcode);const bool enabled=(instruction->difficulty&difficulty)!=0;
        bool ok=true,adjust=false;
        auto integer_arg=[&](u32 n,bool consume=false){i32 value=0;if(16+n*4+4>length){error="Truncated ECL arguments";ok=false;}else if(!integer(n,instruction->argument<i32>(n),value,consume))ok=false;return value;};
        auto float_arg=[&](u32 n,bool consume=false){float value=0;if(16+n*4+4>length){error="Truncated ECL arguments";ok=false;}else if(!floating(n,instruction->argument<float>(n),value,consume))ok=false;return value;};
        auto pop_int=[&](){i32 value=0;if(!pop(value))ok=false;return value;};
        auto pop_float=[&](){float value=0;if(!pop(value))ok=false;return value;};
        auto push_int=[&](i32 value){if(!push(value))ok=false;};auto push_float=[&](float value){if(!push(value))ok=false;};
        auto store_int=[&](u32 n,i32 value){if(16+n*4+4>length){error="Truncated ECL destination";ok=false;return;}auto* destination=integer_destination(n);if(!destination)ok=false;else *destination=value;};
        auto store_float=[&](u32 n,float value){if(16+n*4+4>length){error="Truncated ECL destination";ok=false;return;}auto* destination=float_destination(n);if(!destination)ok=false;else *destination=value;};
        if(enabled)switch(op){
            case Op::Nop:case Op(22):case Op(30):case Op(31):adjust=true;break;
            case Op::Delete:routine=instruction_offset=-1;instruction=nullptr;return -1;
            case Op::Return:{const int result=return_from_call();if(result)return result;length=instruction->length;adjust=true;break;}
            case Op::Call:const_cast<EclInstruction*>(instruction)->stack_adjustment=0;if(!call_into(*this))return -2;continue;
            case Op::Spawn:case Op::SpawnIdentified:{if(!threads)return fail("ECL thread host not bound");i32 id=-1;if(op==Op::SpawnIdentified){const u32 offset=instruction->argument<u32>(0)+20;if(offset+4>length)return fail("Truncated ECL thread identifier");i32 literal;std::memcpy(&literal,reinterpret_cast<const u8*>(instruction)+offset,4);if(!integer(1,literal,id,true))return -2;}if(!threads->spawn(*this,id,op==Op::SpawnIdentified?1:0))return -2;break;}
            case Op::TerminateThread:case Op::PauseThread:case Op::ResumeThread:case Op::SetThreadState:{if(!threads)return fail("ECL thread host not bound");const i32 id=integer_arg(0);if(auto* target=threads->find(id)){if(op==Op::TerminateThread)target->instruction_offset=-1;else if(op==Op::PauseThread)target->thread_flags|=1;else if(op==Op::ResumeThread)target->thread_flags&=~1u;else target->thread_state=integer_arg(1);}adjust=true;break;}
            case Op::TerminateChildren:if(!threads)return fail("ECL thread host not bound");threads->terminate_children();break;
            case Op::Jump:case Op::JumpIfZero:case Op::JumpIfNonzero:{const bool take=op==Op::Jump?true:(pop_int()==0)==(op==Op::JumpIfZero);if(take){if(length<24)return fail("Truncated ECL jump");const i32 relative=instruction->argument<i32>(0),target_time=instruction->argument<i32>(1);if(!ok)return -2;instruction_offset=wrapping_add(instruction_offset,relative);time=float(target_time);continue;}adjust=true;break;}
            case Op::WaitInteger:time=float(time-float(integer_arg(0)));adjust=true;break;
            case Op::WaitFloat:time=float(time-float_arg(0));adjust=true;break;
            case Op::EnterFrame:if(!enter_frame(integer_arg(0)))ok=false;adjust=true;break;
            case Op::LeaveFrame:if(!leave_frame())ok=false;adjust=true;break;
            case Op::PushInt:push_int(integer_arg(0,true));break;
            case Op::PushFloat:push_float(float_arg(0,true));break;
            case Op::StoreInt:{auto* destination=integer_destination(0);if(!destination)ok=false;const i32 value=pop_int();if(destination)*destination=value;break;}
            case Op::StoreFloat:{auto* destination=float_destination(0);if(!destination)ok=false;const float value=pop_float();if(destination)*destination=value;break;}
            case Op::AddInt:case Op::SubtractInt:case Op::MultiplyInt:case Op::DivideInt:case Op::Remainder:
            case Op::EqualInt:case Op::NotEqualInt:case Op::LessInt:case Op::LessEqualInt:case Op::GreaterInt:case Op::GreaterEqualInt:
            case Op::LogicalOr:case Op::LogicalAnd:case Op::Xor:case Op::Or:case Op::And:{
                const i32 right=pop_int(),left=pop_int();i32 value=0;
                switch(op){case Op::AddInt:value=wrapping_add(left,right);break;case Op::SubtractInt:value=wrapping_sub(left,right);break;case Op::MultiplyInt:value=signed_bits(u32(left)*u32(right));break;
                case Op::DivideInt:case Op::Remainder:if(!right||(left==INT32_MIN&&right==-1))return fail("ECL integer divide error");value=op==Op::DivideInt?left/right:left%right;break;
                case Op::EqualInt:value=left==right;break;case Op::NotEqualInt:value=left!=right;break;case Op::LessInt:value=left<right;break;case Op::LessEqualInt:value=left<=right;break;case Op::GreaterInt:value=left>right;break;case Op::GreaterEqualInt:value=left>=right;break;
                case Op::LogicalOr:value=left||right;break;case Op::LogicalAnd:value=left&&right;break;case Op::Xor:value=left^right;break;case Op::Or:value=left|right;break;default:value=left&right;break;}
                push_int(value);if(u16(op)<=58)const_cast<EclInstruction*>(instruction)->stack_adjustment=0;break;
            }
            case Op::AddFloat:case Op::SubtractFloat:case Op::MultiplyFloat:case Op::DivideFloat:{const float right=pop_float(),left=pop_float();push_float(op==Op::AddFloat?float(left+right):op==Op::SubtractFloat?float(left-right):op==Op::MultiplyFloat?float(left*right):float(left/right));const_cast<EclInstruction*>(instruction)->stack_adjustment=0;break;}
            case Op::EqualFloat:case Op::NotEqualFloat:case Op::LessFloat:case Op::LessEqualFloat:case Op::GreaterFloat:case Op::GreaterEqualFloat:{const float right=pop_float(),left=pop_float();push_int(op==Op::EqualFloat?left==right:op==Op::NotEqualFloat?left!=right:op==Op::LessFloat?left<right:op==Op::LessEqualFloat?left<=right:op==Op::GreaterFloat?left>right:left>=right);break;}
            case Op::NotInt:push_int(pop_int()==0);break;case Op::NotFloat:push_int(pop_float()==0);break;
            case Op::NegateInt:push_int(signed_bits(0u-u32(pop_int())));break;
            case Op::NegateFloat:push_float(float(-pop_float()));break;
            case Op::PostDecrement:{const i32 value=integer_arg(0);store_int(0,wrapping_add(value,-1));push_int(value);break;}
            case Op::Sin:case Op::Cos:case Op::Sqrt:{const double value=double(pop_float());push_float(float(op==Op::Sin?std::sin(value):op==Op::Cos?std::cos(value):std::sqrt(value)));break;}
            case Op::Polar:{const float angle=normalize_angle(float_arg(2)),radius=float_arg(3);const float x=float(std::cos(double(angle))*double(radius)),y=float(std::sin(double(angle))*double(radius));store_float(0,x);store_float(1,y);adjust=true;break;}
            case Op::NormalizeAngle:store_float(0,normalize_angle(float_arg(0)));adjust=true;break;
            case Op::LengthSquared:case Op::Length:{const float x=float_arg(1),y=float_arg(2),squared=float(float(y*y)+float(x*x));store_float(0,op==Op::Length?float(std::sqrt(double(squared))):squared);adjust=true;break;}
            case Op::AngleTo:{const float ax=float_arg(1),ay=float_arg(2),bx=float_arg(3),by=float_arg(4);store_float(0,float(std::atan2(double(float(by-ay)),double(float(bx-ax)))));adjust=true;break;}
            case Op::AngleDifference:{const float from=float_arg(1),to=float_arg(2);float difference=float(to-from);if(difference>3.1415927410125732421875f)difference=float(to-float(from+6.283185482025146484375f));else if(float(from-to)>3.1415927410125732421875f)difference=float(to-float(from-6.283185482025146484375f));store_float(0,difference);adjust=true;break;}
            case Op::RotateVector:{const float x=float_arg(2),y=float_arg(3),angle=normalize_angle(float_arg(4));const float sine=float(std::sin(double(angle))),cosine=float(std::cos(double(angle)));const float rx=float(float(cosine*x)-float(sine*y)),ry=float(float(cosine*y)+float(sine*x));store_float(0,rx);store_float(1,ry);adjust=true;break;}
            case Op::Interpolate:case Op::InterpolateHermite:{const i32 slot=integer_arg(0);if(slot<0||u32(slot)>=interpolators.size())return fail("ECL interpolation slot outside context");auto& interpolation=interpolators[slot];interpolation_targets[slot]=instruction;interpolation.step(current_interpolation_rate());const i32 frames=integer_arg(2),mode=integer_arg(3);const float from=float_arg(4),to=float_arg(5);interpolation.begin(frames,mode,{from},{to});store_float(1,from);interpolation.control1={op==Op::InterpolateHermite?float_arg(6):0};interpolation.control2={op==Op::InterpolateHermite?float_arg(7):0};adjust=true;break;}
            case Op::RandomPolar:{if(!visual_rng)return fail("ECL visual random generator not bound");const float minimum=float_arg(2),maximum=float_arg(3),angle=float(visual_rng->signed_unit()*3.1415927410125732421875f),radius=float(float(visual_rng->signed_unit()*float(maximum-minimum))+minimum);store_float(0,float(std::cos(double(angle))*double(radius)));store_float(1,float(std::sin(double(angle))*double(radius)));adjust=true;break;}
            default:if(commands){const int result=commands->execute(*this,u16(op));if(result==-2)return result;if(result==-1)return finish();if(result==1)continue;adjust=true;}else{error="Unimplemented ECL opcode "+std::to_string(u16(op));return -2;}break;
        }
        if(!ok)return -2;
        if(enabled&&adjust&&instruction->stack_adjustment){if(stack_pointer<instruction->stack_adjustment)return fail("ECL instruction stack cleanup underflow");stack_pointer-=instruction->stack_adjustment;}
        instruction_offset=wrapping_add(instruction_offset,i32(length));
    }
    return fail("ECL instruction budget exceeded");
}
}
