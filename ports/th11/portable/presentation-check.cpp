// Executable CPU contract for TH11 ANM presentation. Exercises the real
// sidecar and never touches simulation owners.
#include "AnmPresentation.hpp"
#include <cassert>
#include <cmath>
#include <cstdio>
using namespace th11;
namespace {
void setup(AnmVm& vm){
    vm={};vm.id=7;vm.flags=3;vm.resource=reinterpret_cast<AnmResource*>(std::uintptr_t(0x1000));
    vm.script_index=1;vm.sprite_index=2;vm.timer.current=10;
    vm.position={10,20,0};vm.script_position={0,0,0};vm.child_position={0,0,0};
    vm.scale={1,1};vm.rotation={0,0,0};vm.color=0xffffffff;vm.secondary_color=0xffffffff;
}
}
int main(){
    // A VM created this tick is presented as-is: owners spawn VMs at the
    // origin and write the authored position within the same tick, so the
    // first frame must not sweep in from (0,0).
    presentation::reset();
    AnmVm spawned;setup(spawned);spawned.position={0,0,0};
    presentation::begin_tick();
    presentation::note_spawn(spawned);
    presentation::capture(spawned);
    spawned.position={100,60,0};spawned.timer.current=11;
    high_refresh::begin(.5f,true,true,true);
    AnmVm spawned_copy=spawned;
    assert(!presentation::present(spawned_copy,spawned));
    assert(spawned_copy.position.x==100.f&&spawned_copy.position.y==60.f);
    // The next tick interpolates normally between authored positions.
    presentation::begin_tick();presentation::capture(spawned);
    spawned.position={104,64,0};spawned.timer.current=12;
    high_refresh::begin(.5f,true,true,true);
    AnmVm next_copy=spawned;
    assert(presentation::present(next_copy,spawned));
    assert(std::fabs(next_copy.position.x-102.f)<1e-4f);
    assert(std::fabs(next_copy.position.y-62.f)<1e-4f);
    presentation::reset();
    AnmVm vm;setup(vm);
    presentation::begin_tick();
    presentation::capture(vm);
    // One fixed tick advances the authored endpoint.
    vm.position={14,24,0};vm.timer.current=11;
    high_refresh::begin(.5f,true,true,true);
    AnmVm copy=vm;
    assert(presentation::present(copy,vm));
    assert(std::fabs(copy.position.x-12.f)<1e-4f);
    assert(std::fabs(copy.position.y-22.f)<1e-4f);
    // The authoritative endpoint is untouched.
    assert(vm.position.x==14.f&&vm.position.y==24.f);
    // Repeated Draw is identical.
    AnmVm again=vm;
    assert(presentation::present(again,vm));
    assert(again.position.x==copy.position.x&&again.position.y==copy.position.y);
    // alpha=0 exposes the previous endpoint; alpha=1 the current one.
    high_refresh::begin(0.f,true,true,true);AnmVm before=vm;assert(presentation::present(before,vm));
    assert(std::fabs(before.position.x-10.f)<1e-4f);
    high_refresh::begin(1.f,true,true,true);AnmVm current=vm;assert(presentation::present(current,vm));
    assert(std::fabs(current.position.x-14.f)<1e-4f);
    // A teleport beyond the 128 px gate snaps.
    high_refresh::begin(.5f,true,true,true);AnmVm jumped=vm;jumped.position={400,400,0};AnmVm jumped_copy=jumped;
    presentation::present(jumped_copy,jumped); // teleport must not blend from 14
    assert(jumped_copy.position.x==400.f&&jumped_copy.position.y==400.f);
    // Lifecycle boundaries snap.
    AnmVm scripted=vm;scripted.script_index=3;AnmVm scripted_copy=scripted;
    assert(!presentation::present(scripted_copy,scripted));
    AnmVm rewound=vm;rewound.timer.current=5;AnmVm rewound_copy=rewound;
    assert(!presentation::present(rewound_copy,rewound));
    // scale reflection is authored, not a shrink through zero.
    presentation::begin_tick();AnmVm scale_vm;setup(scale_vm);presentation::capture(scale_vm);
    scale_vm.scale={-1,1};scale_vm.scale_interpolation.duration=1;
    high_refresh::begin(.5f,true,true,true);AnmVm scale_copy=scale_vm;
    assert(presentation::present(scale_copy,scale_vm));
    assert(scale_copy.scale.x==-1.f);
    // A scene owner can lower the continuous-position limit so authored world
    // rebases snap instead of blending.
    presentation::begin_tick();AnmVm scene;setup(scene);presentation::capture(scene);scene.position={70,20,0};
    high_refresh::begin(.5f,true,true,true);high_refresh::position_limit=32.f;AnmVm scene_copy=scene;
    presentation::present(scene_copy,scene);
    assert(scene_copy.position.x==70.f);
    high_refresh::position_limit=128.f;
    // Owner rebase: position and script offset jump in opposite directions but
    // the composed draw position stays continuous. The composed value must be
    // interpolated instead of snapping the object.
    presentation::begin_tick();AnmVm rebase;setup(rebase);rebase.position={512,0,0};rebase.script_position={-500,0,0};
    presentation::capture(rebase);rebase.position={24,0,0};rebase.script_position={0,0,0};
    high_refresh::begin(.5f,true,true,true);AnmVm rebase_copy=rebase;
    presentation::present(rebase_copy,rebase);
    assert(std::fabs((rebase_copy.position.x+rebase_copy.script_position.x)-18.f)<1e-4f);
    // Without a render-only transaction nothing is produced.
    high_refresh::end();AnmVm idle=vm;
    assert(!presentation::present(idle,vm));
    // A sample from an older tick is not a prev/current pair.
    presentation::begin_tick();AnmVm stale;setup(stale);presentation::capture(stale);
    stale.position={18,28,0};presentation::begin_tick();
    high_refresh::begin(.5f,true,true,true);AnmVm stale_copy=stale;
    assert(!presentation::present(stale_copy,stale));
    high_refresh::end();
    std::puts("TH11 ANM presentation CPU contract PASS");
    return 0;
}
