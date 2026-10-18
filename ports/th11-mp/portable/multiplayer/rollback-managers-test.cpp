#include "../../web/cpp/game/BulletManager.hpp"
#include "../../web/cpp/game/ItemManager.hpp"
#include "../../web/cpp/game/AnmRenderer.hpp"
#include <eagler/netplay/RollbackJournal.hpp>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <vector>

using namespace th11;
namespace {
unsigned checks=0,cases=0;
void check(bool value,const char* message){++checks;if(!value){std::fprintf(stderr,"FAIL: %s\n",message);std::exit(1);}}
template<class T> std::vector<u8> bytes(const T& object){
    const auto* first=reinterpret_cast<const u8*>(&object);return {first,first+sizeof object};
}
template<class M> std::vector<u8> pool(const M& manager){
    const auto* first=reinterpret_cast<const u8*>(&manager.at(0));
    return {first,first+sizeof(manager.at(0))*M::capacity};
}
template<class M> struct Image{
    std::vector<u8> owner,payload;
    explicit Image(const M& m):owner(bytes(m)),payload(pool(m)){}
    void compare(const M& m)const{
        check(owner==bytes(m),"complete same-instance manager bytes restored; observer detached");
        check(payload==pool(m),"complete pool bytes restored, including inactive slots and padding");
    }
};
struct Graphics final:ZunGraphics{
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
struct World final:BulletWorld,ItemWorld{
    bool multiplayer=false;unsigned effects=0;
    bool multiplayer_world()const override{return multiplayer;}
    Vec3 multiplayer_target(Vec3)const override{return {0,400,0};}
    bool multiplayer_bullet_collision(const BulletState&,u8& grazed,i32& hit)override{grazed|=3;hit=0;return true;}
    bool multiplayer_item_player(ItemState&,ItemPlayer&)override{return true;}
    unsigned multiplayer_item_copies(i32 type)const override{return type==1||type==4?3:1;}
    bool multiplayer_power_full()const override{return false;}
    i32 collision(const BulletState&)override{return 0;}
    bool effect(const Vec3&,i32)override{++effects;return true;}
    bool effect(Vec3,i32)override{++effects;return true;}
    bool colored_effect(const Vec3&,i32,u32)override{++effects;return true;}
    bool sound(i32,float,bool)override{return true;}
    bool sound(i32,float)override{return true;}
    bool cancel_reward(const Vec3&)override{return true;}
    bool cancel_enemies(const Vec3&,float,bool)override{return true;}
    bool convert_enemies(const Vec3&,float,bool)override{return true;}
    bool cancel_enemy_beam(const Vec3&,float,bool)override{return true;}
    bool cancel_shot(Vec3,float)override{return true;}
    bool collect(ItemState&,bool&)override{return true;}
    bool rank_delta(i32)override{return true;}
};
struct Fixture{
    AnmResource resource;AnmEnvironment animations;World world;Graphics graphics;AnmRenderer renderer{graphics};
    std::unique_ptr<BulletManager> bullets;std::unique_ptr<ItemManager> items;
    Fixture(){
        resource.textures.resize(1);resource.textures[0].width=resource.textures[0].height=512;
        resource.sprites.resize(512);
        for(auto& s:resource.sprites){s.width=s.height=16;s.u1=s.v1=1;}
        resource.scripts.resize(256);
        // Actual interpreter: bind sprite, stop at label-less wait, terminator
        // for the real pending-interrupt scanner. No manager behavior is mocked.
        for(auto& script:resource.scripts){
            script.bytes.resize(28);const AnmInstruction bind{3,12,0,0},wait{63,8,0,0},end{-1,8,0,0};
            std::memcpy(script.bytes.data(),&bind,8);
            std::memcpy(script.bytes.data()+12,&wait,8);
            std::memcpy(script.bytes.data()+20,&end,8);
        }
        bullets=std::make_unique<BulletManager>(resource,animations,world);
        items=std::make_unique<ItemManager>(resource,animations,world);
        items->player.state=2;items->player.position={0,400,0};
    }
    BulletEmitter emitter()const{BulletEmitter e{};e.count=e.layers=1;e.speed=2;e.position={0,100,0};e.aim=1;return e;}
};
Netplay::RollbackJournal journal(std::size_t maxBytes=16*1024*1024,std::size_t maxBlocks=8192){
    Netplay::RollbackJournal out;check(out.Reset({8,maxBytes,maxBlocks}),"journal configured");return out;
}
template<class M,class F> void roundtrip(M& m,Netplay::RollbackJournal& j,u32 frame,F action){
    const Image<M> initial(m);check(j.BeginFrame(frame),"open checkpoint");
    check(m.BeginRollbackFrame(j),"capture manager metadata and active runs");
    check(m.RollbackFrameActive(),"observer attached");check(!m.BeginRollbackFrame(j),"duplicate observer rejected");
    action();m.EndRollbackFrame();check(!m.RollbackFrameActive(),"observer detached before undo");
    check(j.EndFrame(),"complete journal");u32 restored=~0u;
    check(j.UndoTo(frame,&restored)&&restored==frame,"restore exact checkpoint");
    initial.compare(m);++cases;
}
void bullet_cases(){
    Fixture f;auto& m=*f.bullets;auto j=journal();auto e=f.emitter();
    check(!m.BeginRollbackFrame(j),"closed journal rejected");
    // Produce genuine stale callbacks/timers in an inactive, previously used slot.
    check(m.spawn(e,0,0,0)==0&&m.update(),"initial real bullet");
    m.release(m.at(0));m.cursor=0;
    roundtrip(m,j,0,[&]{m.player={1,2,3};m.default_tangent={4,5,6};m.exclusion_squared=10;
        e.position={1,2,0};check(m.spawn(e,0,0,0)==-1,"rejected launch still touches inactive payload");});
    e=f.emitter();
    roundtrip(m,j,1,[&]{check(m.spawn(e,0,0,0)==0,"inactive spawn");check(m.update(),"real bullet update");
        check(m.draw_group(5,f.renderer),"semantic bullet draw");f.renderer.flush();
        check(f.graphics.draws>0,"actual renderer submitted geometry");
        m.release(m.at(0));m.cursor=0;check(m.spawn(e,0,0,0)==0,"same-slot reuse");
        check(m.change_appearance(m.at(0),1,2,true),"real appearance bind");
        check(m.update(),"reused bullet update");check(m.cancel(m.at(0)),"cancel reused bullet");});
    check(m.spawn(e,0,0,0)==0&&m.update(),"active run setup");
    roundtrip(m,j,2,[&]{m.at(0).position.x=77;check(m.cancel_circle({0,100,0},200,true,false),"active cancellation");m.reset();check(m.spawn(e,0,0,0)==0,"spawn after complete reset");});
    // Alternating occupancy forces a large active/inactive partition. Reset
    // captures only the holes, so no partial-overlap journal block is created.
    for(u32 n=0;n<m.capacity;++n){auto& b=m.at(n);std::memset(&b,0xa5,sizeof b);b.state=n%2?0:1;}
    roundtrip(m,j,3,[&]{m.reset();m.reset();});
    m.reset();for(u32 n=0;n<m.capacity;++n)m.at(n).state=1;
    roundtrip(m,j,4,[&]{const auto before=pool(static_cast<const BulletManager&>(m));check(m.spawn(e,0,0,0)==1,"full bullet pool preserves return code");check(before==pool(static_cast<const BulletManager&>(m)),"full bullet pool payload untouched");});
    m.reset();
    roundtrip(m,j,5,[&]{m.at(17).reserved_490[3]=0x8e;m.at(17).position.x=91;});
}
void item_cases(){
    Fixture f;auto& m=*f.items;auto j=journal();
    check(m.spawn(1,{0,100,0})==0,"initial real item");m.at(0).state=0;
    roundtrip(m,j,0,[&]{check(m.spawn(4,{20,90,0})==0,"ordinary inactive reuse");check(m.update(),"real item update");
        check(m.draw(f.renderer),"semantic item draw");f.renderer.flush();check(f.graphics.draws>0,"item geometry submitted");
        m.at(0).state=0;check(m.spawn_transfer(4,{0,100,0},2),"directed item transfer");
        check(m.multiplayer_targets[0]==2&&m.at(0).state==3,"transfer sidecars changed");});
    roundtrip(m,j,1,[&]{check(m.spawn(8,{1,2,0})==0,"inactive cancel item");m.cancel_cursor=0;
        const auto before=bytes(static_cast<const ItemManager&>(m).at(m.ordinary_capacity));
        check(m.spawn(8,{90,91,0})==0,"occupied cancel slot still advances cursor/counter");
        check(before==bytes(static_cast<const ItemManager&>(m).at(m.ordinary_capacity)),"occupied cancel payload unchanged");
        check(m.update(),"cancel item activation uses real binding");});
    f.world.multiplayer=true;
    roundtrip(m,j,2,[&]{check(m.spawn(1,{0,100,0})==0,"multiplayer item fan");check(m.multiplayer_fan[0]&&m.multiplayer_fan[2],"three native slots receive fan");
        check(m.convert_power_items(),"convert-and-reuse ordinary slots");m.attract_all();check(m.update(),"multiplayer item update");});
    f.world.multiplayer=false;
    for(u32 n=0;n<m.capacity;++n){auto& item=m.at(n);std::memset(&item,0x5a,sizeof item);item.state=n%2?0:1;}
    roundtrip(m,j,3,[&]{m.reset();m.reset();});
    m.reset();for(u32 n=0;n<m.ordinary_capacity;++n)m.at(n).state=1;
    roundtrip(m,j,4,[&]{check(m.spawn(1,{0,0,0})==0,"full ordinary item return preserved");check(!m.spawn_transfer(4,{0,0,0},1),"full transfer rejects");});
    m.reset();
    roundtrip(m,j,5,[&]{m.at(19).reserved_44c=12.5f;check(m.activate(m.at(19)),"explicit inactive activation");m.multiplayer_spawning_transfer=true;m.player.communication=13000;});
}
void multi_frame_and_ownership(){
    Fixture f;auto& b=*f.bullets;auto& i=*f.items;auto j=journal();const Image<BulletManager> beforeB(b);const Image<ItemManager> beforeI(i);
    for(u32 frame=0;frame<3;++frame){check(j.BeginFrame(frame),"multi-owner open");check(b.BeginRollbackFrame(j)&&i.BeginRollbackFrame(j),"shared journal distinct owners");
        check(b.spawn(f.emitter(),0,0,0)==0&&i.spawn(1,{float(frame),100,0})==0,"multiple frame production spawns");
        b.EndRollbackFrame();i.EndRollbackFrame();check(j.EndFrame(),"multi-owner seal");}
    f.animations.script_rng.seed=12345;check(j.UndoTo(0),"undo across three manager frames");beforeB.compare(b);beforeI.compare(i);
    check(f.animations.script_rng.seed==12345,"shared ANM/RNG deliberately remains external owner");++cases;
    // End observer must not leak into an unrelated still-open journal frame.
    check(j.BeginFrame(9)&&b.BeginRollbackFrame(j),"detach test open");b.EndRollbackFrame();
    const auto blocks=j.BlocksForFrame(9);b.at(99).position.x=5;check(j.BlocksForFrame(9)==blocks,"detached mutable accessor adds no journal block");
    check(j.EndFrame(),"detach test seal");++cases;
}
void failures(){
    for(bool byteLimit:{true,false}){
        Fixture f;auto& b=*f.bullets;auto j=journal();check(j.BeginFrame(0)&&b.BeginRollbackFrame(j),"measure bullet metadata");b.EndRollbackFrame();const auto metadataBytes=j.BytesForFrame(0),metadataBlocks=j.BlocksForFrame(0);
        j.Clear();check(j.Reset({4,byteLimit?metadataBytes+sizeof(BulletState)-1:16*1024*1024,byteLimit?8192:metadataBlocks}),"bounded bullet journal");
        check(j.BeginFrame(0)&&b.BeginRollbackFrame(j),"bullet metadata fits bound");
        check(b.spawn(f.emitter(),0,0,0)==0,"native spawn may complete after capture failure");b.EndRollbackFrame();
        check(j.Failed()&&!j.EndFrame(),"bullet capture capacity failure locks out commit");++cases;
    }
    for(bool byteLimit:{true,false}){
        Fixture f;auto& i=*f.items;auto j=journal();check(j.BeginFrame(0)&&i.BeginRollbackFrame(j),"measure item metadata");i.EndRollbackFrame();const auto metadataBytes=j.BytesForFrame(0),metadataBlocks=j.BlocksForFrame(0);
        j.Clear();check(j.Reset({4,byteLimit?metadataBytes+sizeof(ItemState)-1:16*1024*1024,byteLimit?8192:metadataBlocks}),"bounded item journal");
        check(j.BeginFrame(0)&&i.BeginRollbackFrame(j),"item metadata fits bound");
        check(i.spawn(8,{0,0,0})==0,"cancel native spawn may write after capture failure");i.EndRollbackFrame();
        check(j.Failed()&&!j.EndFrame(),"item capture capacity failure locks out commit");++cases;
    }
    Fixture f;auto j=journal(1,1);check(j.BeginFrame(0),"tiny metadata frame");
    check(!f.bullets->BeginRollbackFrame(j)&&j.Failed(),"metadata failure is reported and latched");
    check(f.bullets->RollbackFrameActive(),"failed Begin retains observer until explicit detach");
    f.bullets->EndRollbackFrame();check(!j.EndFrame(),"failed metadata cannot seal");++cases;
}
}
template<unsigned Count>void activity_order_cases(){
    ++cases;u32 random=917;
    for(unsigned trial=0;trial<1000;++trial){
        std::array<u32,Count> original{};
        for(auto& s:original){random=random*1664525u+1013904223u;s=(random&15)==0;}
        const auto visit=[&](bool indexed){
            auto states=original;ActiveSlotIndex<Count> index;std::vector<u32> order;
            const auto next=[&](u32 i){if(!indexed)return i;index.ensure([&](u32 n){return states[n]!=0;});return index.next(i);};
            for(u32 i=next(0);i<Count;i=next(i+1)){
                if(!states[i])continue;order.push_back(i);
                // Simulate reentrant callbacks that create/delete arbitrary
                // later and earlier slots, including a newly activated bit
                // in the currently traversed machine word.
                const auto a=(i+7)%Count,b=(i*37+11)%Count;
                states[a]=1;index.set(a,true);states[b]=0;index.set(b,false);
                if(i%5==0){states[(i+1)%Count]=1;index.invalidate();}
            }
            return std::make_pair(order,states);
        };
        const auto scan=visit(false),indexed=visit(true);
        check(scan==indexed,"active-slot iteration matches physical order under reentrant births/deaths and external writes");
    }
    std::array<u32,Count> states{};ActiveSlotIndex<Count> index;
    auto before=bytes(index);Netplay::RollbackJournal journal;check(journal.Reset({3,65536,16,true,true}),"index journal initializes");
    check(journal.BeginFrame(0)&&journal.BeginReadOnlyCapture(),"index read-only frame opens");
    check(journal.Touch(&index,sizeof index)&&journal.Touch(states.data(),sizeof states),"plain cache and complete slot states captured");
    check(journal.EndReadOnlyCapture(),"index copies complete before mutation");
    states[Count-1]=1;index.ensure([&](u32 n){return states[n]!=0;});
    check(index.next(0)==Count-1&&index.next(Count)==Count,"last partial word and terminal index are bounded");
    check(journal.EndFrame()&&journal.UndoTo(0),"index and payload rewind atomically");
    check(bytes(index)==before&&states[Count-1]==0,"even invalid pre-call index bytes restore exactly");
    index.ensure([&](u32 n){return states[n]!=0;});check(index.next(0)==Count,"empty restored pool visits no ghost slots");
}
void retained_slot_aliases(){
    ++cases;Fixture f;auto& b=*f.bullets;
    check(b.spawn(f.emitter(),0,0,0)==0&&b.update(),"alias bullet starts");
    auto& slot=b.at(0);const auto live=slot;b.release(slot);check(b.update()&&b.active_count==0,"alias bullet released");
    slot=live;check(b.update()&&b.active_count==1,"retained reference can reactivate a released slot without another at call");
    auto j=journal();check(j.BeginFrame(0)&&b.BeginRollbackFrame(j),"alias checkpoint");
    auto& later=b.at(91);const auto before=later;later=live;
    b.EndRollbackFrame();check(j.EndFrame()&&j.UndoTo(0),"alias introduced after checkpoint rewinds");
    check(bytes(later)==bytes(before),"aliased slot payload rewinds");
    later=live;check(b.update()&&b.active_count==2,"post-Undo reference remains observed even though it escaped in discarded history");
    auto& items=*f.items;check(items.spawn(1,{0,100,0})==0,"alias item starts");
    auto& item=items.at(0);const auto value=item;item.state=0;check(items.update()&&items.active_count==0,"alias item removal");
    item=value;check(items.update()&&items.active_count==1,"alias item reactivation");
}
int main(){bullet_cases();item_cases();multi_frame_and_ownership();failures();retained_slot_aliases();
    activity_order_cases<1>();activity_order_cases<31>();activity_order_cases<32>();activity_order_cases<33>();activity_order_cases<65>();
    std::printf("PASS: TH11 real BulletManager/ItemManager rollback component: %u cases, %u checks\n",cases,checks);
    std::puts("Scope: manager/pool byte restoration, real interpreter/draw, allocation/reset paths; no Application, shared RNG, audio, network, whole-world rollback or performance claim.");
}
