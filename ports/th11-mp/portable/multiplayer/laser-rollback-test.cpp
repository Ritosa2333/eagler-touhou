// Real LaserManager/Line/Infinite/cut/ANM/renderer component oracle.
// External world effects and resource lifetimes remain separate owners.
#include "../../web/cpp/game/LaserManager.hpp"
#include "../../web/cpp/game/AnmRenderer.hpp"
#ifdef TH11_MULTIPLAYER
#include <eagler/netplay/RollbackJournal.hpp>
#endif
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
void check(bool ok,const char* why){++checks;if(!ok){std::fprintf(stderr,"FAIL laser group %u check %u: %s\n",groups,checks,why);std::exit(1);}}
void group(const char* name){++groups;std::printf("GROUP %u %s\n",groups,name);}
template<class T> void append(std::vector<u8>& out,const T& value){
    static_assert(std::is_trivially_copyable<T>::value,"pure scalar oracle");
    const auto* p=reinterpret_cast<const u8*>(&value);out.insert(out.end(),p,p+sizeof value);
}
void append_bytes(std::vector<u8>& out,const void* value,std::size_t size){
    const auto* p=static_cast<const u8*>(value);out.insert(out.end(),p,p+size);
}
struct Graphics final:ZunGraphics {
    touhou::graphics::PipelineState state{};unsigned draws=0;
    touhou::graphics::PipelineState& pipeline()override{return state;}
    u32 texture(const AnmResource&,u32 index)override{return index+1;}
    void bind_texture(u32)override{}
    void set_layout(touhou::graphics::VertexLayout)override{}
    void set_matrix(touhou::graphics::MatrixKind,const Matrix4&)override{}
    void triangles(u32,const void*,u32)override{++draws;}
    void primitives(touhou::graphics::Topology,u32,const void*,u32)override{++draws;}
    bool select_target(const AnmResource*,u32)override{return true;}
    bool clear_target(u32,const GraphicsViewport*)override{return true;}
};
struct World final:LaserWorld {
    struct Effects {u32 sound=0,cancel=0,reward=0,shot=0,graze=0;} effects;
    bool fail_sound=false,fail_effect=false;
#ifdef TH11_MULTIPLAYER
    bool multiplayer=false;
    bool multiplayer_world()const override{return multiplayer;}
    Vec3 multiplayer_target(Vec3)const override{return {17,399,0};}
#endif
    bool sound(i32,float,bool)override{++effects.sound;return !fail_sound;}
    bool cancel_effect(Vec3,i32)override{++effects.cancel;return !fail_effect;}
    bool cancel_reward(Vec3)override{++effects.reward;return true;}
    bool cancel_shot(Vec3,float)override{++effects.shot;return true;}
    i32 collision(Vec3,float,float,float)override{return 0;}
    i32 warning_collision(Vec3,float,float,float)override{return 0;}
    bool graze_effect(Vec3)override{++effects.graze;return true;}
    bool graze_reward()override{return true;}
    bool graze_register()override{return true;}
    bool boss_position(Vec3&)override{return false;}
};
struct Fixture {
    AnmResource resource;AnmEnvironment animations;World world;Graphics graphics;AnmRenderer renderer{graphics};
    std::unique_ptr<LaserManager> manager;
    Fixture(){
        resource.textures.resize(1);resource.textures[0].width=resource.textures[0].height=512;
        resource.sprites.resize(512);for(auto& sprite:resource.sprites){sprite.width=sprite.height=16;sprite.u1=sprite.v1=1;}
        resource.scripts.resize(256);
        for(auto& script:resource.scripts){
            const AnmInstruction bind{3,12,0,0},wait{63,8,0,0},end{-1,8,0,0};
            script.bytes.resize(28);std::memcpy(script.bytes.data(),&bind,8);
            std::memcpy(script.bytes.data()+12,&wait,8);std::memcpy(script.bytes.data()+20,&end,8);
        }
        manager=std::make_unique<LaserManager>(resource,animations,world);
    }
    LaserLineParameters line()const{
        LaserLineParameters p{};p.position={-80,200,0};p.growth_limit=p.initial_length=160;
        p.end_distance=1000;p.width=12;p.speed=2;p.start_sound=p.transform_sound=-1;return p;
    }
    LaserInfiniteParameters infinite()const{
        LaserInfiniteParameters p{};p.position={-80,100,0};p.initial_length=160;p.max_length=220;
        p.width=12;p.speed=2;p.warning_frames=180;p.expand_frames=5;p.active_frames=300;p.shrink_frames=10;
        p.start_sound=p.transform_sound=-1;return p;
    }
    void tick(){check(manager->update(),"real laser update");check(manager->draw(renderer),"real laser semantic Draw");renderer.flush();}
};
void chain(const LaserManager& manager){
    const LaserState* previous=nullptr;u32 count=0;
    for(auto* node=manager.first();node;node=node->next){
        check(++count<=LaserManager::capacity,"bounded acyclic active chain");
        check(node->type==0||node->type==1,"chain has supported actual payload type");
        if(previous)check(node->previous==previous,"previous link is exact");
        if(node->next)check(node->next->previous==node,"forward link reciprocal");
        previous=node;
    }
    check(count==manager.active_count,"chain count agrees with metadata");
    check(previous==manager.last(),"chain tail agrees with metadata");
}
void ordinary(){
    group("ordinary real line/infinite create, update, Draw, cancel and clear");
    Fixture f;auto& m=*f.manager;const i32 a=m.spawn(f.line()),b=m.spawn(f.infinite());
    check(a>0&&b>a&&m.active_count==2,"both real types created in order");
    check(m.first()->id==a&&m.last()->id==b,"append semantic order");
    f.tick();check(f.graphics.draws>0,"real renderer submitted geometry");chain(m);
    check(m.cancel_id(a)&&!m.find(a),"cancel id retires matching identity");
    f.tick();check(m.active_count==1&&m.first()->id==b,"canceled line removed by update");
    m.mark_all();f.tick();check(m.active_count==0&&!m.first()&&!m.last(),"marked infinite released");
    m.clear();check(m.active_count==0,"empty clear");
}
}

#ifdef TH11_MULTIPLAYER
namespace th11 {
struct LaserRollbackTestAccess {
    static std::vector<u8> image(const LaserManager& m){
        std::vector<u8> out;
        append(out,m.rate);append(out,m.player);append(out,m.active_count);append(out,m.next_id);append(out,m.last_error);
        append(out,m.last_center);append(out,m.last_size);append(out,m.sentinel);append(out,m.tail);append(out,m.updating);
        append(out,m.rollback_occupied);
        // Full aligned pool includes inactive payload, cross-type tail and
        // padding. A not-yet-allocated physical pool has zero initial bytes.
        if(m.rollback_storage)append_bytes(out,m.rollback_storage.get(),sizeof(LaserManager::Storage)*LaserManager::capacity);
        else out.resize(out.size()+sizeof(LaserManager::Storage)*LaserManager::capacity,0);
        return out;
    }
    static void shape(const LaserManager& m){
        u32 occupied=0;
        for(u32 slot=0;slot<LaserManager::capacity;++slot)if(m.rollback_occupied[slot]){
            ++occupied;check(bool(m.rollback_storage),"occupied slot has physical owner");
            const auto* laser=reinterpret_cast<const LaserState*>(m.rollback_storage[slot].bytes);
            unsigned in_chain=0;for(auto* b=m.first();b;b=b->next)if(b==laser)++in_chain;
            check(in_chain==1,"occupied slot occurs exactly once in chain");
        }
        check(occupied==m.active_count,"occupied count equals active chain");
        check(m.first()?m.first()->previous==&m.sentinel:m.tail==&m.sentinel,"sentinel boundary");
        chain(m);
    }
    static std::size_t stride(){return sizeof(LaserManager::Storage);}
    static const void* physical(const LaserManager& m){return m.rollback_storage.get();}
};
}
namespace {
using Access=th11::LaserRollbackTestAccess;
Netplay::RollbackJournalConfig config(){Netplay::RollbackJournalConfig c;c.maxFrames=8;c.maxBytesPerFrame=8*1024*1024;c.maxBlocksPerFrame=1024;return c;}
void setup(Netplay::RollbackJournal& journal){check(journal.Reset(config()),"real journal configured");}
void begin(Fixture& f,Netplay::RollbackJournal& journal,u32 frame){
    check(journal.BeginFrame(frame),"journal checkpoint opens");check(f.manager->BeginRollbackFrame(journal),"laser observer begins");
    // This simple effect ledger is the fixture world's owned state. Neither
    // its polymorphic service header nor the ANM/resource owners are copied.
    check(journal.Touch(&f.world.effects,sizeof f.world.effects),"external effect ledger has its own owner");
    check(journal.Touch(&f.world.rate,sizeof f.world.rate)&&journal.Touch(&f.world.player,sizeof f.world.player),"external world scalar owner captured explicitly");
}
void end(Fixture& f,Netplay::RollbackJournal& journal){
    f.manager->EndRollbackFrame();check(!f.manager->RollbackFrameActive(),"observer detached before end");
    check(journal.EndFrame(),"journal complete transaction succeeds");
}
void undo(Fixture& f,Netplay::RollbackJournal& journal,u32 frame){
    f.manager->EndRollbackFrame();u32 restored=~0u;
    check(journal.UndoTo(frame,&restored)&&restored==frame,"journal exact checkpoint restore");
    Access::shape(*f.manager);
}
void same(const LaserManager& m,const std::vector<u8>& expected,const char* reason){check(Access::image(m)==expected,reason);}
void cross_type(){
    group("same slot line/infinite reuse restores full stride and metadata");
    Fixture f;auto& m=*f.manager;Netplay::RollbackJournal journal;setup(journal);
    const auto line=m.spawn(f.line());auto* address=m.find(line);check(address!=nullptr,"line fixture");
    f.tick();const auto state=Access::image(m);begin(f,journal,10);
    check(m.cancel_id(line),"real line cancellation");f.tick();
    auto infinite=m.spawn(f.infinite());check(m.find(infinite)==address&&address->type==1,"inactive line slot reused as infinite");
    auto& payload=*reinterpret_cast<LaserInfinite*>(address);payload.reserved_644=0x13579BDF;
    payload.origin.reserved_430=0x98765432;payload.parameters.angular_velocity=.025f;
    f.tick();end(f,journal);undo(f,journal,10);
    same(m,state,"entire aligned pool and state restored to line owner");check(m.find(line)==address&&address->type==0,"line address/type recovered");

    begin(f,journal,11);m.clear();infinite=m.spawn(f.infinite());check(m.find(infinite)==address,"same physical slot after clear");
    f.tick();end(f,journal);const auto middle=Access::image(m);
    begin(f,journal,12);m.clear();const auto again=m.spawn(f.line());check(m.find(again)==address,"infinite slot reused as line");
    f.tick();end(f,journal);undo(f,journal,12);same(m,middle,"infinite full payload including larger tail recovered");
    undo(f,journal,11);same(m,state,"older checkpoint recovered after newer rollback");
}
void new_storage_and_replay(){
    group("lazy physical storage survives rewind and new spawn reuses addresses");
    Fixture f;auto& m=*f.manager;Netplay::RollbackJournal journal;setup(journal);
    check(!Access::physical(m),"physical pool is lazy");const auto empty=Access::image(m);
    begin(f,journal,0);const auto a=m.spawn(f.line()),b=m.spawn(f.infinite());auto* pa=m.find(a);auto* pb=m.find(b);
    f.tick();end(f,journal);const auto future=Access::image(m);const auto* physical=Access::physical(m);
    check(pa&&pb&&pa!=pb&&physical,"separate stable real payload slots");
    undo(f,journal,0);same(m,empty,"empty logical world and original inactive bytes recovered");
    check(Access::physical(m)==physical,"physical pool not freed by undo");
    begin(f,journal,0);check(m.spawn(f.line())==a&&m.spawn(f.infinite())==b,"resimulation keeps original issue ids");
    f.tick();end(f,journal);check(m.find(a)==pa&&m.find(b)==pb,"resimulation reuses both original addresses");
    same(m,future,"repeated forward reproduces full pool bytes and topology");
}
void inactive_bytes_and_failed_spawn(){
    group("inactive preimage, rejected initialization and clear preserve exact bytes");
    Fixture f;auto& m=*f.manager;Netplay::RollbackJournal journal;setup(journal);
    auto id=m.spawn(f.infinite());auto* old=m.find(id);check(old!=nullptr,"inactive baseline");f.tick();m.clear();
    const auto before=Access::image(m);begin(f,journal,2);
    auto bad=f.line();bad.sprite=127;check(m.spawn(bad)==0&&m.last_error==-2&&m.active_count==0,"invalid original appearance fails after slot construction");
    auto good=m.spawn(f.line());check(good>id&&m.find(good)==old,"failed spawn consumed id and releases same slot");
    f.tick();m.clear();end(f,journal);undo(f,journal,2);same(m,before,"inactive payload/tail and id/error restored before construction");

    begin(f,journal,3);f.world.fail_sound=true;auto sounding=f.infinite();sounding.start_sound=4;
    check(m.spawn(sounding)==0&&m.last_error==-2,"sound initialization failure preserves original failure result");
    f.world.fail_sound=false;end(f,journal);undo(f,journal,3);same(m,before,"callback failure exact rollback");
}
void cancellation_split(){
    group("real line/infinite cut creates new slots within existing traversal");
    for(bool infinite:{false,true}){
        Fixture f;auto& m=*f.manager;Netplay::RollbackJournal journal;setup(journal);
        const auto id=infinite?m.spawn(f.infinite()):m.spawn(f.line());check(id>0,"split source");
        f.tick();const auto before=Access::image(m);const auto effects=f.world.effects;
        begin(f,journal,6);const float y=infinite?100.f:200.f;
        check(m.cancel_rectangle({0,y,0},{32,32,0},true)>0,"real cut selected middle segments");
        check(m.active_count>1,"real cut spawned surviving line tail");chain(m);
        f.tick();end(f,journal);
        check(f.world.effects.cancel>effects.cancel&&f.world.effects.reward>effects.reward,"real cancellation callback ledger recorded");
        undo(f,journal,6);same(m,before,"split parent/new child slots and chain fully restored");
        check(std::memcmp(&f.world.effects,&effects,sizeof effects)==0,"external ledger restored by its explicit owner");
    }
}
void nested_reflection(){
    group("real transform reflection spawns during update and replays exact chain");
    Fixture f;auto& m=*f.manager;Netplay::RollbackJournal journal;setup(journal);
    auto p=f.line();p.position={160,200,0};p.initial_length=p.growth_limit=64;
    p.transforms[0].opcode=0x100;p.transforms[0].a=3;p.transforms[0].c=1;p.transforms[0].d=8;
    const auto id=m.spawn(p);check(id>0,"reflection source");const auto before=Access::image(m);
    begin(f,journal,4);f.tick();check(m.active_count==2,"actual transform created reflected child");
    auto* child=m.last();check(child&&child!=m.first(),"nested child owns distinct stable slot");
    end(f,journal);const auto future=Access::image(m);
    undo(f,journal,4);same(m,before,"reflection and nested allocation undone");
    begin(f,journal,4);f.tick();end(f,journal);
    check(m.last()==child,"nested spawn replay uses identical child address");same(m,future,"reflection payload and entire chain replay exactly");
}
void capacity_and_id(){
    group("256 capacity, spawn-line full no-op, id wrap and reuse");
    Fixture f;auto& m=*f.manager;Netplay::RollbackJournal journal;setup(journal);
    auto p=f.line();p.initial_length=p.growth_limit=16;
    for(u32 i=0;i<LaserManager::capacity;++i)check(m.spawn(p)>0,"fill original capacity");
    const auto count=m.active_count;const auto next=m.next_id;const auto full=Access::image(m);
    begin(f,journal,9);check(m.spawn(p)==0&&m.next_id==next&&m.active_count==count,"full spawn returns zero without consuming id");
    check(m.spawn_line(p)&&m.active_count==count,"nested full spawn remains successful no-op");
    auto* first=m.first();first->state=1;check(m.update(),"release one actual active slot");
    m.next_id=0x7fffffff;const auto wrap=m.spawn(f.infinite());
    check(wrap==0x10000&&m.find(wrap)==first&&m.active_count==LaserManager::capacity,"signed issue-id wrap and same-slot reuse");
    end(f,journal);undo(f,journal,9);same(m,full,"capacity and cross-type last allocation restored");
}
void other_mutation_paths(){
    group("warning, direct ECL-style writes, frozen update and protected cancellation");
    Fixture f;auto& m=*f.manager;Netplay::RollbackJournal journal;setup(journal);
    const auto a=m.spawn(f.line()),b=m.spawn(f.infinite());check(a&&b,"mixed mutation owners");f.tick();
    const auto before=Access::image(m);begin(f,journal,15);
    auto* line=m.find(a);line->position.x+=.375f;line->speed=4;
    auto* infinite=m.find(b);infinite->width=9;
    check(m.update_warning(.15f),"real infinite warning mutation");
    f.world.rate=.75f;f.world.player={11,398,0};const auto anm_rate=f.animations.rate;
    check(m.update(false,true)&&f.animations.rate==anm_rate,"frozen update restores external animation rate");
    const auto paused=Access::image(m);check(m.update(true)&&m.draw(f.renderer,true),"paused paths accepted");
    same(m,paused,"paused update/draw do not alter payload");
    line->protection=1;reinterpret_cast<LaserInfinite*>(infinite)->parameters.flags|=8;
    check(m.cancel_circle({0,150,0},512,3,true)==0,"protected real circle cancellation skipped");
    line->protection=0;reinterpret_cast<LaserInfinite*>(infinite)->parameters.flags&=~8u;
    check(m.cancel_circle({0,150,0},512,3,false)>0,"unprotected real circle cancellation exercised");
    check(m.cancel_all(true,false),"cancel all remaining real owners");m.clear();
    end(f,journal);undo(f,journal,15);same(m,before,"all mutation paths restore complete payload and metadata");
}
void failure_guards(){
    group("observer admission and capacity failures are never successful commits");
    Fixture f;auto& m=*f.manager;Netplay::RollbackJournal journal;setup(journal);
    check(!m.BeginRollbackFrame(journal),"closed journal observer rejected");
    begin(f,journal,0);check(!m.BeginRollbackFrame(journal),"duplicate observer rejected");end(f,journal);

    Netplay::RollbackJournal tiny;auto small=config();small.maxBytesPerFrame=1;check(tiny.Reset(small),"tiny journal setup");
    check(tiny.BeginFrame(1),"tiny frame opens");check(!m.BeginRollbackFrame(tiny)&&tiny.Failed(),"metadata capture failure propagated");
    m.EndRollbackFrame();check(!m.RollbackFrameActive()&&!tiny.EndFrame(),"failed Begin detaches but cannot commit");tiny.Clear();

    // Empty Begin fits; allocating one real slot then crosses the byte cap.
    Netplay::RollbackJournal late;auto bounded=config();bounded.maxBytesPerFrame=2048;check(late.Reset(bounded),"late failure setup");
    check(late.BeginFrame(2)&&m.BeginRollbackFrame(late),"empty observer begins within bound");
    check(Access::stride()>bounded.maxBytesPerFrame,"complete storage exceeds this explicit fixture limit");
    check(m.spawn(f.line())>0,"allocation may still construct after observer failure");
    check(late.Failed(),"inactive construction attempted full-stride preimage");
    m.EndRollbackFrame();check(!late.EndFrame()&&!m.RollbackFrameActive(),"failed inactive capture cannot be committed");
    u32 restored=~0u;check(!late.UndoTo(2,&restored),"failed journal cannot provide partial rollback");late.Clear();
}
}
#endif
int main(){
    static_assert(sizeof(void*)==4,"32-bit WASI payload oracle");
    static_assert(sizeof(LaserLine)==0xe8c&&sizeof(LaserInfinite)==0xeb0,"actual TH11 laser layouts");
    ordinary();
#ifdef TH11_MULTIPLAYER
    cross_type();new_storage_and_replay();inactive_bytes_and_failed_spawn();cancellation_split();nested_reflection();capacity_and_id();other_mutation_paths();failure_guards();
    std::printf("PASS laser rollback multiplayer: %u groups, %u checks\n",groups,checks);
#else
    std::printf("PASS laser ordinary: %u groups, %u checks\n",groups,checks);
#endif
    return 0;
}
