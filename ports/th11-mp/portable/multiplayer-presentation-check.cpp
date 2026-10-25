#include "AnmPresentation.hpp"
#include "../web/cpp/multiplayer/PresentationSchedule.hpp"
#include <cassert>
#include <cstdio>
int main(){
    using namespace th11;
    AnmVm vm{};vm.id=1;vm.flags=3;
    presentation::begin_tick();presentation::capture(vm);
    vm.position.x=16;
    auto output=vm;
    high_refresh::begin(.5f,true,true);
    assert(presentation::present(output,vm));
    assert(output.position.x==8&&vm.position.x==16);
    // Independent managers can use the same id and resource/script.
    float combat_rate=1,menu_rate=1;
    vm.timer.rate=&combat_rate;vm.position.x=10;
    AnmVm menu=vm;menu.timer.rate=&menu_rate;menu.position.x=100;
    presentation::begin_tick();presentation::capture(vm);presentation::capture(menu);
    vm.position.x=20;menu.position.x=120;
    output=vm;assert(presentation::present(output,vm)&&output.position.x==15);
    output=menu;assert(presentation::present(output,menu)&&output.position.x==110);
    // A correction invalidates both owner pairs, even when VM ids rewind.
    presentation::reset();output=vm;assert(!presentation::present(output,vm));
    high_refresh::end();presentation::reset();
    using multiplayer::PresentationSchedule;
    for(unsigned hz:{60u,120u,144u}){
        PresentationSchedule clock;unsigned frame=0,draws=0;
        for(unsigned n=0;n<hz*10;++n){
            frame=unsigned((std::uint64_t(n)*60)/hz);
            const double phase=double((std::uint64_t(n)*60)%hz)/hz/60.;
            if(clock.observe(1./hz,true,1,0,1,frame,phase)){
                ++draws;assert(clock.alpha()>=0&&clock.alpha()<=1);
            }
        }
        assert(frame==599);assert(draws>=hz*9);
    }
    PresentationSchedule clock;
    assert(!clock.observe(0,true,1,0,1,1,0));
    assert(clock.observe(1./120,true,1,0,1,2,0));assert(clock.alpha()==0);
    assert(clock.observe(1./120,true,1,0,1,2,0));assert(clock.alpha()==.5f);
    // Missing inputs hold at the endpoint. Retained network phase may stay 0.
    for(unsigned n=0;n<100;++n)assert(clock.observe(1./120,true,1,0,1,2,0));
    assert(clock.alpha()==1);
    assert(clock.observe(0,true,1,0,1,3,0)&&clock.alpha()==0);
    assert(!clock.observe(0,true,1,1,1,3,0)); // rollback revision
    assert(!clock.observe(0,true,2,1,1,0,0)); // new generation
    assert(!clock.observe(0,true,2,1,2,1,0)); // stage handoff
    assert(clock.observe(0,true,2,1,2,2,0));
    assert(!clock.observe(0,false,2,1,2,3,0)); // pause, seek or 60 Hz limit
    assert(!clock.observe(0,true,2,1,2,3,0));
    assert(clock.observe(0,true,2,1,2,4,.1)&&clock.alpha()==1); // CPU debt
    assert(!clock.observe(0,true,2,1,2,1,0)); // backwards seek
    clock.reset();assert(!clock.observe(0,true,2,1,2,2,0));
    std::puts("TH11 MP interpolation, owner isolation and display schedule contract PASS");
}
