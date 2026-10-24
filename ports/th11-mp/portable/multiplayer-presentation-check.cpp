#include "AnmPresentation.hpp"
#include <cassert>
#include <cstdio>
int main(){
    using namespace th11;
    AnmVm vm{};vm.id=1;vm.flags=3;
    presentation::begin_tick();presentation::capture(vm);
    vm.position.x=16;
    auto output=vm;
    high_refresh::begin(.5f,true,true);
    assert(!presentation::present(output,vm));
    assert(output.position.x==16&&vm.position.x==16);
    high_refresh::end();presentation::reset();
    std::puts("TH11 MP authoritative frontier presentation contract PASS");
}
