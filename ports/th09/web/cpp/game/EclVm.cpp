#include "EclVm.hpp"
#include "Arithmetic.hpp"
#include "GameMath.hpp"
#include <algorithm>
#include <cmath>
namespace th09 {
namespace {
bool variable(const EclInstruction& i,u32 n){return n<16&&(i.variable_mask&(1u<<n));}
EclInstruction* next(EclInstruction& i){return reinterpret_cast<EclInstruction*>(reinterpret_cast<u8*>(&i)+i.size);}
bool fail(EclVm& vm,i32 opcode=-1){vm.invalid=true;vm.failed_opcode=opcode;return false;}
}
u32 EclVm::raw(const EclInstruction& i,u32 n) const noexcept {
    u32 value=0;if(i.size>=16&&n<u32(i.size-12)/4)std::memcpy(&value,reinterpret_cast<const u8*>(&i)+12+n*4,4);return value;
}
float EclVm::raw_float(const EclInstruction& i,u32 n) const noexcept {const u32 b=raw(i,n);float f;std::memcpy(&f,&b,4);return f;}
i32 EclVm::integer(const EclInstruction& i,u32 n) noexcept {const i32 v=signed_bits(raw(i,n));return variable(i,n)?values.read_int(v):v;}
double EclVm::wide(const EclInstruction& i,u32 n) noexcept {const float v=raw_float(i,n);return variable(i,n)?values.read_value(v):v;}
i32* EclVm::integer_target(EclInstruction& i,u32 n) noexcept {
    if(i.size<16||n>=u32(i.size-12)/4){fail(*this,i.opcode);return nullptr;}
    if(variable(i,n))if(auto p=values.integer_field(signed_bits(raw(i,n))))return p;
    return reinterpret_cast<i32*>(reinterpret_cast<u8*>(&i)+12+n*4);
}
float* EclVm::float_target(EclInstruction& i,u32 n) noexcept {
    if(i.size<16||n>=u32(i.size-12)/4){fail(*this,i.opcode);return nullptr;}
    if(variable(i,n))if(auto p=values.float_field(truncate(raw_float(i,n))))return p;
    return reinterpret_cast<float*>(reinterpret_cast<u8*>(&i)+12+n*4);
}
bool EclExecutor::initialize(EclVm& vm,EclContext& c,i32 subroutine,EclProgram* source){
    const i16 sub=i16(subroutine);if(sub<0)return true;
    if(!source)source=vm.program;auto entry=source?source->sub(sub):nullptr;if(!entry)return fail(vm);
    c.program=source;c.instruction=entry;c.subroutine=sub;c.timer.reset();c.wait.reset();return true;
}
bool EclExecutor::start(EclVm& vm,EclProgram& program,i32 subroutine){
    vm.program=&program;vm.active=nullptr;vm.active_slot=-1;vm.primary=EclContext{};
    for(auto& context:vm.asynchronous)context.reset();vm.finished=vm.invalid=false;vm.failed_opcode=-1;
    vm.scratch_depth=0;vm.bind_context();return initialize(vm,vm.primary,subroutine);
}
bool EclExecutor::jump(EclVm& vm,EclInstruction& i,i32 time,i32 relative,bool reset){
    const auto program=vm.context().program;if(!program)return fail(vm,i.opcode);
    const auto base=reinterpret_cast<std::uintptr_t>(program->data());
    const i64 offset=i64(reinterpret_cast<std::uintptr_t>(&i)-base)+relative;
    if(offset<0||offset>=program->size())return fail(vm,i.opcode);
    auto destination=reinterpret_cast<EclInstruction*>(base+offset);
    if(!program->has_instruction(destination))return fail(vm,i.opcode);
    vm.context().instruction=destination;
    if(reset)vm.context().timer.reset(time);else vm.context().timer.current=time;
    return true;
}
bool EclExecutor::call(EclVm& vm,EclInstruction& i,i32 sub){
    auto& c=vm.context();c.instruction=next(i);
    if(!(vm.behavior_flags&0x800000))c.stack[vm.scratch_depth]=static_cast<EclFrame&>(c);
    if(!initialize(vm,c,sub,c.program))return false;
    if(vm.values.field){std::copy_n(vm.values.field->integer_arguments,4,c.locals.integer_arguments);std::copy_n(vm.values.field->float_arguments,4,c.locals.float_arguments);}
    if(!(vm.behavior_flags&0x800000)&&vm.scratch_depth<15)++vm.scratch_depth;return true;
}
bool EclExecutor::interrupt(EclVm& vm,EclInstruction& i){
    if(vm.pending_interrupt<0||vm.pending_interrupt>=32)return fail(vm,i.opcode);
    vm.context().instruction=next(i);
    if(!(vm.behavior_flags&0x800000))vm.context().stack[vm.scratch_depth]=static_cast<EclFrame&>(vm.primary);
    if(!initialize(vm,vm.primary,vm.interrupt_subroutines[vm.pending_interrupt]))return false;
    if(vm.scratch_depth<15)++vm.scratch_depth;vm.pending_interrupt=-1;return true;
}
EclExecutor::Flow EclExecutor::dispatch(EclVm& vm,EclInstruction& i){
    const auto iv=[&](u32 n){return vm.integer(i,n);};const auto fv=[&](u32 n){return vm.real(i,n);};
    const auto puti=[&](u32 n,i32 v){if(auto p=vm.integer_target(i,n))*p=v;};
    const auto putf=[&](u32 n,float v){if(auto p=vm.float_target(i,n))*p=v;};
    const auto geti=[&](u32 n){const auto p=vm.integer_target(i,n);return p?*p:0;};
    const auto getf=[&](u32 n){const auto p=vm.float_target(i,n);return p?*p:0.f;};
    switch(i.opcode){
    case 0:case 3:case 83:case 84:case 85:case 90:case 91:case 92:case 122:case 123:case 141:case 142:
    case 158:case 164:case 168:case 174:case 176:case 179:case 180:case 181:case 184:break;
    case 1:vm.finished=true;return Flow::failed;
    case 2:vm.context().wait.reset(iv(0));break;
    case 4:return jump(vm,i,signed_bits(vm.raw(i,0)),signed_bits(vm.raw(i,1)),true)?Flow::branch:Flow::failed;
    case 5:puti(2,wrapping_sub(geti(2),1));if(iv(2)>0)return jump(vm,i,signed_bits(vm.raw(i,0)),signed_bits(vm.raw(i,1)),true)?Flow::branch:Flow::failed;break;
    case 6:puti(0,iv(1));break;
    case 7:putf(0,fv(1));break;
    case 8:{const i32 a=iv(1);if(!vm.values.world)return Flow::failed;const bool positive=vm.values.world->random.next16()&1;puti(0,positive?a:wrapping_sub(0,a));break;}
    case 9:{if(!vm.values.world)return Flow::failed;const float sign=(vm.values.world->random.next16()&1)?1.f:-1.f;const float a=fv(1);putf(0,a*sign);break;}
    case 10:{const i32 a=iv(1);puti(0,wrapping_add(geti(0),a));break;}
    case 11:{const i32 a=iv(1);puti(0,wrapping_sub(geti(0),a));break;}
    case 12:{const i32 a=iv(1);puti(0,signed_bits(u32(geti(0))*u32(a)));break;}
    case 13:case 14:{const i32 b=iv(1),a=geti(0);if(!b){fail(vm,i.opcode);return Flow::failed;}puti(0,i.opcode==13?signed_bits(u32(i64(a)/b)):i32(i64(a)%b));break;}
    case 15:{const float b=fv(1);putf(0,b+getf(0));break;}
    case 16:{const float b=fv(1);putf(0,getf(0)-b);break;}
    case 17:{const float b=fv(1);putf(0,b*getf(0));break;}
    case 18:{const float b=fv(1);putf(0,getf(0)/b);break;}
    case 19:{const float b=fv(1),a=fv(0);putf(0,script_remainder(a,b));break;}
    case 20:{const i32 a=iv(1),b=iv(2);puti(0,wrapping_add(a,b));break;}
    case 21:{const i32 a=iv(1),b=iv(2);puti(0,wrapping_sub(a,b));break;}
    case 22:{const i32 a=iv(1),b=iv(2);puti(0,signed_bits(u32(a)*u32(b)));break;}
    case 23:case 24:{const i32 a=iv(1),b=iv(2);if(!b){fail(vm,i.opcode);return Flow::failed;}puti(0,i.opcode==23?signed_bits(u32(i64(a)/b)):i32(i64(a)%b));break;}
    case 25:{const float a=fv(1),b=fv(2);putf(0,b+a);break;}
    case 26:{const float a=fv(1),b=fv(2);putf(0,a-b);break;}
    case 27:{const float a=fv(1),b=fv(2);putf(0,b*a);break;}
    case 28:{const float a=fv(1),b=fv(2);putf(0,a/b);break;}
    case 29:{const float b=fv(2),a=fv(1);putf(0,script_remainder(a,b));break;}
    case 30:puti(0,wrapping_add(geti(0),1));break;
    case 31:puti(0,wrapping_sub(geti(0),1));break;
    case 32:putf(0,float(std::sin(double(fv(1)))));break;
    case 33:putf(0,float(std::cos(double(fv(1)))));break;
    case 34:{const float x2=fv(3),x1=fv(1),y2=fv(4),y1=fv(2);putf(0,float(std::atan2(double(y2-y1),double(x2-x1))));break;}
    case 35:{const float a=fv(1);const double b=vm.wide(i,2);const float t=fv(3),base=fv(2);putf(0,t*float(double(a)-b)+base);break;}
    case 36:{
        if(i.size<44){fail(vm,i.opcode);return Flow::failed;}
        for(auto& s:vm.context().interpolations)if(!s.active||s.target==vm.raw_float(i,0)){
            s.time.reset();s.target=vm.raw_float(i,0);s.duration=iv(1);s.curve=iv(2);s.easing=iv(3);
            if(s.curve<0||s.curve>7){fail(vm,i.opcode);return Flow::failed;}s.active=1;
            for(u32 n=0;n<4;++n)s.values[n]=fv(n+4);break;
        }break;
    }
    case 37:putf(0,float(add_angle(fv(0),0)));break;
    case 38:{const float a=float(add_angle(fv(2),0)),r=fv(3);putf(0,float(std::cos(double(a))*double(r)));putf(1,float(std::sin(double(a))*double(r)));break;}
    case 39:{const float x1=fv(1);const float dx=float(double(x1)-vm.wide(i,3));const float y1=fv(2);const float dy=float(double(y1)-vm.wide(i,4));putf(0,float(std::sqrt(double(dx*dx+dy*dy))));break;}
    case 40:case 41:case 42:case 43:case 44:case 45:case 46:case 47:case 48:case 49:case 50:case 51:{
        double a,b;if(i.opcode&1){a=fv(0);b=vm.wide(i,1);}else {a=iv(0);b=iv(1);}
        const u32 kind=(i.opcode-40)/2;const bool take=kind==0?a==b:kind==1?a!=b:kind==2?a<b:kind==3?a<=b:kind==4?a>b:a>=b;
        if(take)return jump(vm,i,signed_bits(vm.raw(i,2)),signed_bits(vm.raw(i,3)),false)?Flow::branch:Flow::failed;break;
    }
    case 52:return call(vm,i,signed_bits(vm.raw(i,0)))?Flow::branch:Flow::failed;
    case 53:{
        if(--vm.scratch_depth<0){vm.context().returned=true;vm.scratch_depth=vm.primary.depth;return Flow::returned;}
        auto& c=vm.context();static_cast<EclFrame&>(c)=c.stack[vm.scratch_depth];vm.bind_context();return Flow::branch;
    }
    case 54:case 55:case 56:case 57:case 58:case 59:case 60:case 61:case 62:
        if(!animations||!vm.animation.command(vm,i,*animations)){fail(vm,i.opcode);return Flow::failed;}break;
    case 63:case 64:case 65:case 66:case 67:case 68:case 69:case 70:case 71:case 72:case 73:case 74:case 75:case 76:
    case 77:case 78:case 79:case 80:case 81:case 82:case 178:
        if(!vm.movement.command(vm,i)){fail(vm,i.opcode);return Flow::failed;}break;
    case 96:case 97:case 98:case 99:case 100:case 101:case 102:case 103:case 104:case 105:case 106:case 107:case 108:
    case 109:case 110:case 111:case 112:case 113:
        if(!emissions||!vm.emitter.command(vm,i,*emissions)){fail(vm,i.opcode);return Flow::failed;}break;
    case 114:case 115:case 116:case 117:case 118:case 119:case 120:case 121:case 154:case 167:case 170:case 171:case 172:
        if(!laser_emissions||!vm.lasers.command(vm,i,*laser_emissions)){fail(vm,i.opcode);return Flow::failed;}break;
    case 125:vm.pending_interrupt=i16(iv(0));return interrupt(vm,i)?Flow::branch:Flow::failed;
    case 126:{const i16 sub=i16(iv(0));const i32 index=iv(1);if(index<0||index>=32){fail(vm,i.opcode);return Flow::failed;}vm.interrupt_subroutines[index]=sub;break;}
    case 135:{
        const i32 slot=iv(0);if(slot<0||slot>=4){fail(vm,i.opcode);return Flow::failed;}
        ++vm.generations[slot];vm.asynchronous[slot].reset();
        if(iv(1)>=0){const i16 sub=i16(iv(1));auto c=std::make_unique<EclContext>();
            if(!initialize(vm,*c,sub))return Flow::failed;c->locals=vm.context().locals;vm.asynchronous[slot]=std::move(c);}
        break;
    }
    case 136:if(!native||!native->execute(vm,iv(0),i)){fail(vm,i.opcode);return Flow::failed;}break;
    case 137:if(iv(0)<0)vm.context().native_callback=-1;else {vm.context().native_callback=iv(0);vm.context().native_instruction=&i;}break;
    case 146:vm.context().timer.advance(float(iv(0)),timing.rate,timing.force_step?32:0);break;
    case 151:vm.behavior_flags=(vm.behavior_flags&~0x800000u)|((vm.raw(i,0)&1)<<23);break;
    case 157:if(!vm.trail.command(vm,i)){fail(vm,i.opcode);return Flow::failed;}break;
    case 128:case 129:case 130:case 131:case 132:case 133:case 134:case 138:case 143:case 144:case 145:
    case 149:case 150:case 152:case 153:case 155:case 156:case 159:case 160:case 161:case 162:case 165:
    case 173:case 177:case 182:case 183:case 185:case 187:
        if(!vm.status.command(vm,i)){fail(vm,i.opcode);return Flow::failed;}break;
    case 166:{
        const float angle=fv(2),length=fv(3);auto y=vm.float_target(i,1);if(y)*y=float(std::sin(double(angle))*double(length));
        const float second_angle=fv(2),second_length=fv(3);auto x=vm.float_target(i,0);if(x)*x=float(std::cos(double(second_angle))*double(second_length));break;
    }
    case 169:{
        if(!vm.values.field||!vm.values.world){fail(vm,i.opcode);return Flow::failed;}
        const float x=vm.values.position.x,player_x=vm.values.field->player.x;
        const bool left=(player_x<x&&96.f<x)||288.f<x;auto output=vm.float_target(i,0);
        const float random=float(vm.values.world->random.range(1.5707963705062866f));
        if(output)*output=left?float(add_angle(random+2.356194496154785f,0)):random-.7853981852531433f;break;
    }
    default:
        if(i.opcode>187||i.opcode<0)break;
        if(!commands||!commands->execute(vm,i)){fail(vm,i.opcode);return Flow::failed;}
    }
    return vm.invalid?Flow::failed:Flow::next;
}
bool EclExecutor::interpolate(EclVm& vm){
    auto& c=vm.context();const Vec3 before=vm.values.position;bool moved=false;
    if(c.native_callback>=0&&(!native||!c.native_instruction||!native->execute(vm,c.native_callback,*c.native_instruction)))return fail(vm,137);
    for(auto& s:c.interpolations){
        if(!s.active)continue;s.time.advance(frame_step,timing.rate,timing.force_step?32:0);
        if(s.time.current>=s.duration)s.time.reset(s.duration);
        float fraction=s.time.time/float(s.duration),power;
        if(s.easing>=1&&s.easing<=3){power=fraction*fraction;for(i32 n=1;n<s.easing;++n)power*=fraction;fraction=power;}
        else if(s.easing>=4&&s.easing<=6){const float inverse=1.f-fraction;power=inverse*inverse;for(i32 n=4;n<s.easing;++n)power*=inverse;fraction=1.f-power;}
        const float a=vm.values.read_float(s.values[0]),b=vm.values.read_float(s.values[1]);float result;
        if(s.curve!=7)result=(b-a)*fraction+a;
        else {const float ta=vm.values.read_float(s.values[2]),tb=vm.values.read_float(s.values[3]),t=fraction,m=t-1.f,r=1.f-t;
            const float ca=((t+t+1.f)*m)*m,cb=((3.f-(t+t))*t)*t,cc=(r*r)*t,cd=(m*t)*t;
            result=((ca*a+cb*b)+cc*ta)+cd*tb;}
        auto output=vm.values.float_field(truncate(s.target));if(output)*output=result;else s.target=result;
        if(s.time.current>=s.duration)s.active=0;
        if(s.target==10042.f||s.target==10043.f||s.target==10044.f)moved=true;
    }
    if(moved){vm.velocity.x=vm.values.position.x-before.x;vm.velocity.y=vm.values.position.y-before.y;
        vm.values.direction=float(std::atan2(double(vm.velocity.y),double(vm.velocity.x)));vm.values.position=before;}
    return true;
}
bool EclExecutor::step_context(EclVm& vm){
    if(vm.finished||vm.invalid||!vm.context().instruction)return false;
    vm.bind_context();vm.context().returned=false;vm.scratch_depth=vm.context().depth;u32 count=0;
    if(vm.pending_interrupt>=0&&!interrupt(vm,*vm.context().instruction))return false;
    // The original interpreter keeps its current instruction in a local cursor.
    // Calls commit a context immediately; normal progress commits at frame end.
    // A return that ends a context leaves the last committed cursor untouched.
    auto cursor=vm.context().instruction;
    for(;;){
        if(++count>100000)return fail(vm);
        const Vec3& p=vm.values.position;const Vec3& d=vm.position_offset;vm.values.resolved_position={p.x+d.x,p.y+d.y,p.z+d.z};
        auto& c=vm.context();if(!c.program||!c.program->has_instruction(cursor))return fail(vm);
        if(c.wait.current>0){c.wait.decrement(1,timing);c.timer.decrement(1,timing);break;}
        if(cursor->time!=c.timer.current)break;
        const u32 mask=difficulty_mask|vm.difficulty_flags;
        const auto committed=c.instruction;const i32 opcode=cursor->opcode;
        const Flow flow=(cursor->difficulties&mask)==mask?dispatch(vm,*cursor):Flow::next;
        if(flow==Flow::failed)return false;if(flow==Flow::returned)return true;
        if(flow==Flow::next)cursor=next(*cursor);
        else {cursor=vm.context().instruction;if(opcode==4||opcode==5||(opcode>=40&&opcode<=51))vm.context().instruction=committed;}
    }
    if(vm.values.life>0&&!interpolate(vm))return false;
    vm.context().depth=vm.scratch_depth;vm.context().instruction=cursor;vm.context().timer.advance(frame_step,timing.rate,timing.force_step?32:0);return true;
}
bool EclExecutor::step(EclVm& vm){
    vm.active=nullptr;vm.active_slot=-1;
    if(!step_context(vm))return false;
    for(u32 slot=0;slot<4;++slot)if(vm.asynchronous[slot]){
        const u32 generation=vm.generations[slot];auto c=std::move(vm.asynchronous[slot]);vm.active=c.get();vm.active_slot=i32(slot);
        const bool result=step_context(vm);vm.active=nullptr;vm.active_slot=-1;vm.bind_context();
        if(!c->returned&&generation==vm.generations[slot])vm.asynchronous[slot]=std::move(c);
        if(!result)return false;
    }
    vm.bind_context();return !vm.invalid&&!vm.finished;
}
}
