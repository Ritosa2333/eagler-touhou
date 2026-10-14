// Real TH11 ANM owners, interpreter, geometry and common byte journal.
// This component oracle does not claim full-world or browser performance coverage.
#include "../../th11_web/cpp/game/AnmManager.hpp"
#ifdef TH11_MULTIPLAYER
#include <eagler/netplay/RollbackJournal.hpp>
#endif
#include <algorithm>
#include <array>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <type_traits>
#include <vector>

namespace {
using namespace th11;
unsigned checks=0,groups=0;
void check(bool condition,const char* message){
    ++checks;
    if(!condition){std::fprintf(stderr,"FAIL ANM rollback group %u check %u: %s\n",groups,checks,message);std::exit(1);}
}
void group(const char* name){++groups;std::printf("GROUP %u %s\n",groups,name);}
template<class T> void append(std::vector<u8>& out,const T& value){
    static_assert(std::is_trivially_copyable<T>::value,"oracle scalar");
    const auto* p=reinterpret_cast<const u8*>(&value);out.insert(out.end(),p,p+sizeof value);
}
void bytes(std::vector<u8>& out,const void* value,std::size_t count){
    const auto* p=static_cast<const u8*>(value);out.insert(out.end(),p,p+count);
}
u32 float_word(float value){u32 out;std::memcpy(&out,&value,sizeof out);return out;}
void op(AnmScript& script,i16 code,std::initializer_list<u32> args={},u16 references=0,i16 time=0){
    const AnmInstruction instruction{code,u16(8+4*args.size()),time,references};
    append(script.bytes,instruction);for(u32 value:args)append(script.bytes,value);
}
AnmResource authored_resource(){
    AnmResource resource;resource.scripts.resize(4);
    op(resource.scripts[0],63);op(resource.scripts[0],-1);
    op(resource.scripts[1],88,{2});op(resource.scripts[1],63);op(resource.scripts[1],-1);
    op(resource.scripts[2],88,{0});op(resource.scripts[2],63);op(resource.scripts[2],-1);
    op(resource.scripts[3],87,{0}); // Authoritative RNG, rather than the default visual RNG.
    op(resource.scripts[3],40,{10000,1000003},1);
    op(resource.scripts[3],41,{float_word(10004.f),float_word(19.f)},1);
    op(resource.scripts[3],63);op(resource.scripts[3],-1);
    return resource;
}
AnmVm* create(AnmManager& manager,AnmResource& resource,i32 script=0,u32 layer=3,bool overlay=false,bool prepend=false){
    auto* vm=manager.create(resource,script,7,layer,overlay,prepend);
    check(vm!=nullptr,"real ANM create succeeded");return vm;
}
void tick(AnmManager& manager){
    check(manager.update(false),"real normal registry update");
    check(manager.update(true),"real overlay registry update");
}
void callback(AnmVm& vm){
    vm.reserved_418+=17;vm.position.x+=.125f;vm.color^=0x00010203u;
    vm.integers[2]+=7;vm.transform_matrix.m[11]+=.25f;
}
void ordinary_basics(){
    group("ordinary create/free/layers/geometry and interpreter");
    auto resource=authored_resource();AnmManager manager;
    auto* first=create(manager,resource);auto* second=create(manager,resource,0,3,false,true);
    const auto first_id=first->id;tick(manager);
    check(manager.layer_first(3)==second&&second->draw_next==first,"prepend and layer order");
    check(manager.find(first_id)==first&&manager.active_count()==2,"active lookup");
    check(manager.allocate_geometry(*first,96),"real geometry allocation");
    std::memset(first->geometry,0x31,96);
    check(!manager.allocate_geometry(*first,0),"zero geometry allocation rejected");
    manager.release_geometry(*first);check(!first->geometry,"release clears VM pointer");
    manager.destroy(*first);check(!manager.find(first_id)&&manager.active_count()==1,"destroy unlinks active owner");
    check(second->draw_next==nullptr,"destroy unlinks prior layer traversal");
    auto* reused=create(manager,resource);check(reused==first,"inactive slot reused");
    const auto seed=manager.script_rng.seed;auto* random=create(manager,resource,3);
    check(random->integers[0]>=0&&random->integers[0]<1000003,"real integer RNG instruction");
    check(manager.script_rng.calls==4&&manager.script_rng.seed!=seed,"real integer and float RNG consumed");
    manager.retire_resource(resource);tick(manager);
    check(manager.active_count()==0&&!manager.references(resource),"retire both registries");
    manager.clear();check(!manager.layer_first(3),"clear empties render traversal");
}
}

#ifdef TH11_MULTIPLAYER
namespace th11 {
// The oracle observes metadata, never mutates production internals. It compares
// every live/inactive logical VM byte and original geometry addresses and bytes;
// STL implementation nodes and physical reserve capacity are intentionally not state.
struct AnmRollbackTestAccess {
    static const void* registry(const AnmRollbackFrame& frame){return frame.active_.get();}
    static std::vector<u8> image(const AnmManager& manager){
        std::vector<u8> out;
        append(out,manager.rate);append(out,manager.script_rng);append(out,manager.visual_rng);
        append(out,manager.reference_positions);append(out,manager.camera_delta);append(out,manager.default_tangent);
        append(out,manager.last_error);append(out,manager.next_id);append(out,manager.spawn_depth);
        append(out,manager.heads);append(out,manager.tails);append(out,manager.layers);
        append(out,manager.multiplayer_owner);append(out,manager.logical_storage_size_);
        std::vector<std::pair<u32,AnmVm*>> active(manager.active.begin(),manager.active.end());
        std::sort(active.begin(),active.end(),[](const auto& a,const auto& b){return a.first<b.first;});
        append(out,active.size());
        for(const auto& entry:active){append(out,entry.first);append(out,entry.second);bytes(out,entry.second,sizeof(AnmVm));}
        append(out,manager.available.size());
        for(auto* vm:manager.available){append(out,vm);bytes(out,vm,sizeof(AnmVm));}
        std::vector<std::pair<std::uintptr_t,unsigned>> tags;
        for(const auto& entry:manager.multiplayer_tags)tags.emplace_back(reinterpret_cast<std::uintptr_t>(entry.first),entry.second);
        std::sort(tags.begin(),tags.end());append(out,tags.size());
        for(const auto& entry:tags){append(out,entry.first);append(out,entry.second);}
        std::vector<AnmVm*> geometry;
        for(const auto& entry:manager.geometry_sizes)geometry.push_back(entry.first);
        std::sort(geometry.begin(),geometry.end(),[](const auto* a,const auto* b){return reinterpret_cast<std::uintptr_t>(a)<reinterpret_cast<std::uintptr_t>(b);});
        append(out,geometry.size());
        for(auto* vm:geometry){
            const auto& allocation=manager.geometry_sizes.at(vm);
            append(out,vm);const auto pointer=allocation.memory.get();append(out,pointer);append(out,allocation.bytes);
            check(pointer==vm->geometry,"geometry owner and VM pointer agree");
            bytes(out,pointer,allocation.bytes);
        }
        return out;
    }
    static std::weak_ptr<void> lease(const AnmManager& manager,const AnmVm* vm){
        return manager.geometry_sizes.at(const_cast<AnmVm*>(vm)).memory;
    }
    static void topology(const AnmManager& manager){
        std::vector<const AnmVm*> registry;
        for(unsigned list=0;list<2;++list){
            const AnmVm::Node* previous=nullptr;unsigned count=0;
            for(auto* node=manager.heads[list];node;node=node->next){
                check(++count<=manager.active.size(),"registry is bounded/acyclic");
                check(node->previous==previous,"registry back-link");
                check(node->value&&&node->value->registry==node,"registry node identity");
                auto it=manager.active.find(node->value->id);
                check(it!=manager.active.end()&&it->second==node->value,"registry has an active owner");
                registry.push_back(node->value);previous=node;
            }
            check(previous==manager.tails[list],"registry tail");
        }
        check(registry.size()==manager.active.size(),"every active owner has one registry entry");
        std::sort(registry.begin(),registry.end(),[](const auto* a,const auto* b){return reinterpret_cast<std::uintptr_t>(a)<reinterpret_cast<std::uintptr_t>(b);});
        check(std::adjacent_find(registry.begin(),registry.end())==registry.end(),"no duplicate registry owner");
        for(const auto& entry:manager.active){
            const auto& vm=*entry.second;check(vm.id==entry.first,"active key equals VM id");
            const auto* previous=&vm.child;unsigned count=0;
            for(auto* node=vm.child.next;node;node=node->next){
                check(++count<=manager.active.size(),"child chain is bounded/acyclic");
                check(node->previous==previous&&node->value&&&node->value->child==node,"child backlinks and node identity");
                check(manager.active.count(node->value->id)==1,"child remains active");previous=node;
            }
        }
        for(unsigned layer=0;layer<manager.layers.size();++layer){
            unsigned count=0;for(auto* vm=manager.layers[layer];vm;vm=vm->draw_next){
                check(++count<=manager.active.size(),"draw layer is bounded/acyclic");
                check(vm->layer==layer&&manager.active.count(vm->id)==1,"layer owner and index");
            }
        }
    }
};
}
namespace {
using Access=th11::AnmRollbackTestAccess;
Netplay::RollbackJournalConfig config(){Netplay::RollbackJournalConfig c;c.maxFrames=8;c.maxBytesPerFrame=1024*1024;c.maxBlocksPerFrame=512;return c;}
void setup(Netplay::RollbackJournal& journal){check(journal.Reset(config()),"journal configured");}
std::unique_ptr<AnmRollbackFrame> begin(AnmManager& manager,Netplay::RollbackJournal& journal,u32 number){
    auto frame=std::make_unique<AnmRollbackFrame>();check(!frame->valid(),"new frame starts invalid");
    check(journal.BeginFrame(number),"journal begins checkpoint");check(manager.BeginRollbackFrame(journal,*frame),"manager begins checkpoint");
    check(frame->valid()&&frame->frame()==number&&manager.RollbackFrameActive(),"checkpoint identity and observer");
    return frame;
}
void end(AnmManager& manager,Netplay::RollbackJournal& journal){
    manager.EndRollbackFrame();check(!manager.RollbackFrameActive(),"observer stopped before journal end");
    check(journal.EndFrame(),"journal ends without capture failure");
}
void undo(AnmManager& manager,Netplay::RollbackJournal& journal,const AnmRollbackFrame& frame){
    manager.EndRollbackFrame();u32 restored=~0u;
    check(journal.UndoTo(frame.frame(),&restored),"journal restores checkpoint bytes");
    check(restored==frame.frame(),"journal returns exact checkpoint frontier");
    check(manager.RestoreRollbackFrame(frame,restored),"typed metadata restored after bytes");
    Access::topology(manager);
}
void same(AnmManager& manager,const std::vector<u8>& expected,const char* message){check(Access::image(manager)==expected,message);}
void full_state(){
    group("full VM bytes, environment, callbacks, layers and RNG");
    auto resource=authored_resource();AnmManager manager;Netplay::RollbackJournal journal;setup(journal);
    manager.multiplayer_owner=2;auto* p=create(manager,resource);auto* overlay=create(manager,resource,0,30,true);
    p->before_update=callback;p->reserved_420[1]=0x12345678;p->reserved_430=0x22334455;
    manager.reference_positions[0]={4,8,16};manager.default_tangent={.5f,.25f,.125f};tick(manager);
    const auto expected=Access::image(manager);const auto original=*p;auto frame=begin(manager,journal,10);
    p->integers[1]=-774;p->floats[3]=17.75f;p->reserved_37e=777;p->sprite_matrix.m[7]=7.25f;
    overlay->layer=29;manager.rate=.5f;manager.reference_positions[1]={32,64,128};
    manager.camera_delta={9,3,7};manager.default_tangent={5,6,7};manager.last_error=113;
    manager.script_rng.next32();manager.visual_rng.next32();create(manager,resource,3);
    tick(manager);end(manager,journal);
    check(std::memcmp(p,&original,sizeof original)!=0,"full VM oracle saw mutations");
    undo(manager,journal,*frame);same(manager,expected,"all logical VM bytes and manager state restored");
    check(std::memcmp(p,&original,sizeof original)==0,"original VM exact memcmp");
}
void geometry_history(){
    group("multiple checkpoints retain replaced/released geometry addresses and bytes");
    auto resource=authored_resource();AnmManager manager;Netplay::RollbackJournal journal;setup(journal);
    auto* p=create(manager,resource);check(manager.change_draw_mode(*p),"real ripple allocation/initialization");tick(manager);
    const auto base=Access::image(manager);void* first=p->geometry;auto first_lease=Access::lease(manager,p);
    auto frame10=begin(manager,journal,10);tick(manager);
    check(manager.allocate_geometry(*p,96),"replace ripple allocation");p->before_update=nullptr;
    std::memset(p->geometry,0xA6,96);void* second=p->geometry;auto second_lease=Access::lease(manager,p);
    check(second!=first&&!first_lease.expired(),"checkpoint retains original allocation while replaced");
    end(manager,journal);const auto middle=Access::image(manager);
    auto frame11=begin(manager,journal,11);
    std::memset(p->geometry,0xB7,96);manager.release_geometry(*p);
    check(!p->geometry&&!second_lease.expired(),"released geometry remains checkpoint-owned");
    manager.destroy(*p);auto* reused=create(manager,resource,3);check(reused==p,"same physical VM reused");
    check(manager.allocate_geometry(*reused,144),"new generation geometry");std::memset(reused->geometry,0xC8,144);
    tick(manager);end(manager,journal);
    undo(manager,journal,*frame11);same(manager,middle,"newer checkpoint exact state");
    check(p->geometry==second,"newer original geometry pointer");frame11.reset();
    undo(manager,journal,*frame10);same(manager,base,"older checkpoint exact state");
    check(p->geometry==first&&second_lease.expired(),"intermediate geometry released after last owner left");
    frame10.reset();manager.release_geometry(*p);check(first_lease.expired(),"original geometry released after final checkpoint and manager owner");
}
void nested_new_slots(){
    group("new physical slots replay at identical addresses with nested spawn");
    auto resource=authored_resource();AnmManager manager;Netplay::RollbackJournal journal;setup(journal);
    manager.multiplayer_owner=3;const auto empty=Access::image(manager);
    auto frame=begin(manager,journal,0);auto* parent=create(manager,resource,1);
    // The existing interpreter overwrites the nested child's child.next when
    // attaching it to its parent. Do not invent a tree contract here: every
    // actual VM (including the grandchild absent from child.next traversal)
    // is compared byte-for-byte through the active registry oracle below.
    check(manager.active_count()==3&&parent->child.next,"real nested spawn creates three registered owners");
    auto* child=parent->child.next->value;auto* grandchild=manager.find(1);
    check(grandchild&&grandchild!=child&&grandchild!=parent&&grandchild->script_index==0,"nested grandchild retained in registry");
    check(manager.multiplayer_tag(*parent)==3&&manager.multiplayer_tag(*child)==3&&manager.multiplayer_tag(*grandchild)==3,"nested tags inherit original seat");
    tick(manager);Access::topology(manager);end(manager,journal);const auto first=Access::image(manager);
    undo(manager,journal,*frame);same(manager,empty,"empty logical frontier restored without retaining new owners");frame.reset();
    auto replay=begin(manager,journal,0);auto* second=create(manager,resource,1);tick(manager);end(manager,journal);
    check(second==parent&&second->child.next->value==child&&manager.find(1)==grandchild,"all new physical slots reused at original addresses");
    same(manager,first,"nested resimulation exactly reproduces VM bytes, ids and topology");
    undo(manager,journal,*replay);same(manager,empty,"repeated rewind remains exact");
}
void inactive_reuse(){
    group("inactive reuse, initialize barriers and clear restore");
    auto resource=authored_resource();AnmManager manager;Netplay::RollbackJournal journal;setup(journal);
    auto* p=create(manager,resource);auto* q=create(manager,resource,0,5);tick(manager);
    p->position={3,4,5};manager.destroy(*p);const auto inactive=*p;const auto before=Access::image(manager);
    auto frame=begin(manager,journal,23);Vec3 initial{91,37,5};
    auto* reused=manager.create(resource,3,9,8,false,false,&initial);check(reused==p,"inactive VM consumed first");
    check(std::memcmp(p,&inactive,sizeof inactive)!=0,"inactive VM overwritten by bind/initialize");
    manager.destroy(*q);create(manager,resource,1);tick(manager);manager.clear();
    check(manager.active_count()==0,"clear mutates all owner metadata");end(manager,journal);
    undo(manager,journal,*frame);same(manager,before,"clear/reuse restored active and inactive VM exact bytes");
    check(std::memcmp(p,&inactive,sizeof inactive)==0,"inactive original VM bytes restored");
}
void external_geometry(){
    group("embedded external VM body is captured by its owning caller");
    auto resource=authored_resource();AnmVm external{};external.initialize();
    AnmManager manager;Netplay::RollbackJournal journal;setup(journal);
    check(external.bind_script(resource,0,7,&manager.rate),"external VM binds real script");
    check(manager.allocate_geometry(external,64),"external geometry owner allocation");std::memset(external.geometry,0x19,64);
    const auto body=external;const auto state=Access::image(manager);auto lease=Access::lease(manager,&external);
    auto frame=begin(manager,journal,1);
    check(journal.Touch(&external,sizeof external),"external owner journals its own complete VM");
    check(external.update(manager)==0,"external real interpreter update");
    std::memset(external.geometry,0x29,64);manager.release_geometry(external);
    check(manager.allocate_geometry(external,128),"replace external geometry");std::memset(external.geometry,0x39,128);
    external.color=0x31415926;end(manager,journal);
    undo(manager,journal,*frame);same(manager,state,"external geometry owner and bytes restored");
    check(std::memcmp(&external,&body,sizeof body)==0,"external caller-owned body restored");
    frame.reset();manager.release_geometry(external);check(lease.expired(),"external allocation releases after all leases");
}
void embedded_tag_geometry_history(){
    group("embedded player tags and geometry rebuild across two retained checkpoints");
    auto resource=authored_resource();std::array<AnmVm,3> bodies{};
    AnmManager manager;Netplay::RollbackJournal journal;setup(journal);
    for(auto& body:bodies){body.initialize();check(body.bind_script(resource,0,7,&manager.rate),"embedded player body binds");}
    manager.multiplayer_owner=1;auto* owned=create(manager,resource);
    // Match MultiplayerPilot's public tag path, including a body with no
    // geometry and no entry in the manager's active VM registry.
    for(unsigned i=0;i<bodies.size();++i)manager.multiplayer_tags[&bodies[i]]=i+1;
    check(manager.active_count()==1,"embedded bodies are not manager-owned active slots");
    check(manager.allocate_geometry(bodies[0],64),"first embedded geometry");
    check(manager.allocate_geometry(bodies[1],80),"second embedded geometry");
    std::memset(bodies[0].geometry,0x41,64);std::memset(bodies[1].geometry,0x52,80);
    tick(manager);const auto expected=Access::image(manager);const auto original_bodies=bodies;
    auto first_lease=Access::lease(manager,&bodies[0]),other_lease=Access::lease(manager,&bodies[1]);
    auto first=begin(manager,journal,30);
    for(auto& body:bodies)check(journal.Touch(&body,sizeof body),"external owner captures each original body");
    check(manager.allocate_geometry(bodies[0],96),"replace first embedded allocation");
    std::memset(bodies[0].geometry,0x63,96);auto middle_lease=Access::lease(manager,&bodies[0]);
    manager.release_geometry(bodies[1]);manager.multiplayer_tags.erase(&bodies[1]);
    manager.multiplayer_tags[&bodies[0]]=3;manager.multiplayer_tags[&bodies[2]]=1;
    bodies[2].position.x=19;end(manager,journal);
    auto second=begin(manager,journal,31);
    for(auto& body:bodies)check(journal.Touch(&body,sizeof body),"external owner captures next body frontier");
    manager.release_geometry(bodies[0]);
    check(manager.allocate_geometry(bodies[1],128),"future embedded allocation");
    std::memset(bodies[1].geometry,0x74,128);auto future_lease=Access::lease(manager,&bodies[1]);
    manager.multiplayer_tags.clear();manager.multiplayer_tags[owned]=2;bodies[2].color=0x12345678;
    end(manager,journal);
    check(!first_lease.expired()&&!other_lease.expired()&&!middle_lease.expired(),"both original and intermediate leases remain alive");
    // Undo crosses both raw frames before typed restoration. Keep the newer
    // snapshot until after every old geometry pointer and tag has returned.
    undo(manager,journal,*first);same(manager,expected,"flat keyed metadata restores complete original state");
    for(unsigned i=0;i<bodies.size();++i){
        check(std::memcmp(&bodies[i],&original_bodies[i],sizeof(AnmVm))==0,"caller-owned embedded body exactly restored");
        check(manager.multiplayer_tag(bodies[i])==i+1,"embedded seat tag restored independently of active and geometry maps");
    }
    check(manager.multiplayer_tag(*owned)==1&&manager.active_count()==1,"manager-owned tag and active registry preserved");
    check(!middle_lease.expired(),"crossed newer checkpoint still retains intermediate geometry after typed restore");
    check(future_lease.expired(),"unreferenced future geometry retires after raw undo");
    second.reset();check(middle_lease.expired(),"intermediate geometry retires only after its final checkpoint");
    first.reset();check(!first_lease.expired()&&!other_lease.expired(),"restored manager still owns both original allocations");
    manager.release_geometry(bodies[0]);manager.release_geometry(bodies[1]);
    check(first_lease.expired()&&other_lease.expired(),"all original leases release after final live owners");
}
void guards_and_failure(){
    group("wrong owner/frame, duplicate begin, failed journal end and expired identity");
    auto resource=authored_resource();AnmManager manager,other;Netplay::RollbackJournal journal;setup(journal);
    auto* p=create(manager,resource);AnmRollbackFrame unavailable;
    check(!manager.BeginRollbackFrame(journal,unavailable)&&!unavailable.valid(),"begin requires open journal");
    auto frame=begin(manager,journal,5);AnmRollbackFrame duplicate;
    check(!manager.BeginRollbackFrame(journal,duplicate)&&!duplicate.valid(),"duplicate begin rejected");
    check(!manager.RestoreRollbackFrame(*frame,5),"restore rejects active observer");
    p->color=7;end(manager,journal);const auto changed=Access::image(manager);
    check(!other.RestoreRollbackFrame(*frame,5),"foreign manager rejected");
    check(!manager.RestoreRollbackFrame(*frame,6),"wrong restored frontier rejected");
    same(manager,changed,"rejected restore leaves live state untouched");undo(manager,journal,*frame);
    check(journal.BeginFrame(6),"reopen journal");check(!manager.BeginRollbackFrame(journal,*frame),"ready snapshot cannot be overwritten");
    check(!manager.RollbackFrameActive(),"rejected begin did not attach observer");check(journal.EndFrame(),"empty journal closes");

    Netplay::RollbackJournal tiny;auto c=config();c.maxBytesPerFrame=1;check(tiny.Reset(c),"tiny journal configured");
    check(tiny.BeginFrame(0),"tiny frame begins");AnmRollbackFrame failed;
    check(!manager.BeginRollbackFrame(tiny,failed)&&!failed.valid()&&tiny.Failed(),"capture capacity failure invalidates checkpoint");
    manager.EndRollbackFrame();check(!manager.RollbackFrameActive(),"failed begin observer explicitly detached");
    check(!tiny.EndFrame(),"failed capture cannot be committed");u32 restored=~0u;
    check(!tiny.UndoTo(0,&restored)&&!manager.RestoreRollbackFrame(failed,0),"failed frame cannot be restored");
    tiny.Clear();

    Netplay::RollbackJournal overflow;auto small=config();small.maxBytesPerFrame=65536;check(overflow.Reset(small),"overflow journal configured");
    auto live=begin(manager,overflow,20);std::vector<u8> too_large(65537,0x52);
    check(!overflow.Touch(too_large.data(),too_large.size())&&overflow.Failed(),"late capture exceeds capacity");
    manager.EndRollbackFrame();check(!overflow.EndFrame()&&!manager.RollbackFrameActive(),"failed End stays failed and observer is clear");overflow.Clear();

    std::unique_ptr<AnmRollbackFrame> orphan;Netplay::RollbackJournal retired;setup(retired);
    {auto owner=std::make_unique<AnmManager>();create(*owner,resource);orphan=begin(*owner,retired,9);end(*owner,retired);retired.Clear();}
    check(!orphan->valid()&&!other.RestoreRollbackFrame(*orphan,9),"destroyed owner expires snapshot identity");
}

// Literal destination words are mutable resource state shared by every VM.
// Never capture a whole script here: the production hook owns exact 4-byte
// writes, while resources/backing storage outlive every checkpoint.
AnmResource literal_resource(i16 code,unsigned mode){
    AnmResource resource;resource.scripts.resize(1);auto& script=resource.scripts[0];
    const bool floating=(code>=6&&code<=27&&(code&1))||code>=41;
    const u32 destination=mode==2?(floating?float_word(10004.f):10000u):
        floating?float_word(code==47?7.25f:2.25f):code==5?1u:code==40?987654321u:12u;
    const u16 references=mode?1:0;
    if(code==5)op(script,code,{destination,0,0},references,1);
    else if(code<=27){
        if(code>=18)op(script,code,{destination,floating?float_word(.5f):5u,floating?float_word(.25f):3u},references,1);
        else op(script,code,{destination,floating?float_word(.5f):5u},references,1);
    }else if(code==40)op(script,code,{destination,97},references,1);
    else if(code==41)op(script,code,{destination,float_word(.5f)},references,1);
    else if(code<=46)op(script,code,{destination,float_word(.25f)},references,1);
    else op(script,code,{destination},references,1);
    op(script,63,{},0,1);op(script,-1);return resource;
}
void literal_opcode_matrix(){
    group("all 31 destination opcodes restore literal words and invalid-reference fallback");
    unsigned cases=0;
    for(i16 code=5;code<=47;++code){
        if(code>=28&&code<=39)continue;
        for(unsigned mode=0;mode<3;++mode){
            ++cases;auto resource=literal_resource(code,mode);AnmManager manager;Netplay::RollbackJournal journal;setup(journal);
            auto* vm=create(manager,resource);vm->integers[0]=code==5?1:code==40?987654321:12;vm->floats[0]=code==47?7.25f:2.25f;
            const auto initial=Access::image(manager);const auto script=resource.scripts[0].bytes;const auto body=*vm;
            auto frame=begin(manager,journal,1);const auto captured=journal.BytesForFrame(1),blocks=journal.BlocksForFrame(1);
            tick(manager);end(manager,journal);
            check(std::memcmp(vm,&body,sizeof body)!=0,"real opcode/VM execution occurred");
            if(mode==2){
                check(resource.scripts[0].bytes==script,"valid VM reference does not mutate script bytes");
                check(journal.BytesForFrame(1)==captured&&journal.BlocksForFrame(1)==blocks,"VM destination causes no overlapping subobject Touch");
            }else{
                check(resource.scripts[0].bytes!=script,"literal or invalid-reference fallback actually self-modifies");
                check(std::memcmp(resource.scripts[0].bytes.data(),script.data(),8)==0&&
                    std::memcmp(resource.scripts[0].bytes.data()+12,script.data()+12,script.size()-12)==0,"only the destination word changed");
                check(journal.BytesForFrame(1)==captured+4&&journal.BlocksForFrame(1)==blocks+1,"one exact first-write word captured");
            }
            undo(manager,journal,*frame);
            check(resource.scripts[0].bytes==script,"full instruction byte vector exactly restored");
            same(manager,initial,"opcode restores full VM, manager state and both RNGs");
        }
    }
    check(cases==93,"all 31 opcodes covered in three destination modes");
}
void mapped_destination_ranges(){
    group("all supported integer/float destination ranges stay inside the existing VM owner");
    AnmResource resource;resource.scripts.resize(1);auto& script=resource.scripts[0];
    for(u32 id:{10000u,10001u,10002u,10003u,10008u,10009u})op(script,6,{id,1234},1,1);
    for(float id:{10004.f,10005.f,10006.f,10007.f,10013.f,10014.f,10015.f})op(script,7,{float_word(id),float_word(.625f)},1,1);
    op(script,63,{},0,1);op(script,-1);
    AnmManager manager;Netplay::RollbackJournal journal;setup(journal);auto* vm=create(manager,resource);
    const auto initial=Access::image(manager);const auto original=script.bytes;auto frame=begin(manager,journal,2);
    const auto captured=journal.BytesForFrame(2),blocks=journal.BlocksForFrame(2);tick(manager);end(manager,journal);
    for(auto value:vm->integers)check(value==1234,"integer destination array executed");
    for(auto value:vm->extra_integers)check(value==1234,"extra integer destination executed");
    for(auto value:vm->floats)check(value==.625f,"float destination array executed");
    check(vm->script_position.x==.625f&&vm->script_position.y==.625f&&vm->script_position.z==.625f,"script-position float destinations executed");
    check(script.bytes==original&&journal.BytesForFrame(2)==captured&&journal.BlocksForFrame(2)==blocks,"all mapped references avoid literal hook and overlap");
    undo(manager,journal,*frame);same(manager,initial,"all VM reference families exactly restore");check(script.bytes==original,"mapped script remains immutable");
}
void shared_literal_history(){
    group("two VMs share one literal; first-write dedup and multi-checkpoint restore");
    auto resource=literal_resource(8,0);AnmManager manager;Netplay::RollbackJournal journal;setup(journal);
    auto* first=create(manager,resource);auto* second=create(manager,resource);const auto initial=Access::image(manager);
    const auto original=resource.scripts[0].bytes;auto a=begin(manager,journal,10);
    const auto captured=journal.BytesForFrame(10),blocks=journal.BlocksForFrame(10);tick(manager);end(manager,journal);
    auto word=[&](){i32 out;std::memcpy(&out,resource.scripts[0].bytes.data()+8,4);return out;};
    check(word()==22,"both VMs mutated the same 12+5+5 literal");
    check(journal.BytesForFrame(10)==captured+4&&journal.BlocksForFrame(10)==blocks+1,"shared operand is captured only on first write");
    const auto middle=Access::image(manager),middle_script=resource.scripts[0].bytes;auto b=begin(manager,journal,11);
    for(auto* vm:{first,second}){vm->instruction=vm->script_begin;vm->timer.set(1,&manager.rate);}
    tick(manager);end(manager,journal);check(word()==32,"next checkpoint shares another pair of writes");
    undo(manager,journal,*b);same(manager,middle,"newer shared checkpoint VM state");check(resource.scripts[0].bytes==middle_script,"newer original operand restored");b.reset();
    undo(manager,journal,*a);same(manager,initial,"older shared checkpoint VM state");check(resource.scripts[0].bytes==original,"oldest original operand restored");
}
void nested_and_external_literals(){
    group("nested create and external embedded VM use the same script pre-write owner");
    AnmResource resource;resource.scripts.resize(2);
    op(resource.scripts[0],88,{1});op(resource.scripts[0],63);op(resource.scripts[0],-1);
    op(resource.scripts[1],8,{12,5});op(resource.scripts[1],63);op(resource.scripts[1],-1);
    AnmManager manager;Netplay::RollbackJournal journal;setup(journal);const auto initial=Access::image(manager);
    const auto original=resource.scripts[1].bytes;auto frame=begin(manager,journal,20);create(manager,resource);end(manager,journal);
    check(manager.active_count()==2&&resource.scripts[1].bytes!=original,"nested spawn self-modifies during create");
    undo(manager,journal,*frame);same(manager,initial,"nested manager restoration");check(resource.scripts[1].bytes==original,"nested resource literal restored");frame.reset();
    auto external_resource=literal_resource(8,0);AnmVm external{};external.initialize();
    check(external.bind_script(external_resource,0,7,&manager.rate),"external real VM binds shared resource");
    // Time 1 lets binding remain outside the mutation checkpoint.
    external.timer.set(1,&manager.rate);const auto body=external;const auto external_script=external_resource.scripts[0].bytes;
    const auto external_owner=Access::image(manager);auto checkpoint=begin(manager,journal,21);
    check(journal.Touch(&external,sizeof external),"external body is captured by its actual owner");
    const auto captured=journal.BytesForFrame(21);check(external.update(manager)==0,"external interpreter executes literal destination");end(manager,journal);
    check(external_resource.scripts[0].bytes!=external_script&&journal.BytesForFrame(21)==captured+4,"manager environment captures only external script word");
    undo(manager,journal,*checkpoint);same(manager,external_owner,"external environment owner restores");
    check(std::memcmp(&external,&body,sizeof body)==0&&external_resource.scripts[0].bytes==external_script,"external body and resource word restore through separate owners");
}
void literal_capacity_failure(){
    group("script word first-write byte/block capacity failure rejects the transaction");
    for(unsigned bound=0;bound<2;++bound){
        auto resource=literal_resource(8,0);AnmManager manager;create(manager,resource);
        Netplay::RollbackJournal probe;setup(probe);auto measure=begin(manager,probe,0);
        const auto required_bytes=probe.BytesForFrame(0),required_blocks=probe.BlocksForFrame(0);end(manager,probe);probe.Clear();measure.reset();
        Netplay::RollbackJournal journal;auto limits=config();
        if(bound==0)limits.maxBytesPerFrame=required_bytes;else limits.maxBlocksPerFrame=required_blocks;
        check(journal.Reset(limits),"budget fits initial complete capture but not new script word");auto frame=begin(manager,journal,1);
        const auto original=resource.scripts[0].bytes;tick(manager);
        check(journal.Failed()&&resource.scripts[0].bytes!=original,"late literal capture fails and semantic mutation still occurs");
        manager.EndRollbackFrame();check(!journal.EndFrame()&&!journal.UndoTo(1),"partial self-modifying transaction cannot commit or restore");
        journal.Clear();
    }
}

void page_first_write_and_recreation(){
    group("paged capture retains inactive bytes, stable addresses and first-write boundaries");
    auto resource=authored_resource();AnmManager manager;Netplay::RollbackJournal journal;setup(journal);
    std::vector<AnmVm*> original;
    for(u32 i=0;i<65;++i)original.push_back(create(manager,resource));
    for(u32 i=32;i<64;++i)manager.destroy(*original[i]);
    tick(manager);const auto before=Access::image(manager);
    auto frame=begin(manager,journal,0);
    const auto bytes_before=journal.BytesForFrame(0),blocks_before=journal.BlocksForFrame(0);
    auto* reused=create(manager,resource);check(reused==original[63],"original LIFO order across inactive page");
    check(journal.BytesForFrame(0)==bytes_before+32*sizeof(AnmVm)&&journal.BlocksForFrame(0)==blocks_before+1,
        "first write of wholly inactive page captures exactly one complete owner page");
    for(u32 i=1;i<32;++i)check(create(manager,resource)==original[63-i],"remaining inactive slots keep their original addresses");
    check(journal.BytesForFrame(0)==bytes_before+32*sizeof(AnmVm)&&journal.BlocksForFrame(0)==blocks_before+1,
        "all subsequent writes in the same page reuse its original preimage");
    std::vector<AnmVm*> added;
    for(u32 i=0;i<33;++i)added.push_back(create(manager,resource));
    tick(manager);end(manager,journal);const auto after=Access::image(manager);
    undo(manager,journal,*frame);same(manager,before,"complete page rewind includes inactive payload and logical frontier");frame.reset();
    auto replay=begin(manager,journal,0);
    for(u32 i=0;i<32;++i)check(create(manager,resource)==original[63-i],"replayed inactive allocation preserves exact order");
    for(auto* vm:added)check(create(manager,resource)==vm,"new physical pages survive rewind at identical VM addresses");
    tick(manager);end(manager,journal);
    same(manager,after,"recreated cross-page state is byte-exact, including RNG, links and IDs");
}

void immutable_registry_sharing(){
    group("immutable registry snapshots share only unchanged membership, not VM payload");
    auto resource=authored_resource();AnmManager manager;Netplay::RollbackJournal journal;setup(journal);
    auto* vm=create(manager,resource);tick(manager);const auto original=Access::image(manager);
    auto first=begin(manager,journal,0);vm->integers[1]=77;tick(manager);end(manager,journal);
    auto second=begin(manager,journal,1);
    check(Access::registry(*first)==Access::registry(*second),"unchanged membership reuses exact immutable map");
    manager.destroy(*vm);auto* reused=create(manager,resource);check(reused==vm,"original free slot reused");
    tick(manager);end(manager,journal);
    auto third=begin(manager,journal,2);
    check(Access::registry(*third)!=Access::registry(*first),"destroy/recreate invalidates snapshot even at equal population");
    end(manager,journal);
    undo(manager,journal,*first);same(manager,original,"VM bytes and original registry both rewind exactly");
    third.reset();second.reset();
    auto replay=begin(manager,journal,0);
    check(Access::registry(*replay)==Access::registry(*first),"restored registry is reusable without another node copy");
    end(manager,journal);
}
void lease_retirement(){
    group("shared geometry survives all retained checkpoints and expires on final release");
    auto resource=authored_resource();AnmManager manager;Netplay::RollbackJournal journal;setup(journal);
    auto* vm=create(manager,resource);check(manager.allocate_geometry(*vm,32),"lease fixture geometry");
    std::memset(vm->geometry,0x5A,32);auto weak=Access::lease(manager,vm);const void* original=vm->geometry;
    auto first=begin(manager,journal,0);end(manager,journal);
    auto second=begin(manager,journal,1);manager.release_geometry(*vm);end(manager,journal);
    check(!weak.expired(),"two snapshots retain released allocation");
    journal.DiscardBefore(1);first.reset();check(!weak.expired(),"remaining checkpoint retains allocation");
    undo(manager,journal,*second);check(vm->geometry==original,"remaining checkpoint restores exact address");
    second.reset();check(!weak.expired(),"restored live metadata still owns allocation");
    manager.release_geometry(*vm);check(weak.expired(),"final live owner removal releases allocation");
}
}
#endif

int main(){
    static_assert(sizeof(void*)==4,"run the real 32-bit WASI layout");
    static_assert(sizeof(th11::AnmVm)==0x434,"TH11 VM layout remains exact");
    ordinary_basics();
#ifdef TH11_MULTIPLAYER
    full_state();geometry_history();nested_new_slots();inactive_reuse();external_geometry();embedded_tag_geometry_history();guards_and_failure();lease_retirement();page_first_write_and_recreation();
    literal_opcode_matrix();mapped_destination_ranges();shared_literal_history();nested_and_external_literals();literal_capacity_failure();immutable_registry_sharing();
    std::printf("PASS ANM rollback multiplayer: %u groups, %u checks\n",groups,checks);
#else
    std::printf("PASS ANM ordinary: %u groups, %u checks\n",groups,checks);
#endif
    return 0;
}
