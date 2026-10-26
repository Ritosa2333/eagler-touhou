#include "multiplayer/AnimationState.hpp"
#include "game/AnmOverlay.hpp"
#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>

using namespace th15;
using th15::multiplayer::AnimationState;
namespace {
void require(bool ok,const char* why){if(!ok){std::fprintf(stderr,"FAIL: %s\n",why);std::exit(1);}}
struct Sprite final:AnmSpriteSource {i32 value=0;i32 sprite_index(i32)const noexcept override{return value;}};
struct Shape {u32 handle,slot;Timer timer,age;Vec3 position;};
std::array<std::vector<u32>,2> order(const AnmRegistry& r){
    std::array<std::vector<u32>,2> result;
    for(unsigned g=0;g<2;++g)for(u32 i=0;i<r.ordered_count(g);++i)result[g].push_back(r.ordered_handle(g,i));
    return result;
}
void exact_graph(){
    AnmRegistry registry;Rng rng;AnmVm root,replacement;Sprite old_sprite,new_sprite;
    root.visual.translation={18,24,0};std::vector<Shape> states;
    for(unsigned i=0;i<32;++i){
        auto* vm=registry.allocate();vm->visual.layer=i%2?36:4;vm->visual.translation={float(i*3),float(i*7),0};
        vm->timer.set(13);float rate=.625f;vm->timer.tick(&rate);vm->age.set(9);vm->age.tick(&rate);
        auto h=registry.submit(*vm,i%4==0?4:i%4==1?6:i%4==2?0:2);require(h!=0,"submit real animation");
        if(i%8==0){require(registry.attach(*vm,root),"borrowed root attachment");vm->creation_parent=vm->rotation_parent=&root;}
        else {auto* parent=registry.find(states.back().handle);require(registry.attach(*vm,*parent),"nested child attachment");vm->creation_parent=vm->rotation_parent=parent;}
        vm->sprite_source=&old_sprite;states.push_back({h,registry.slot(*vm),vm->timer,vm->age,vm->visual.translation});
    }
    require(registry.update(false,rng,1)&&registry.update(true,rng,1),"real ANM update/layer traversal");
    // Snapshot fractional timers after running the actual registry callbacks.
    for(auto& s:states){auto* vm=registry.find(s.handle);s.timer=vm->timer;s.age=vm->age;}
    auto* first=registry.find(states[0].handle);
    first->geometry.overlay=std::make_unique<AnmOverlay>();
    first->geometry.overlay->panels[0]=std::make_unique<AnmVm>();
    auto& panel=*first->geometry.overlay->panels[0];panel.creation_parent=first;panel.rotation_parent=&root;panel.visual.translation={71,92,0};
    const auto expected_order=order(registry);const auto identity=AnmVm::presentation_counter;
    const auto expected_layer=registry.layer(4).size();AnimationState saved;
    require(saved.capture(registry),"capture registry");require(AnmVm::presentation_counter==identity,"capture does not consume presentation identities");
    const auto bytes=saved.retained_bytes();require(saved.instances()==33,"capture live instances and borrowed root");
    auto* next=registry.allocate();const auto next_slot=registry.slot(*next);const auto next_identity=next->presentation_generation;
    const auto next_handle=registry.submit(*next,2);
    require(registry.destroy_tree(*first),"destroy live parent tree");root.visual.translation={999,999,0};
    for(unsigned n=0;n<80;++n){auto* vm=registry.allocate();registry.submit(*vm,2);}
    require(saved.restore(registry,[&](AnmVm* old){return old==&root?&replacement:old;},[&](const AnmSpriteSource* old){return old==&old_sprite?&new_sprite:old;}),"restore exact typed registry");
    require(order(registry)==expected_order,"preserve both live callback orders");
    require(registry.layer(4).size()==expected_layer,"restore draw queues");
    require(replacement.visual.translation.x==18&&root.visual.translation.x==999,"borrowed owner is explicitly rebound");
    require(AnmVm::presentation_counter==identity,"restore presentation identity clock");
    for(const auto& s:states){auto* vm=registry.find(s.handle);require(vm&&registry.slot(*vm)==s.slot,"same generation and slot handles");require(!std::memcmp(&vm->timer,&s.timer,sizeof(Timer))&&!std::memcmp(&vm->age,&s.age,sizeof(Timer)),"fractional and previous timer values are exact");require(vm->visual.translation.x==s.position.x&&vm->visual.translation.y==s.position.y,"original VM state");require(vm->sprite_source==&new_sprite,"dynamic sprite source is rebound");}
    first=registry.find(states[0].handle);require(first->creation_parent==&replacement&&first->rotation_parent==&replacement,"embedded parent pointer remapping");
    auto& restored_panel=*first->geometry.overlay->panels[0];require(restored_panel.creation_parent==first&&restored_panel.rotation_parent==&replacement&&restored_panel.visual.translation.x==71,"deep overlay values and internal references");
    next=registry.allocate();require(registry.slot(*next)==next_slot&&next->presentation_generation==next_identity&&registry.submit(*next,2)==next_handle,"next allocation matches on-time slot/generation/identity");
    require(saved.retained_bytes()==bytes,"restore leaves retained snapshot untouched");
    for(unsigned n=0;n<100;++n)require(saved.restore(registry,[&](AnmVm* old){return old==&root?&replacement:old;}),"repeat restoration");
    std::printf("exact animation graph: %zu instances, %zu retained bytes\n",saved.instances(),bytes);
}
void frozen_root(){
    AnmRegistry registry;AnmVm root;root.visual.translation={37,11,0};
    auto* child=registry.allocate();const auto h=registry.submit(*child,2);require(registry.attach(*child,root),"attach to embedded VM");child->creation_parent=child->rotation_parent=&root;
    require(registry.preserve_embedded_children(root),"native frozen-root ownership");
    AnimationState saved;require(saved.capture(registry),"capture frozen root");
    require(registry.destroy_tree(*child),"remove frozen-root child");Rng rng;require(registry.update(false,rng,1),"collect empty frozen root");
    require(saved.restore(registry),"reconstruct frozen root");child=registry.find(h);
    require(child&&child->creation_parent&&child->creation_parent!=&root,"child references reconstructed frozen owner");
    require(child->creation_parent->visual.translation.x==37&&registry.children(*child->creation_parent).size()==1,"frozen geometry and exact child list");
}
void overflow_and_wrap(){
    AnmRegistry registry;std::vector<u32> handles;handles.reserve(8193);
    registry.generation_counter()=0x7fffd;
    for(unsigned n=0;n<8193;++n){auto* vm=registry.allocate();handles.push_back(registry.submit(*vm,n%2?2:4));}
    AnimationState saved;require(saved.capture(registry),"capture overflow instances");
    const auto expected=order(registry);auto* overflow=registry.find(handles.back());require(overflow!=nullptr,"overflow generation lookup");require(registry.destroy_tree(*overflow),"free overflow allocation");
    require(saved.restore(registry),"restore overflow ownership");
    require(registry.count()==8193&&order(registry)==expected,"overflow and generation-wrap order retained");
    for(auto h:handles)require(registry.find(h)!=nullptr,"all exact wrapped handles resolve");
}
void unregistered_overlay_owner(){
    AnmRegistry registry;AnmVm owner;owner.geometry.overlay=std::make_unique<AnmOverlay>();
    owner.geometry.overlay->panels[0]=std::make_unique<AnmVm>();auto* panel=owner.geometry.overlay->panels[0].get();
    panel->creation_parent=&owner;panel->visual.translation={43,81,0};
    auto* child=registry.allocate();const auto handle=registry.submit(*child,2);require(registry.attach(*child,*panel),"attach to nested owned VM");child->creation_parent=child->rotation_parent=panel;
    AnimationState saved;require(saved.capture(registry,{&owner}),"capture unregistered overlay owner");
    require(registry.destroy_tree(*child),"remove future nested child");owner.geometry.overlay.reset();
    require(saved.restore(registry),"recreate owning root before borrowed panel binding");
    auto* restored=owner.geometry.overlay->panels[0].get();child=registry.find(handle);
    require(restored&&child&&child->creation_parent==restored&&restored->creation_parent==&owner,"nested registry and owning-VM references rebound together");
    require(restored->visual.translation.x==43&&registry.children(*restored).size()==1,"nested owned payload and registry graph");
}
}
int main(){exact_graph();frozen_root();overflow_and_wrap();unregistered_overlay_owner();std::puts("TH15 animation rollback ownership checks passed");}
