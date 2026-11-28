#include "game/BulletExtension16.hpp"
#include "game/BulletRules.hpp"
#include <cmath>
#include <cstdio>
#include <exception>

using namespace th128::bullet;

static void require(bool condition,const char* message) {
    if(!condition) throw std::runtime_error(message);
}
static ExtensionRaw mode1Angle3Record() {
    ExtensionRaw record;
    record.control=0;
    record.type=16;
    record.parameterA=40;
    record.parameterB=1;
    record.parameterC=1;
    record.parameterD=3;
    record.parameterR=0;
    record.parameterS=3.5f;
    record.parameterT=-999999.f;
    record.parameterU=-999999.f;
    return record;
}
int main() {
    try {
        const auto record=mode1Angle3Record();
        const auto initialized=initializeExtension16(record,2.5f,-3.002182f);
        require(initialized.has_value(),"C1/D3 type16 record must initialize");
        require(initialized->mode==1&&initialized->angleMode==3&&initialized->period==40&&
                initialized->limit==1&&initialized->targetSpeed==3.5f,"C1/D3 fields");
        {
            auto state=*initialized;
            const auto step=stepExtension16(state,2.5f,-3.002182f,{1,2,1.75f},{0,0,1.25f},{0,400,0},-1);
            require(step.has_value()&&step->speed==2.5f&&step->angle==-3.002182f&&
                    step->velocity.z==1.75f&&step->next.time==1,"C1/D3 first pre-boundary step");
        }
        {
            auto state=*initialized;
            state.time=39;state.fraction=39;
            const auto step=stepExtension16(state,2.5f,-3.002182f,{1,2,0},{0,0,0},{0,400,0},-1);
            require(step.has_value()&&step->speed==2.5f&&
                    std::abs(step->velocity.x-.0625f*std::cos(-3.002182f))<1e-6f&&
                    std::abs(step->velocity.y-.0625f*std::sin(-3.002182f))<1e-6f&&
                    step->angle==-3.002182f,"C1/D3 last pre-boundary speed");
        }
        const Position cached{0,0,0}, player{0,400,0};
        {
            auto state=*initialized;
            state.time=40;state.fraction=40;
            const auto sound=stepExtension16(state,2.5f,-3.002182f,{1,2,1.75f},{0,0,1.25f},player,40,&cached);
            require(sound.has_value()&&sound->soundRequired&&!sound->finished&&
                    sound->next.count==0&&sound->next.time==40&&sound->velocity.z==1.75f,
                    "C1/D3 sound must precede boundary mutation");
            const auto step=stepExtension16(state,2.5f,-3.002182f,{1,2,1.75f},{0,0,1.25f},player,-1,&cached);
            const auto aim=targetAnglePrefix(cached,player);
            require(aim.has_value()&&step.has_value(),"C1/D3 boundary vectors");
            require(std::abs(step->angle-*aim)<1e-6f&&step->speed==3.5f&&
                    step->next.count==1&&step->finished&&!step->next.active&&
                    std::abs(step->velocity.x-3.5f*std::cos(*aim))<1e-6f&&
                    std::abs(step->velocity.y-3.5f*std::sin(*aim))<1e-6f&&
                    step->velocity.z==1.75f,"C1/D3 snapshot aim boundary");
            require(!stepExtension16(state,2.5f,-3.002182f,{1,2,0},{0,0,0},player,-1).has_value(),
                    "D3 without cached 0x800000 position must reject");
        }
        {
            auto mode1=record;mode1.parameterD=0;mode1.parameterR=.25f;
            const auto initialized1=initializeExtension16(mode1,2.5f,-3.002182f);
            require(initialized1.has_value(),"C1/D0 record");
            auto state=*initialized1;state.time=40;state.fraction=40;
            const auto step=stepExtension16(state,2.5f,-3.002182f,{1,2,0},{0,0,0},player,-1);
            require(step.has_value()&&std::abs(step->angle-.25f)<1e-6f,"C1/D0 absolute R angle");
        }
        {
            auto unknown=record;unknown.parameterD=1;
            require(!initializeExtension16(unknown,2.5f,-3.002182f),"D1 must reject");
            unknown=record;unknown.parameterD=2;
            require(!initializeExtension16(unknown,2.5f,-3.002182f),"D2 must reject");
            auto c0d3=record;c0d3.parameterC=0;
            require(!initializeExtension16(c0d3,2.5f,-3.002182f),"C0/D3 must remain strict");
            auto callback=record;callback.callback=1;
            require(!initializeExtension16(callback,2.5f,-3.002182f),"callback must reject");
            auto mode3=record;mode3.parameterC=3;
            require(!initializeExtension16(mode3,2.5f,-3.002182f),"mode3 must reject");
        }
        {
            ExtensionRaw old;
            old.control=0;old.type=16;old.parameterA=60;old.parameterB=1;old.parameterC=0;old.parameterD=0;
            old.parameterR=2.094395160675049f;old.parameterS=.5f;
            const auto oldInit=initializeExtension16(old,3.f,.5f);
            require(oldInit.has_value(),"old C0/D0 case initialization");
            auto boundary=*oldInit;boundary.time=60;boundary.fraction=60;
            const auto oldStep=stepExtension16(boundary,3.f,.5f,{1,2,0},{0,0,.1f},{0,400,0},-1);
            require(oldStep.has_value()&&std::abs(oldStep->angle-(.5f+old.parameterR))<1e-6f&&
                    oldStep->speed==.5f&&oldStep->finished,"old mode0 case preserved");
            auto mode2=old;mode2.parameterC=2;mode2.parameterR=0;
            const auto mode2Init=initializeExtension16(mode2,3.f,.5f);
            require(mode2Init.has_value(),"old mode2 case initialization");
            auto m2=*mode2Init;m2.time=60;m2.fraction=60;
            const auto aim=targetAnglePrefix({0,0,.1f},{0,400,0});
            const auto m2Step=stepExtension16(m2,3.f,.5f,{1,2,0},{0,0,.1f},{0,400,0},-1);
            require(aim.has_value()&&m2Step.has_value()&&std::abs(m2Step->angle-*aim)<1e-6f,
                    "old mode2 boundary preserved");
        }
        std::puts("PASS extension16 C1/D3 snapshot-angle, sound order, Z preserve and old C0/2 cases");
    }catch(const std::exception& error){
        std::fprintf(stderr,"%s\n",error.what());
        return 1;
    }
}
