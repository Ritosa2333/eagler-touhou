#include "multiplayer/AnimationState.hpp"
#include "multiplayer/FramePointerIndex.hpp"
#include "game/AnmOverlay.hpp"
#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <new>

namespace {bool count_allocations=false;size_t allocation_count=0,largest_allocation=0;}
void* operator new(size_t bytes){if(count_allocations){++allocation_count;largest_allocation=std::max(largest_allocation,bytes);}if(auto* value=std::malloc(bytes?bytes:1))return value;throw std::bad_alloc();}
void operator delete(void* value)noexcept{std::free(value);}
void* operator new[](size_t bytes){return ::operator new(bytes);}
void operator delete[](void* value)noexcept{std::free(value);}
void operator delete(void* value,size_t)noexcept{std::free(value);}
void operator delete[](void* value,size_t)noexcept{std::free(value);}

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
void repeated_restore_allocations(){
    AnmRegistry registry;std::vector<u32> handles;
    for(unsigned i=0;i<512;++i){auto* vm=registry.allocate();handles.push_back(registry.submit(*vm,i%2?2:4));
        if(i%4)require(registry.attach(*vm,*registry.find(handles[i-i%4])),"attach allocation fixture child");}
    AnimationState saved;require(saved.capture(registry),"capture allocation fixture");
    const auto expected=order(registry);size_t total=0;
    for(unsigned round=0;round<8;++round){
        allocation_count=0;count_allocations=true;const bool restored=saved.restore(registry);count_allocations=false;
        require(restored,"restore allocation fixture");total+=allocation_count;
        require(order(registry)==expected,"recycled callback links preserve both group orders");
        for(unsigned i=0;i<handles.size();++i){auto* vm=registry.find(handles[i]);require(vm!=nullptr,"recycled node retains its handle");
            if(i%4==0){const auto children=registry.children(*vm);require(children.size()==3,"recycled child link count");
                for(unsigned j=0;j<3;++j)require(children[j]==registry.find(handles[i+3-j]),"recycled child order");}}
    }
    std::printf("repeated 512-VM restore allocations: %zu across 8 restores\n",total);
    require(total<8*128,"fixed graph restoration must avoid allocations proportional to VM count");
}
void capture_index_and_root_reuse(){
    multiplayer::FramePointerIndex<int> index;std::array<int,513> keys{};
    require(index.find(&keys[0])==decltype(index)::missing,"empty index lookup");
    index.reset(0);
    for(unsigned i=0;i<keys.size();++i)require(index.insert(&keys[i],i),"capture index grows without losing entries");
    const auto bytes=index.retained_bytes();
    for(unsigned i=0;i<keys.size();++i){require(index.find(&keys[i])==i,"capture index collision/growth values");require(!index.insert(&keys[i],999),"capture index preserves the first duplicate value");}
    index.reset(1);require(index.retained_bytes()==bytes,"capture index retains its high water storage");
    for(const auto& key:keys)require(index.find(&key)==decltype(index)::missing,"capture index removes every previous-frame key");

    AnmRegistry registry;std::array<AnmVm,64> roots;AnimationState saved;
    auto* vm=registry.allocate();const auto handle=registry.submit(*vm,2);
    std::vector<AnmVm*> input;
    for(unsigned round=0;round<6;++round){
        const unsigned count=round%2?17:64;input.clear();
        for(unsigned n=0;n<count;++n){const auto i=count-1-n;roots[i].visual.translation.x=float(i+round*100);input.push_back(&roots[i]);input.push_back(nullptr);input.push_back(&roots[i]);input.push_back(vm);}
        if(round%2)std::reverse(input.begin(),input.end());
        require(saved.capture(registry,input),"capture duplicate, null and registered roots");
        require(saved.instances()==count+1,"capture each owning root exactly once");
        for(unsigned i=0;i<count;++i)roots[i].visual.translation.x=-1;
        require(saved.restore(registry),"restore graph after root count and order change");
        require(registry.find(handle)==vm,"root dedup keeps the registered VM owner");
        for(unsigned i=0;i<count;++i)require(roots[i].visual.translation.x==float(i+round*100),"root values survive dedup and table reuse");
    }
}
void bounded_snapshot_growth(){
    // Cross a power-of-two boundary with live owning roots, as cancellation
    // pickups do during a bomb. Growing by one VM must not allocate/copy a
    // second complete image of thousands of roots before freeing the old one.
    AnmRegistry registry;std::vector<AnmVm> roots(2049);std::vector<AnmVm*> input;
    for(unsigned i=0;i<2048;++i){roots[i].visual.translation.x=float(i);input.push_back(&roots[i]);}
    AnimationState saved;require(saved.capture(registry,input),"warm root snapshot storage");
    roots.back().visual.translation.x=2048;input.push_back(&roots.back());
    largest_allocation=0;count_allocations=true;const bool captured=saved.capture(registry,input);count_allocations=false;
    require(captured,"grow root snapshot storage");
    std::printf("2048 -> 2049 roots: largest allocation %zu; retained payload %zu\n",largest_allocation,saved.retained_bytes());
    require(largest_allocation<sizeof(AnmVm)*128,"one new root requested a second full root image array");
    require(saved.retained_bytes()<roots.size()*(sizeof(AnmVm)+256)+512*1024,"root snapshot retains geometric spare VM payloads");
    for(auto& vm:roots)vm.visual.translation.x=-1;
    require(saved.restore(registry),"restore after bounded root growth");
    for(unsigned i=0;i<roots.size();++i)require(roots[i].visual.translation.x==float(i),"bounded root growth lost VM state");

    AnmRegistry graph;AnimationState nodes;std::vector<u32> handles;
    for(unsigned i=0;i<2048;++i){auto* vm=graph.allocate();vm->visual.translation.x=float(i);handles.push_back(graph.submit(*vm,2));}
    require(nodes.capture(graph),"warm registered snapshot storage");auto* added=graph.allocate();added->visual.translation.x=2048;handles.push_back(graph.submit(*added,2));
    largest_allocation=0;count_allocations=true;const bool grown=nodes.capture(graph);count_allocations=false;
    require(grown,"grow registered snapshot storage");
    require(largest_allocation<sizeof(AnmVm)*128,"one new node requested a second full node image array");
    for(auto handle:handles)graph.find(handle)->visual.translation.x=-1;
    require(nodes.restore(graph),"restore after bounded node growth");
    for(unsigned i=0;i<handles.size();++i)require(graph.find(handles[i])->visual.translation.x==float(i),"bounded node growth lost VM state");
}

}
int main(){exact_graph();frozen_root();overflow_and_wrap();unregistered_overlay_owner();repeated_restore_allocations();capture_index_and_root_reuse();bounded_snapshot_growth();std::puts("TH15 animation rollback ownership checks passed");}
