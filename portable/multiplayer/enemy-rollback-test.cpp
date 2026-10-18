#include "../../web/cpp/game/EnemyManager.hpp"
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <map>
#include <type_traits>
#include <vector>
#ifdef TH11_MULTIPLAYER
#include <eagler/netplay/RollbackJournal.hpp>
#endif
using namespace th11;
namespace {
unsigned checks=0,cases=0;
void check(bool value,const char* message){++checks;if(!value){std::fprintf(stderr,"FAIL: %s\n",message);std::exit(1);}}
template<class T>std::vector<u8> bytes(const T& v){const auto* p=reinterpret_cast<const u8*>(&v);return {p,p+sizeof v};}
template<class T>void append(std::vector<u8>& out,const T& v){auto b=bytes(v);out.insert(out.end(),b.begin(),b.end());}
std::vector<u8> instruction(u16 op,i32 time=0,std::vector<u32> args={}){
    EclInstruction head{time,op,u16(16+args.size()*4),0,255,u8(args.size()),0};
    std::vector<u8> out(head.length);std::memcpy(out.data(),&head,16);
    if(!args.empty())std::memcpy(out.data()+16,args.data(),args.size()*4);return out;
}
void concat(std::vector<u8>& a,const std::vector<u8>& b){a.insert(a.end(),b.begin(),b.end());}
std::vector<u8> named(u16 op,const char* name,const std::vector<u32>& args={},u8 count=1){
    const u32 length=(u32(std::strlen(name))+4)&~3u;
    std::vector<u8> out(20+length+args.size()*4);
    EclInstruction head{0,op,u16(out.size()),0,255,count,0};
    std::memcpy(out.data(),&head,16);std::memcpy(out.data()+16,&length,4);
    std::memcpy(out.data()+20,name,std::strlen(name)+1);
    if(!args.empty())std::memcpy(out.data()+20+length,args.data(),args.size()*4);return out;
}
EclProgram program(){
    std::map<std::string,std::vector<u8>> scripts;
    const auto idle=instruction(0,1000000);
    scripts["child"]=scripts["idle"]=idle;
    auto diagnostic=named(30,"fixture");concat(diagnostic,idle);scripts["diagnostic"]=diagnostic;
    auto nested=named(0x101,"child",{float_bits(20.f),float_bits(80.f),100,0,0},6);
    auto failure=nested;concat(failure,instruction(0xffff));concat(failure,idle);scripts["parent_fail"]=failure;
    concat(nested,idle);scripts["nested"]=nested;
    auto threads=named(16,"worker",{7},2);concat(threads,named(16,"worker",{8},2));concat(threads,idle);scripts["threads"]=threads;
    auto worker=instruction(42,0,{12345});concat(worker,idle);scripts["worker"]=worker;
    auto resource=std::make_unique<EclResource>();
    for(const auto& script:scripts){
        const u32 offset=resource->bytes.size();resource->bytes.resize(offset+16);
        concat(resource->bytes,script.second);resource->subroutines.push_back({script.first,offset,u32(16+script.second.size())});
    }
    EclProgram p;check(p.attach_decoded(std::move(resource))==0,"attach real decoded synthetic ECL");return p;
}
struct World final:EnemyFrameWorld{
    unsigned cleared=0;
    bool shot_damage(EnemyState&,i32& damage)override{damage=0;return true;}
    bool player_collision(EnemyState&,float,i32& damage)override{damage=0;return true;}
    void clear_shot_targets(Enemy*)override{++cleared;}
};
struct Fixture{
    EclProgram scripts=program();EnemyEnvironment environment{};EnemyCommandEnvironment commands{};
    World world;AnmManager anm;EnemyAnimations animations{anm};EnemyManager manager{scripts,environment,commands,world};
    Fixture(){commands.animations=&animations;}
    Enemy* spawn(const char* name="idle"){EnemySpawn p;p.position={0,100,0};p.health=100;return manager.spawn(name,p);}
#ifdef TH11_MULTIPLAYER
    void capture_external(Netplay::RollbackJournal& j){
        // These fixture-owned plain fields demonstrate the owner boundary.
        // EnemyManager deliberately does not capture these external objects.
        static_assert(std::is_trivially_copyable<EnemyEnvironment>::value);
        static_assert(std::is_trivially_copyable<EnemyCommandEnvironment>::value);
        check(j.Touch(&environment,sizeof environment),"fixture environment captured separately");
        check(j.Touch(&commands,sizeof commands),"fixture commands captured separately");
        check(j.Touch(&world.target,sizeof world.target),"fixture target captured separately");
        check(j.Touch(&world.target_locked,sizeof world.target_locked),"fixture target lock captured separately");
    }
#endif
};
void ordinary_semantics(){
    Fixture f;auto* parent=f.spawn("nested");check(parent&&f.manager.count==2,"real nested ECL spawns two enemies");
    auto* child=f.manager.first->value;
    check(child!=parent&&f.manager.last->value==parent,"native child-before-parent list order");
    check(child->state.current.position.x==20&&child->state.current.position.y==80,"nested arguments preserved");
    check(f.manager.destroy(*child)&&f.manager.count==1,"real child destruction");
    check(f.manager.reset(f.scripts)&&f.manager.count==0,"real manager reset");
    auto* enemy=f.spawn("threads");check(enemy&&enemy->script.threads.next&&enemy->script.threads.next->next,"real secondary ECL threads");
    check(enemy->script.threads.next->value->thread_id==8&&enemy->script.threads.next->next->value->thread_id==7,"native newest-thread-first links");
    check(f.manager.update()&&f.manager.update(),"real ECL thread update");
    check(enemy->script.threads.next->value->stack.top>12,"secondary stack instruction executed");
    check(f.manager.reset(f.scripts)&&f.spawn("diagnostic"),"real diagnostic scratch allocation/release");
    check(f.manager.reset(f.scripts),"reset diagnostic");
    check(!f.spawn("parent_fail")&&f.manager.last_error==-2&&f.manager.count==1,"failed parent preserves already spawned child semantics");
    check(f.manager.script_error.opcode==0xffff,"script diagnostic retained");
    ++cases;
}
}
#ifdef TH11_MULTIPLAYER
namespace th11 {
struct EnemyRollbackTestAccess{
    static EnemyScriptServices& service(EnemyManager& m,Enemy* e){return m.entries.at(e)->script;}
    static u32 slot(EnemyManager& m,Enemy* e){return m.rollback_slots_.at(e);}
    static std::size_t physical(EnemyManager& m){return m.rollback_storage_.size();}
    static std::size_t allocations(EnemyManager& m){return m.rollback_allocator_.state_.active.size();}
    static std::size_t physical_allocations(EnemyManager& m,u32 bytes){return m.rollback_allocator_.storage_[bytes].size();}
    static const EclRollbackAllocations& allocation_state(EnemyManager& m){return m.rollback_allocator_.state_;}
    static std::vector<u8> metadata(EnemyManager& m){
        std::vector<u8> out;append(out,m.first);append(out,m.last);append(out,m.count);append(out,m.total_created);
        append(out,m.timer);append(out,m.last_error);append(out,m.script_error);append(out,m.program);
        append(out,m.spawn_depth);append(out,m.rollback_logical_size_);return out;
    }
    static std::vector<u32> available(EnemyManager& m){return m.rollback_available_;}
    static std::map<Enemy*,EnemyRollbackEntry*> entries(EnemyManager& m){
        std::map<Enemy*,EnemyRollbackEntry*> out;for(auto& [key,entry]:m.entries)out[key]=entry.get();return out;
    }
    static std::map<const void*,std::vector<u8>> payload(EnemyManager& m){
        std::map<const void*,std::vector<u8>> out;
        for(auto& e:m.rollback_storage_){out[&e->enemy]=bytes(e->enemy);out[&e->script.error]=bytes(e->script.error);}
        for(auto& [size,storage]:m.rollback_allocator_.storage_)for(auto& memory:storage){
            auto* p=static_cast<const u8*>(memory.get());out[p]={p,p+size};
        }
        return out;
    }
};
}
namespace {
using Access=EnemyRollbackTestAccess;
struct Image{
    std::vector<u8> metadata;std::vector<u32> free;
    std::map<Enemy*,EnemyRollbackEntry*> entries;
    EclRollbackAllocations allocations;
    std::map<const void*,std::vector<u8>> payload;
    explicit Image(EnemyManager& m):metadata(Access::metadata(m)),free(Access::available(m)),entries(Access::entries(m)),
        allocations(Access::allocation_state(m)),payload(Access::payload(m)){}
    void compare(EnemyManager& m)const{
        check(metadata==Access::metadata(m),"explicit manager metadata bytes restored");
        check(free==Access::available(m),"Entry LIFO reuse order restored");
        check(entries==Access::entries(m),"typed Entry membership and stable service identities restored");
        const auto& current=Access::allocation_state(m);
        check(allocations.available==current.available&&allocations.logical_sizes==current.logical_sizes,"ECL per-size free order/frontier restored");
        check(allocations.active.size()==current.active.size(),"ECL live membership restored");
        for(auto& [pointer,a]:allocations.active){
            auto found=current.active.find(pointer);
            check(found!=current.active.end()&&found->second.memory==a.memory&&found->second.bytes==a.bytes&&found->second.index==a.index,"ECL typed physical lease restored");
        }
        for(auto& [pointer,value]:payload)check(std::memcmp(pointer,value.data(),value.size())==0,"complete pre-existing Enemy/error/ECL bytes restored, including inactive stack storage");
        for(auto& [enemy,entry]:entries){
            check(&entry->script.globals.enemy==&enemy->state,"service enemy reference remains bound to stable Entry");
        }
    }
};
Netplay::RollbackJournal journal(std::size_t bytes=8*1024*1024,std::size_t blocks=4096){
    Netplay::RollbackJournal j;check(j.Reset({8,bytes,blocks}),"journal configured");return j;
}
void begin(Fixture& f,Netplay::RollbackJournal& j,EnemyRollbackFrame& frame,u32 n){
    check(j.BeginFrame(n),"open checkpoint");f.capture_external(j);
    check(f.manager.BeginRollbackFrame(j,frame),"capture Enemy metadata/active bytes/typed leases");
    check(frame.valid()&&frame.frame()==n&&f.manager.RollbackFrameActive(),"frame owner attached");
    check(!f.manager.BeginRollbackFrame(j,frame),"duplicate begin rejected");
}
void seal(Fixture& f,Netplay::RollbackJournal& j){
    f.manager.EndRollbackFrame();check(!f.manager.RollbackFrameActive(),"observer detached");
    check(j.EndFrame(),"healthy frame sealed");
}
void undo(Fixture& f,Netplay::RollbackJournal& j,const EnemyRollbackFrame& frame,u32 n){
    u32 actual=~0u;check(j.UndoTo(n,&actual)&&actual==n,"entire shared journal undone before owner metadata");
    check(f.manager.RestoreRollbackFrame(frame,actual),"typed membership restored after bytes");
}
void nested_replay(){
    Fixture f;auto j=journal();const Image baseline(f.manager);EnemyRollbackFrame frame;begin(f,j,frame,10);
    auto* parent=f.spawn("nested");check(parent,"nested spawn during checkpoint");auto* child=f.manager.first->value;
    check(Access::slot(f.manager,parent)==0&&Access::slot(f.manager,child)==1,"parent slot reserved before recursive child");
    check(f.manager.first->value==child&&f.manager.last->value==parent,"list insertion remains child before parent");
    const Image expected(f.manager);seal(f,j);undo(f,j,frame,10);baseline.compare(f.manager);
    const auto physical=Access::physical(f.manager);EnemyRollbackFrame replay;begin(f,j,replay,10);
    check(f.spawn("nested")==parent&&f.manager.first->value==child,"replayed nested spawn reuses both exact addresses");
    check(Access::physical(f.manager)==physical,"replay creates no new Entry allocation");expected.compare(f.manager);
    seal(f,j);undo(f,j,replay,10);++cases;
}
void entry_reuse_and_failure(){
    Fixture f;auto* a=f.spawn();auto* b=f.spawn();auto* c=f.spawn();check(a&&b&&c,"three initial Entries");
    check(f.manager.destroy(*b),"create genuinely inactive stale slot");
    const auto stale=bytes(*b);auto j=journal();const Image baseline(f.manager);EnemyRollbackFrame frame;begin(f,j,frame,20);
    check(f.spawn("diagnostic")==b,"inactive Entry reused before reset");
    check(f.manager.destroy(*c)&&f.manager.destroy(*a),"release several slots in order");
    check(f.manager.reset(f.scripts),"manager reset with live Entry");
    auto* after=f.spawn("threads");check(after,"spawn with ECL threads after reset");
    const Image expected(f.manager);seal(f,j);undo(f,j,frame,20);baseline.compare(f.manager);
    check(stale==bytes(*b),"complete old inactive Enemy slot preserved before initialization");
    EnemyRollbackFrame replay;begin(f,j,replay,20);
    check(f.spawn("diagnostic")==b&&f.manager.destroy(*c)&&f.manager.destroy(*a)&&f.manager.reset(f.scripts),"replay reset/reuse sequence");
    check(f.spawn("threads")==after,"reset/replay resumes same slot");expected.compare(f.manager);seal(f,j);undo(f,j,replay,20);
    ++cases;
    EnemyRollbackFrame failed;const Image before_failure(f.manager);begin(f,j,failed,21);
    check(!f.spawn("parent_fail")&&f.manager.last_error==-2,"real failed parent path");
    check(f.manager.count==3&&f.manager.script_error.opcode==0xffff,"child survives failed parent until rollback");
    auto* child=f.manager.last->value;seal(f,j);undo(f,j,failed,21);before_failure.compare(f.manager);
    EnemyRollbackFrame retry;begin(f,j,retry,21);check(!f.spawn("parent_fail")&&f.manager.last->value==child,"failed nested replay keeps same child address");
    seal(f,j);undo(f,j,retry,21);++cases;
    EnemyRollbackFrame missing;begin(f,j,missing,22);
    check(!f.spawn("missing")&&f.manager.last_error==-2,"missing subroutine releases reserved Entry");
    seal(f,j);undo(f,j,missing,22);before_failure.compare(f.manager);++cases;
}
void threads_and_scratch(){
    Fixture f;auto* e=f.spawn("threads");check(e&&f.manager.update()&&f.manager.update(),"active real ECL stacks initialized");
    auto* top=e->script.threads.next;auto* second=top->next;
    auto* context=top->value;auto& services=Access::service(f.manager,e);
    context->stack.data[4095]=0xa5;second->value->stack.data[4094]=0x6c;
    check(Access::allocations(f.manager)==4,"two real contexts plus two real nodes");
    auto j=journal();const Image baseline(f.manager);EnemyRollbackFrame frame;begin(f,j,frame,30);
    e->script.cancel_secondary_threads();check(e->script.update_threads(1,services)==0,"real interpreter reaps cancelled secondary threads");
    check(Access::allocations(f.manager)==0&&e->script.threads.next==nullptr,"logical ECL release without dangling live nodes");
    e->script.root.instruction=f.scripts.find("threads");e->script.root.time=0;
    check(e->script.update_threads(1,services)==0,"real interpreter reallocates secondary threads");
    check(e->script.threads.next==top&&top->next==second&&top->value==context,"ECL allocation order reuses exact context/node addresses");
    check(context->stack.data[4095]==0,"reused full context zeroed including stale stack tail");
    check(e->script.update_threads(1,services)==0,"new thread stacks execute");
    services.error={f.scripts.find("idle"),77,3.5f,123};
    const Image expected(f.manager);seal(f,j);undo(f,j,frame,30);baseline.compare(f.manager);
    check(context->stack.data[4095]==0xa5&&services.error.instruction==nullptr,"stack tail and service diagnostics restored");
    EnemyRollbackFrame replay;begin(f,j,replay,30);
    e->script.cancel_secondary_threads();check(e->script.update_threads(1,services)==0,"replay secondary release");
    e->script.root.instruction=f.scripts.find("threads");e->script.root.time=0;
    check(e->script.update_threads(1,services)==0&&e->script.update_threads(1,services)==0,"replay secondary spawn/update");
    services.error={f.scripts.find("idle"),77,3.5f,123};expected.compare(f.manager);seal(f,j);undo(f,j,replay,30);++cases;
    void* scratch=services.allocate(1024);std::memset(scratch,0x5d,1024);services.release(scratch);
    const Image stale(f.manager);EnemyRollbackFrame diagnostic;begin(f,j,diagnostic,31);
    e->script.root.instruction=f.scripts.find("diagnostic");e->script.root.time=0;
    check(e->script.update_threads(1,services)==0,"actual diagnostic opcode allocates/frees 1024-byte scratch");
    check(Access::physical_allocations(f.manager,1024)==1,"diagnostic uses retained size-specific scratch slot");
    check(static_cast<u8*>(scratch)[1023]==0,"diagnostic allocation zeroed full scratch");
    seal(f,j);undo(f,j,diagnostic,31);stale.compare(f.manager);++cases;
}
void history_and_guards(){
    Fixture f;auto* first=f.spawn("threads");check(first,"history baseline");auto j=journal();const Image baseline(f.manager);
    EnemyRollbackFrame oldest,newest;begin(f,j,oldest,40);check(f.manager.destroy(*first)&&f.spawn("nested"),"history frame 40");seal(f,j);
    const Image middle(f.manager);begin(f,j,newest,41);check(f.manager.reset(f.scripts)&&f.spawn("threads"),"history frame 41");seal(f,j);
    undo(f,j,newest,41);middle.compare(f.manager);undo(f,j,oldest,40);baseline.compare(f.manager);++cases;
    EnemyRollbackFrame none;check(!f.manager.BeginRollbackFrame(j,none),"closed journal rejected");
    begin(f,j,none,42);check(!f.manager.RestoreRollbackFrame(none,42),"restore while observer attached rejected");seal(f,j);
    Fixture other;check(!other.manager.RestoreRollbackFrame(none,42),"wrong owner rejected");
    check(!f.manager.RestoreRollbackFrame(none,41),"wrong restored frame rejected");undo(f,j,none,42);
    check(j.BeginFrame(43),"new journal frame");check(!f.manager.BeginRollbackFrame(j,none),"snapshot object reuse rejected");check(j.EndFrame(),"empty journal frame seal");++cases;
    EnemyRollbackFrame expired;auto temp=std::make_unique<Fixture>();auto j2=journal();
    auto* leased=temp->spawn("threads");check(leased,"lease lifetime setup");
    auto* leased_context=leased->script.threads.next->value;
    begin(*temp,j2,expired,1);check(temp->manager.destroy(*leased),"logical release while snapshot retains typed Entry and ECL leases");
    seal(*temp,j2);temp.reset();
    check(!expired.valid(),"manager lifetime token invalidates frame after owner destruction");
    check(leased_context->thread_id==8,"typed frame retains physical ECL storage after owner destruction");
    check(leased->script.root.owner==&leased->script,"typed frame retains stable Enemy storage after owner destruction");
    j2.Clear();++cases;
}
void journal_failure(){
    {
        Fixture f;auto j=journal(4096);EnemyRollbackFrame frame;
        check(j.BeginFrame(50)&&f.manager.BeginRollbackFrame(j,frame),"small journal can hold empty manager metadata");
        auto* e=f.spawn("threads");
        check(e&&f.manager.last_error==0&&Access::allocations(f.manager)==4,"journal byte failure is not physical allocator failure or script error");
        check(j.Failed(),"new Entry first-write failure latched");f.manager.EndRollbackFrame();
        check(!j.EndFrame(),"failed frame cannot be sealed");j.Clear();++cases;
    }
    {
        Fixture f;auto* e=f.spawn("threads");check(e,"block limit setup");auto j=journal(1024*1024,1);EnemyRollbackFrame frame;
        check(j.BeginFrame(51)&&!f.manager.BeginRollbackFrame(j,frame),"insufficient metadata block budget rejects capture");
        check(j.Failed()&&!frame.valid(),"failed initial capture has no valid restore snapshot");
        f.manager.EndRollbackFrame();check(!j.EndFrame(),"failed initial capture cannot seal");j.Clear();++cases;
    }
}
}
#endif
int main(){
    ordinary_semantics();
#ifdef TH11_MULTIPLAYER
    nested_replay();entry_reuse_and_failure();threads_and_scratch();history_and_guards();journal_failure();
#endif
    std::printf("TH11 Enemy/ECL rollback components: %u cases, %u checks PASS\n",cases,checks);return 0;
}
