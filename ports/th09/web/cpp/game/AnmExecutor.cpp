#include "AnmExecutor.hpp"
#include "GameMath.hpp"
#include "Arithmetic.hpp"
#include <cmath>
namespace th09 {
namespace {
float add(float a,float b){return Scalar::add(a,b);}
float sub(float a,float b){return Scalar::sub(a,b);}
float mul(float a,float b){return Scalar::mul(a,b);}
float mix(float from,float to,float t){return float((number(t)*(number(to)-number(from))+number(from)));}
u8 mix_byte(u8 from,u8 to,float t){return u8(truncate((number(t)*(float(to)-float(from))+float(from))));}
void wrap_uv(float& value){if(value>=1)value=sub(value,1);else if(value<0)value=add(value,1);}
i32* integer(AnmVm& vm,i32 id){switch(id){case 10000:return &vm.intVar0;case 10001:return &vm.intVar1;case 10002:return &vm.intVar2;case 10003:return &vm.intVar3;case 10008:return &vm.counterVar0;case 10009:return &vm.counterVar1;default:return nullptr;}}
float* floating(AnmVm& vm,i32 id){switch(id){case 10004:return &vm.floatVar0;case 10005:return &vm.floatVar1;case 10006:return &vm.floatVar2;case 10007:return &vm.floatVar3;default:return nullptr;}}
struct Arguments {
    AnmVm& vm;AnmRawInstr* instruction;Rng& rng;
    i32 i(u32 n) const {i32 v;std::memcpy(&v,reinterpret_cast<const u8*>(instruction+1)+n*4,4);return v;}
    float f(u32 n) const {float v;std::memcpy(&v,reinterpret_cast<const u8*>(instruction+1)+n*4,4);return v;}
    i32 iv(u32 n) const {return instruction->varMask&(1u<<n)?vm.GetIntVar(i(n)):i(n);}
    float fv(u32 n) const {return instruction->varMask&(1u<<n)?float(vm.GetFloatVar(f(n),rng)):f(n);}
    i32* ip(u32 n) const {return vm.GetIntVarPtr(reinterpret_cast<i32*>(instruction+1)+n,instruction->varMask,n);}
    float* fp(u32 n) const {return vm.GetFloatVarPtr(reinterpret_cast<float*>(instruction+1)+n,instruction->varMask,n);}
    Vec3 vec(u32 n) const {return {fv(n),fv(n+1),fv(n+2)};}
};
}
double AnmVm::GetFloatVar(float id,Rng& random){const i32 n=Scalar::truncate(id);if(auto* p=integer(*this,n))return double(*p);if(auto* p=floating(*this,n))return number(*p);if(n==10010)return random.signed_unit()*double(0x1.921fb6p1f);if(n==10011)return random.unit();if(n==10012)return random.signed_unit();return number(id);}
i32 AnmVm::GetIntVar(i32 id){if(auto* p=integer(*this,id))return *p;if(auto* p=floating(*this,id))return Scalar::truncate(*p);return id;}
float* AnmVm::GetFloatVarPtr(float* p,u16 mask,u32 argument){if(mask&(1u<<argument))if(auto* variable=floating(*this,Scalar::truncate(*p)))return variable;return p;}
i32* AnmVm::GetIntVarPtr(i32* p,u16 mask,u32 argument){if(mask&(1u<<argument))if(auto* variable=integer(*this,*p))return variable;return p;}
i32 AnmLoaded::SetSprite(AnmVm* vm,i32 index) noexcept {
    if(!rawData||numberEntriesToBeLoaded||index<0||(spriteCount&&u32(index)>=spriteCount))return -1;
    vm->anmFile=this;vm->activeSpriteIndex=i16(index);vm->loadedSprite=&sprites[index];const auto& s=sprites[index];
    vm->spriteSize={s.widthPx,s.heightPx};vm->matrix1.identity();vm->matrix3.identity();
    vm->matrix1.m[0][0]=Scalar::div(s.widthPx,256);vm->matrix1.m[1][1]=Scalar::div(s.heightPx,256);
    vm->matrix3.m[0][0]=float((number(s.scaleFactor.x)/number(s.width)*number(s.widthPx)));
    vm->matrix3.m[1][1]=float((number(s.scaleFactor.y)/number(s.height)*number(s.heightPx)));
    vm->matrix2=vm->matrix1;return 0;
}
void AnmExecutor::start(AnmLoaded& file,AnmVm& vm,AnmRawInstr* script){
    if(!script||file.numberEntriesToBeLoaded){std::memset(&vm,0,sizeof(vm));return;}
    vm.Initialize();vm.anmFileIndex=i16(file.anmIdx);vm.anmFile=&file;vm.flip=0;vm.beginningOfScript=script;vm.currentInstruction=script;vm.currentTimeInScript.set(0);vm.visible=false;execute(vm);
}
bool AnmExecutor::execute(AnmVm& vm){
    if(!vm.currentInstruction)return true;if(vm.flag19)return false;
    u32 budget=0;
    auto jump=[&](i32 offset,i32 time){vm.currentTimeInScript.set(time);vm.currentInstruction=reinterpret_cast<AnmRawInstr*>(reinterpret_cast<u8*>(vm.beginningOfScript)+offset);};
    auto interp=[&](u32 index,i32 duration,u8 mode){vm.interpCurrentTimers[index].set(0);vm.interpEndTimers[index].set(duration);vm.interpModes[index]=mode;};
    for(;;){
        if(++budget>100000){invalid=true;return true;}
        if(vm.pendingInterrupt){
            AnmRawInstr* label=nullptr;auto* cursor=vm.beginningOfScript;
            while(cursor&&cursor->opcode!=-1){
                if(++budget>100000||cursor->instructionSize<8){invalid=true;return true;}
                if(cursor->opcode==21){i32 value;std::memcpy(&value,cursor+1,4);if(value==vm.pendingInterrupt){label=cursor;break;}if(value==-1)label=cursor;}
                cursor=reinterpret_cast<AnmRawInstr*>(reinterpret_cast<u8*>(cursor)+cursor->instructionSize);
            }
            vm.pendingInterrupt=0;vm.stopped=false;
            if(!label){vm.currentTimeInScript.decrement(1,timing);break;}
            vm.interruptReturnTime=vm.currentTimeInScript;vm.interruptReturnInstruction=vm.currentInstruction;
            vm.currentInstruction=reinterpret_cast<AnmRawInstr*>(reinterpret_cast<u8*>(label)+label->instructionSize);
            vm.currentTimeInScript.set(vm.currentInstruction->time);vm.visible=true;
        }
        auto* instruction=vm.currentInstruction;if(instruction->time>vm.currentTimeInScript.current)break;
        const Arguments a{vm,instruction,rng};const i32 op=instruction->opcode;
        if(op==-1||op==1||op==2){if(op!=2)vm.visible=false;vm.currentInstruction=nullptr;return true;}
        if(instruction->instructionSize<8){invalid=true;return true;}
        bool finish=false;
        switch(op){
        case 3:vm.visible=true;if(vm.anmFile)vm.anmFile->SetSprite(&vm,a.iv(0));vm.timeOfLastSpriteSet=vm.currentTimeInScript.current;break;
        case 4:jump(a.i(0),a.i(1));continue;
        case 5:*a.ip(0)=wrapping_sub(*a.ip(0),1);if(a.iv(0)>0){jump(a.i(1),a.i(2));continue;}break;
        case 6:if(vm.usePosOffset)vm.pos2=a.vec(0);else vm.pos=a.vec(0);break;
        case 7:vm.scale={a.fv(0),a.fv(1)};vm.updateScale=true;break;
        case 8:vm.color1.a=u8(a.iv(0));break;
        case 9:vm.color1.r=u8(a.iv(0));vm.color1.g=u8(a.iv(1));vm.color1.b=u8(a.iv(2));break;
        case 10:vm.flip^=1;vm.scale.x=-vm.scale.x;vm.updateScale=true;break;
        case 11:vm.flip^=2;vm.scale.y=-vm.scale.y;vm.updateScale=true;break;
        case 12:vm.rotation=a.vec(0);vm.updateRotation=true;break;
        case 13:vm.angleVel=a.vec(0);vm.updateRotation=true;break;
        case 14:vm.scaleGrowth={a.fv(0),a.fv(1)};break;
        case 15:vm.color1Initial.a=vm.color1.a;vm.color1Final.a=u8(a.i(0));interp(2,a.iv(1),0);break;
        case 16:vm.blendMode=a.i(0)!=0;break;
        case 17:case 18:case 19:interp(0,a.iv(3),op==17?0:op==18?4:6);vm.posInitial=vm.usePosOffset?vm.pos2:vm.pos;vm.posFinal=a.vec(0);break;
        case 20:case 23:if(op==23)vm.visible=false;vm.stopped=true;vm.currentTimeInScript.decrement(1,timing);finish=true;break;
        case 22:vm.anchor=3;break;
        case 24:vm.usePosOffset=a.i(0);break;
        case 25:vm.type=i16(a.i(0));break;
        case 26:vm.uvScrollPos.x=add(vm.uvScrollPos.x,a.fv(0));wrap_uv(vm.uvScrollPos.x);break;
        case 27:vm.uvScrollPos.y=add(vm.uvScrollPos.y,a.fv(0));wrap_uv(vm.uvScrollPos.y);break;
        case 28:vm.visible=a.i(0);break;
        case 29:interp(4,a.iv(2),0);vm.scaleInitial=vm.scale;vm.scaleFinal={a.fv(0),a.fv(1)};break;
        case 30:vm.zWriteDisabled=a.i(0);break;
        case 31:vm.flag15=a.i(0);break;
        case 32:interp(0,a.iv(0),u8(a.i(1)));vm.posInitial=vm.usePosOffset?vm.pos2:vm.pos;vm.posFinal=a.vec(2);break;
        case 33:case 86:{const bool second=op==86;interp(second?5:1,a.iv(0),u8(a.i(1)));auto& current=second?vm.color2:vm.color1;auto& initial=second?vm.color2Initial:vm.color1Initial;auto& final=second?vm.color2Final:vm.color1Final;initial.r=current.r;initial.g=current.g;initial.b=current.b;final.r=u8(a.iv(2));final.g=u8(a.iv(3));final.b=u8(a.iv(4));break;}
        case 34:case 87:{const bool second=op==87;interp(second?6:2,a.iv(0),u8(a.i(1)));(second?vm.color2Initial:vm.color1Initial).a=(second?vm.color2:vm.color1).a;(second?vm.color2Final:vm.color1Final).a=u8(a.iv(2));break;}
        case 35:interp(3,a.iv(0),u8(a.i(1)));vm.rotateInitial=vm.rotation;vm.rotateFinal=a.vec(2);vm.updateRotation=true;break;
        case 36:interp(4,a.iv(0),u8(a.i(1)));vm.scaleInitial=vm.scale;vm.scaleFinal={a.fv(2),a.fv(3)};vm.updateScale=true;break;
        case 37:*a.ip(0)=a.iv(1);break;
        case 38:*a.fp(0)=a.fv(1);break;
        case 39:*a.ip(0)=wrapping_add(*a.ip(0),a.iv(1));break;
        case 40:*a.fp(0)=add(*a.fp(0),a.fv(1));break;
        case 41:*a.ip(0)=wrapping_sub(*a.ip(0),a.iv(1));break;
        case 42:*a.fp(0)=sub(*a.fp(0),a.fv(1));break;
        case 43:*a.ip(0)=signed_bits(u32(*a.ip(0))*u32(a.iv(1)));break;
        case 44:*a.fp(0)=mul(*a.fp(0),a.fv(1));break;
        case 45:case 47:{const i32 divisor=a.iv(1),value=*a.ip(0);if(!divisor||(value==(-2147483647-1)&&divisor==-1)){invalid=true;return true;}*a.ip(0)=op==45?value/divisor:value%divisor;break;}
        case 46:*a.fp(0)=float((number(*a.fp(0))/number(a.fv(1))));break;
        case 48:*a.fp(0)=script_remainder(a.fv(0),a.fv(1));break;
        case 49:*a.ip(0)=wrapping_add(a.iv(1),a.iv(2));break;
        case 50:*a.fp(0)=add(a.fv(1),a.fv(2));break;
        case 51:*a.ip(0)=wrapping_sub(a.iv(1),a.iv(2));break;
        case 52:*a.fp(0)=sub(a.fv(1),a.fv(2));break;
        case 53:*a.ip(0)=signed_bits(u32(a.iv(1))*u32(a.iv(2)));break;
        case 54:*a.fp(0)=mul(a.fv(1),a.fv(2));break;
        case 55:case 57:{const i32 divisor=a.iv(2),value=a.iv(1);if(!divisor||(value==(-2147483647-1)&&divisor==-1)){invalid=true;return true;}*a.ip(0)=op==55?value/divisor:value%divisor;break;}
        case 56:*a.fp(0)=float((number(a.fv(1))/number(a.fv(2))));break;
        case 58:*a.fp(0)=script_remainder(a.fv(1),a.fv(2));break;
        case 59:*a.ip(0)=signed_bits(rng.bounded32(u32(a.iv(1))));break;
        case 60:*a.fp(0)=float(rng.range(a.fv(1)));break;
        case 61:*a.fp(0)=float(std::sin(double(a.fv(1))));break;
        case 62:*a.fp(0)=float(std::cos(double(a.fv(1))));break;
        case 63:*a.fp(0)=float(std::tan(double(a.fv(1))));break;
        case 64:*a.fp(0)=float(std::acos(double(a.fv(1))));break;
        case 65:*a.fp(0)=float(std::atan(double(a.fv(1))));break;
        case 66:*a.fp(0)=add_angle(a.fv(0),0);break;
        case 67:case 68:case 69:case 70:case 71:case 72:case 73:case 74:case 75:case 76:case 77:case 78:{
            bool condition;const i32 compare=(op-67)/2;
            if(op&1){const i32 x=a.iv(0),y=a.iv(1);condition=compare==0?x==y:compare==1?x!=y:compare==2?x<y:compare==3?x<=y:compare==4?x>y:x>=y;}
            else {const float x=a.fv(0),y=a.fv(1);condition=compare==0?x==y:compare==1?x!=y:compare==2?x<y:compare==3?x<=y:compare==4?x>y:x>=y;}
            if(condition){jump(a.i(2),a.i(3));continue;}break;
        }
        case 79:if(vm.waitTimer.current==0)vm.waitTimer.set(a.iv(0));else vm.waitTimer.decrement(1,timing);if(vm.waitTimer.current<=0)vm.waitTimer.set(0);else{vm.currentTimeInScript.decrement(1,timing);finish=true;}break;
        case 80:vm.uvScrollVel.x=a.fv(0);break;
        case 81:vm.uvScrollVel.y=a.fv(0);break;
        case 82:vm.blendMode=a.i(0);break;
        case 83:vm.playerBulletHitAnimationType=a.i(0);break;
        case 84:vm.color2.r=u8(a.iv(0));vm.color2.g=u8(a.iv(1));vm.color2.b=u8(a.iv(2));break;
        case 85:vm.color2.a=u8(a.iv(0));break;
        case 88:vm.flag17=reinterpret_cast<u8*>(instruction+1)[1];break;
        case 89:vm.currentTimeInScript=vm.interruptReturnTime;vm.currentInstruction=vm.interruptReturnInstruction;continue;
        default:break;
        }
        if(finish)break;
        vm.currentInstruction=reinterpret_cast<AnmRawInstr*>(reinterpret_cast<u8*>(instruction)+instruction->instructionSize);
    }
    advance(vm);return false;
}
void AnmExecutor::advance(AnmVm& vm){
    for(u32 axis=0;axis<3;++axis){float* velocity=axis==0?&vm.angleVel.x:axis==1?&vm.angleVel.y:&vm.angleVel.z;float* angle=axis==0?&vm.rotation.x:axis==1?&vm.rotation.y:&vm.rotation.z;if(*velocity!=0){*angle=add_angle(*angle,mul(timing.rate,*velocity));vm.updateRotation=true;}}
    for(u32 i=0;i<7;++i)if(vm.interpEndTimers[i].current>0){
        vm.interpCurrentTimers[i].tick(timing);float weight;
        if(vm.interpCurrentTimers[i].current>=vm.interpEndTimers[i].current){weight=1;vm.interpEndTimers[i].set(0);}
        else weight=float((number(float(vm.interpCurrentTimers[i].value()))/vm.interpEndTimers[i].value()));
        switch(vm.interpModes[i]){
        case 1:weight=mul(weight,weight);break;
        case 2:weight=float((number(weight)*number(weight)*number(weight)));break;
        // Every arithmetic operation uses the original 24-bit game precision.
        case 3:{const float square=weight*weight;weight=float(square*square);break;}
        case 4:{const float inverse=1.0f-weight;weight=float(1.0f-inverse*inverse);break;}
        case 5:{const float inverse=1.0f-weight;weight=float(1.0f-inverse*inverse*inverse);break;}
        case 6:{const float inverse=1.0f-weight,square=inverse*inverse;weight=float(1.0f-square*square);break;}
        default:break;
        }
        switch(i){
        case 0:{auto& p=vm.usePosOffset?vm.pos2:vm.pos;p={mix(vm.posInitial.x,vm.posFinal.x,weight),mix(vm.posInitial.y,vm.posFinal.y,weight),mix(vm.posInitial.z,vm.posFinal.z,weight)};break;}
        case 1:case 5:{auto& c=i==1?vm.color1:vm.color2;const auto& a=i==1?vm.color1Initial:vm.color2Initial;const auto& b=i==1?vm.color1Final:vm.color2Final;c.r=mix_byte(a.r,b.r,weight);c.g=mix_byte(a.g,b.g,weight);c.b=mix_byte(a.b,b.b,weight);break;}
        case 2:case 6:(i==2?vm.color1:vm.color2).a=mix_byte((i==2?vm.color1Initial:vm.color2Initial).a,(i==2?vm.color1Final:vm.color2Final).a,weight);break;
        case 3:vm.rotation={float(add_angle(float((number(vm.rotateFinal.x)-number(vm.rotateInitial.x))*number(weight)),vm.rotateInitial.x)),float(add_angle(float((number(vm.rotateFinal.y)-number(vm.rotateInitial.y))*number(weight)),vm.rotateInitial.y)),float(add_angle(float((number(vm.rotateFinal.z)-number(vm.rotateInitial.z))*number(weight)),vm.rotateInitial.z))};vm.updateRotation=true;break;
        case 4:vm.scale={mix(vm.scaleInitial.x,vm.scaleFinal.x,weight),mix(vm.scaleInitial.y,vm.scaleFinal.y,weight)};vm.updateScale=true;break;
        }
    }
    if(vm.scaleGrowth.y!=0){vm.scale.y=float((number(vm.scale.y)+number(timing.rate)*number(vm.scaleGrowth.y)));vm.updateScale=true;}
    if(vm.scaleGrowth.x!=0){vm.scale.x=float((number(vm.scale.x)+number(timing.rate)*number(vm.scaleGrowth.x)));vm.updateScale=true;vm.updateRotation=true;}
    vm.uvScrollPos.x=add(vm.uvScrollPos.x,vm.uvScrollVel.x);wrap_uv(vm.uvScrollPos.x);
    vm.uvScrollPos.y=add(vm.uvScrollPos.y,vm.uvScrollVel.y);wrap_uv(vm.uvScrollPos.y);
    vm.currentTimeInScript.tick(timing);++executed;
}
}
