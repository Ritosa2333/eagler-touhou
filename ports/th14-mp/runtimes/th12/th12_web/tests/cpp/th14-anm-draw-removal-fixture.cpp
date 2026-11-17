#include "../../cpp/game/AnmManager.hpp"
#include "../../cpp/game/AnmOpcodes.hpp"
using namespace th11;

namespace {
AnmManager manager;
AnmResource resource;
AnmVm* retire;
void retire_during_update(AnmVm&) { manager.destroy(*retire); }
bool layer_is(u32 layer,std::initializer_list<const AnmVm*> expected) {
    const auto* vm=manager.layer_first(layer);
    for(const auto* next:expected){if(vm!=next)return false;vm=vm->draw_next;}
    return vm==nullptr;
}
}

extern "C" __attribute__((export_name("check_draw_removal"))) int check_draw_removal() {
    manager.clear();resource={};resource.scripts.resize(1);
    const AnmInstruction code[]={{i16(anm_canonical_opcode(2)),8,0,0},{-1,8,0,0}};
    resource.scripts[0].bytes.resize(sizeof code);
    std::memcpy(resource.scripts[0].bytes.data(),code,sizeof code);
    if(!manager.prepare_resource(resource,0))return 1;
    const auto create=[](u32 layer,bool overlay=false){return manager.create(resource,0,0,layer,overlay);};
    auto* head=create(3);auto* middle=create(3);auto* tail=create(3);auto* hud=create(35,true);
    if(!head||!middle||!tail||!hud||!manager.update(false)||!manager.update(true))return 2;
    if(!layer_is(3,{head,middle,tail})||!layer_is(35,{hud}))return 3;
    middle->layer=9;manager.destroy(*middle);
    if(!layer_is(3,{head,tail})||!layer_is(9,{})||!layer_is(35,{hud}))return 4;
    manager.destroy(*head);
    if(!layer_is(3,{tail}))return 5;
    auto* trigger=create(3);auto* survivor=create(3);
    if(!trigger||!survivor||trigger!=head)return 6;
    // Rebuild the layer; the callback retires the current draw tail while a
    // later registry entry still needs to be appended to that same layer.
    retire=tail;trigger->before_update=retire_during_update;
    if(!manager.update(false)||!layer_is(3,{trigger,survivor})||!layer_is(35,{hud}))return 7;
    trigger->before_update=nullptr;
    const u32 keep=survivor->id;manager.clear(keep);
    if(!layer_is(3,{})||!layer_is(35,{})||manager.find(keep)!=survivor)return 8;
    survivor->layer=7;
    auto* next=create(7);
    if(!next||!manager.update(false)||!layer_is(7,{survivor,next}))return 9;
    manager.destroy(*next);manager.destroy(*survivor);
    return layer_is(7,{})&&manager.active_count()==0?0:10;
}
