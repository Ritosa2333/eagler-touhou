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

namespace {bool count_allocations=false;size_t allocation_count=0,largest_allocation=0,allocation_bytes=0;}
void* operator new(size_t bytes){if(count_allocations){++allocation_count;allocation_bytes+=bytes;largest_allocation=std::max(largest_allocation,bytes);}if(auto* value=std::malloc(bytes?bytes:1))return value;throw std::bad_alloc();}
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
void restore_pool_growth(){
    AnmRegistry registry;
    auto* first=registry.allocate();
    auto* recycled=registry.allocate();
    auto* third=registry.allocate();
    require(registry.slot(*first)==0&&registry.slot(*recycled)==1&&registry.slot(*third)==2,"initial animation slots");
    require(registry.discard(*recycled),"recycle a slot below the allocation frontier");
    AnimationState before,after;
    require(before.capture(registry),"capture recycled slot and untouched suffix");
    require(registry.slot(*registry.allocate())==1&&registry.slot(*registry.allocate())==3,"use recycled slot before growing");
    require(after.capture(registry),"capture grown pool");
    require(before.restore(registry),"restore smaller pool");
    require(registry.slot(*registry.allocate())==1&&registry.slot(*registry.allocate())==3,"restore exact recycled and untouched order");
    require(before.restore(registry)&&after.restore(registry),"restore a larger pool from a smaller one");
    require(registry.count()==4&&registry.slot(*registry.allocate())==4,"grown pool keeps its next untouched slot");
}
void unregistered_overlay_owner(){
    AnmRegistry registry;AnmVm owner;owner.geometry.overlay=std::make_unique<AnmOverlay>();
    owner.geometry.overlay->panels[0]=std::make_unique<AnmVm>();auto* panel=owner.geometry.overlay->panels[0].get();
    panel->creation_parent=&owner;panel->visual.translation={43,81,0};
    panel->geometry.overlay=std::make_unique<AnmOverlay>();
    auto& nested=panel->geometry.overlay->panels[1];
    nested=std::make_unique<AnmVm>();
    auto& sibling=owner.geometry.overlay->panels[2];
    sibling=std::make_unique<AnmVm>();
    nested->rotation_parent=sibling.get();
    sibling->creation_parent=nested.get();
    auto* child=registry.allocate();const auto handle=registry.submit(*child,2);require(registry.attach(*child,*panel),"attach to nested owned VM");child->creation_parent=child->rotation_parent=panel;
    auto* deep_child=registry.allocate();
    const auto deep_handle=registry.submit(*deep_child,2);
    require(registry.attach(*deep_child,*nested),"attach to deeper owned VM");
    deep_child->rotation_parent=nested.get();
    AnimationState saved;require(saved.capture(registry,{&owner}),"capture unregistered overlay owner");
    require(registry.destroy_tree(*panel)&&registry.destroy_tree(*nested),"remove future nested owners");
    owner.geometry.overlay.reset();
    AnmVm replacement;
    require(saved.restore(registry,[&](AnmVm* original){return original==&owner?&replacement:original;}),"recreate owning root before borrowed panel binding");
    auto* restored=replacement.geometry.overlay->panels[0].get();child=registry.find(handle);
    require(restored&&child&&child->creation_parent==restored&&restored->creation_parent==&replacement,"nested registry and owning-VM references rebound together");
    require(restored->visual.translation.x==43&&registry.children(*restored).size()==1,"nested owned payload and registry graph");
    auto* restored_nested=restored->geometry.overlay->panels[1].get();
    auto* restored_sibling=replacement.geometry.overlay->panels[2].get();
    deep_child=registry.find(deep_handle);
    require(restored_nested&&restored_sibling&&deep_child&&deep_child->rotation_parent==restored_nested,
            "deeper panel and its registered child rebound together");
    require(restored_nested->rotation_parent==restored_sibling&&restored_sibling->creation_parent==restored_nested,
            "panel references survive sibling and depth changes in traversal");
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
void shared_snapshot_storage(){
    AnmRegistry registry;std::vector<AnmVm> roots(512);std::vector<AnmVm*> input;
    for(unsigned i=0;i<roots.size();++i){roots[i].visual.translation.x=float(i);input.push_back(&roots[i]);}
    std::array<AnimationState,13> ring;
    require(ring[0].capture(registry,input),"warm shared frame images");
    allocation_bytes=0;count_allocations=true;
    for(unsigned i=1;i<ring.size();++i)require(ring[i].capture(registry,input),"capture unchanged owning roots");
    count_allocations=false;
    std::printf("13 frames / 512 unchanged roots: %zu additional allocated bytes; VM bytes %zu\n",allocation_bytes,sizeof(AnmVm));
    require(allocation_bytes<roots.size()*sizeof(AnmVm)*2,"unchanged VMs were cloned into every rollback slot");
    roots[5].visual.translation.x=777;
    roots[7].geometry.trail=std::make_unique<AnmTrail>();roots[7].geometry.trail->angle=3;
    AnimationState changed;require(changed.capture(registry,input),"capture changed shared root and geometry");
    roots[5].visual.translation.x=-1;roots[7].geometry.trail->angle=-1;
    for(auto& frame:ring){
        require(frame.restore(registry),"restore immutable shared frame");
        require(roots[5].visual.translation.x==5&&!roots[7].geometry.trail,"future writes changed a retained snapshot");
    }
    require(changed.restore(registry),"restore copy-on-write frame");
    require(roots[5].visual.translation.x==777&&roots[7].geometry.trail&&roots[7].geometry.trail->angle==3,"copy-on-write lost updated VM or geometry");
    for(auto& frame:ring){frame.discard();require(!frame.ready()&&frame.instances()==0,"confirmed snapshot remains ready");
        require(frame.retained_bytes()<roots.size()*sizeof(AnmVm)/2,"confirmed snapshot still owns its VM payloads");}
    changed.discard();
    require(ring[0].capture(registry,{input[0]}),"reuse after releasing confirmed snapshots");
    roots[0].visual.translation.x=-1;require(ring[0].restore(registry)&&roots[0].visual.translation.x==0,"discard/reuse lost root identity");
}
void confirmed_image_reuse(){
    AnmRegistry registry;
    std::array<AnmVm,64> roots;
    std::vector<AnmVm*> input;
    for(auto& root:roots)input.push_back(&root);
    AnimationState confirmed,pending;
    require(confirmed.capture(registry,input),"capture first flat frame");
    for(auto& root:roots)root.visual.translation.x=1;
    require(pending.capture(registry,input),"retain next flat frame");
    confirmed.discard();
    for(auto& root:roots)root.visual.translation.x=2;
    allocation_count=0;
    count_allocations=true;
    const bool captured=confirmed.capture(registry,input);
    count_allocations=false;
    require(captured&&allocation_count==0,"confirmed flat images must be reused without allocation");
    require(pending.restore(registry),"restore the still-pending frame");
    for(const auto& root:roots)require(root.visual.translation.x==1,"image reuse overwrote a retained frame");
    require(confirmed.restore(registry),"restore the frame using recycled images");
    for(const auto& root:roots)require(root.visual.translation.x==2,"recycled image retained stale state");
}
void geometry_copy_reuse(){
    AnmVm source,target;
    source.geometry.distortion=std::make_unique<AnmDistortion>();
    source.geometry.trail=std::make_unique<AnmTrail>();
    source.geometry.gather=std::make_unique<AnmGatherEffect>();
    source.geometry.overlay=std::make_unique<AnmOverlay>();
    source.geometry.overlay->panels[0]=std::make_unique<AnmVm>();
    source.geometry.screen_vertices.resize(8);source.geometry.world_vertices.resize(8);
    target=source;
    for(unsigned n=0;n<32;++n){
        source.geometry.overlay->frames=n;source.geometry.overlay->panels[0]->variables.integers[0]=n;
        source.geometry.distortion->radius[0]=float(n);source.geometry.trail->points[0].x=float(n);
        source.geometry.gather->phase[0]=n;source.geometry.screen_vertices[0].color=n;
        source.geometry.world_vertices[0].color=n;source.timer.set(n);
        allocation_count=0;count_allocations=true;target=source;count_allocations=false;
        require(allocation_count==0,"steady VM copies must reuse independent geometry storage");
        require(target.geometry.overlay.get()!=source.geometry.overlay.get()&&target.geometry.overlay->panels[0].get()!=source.geometry.overlay->panels[0].get(),"copied overlays must own their panels");
        require(target.timer.current==int(n)&&target.geometry.overlay->frames==int(n)&&target.geometry.overlay->panels[0]->variables.integers[0]==int(n),"VM and nested state copied exactly");
        require(target.geometry.distortion->radius[0]==float(n)&&target.geometry.trail->points[0].x==float(n)&&target.geometry.gather->phase[0]==int(n)&&target.geometry.screen_vertices[0].color==n&&target.geometry.world_vertices[0].color==n,"all geometry payloads copied exactly");
    }
    source.geometry.clear();target=source;
    require(!target.geometry.distortion&&!target.geometry.trail&&!target.geometry.gather&&!target.geometry.overlay&&target.geometry.screen_vertices.empty()&&target.geometry.world_vertices.empty(),"copying an empty geometry clears the abandoned payload");
}
void tracked_interpolation(){
    AnmResource resource;resource.scripts.resize(1);resource.sprites.resize(1);resource.textures.resize(1);resource.textures[0].width=resource.textures[0].height=256;resource.sprites[0].width=resource.sprites[0].height=16;
    auto bits=[](float value){u32 out;std::memcpy(&out,&value,4);return out;};auto& code=resource.scripts[0].bytes;
    const auto command=[&](i16 opcode,i16 time,std::initializer_list<u32> args){const auto offset=code.size();const u16 length=u16(8+args.size()*4),mask=0;code.resize(offset+length);std::memcpy(code.data()+offset,&opcode,2);std::memcpy(code.data()+offset+2,&length,2);std::memcpy(code.data()+offset+4,&time,2);std::memcpy(code.data()+offset+6,&mask,2);size_t at=offset+8;for(auto value:args){std::memcpy(code.data()+at,&value,4);at+=4;}};
    command(400,0,{bits(1),bits(2),bits(0)});command(415,5,{bits(0),bits(0),bits(.05f)});command(407,12,{3,0,bits(20),bits(-1),bits(0)});command(300,17,{0});command(425,25,{bits(.001f)});command(435,30,{5,0,bits(3),bits(4)});command(3,50,{});command(5,0,{42});command(409,0,{4,0,32});command(7,1,{});command(-1,0,{});
    AnmVm fast,scalar;require(fast.bind(resource,0)&&scalar.bind(resource,0),"bind interpolation comparison");Rng a,b;
    for(unsigned frame=0;frame<240;++frame){if(frame==120)fast.pending_interrupt=scalar.pending_interrupt=42;
        const auto first=fast.tick_tracked(a,.625f),second=scalar.tick(b,.625f);
        require(first==second&&fast.error==scalar.error&&fast.instruction_offset==scalar.instruction_offset&&fast.pending_interrupt==scalar.pending_interrupt&&fast.visible==scalar.visible,"tracked interpolation changed execution flow");
        require(!std::memcmp(&fast.variables,&scalar.variables,sizeof fast.variables)&&!std::memcmp(&fast.visual,&scalar.visual,sizeof fast.visual)&&!std::memcmp(&fast.interpolators,&scalar.interpolators,sizeof fast.interpolators)&&!std::memcmp(&fast.timer,&scalar.timer,sizeof fast.timer)&&!std::memcmp(&fast.age,&scalar.age,sizeof fast.age),"tracked interpolation differs from full stepping");
    }
    const auto saved=fast;require(fast.select_sprite(0),"change atlas identity");const auto abandoned=fast.program_version;fast=saved;require(fast.select_sprite(0)&&fast.program_version!=abandoned,"corrected future reused a program identity");
    fast=scalar;fast.interpolators.alpha.begin(4,0,{0},{255});scalar.interpolators.alpha.begin(4,0,{0},{255});require(fast.tick(a,1)==scalar.tick(b,1)&&fast.visual.color==scalar.visual.color,"external interpolation bypassed full stepping");
}
void stopped_wait(){
    for(i16 opcode:{i16(3),i16(4)})for(float rate:{0.f,.625f,1.f,1.005f,1.25f}){
        AnmScript code;code.bytes.resize(16);const u16 length=8;const i16 end=-1;std::memcpy(code.bytes.data(),&opcode,2);std::memcpy(code.bytes.data()+2,&length,2);std::memcpy(code.bytes.data()+8,&end,2);
        AnmVm fast,scalar,parent;AnmEnvironment environment;Rng a,b;require(fast.bind(code)&&scalar.bind(code),"bind stopped comparison");
        fast.environment=scalar.environment=&environment;fast.visual.flags=scalar.visual.flags=1;fast.visual.render_flags=scalar.visual.render_flags=0x4000;
        fast.slowdown=scalar.slowdown=.125f;
        for(unsigned frame=0;frame<96;++frame){
            environment.paused=frame==5;environment.background_delta={1,2,3};
            if(frame==10)fast.visual.flags=scalar.visual.flags=fast.visual.flags|0x8000;
            if(frame==20){fast.visual.flags=scalar.visual.flags=fast.visual.flags&~0x8000u;fast.visual.render_flags=scalar.visual.render_flags=0x1004000;fast.visual.angular_velocity.z=scalar.visual.angular_velocity.z=.1f;}
            if(frame==30)fast.visual.angular_velocity.z=scalar.visual.angular_velocity.z=0;
            if(frame==40)fast.pending_interrupt=scalar.pending_interrupt=999;
            if(frame==50)fast.visual.flags=scalar.visual.flags=fast.visual.flags|(9u<<25);
            if(frame==60)fast.visual.flags=scalar.visual.flags=fast.visual.flags&~(31u<<25);
            if(frame==70)fast.rotation_parent=scalar.rotation_parent=&parent;
            if(frame==80)fast.rotation_parent=scalar.rotation_parent=nullptr;
            require(fast.tick_tracked(a,rate)==scalar.tick(b,rate)&&fast.error==scalar.error,"stopped fast path changed its result");
            require(!std::memcmp(&fast.variables,&scalar.variables,sizeof fast.variables)&&!std::memcmp(&fast.visual,&scalar.visual,sizeof fast.visual)&&!std::memcmp(&fast.timer,&scalar.timer,sizeof fast.timer)&&!std::memcmp(&fast.age,&scalar.age,sizeof fast.age)&&fast.visible==scalar.visible&&fast.instruction_offset==scalar.instruction_offset&&fast.pending_interrupt==scalar.pending_interrupt,"stopped fast path changed durable VM state");
        }
    }
}

}
int main(){exact_graph();frozen_root();overflow_and_wrap();restore_pool_growth();unregistered_overlay_owner();repeated_restore_allocations();capture_index_and_root_reuse();bounded_snapshot_growth();shared_snapshot_storage();confirmed_image_reuse();geometry_copy_reuse();tracked_interpolation();stopped_wait();std::puts("TH15 animation rollback ownership checks passed");}
