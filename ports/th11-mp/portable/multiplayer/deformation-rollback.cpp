// Real screen deformation / ANM / renderer / journal component oracle.
// Vector values are typed snapshots; neither STL nor polymorphic headers are copied.
#include "../../th11_web/cpp/game/ScreenDeformation.hpp"
#include "../../th11_web/cpp/game/ScreenDeformationPool.hpp"
#include "../../th11_web/cpp/game/AnmRenderer.hpp"
#ifdef TH11_MULTIPLAYER
#include <eagler/netplay/RollbackJournal.hpp>
#endif
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <type_traits>
#include <vector>
namespace {
using namespace th11;
unsigned checks=0,groups=0;
void check(bool ok,const char* why){++checks;if(!ok){std::fprintf(stderr,"FAIL deformation group %u check %u: %s\n",groups,checks,why);std::exit(1);}}
void group(const char* name){++groups;std::printf("GROUP %u %s\n",groups,name);}
template<class T> void append(std::vector<u8>& out,const T& value){
    static_assert(std::is_trivially_copyable<T>::value,"oracle scalar");
    const auto* p=reinterpret_cast<const u8*>(&value);out.insert(out.end(),p,p+sizeof value);
}
void bytes(std::vector<u8>& out,const void* value,std::size_t count){
    if(!count)return;const auto* p=static_cast<const u8*>(value);out.insert(out.end(),p,p+count);
}
template<class T> void vector_bytes(std::vector<u8>& out,const std::vector<T>& value){
    append(out,value.size());bytes(out,value.data(),value.size()*sizeof(T));
}
void payload(std::vector<u8>& out,const ScreenDeformation& value){
    append(out,value.columns);append(out,value.rows);vector_bytes(out,value.vertices);
    vector_bytes(out,value.positions);vector_bytes(out,value.animations);
}
void op(AnmScript& script,i16 code,std::initializer_list<u32> args={},u16 references=0){
    const AnmInstruction instruction{code,u16(8+4*args.size()),0,references};
    append(script.bytes,instruction);for(auto arg:args)append(script.bytes,arg);
}
AnmResource authored_resource(bool partial_failure=false){
    AnmResource resource;resource.textures.resize(1);resource.textures[0].width=640;resource.textures[0].height=480;
    resource.sprites.resize(1);resource.sprites[0].width=640;resource.sprites[0].height=480;resource.sprites[0].u1=resource.sprites[0].v1=1;
    resource.scripts.resize(83);
    for(unsigned index:{81u,82u}){
        auto& script=resource.scripts[index];
        if(partial_failure&&index==81){op(script,87,{0});op(script,40,{10000,2},1);op(script,3,{10000},1);}
        else op(script,3,{0});
        op(script,63);op(script,-1);
    }
    return resource;
}
struct Graphics final:ZunGraphics {
    touhou::graphics::PipelineState state{};u32 draws=0,triangles_seen=0;
    touhou::graphics::PipelineState& pipeline()override{return state;}
    u32 texture(const AnmResource&,u32 index)override{return index+1;}
    void bind_texture(u32)override{}
    void set_layout(touhou::graphics::VertexLayout)override{}
    void set_matrix(touhou::graphics::MatrixKind,const Matrix4&)override{}
    void triangles(u32 count,const void*,u32)override{++draws;triangles_seen+=count;}
    void primitives(touhou::graphics::Topology,u32 count,const void*,u32)override{++draws;triangles_seen+=count;}
    bool select_target(const AnmResource*,u32)override{return true;}
    bool clear_target(u32,const GraphicsViewport*)override{return true;}
};
struct Fixture {
    AnmResource resource=authored_resource();AnmManager manager;Graphics graphics;AnmRenderer renderer{graphics};
    EnemyState enemy{};
#ifdef TH11_MULTIPLAYER
    ScreenDeformationPool pool{manager};
#endif
    Fixture(){enemy.current.position={8,120,0};enemy.deformation_radius=48;enemy.deformation_target=96;
        enemy.deformation_color=0xabcdef;enemy.deformation_phase_x=.25f;enemy.deformation_phase_y=.75f;}
    void tick(){check(manager.update(false)&&manager.update(true),"real ANM registries update");}
    void draw(){check(renderer.draw_layer(manager.layer_first(27))!=-2,"first-target real layer draw");
        check(renderer.draw_layer(manager.layer_first(28))!=-2,"second-target real layer draw");renderer.flush();}
};
void strips(ScreenDeformation& value,i32 script,u32 layer){
    check(value.vertices.size()==value.columns*value.rows&&value.positions.size()==value.vertices.size(),"complete typed grids");
    check(value.animations.size()==value.columns-1,"one strip per adjacent column");
    for(u32 x=0;x<value.columns-1;++x){
        auto* vm=value.manager.find(value.animations[x]);check(vm!=nullptr,"strip VM exists");
        check(vm->script_index==script&&vm->layer==layer,"real target script and layer");
        check(vm->draw_callback==ScreenDeformation::draw_callback&&vm->reserved_418==u32(reinterpret_cast<uintptr_t>(&value)),"callback owns stable deformation address");
        check(vm->integers[0]==i32(value.rows)&&vm->geometry,"strip geometry and row count");
        const auto* data=static_cast<const AnmVertex*>(vm->geometry);
        for(u32 y=0;y<value.rows;++y){
            check(std::memcmp(&data[2*y],&value.vertices[x*value.rows+y],sizeof(AnmVertex))==0,"strip left column exact");
            check(std::memcmp(&data[2*y+1],&value.vertices[(x+1)*value.rows+y],sizeof(AnmVertex))==0,"strip right column exact");
        }
    }
}
void ordinary(){
    group("ordinary scripts 81/82, geometry, draw callback, update and destructor");
    Fixture f;std::vector<u32> ids;
    {ScreenDeformation first(f.manager),second(f.manager);
        check(!first.initialize(f.resource,1,3),"invalid width rejected");
        check(first.initialize(f.resource,4,5,false),"first target initialized");
        check(second.initialize(f.resource,3,4,true),"second target initialized");
        check(!first.initialize(f.resource,4,5,false),"double initialization rejected");
        first.rectangle(-90,10,180,200);second.rectangle(-40,30,80,100);
        strips(first,81,27);strips(second,82,28);f.tick();
        first.update(f.enemy,0xad);first.vertices[0].position.x+=.5f;f.draw();
        strips(first,81,27);strips(second,82,28);
        check(f.graphics.draws==5&&f.graphics.triangles_seen>0,"real renderer submitted every strip");
        ids=first.animations;ids.insert(ids.end(),second.animations.begin(),second.animations.end());
    }
    for(auto id:ids)check(f.manager.find(id)&&(f.manager.find(id)->flags&0x4000000),"ordinary destructor marks semantic erase");
    f.tick();check(f.manager.active_count()==0,"ordinary strips retire on update");
}
}
#ifdef TH11_MULTIPLAYER
namespace th11 {
struct AnmRollbackTestAccess {
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
struct DeformationRollbackTestAccess {
    static std::vector<u8> image(const ScreenDeformationPool& pool){
        std::vector<u8> out;append(out,pool.logical_size_);
        for(std::size_t i=0;i<pool.logical_size_;++i){const auto& slot=pool.storage_[i];
            auto* address=slot.object.get();append(out,address);append(out,slot.occupied);payload(out,*address);}
        return out;
    }
    static std::size_t logical(const ScreenDeformationPool& pool){return pool.logical_size_;}
    static std::size_t physical(const ScreenDeformationPool& pool){return pool.storage_.size();}
    static ScreenDeformation* at(ScreenDeformationPool& pool,std::size_t i){return pool.storage_.at(i).object.get();}
    static bool occupied(const ScreenDeformationPool& pool,std::size_t i){return pool.storage_.at(i).occupied;}
    static std::size_t saved(const DeformationRollbackFrame& frame){return frame.slots_.size();}
};
}
namespace {
using PoolAccess=th11::DeformationRollbackTestAccess;using AnmAccess=th11::AnmRollbackTestAccess;
struct Checkpoint {AnmRollbackFrame anm;DeformationRollbackFrame deformation;};
struct Image {std::vector<u8> pool,anm,enemy;};
Image image(Fixture& f){Image out;out.pool=PoolAccess::image(f.pool);out.anm=AnmAccess::image(f.manager);append(out.enemy,f.enemy);return out;}
void same(Fixture& f,const Image& expected,const char* why){
    const auto actual=image(f);check(actual.pool==expected.pool,why);check(actual.anm==expected.anm,"all ANM metadata, full VM and geometry bytes restored");
    check(actual.enemy==expected.enemy,"external EnemyState remains an independent complete byte owner");AnmAccess::topology(f.manager);
}
Netplay::RollbackJournalConfig config(){Netplay::RollbackJournalConfig c;c.maxFrames=8;c.maxBytesPerFrame=1024*1024;c.maxBlocksPerFrame=2048;return c;}
void setup(Netplay::RollbackJournal& journal){check(journal.Reset(config()),"journal configured");}
std::unique_ptr<Checkpoint> begin(Fixture& f,Netplay::RollbackJournal& journal,u32 frame){
    auto saved=std::make_unique<Checkpoint>();check(journal.BeginFrame(frame),"journal checkpoint starts");
    check(f.manager.BeginRollbackFrame(journal,saved->anm),"ANM observer begins");
    check(f.pool.BeginRollbackFrame(journal,saved->deformation),"typed pool observer begins");
    check(journal.Touch(&f.enemy,sizeof f.enemy),"owning fixture captures external EnemyState");
    check(saved->deformation.valid()&&saved->deformation.frame()==frame&&f.pool.RollbackFrameActive(),"pool checkpoint identity");
    return saved;
}
void end(Fixture& f,Netplay::RollbackJournal& journal){
    f.pool.EndRollbackFrame();f.manager.EndRollbackFrame();
    check(!f.pool.RollbackFrameActive()&&!f.manager.RollbackFrameActive(),"all observers ended");
    check(journal.EndFrame(),"healthy frame committed");
}
void undo(Fixture& f,Netplay::RollbackJournal& journal,const Checkpoint& saved){
    f.pool.EndRollbackFrame();f.manager.EndRollbackFrame();u32 restored=~0u;
    check(journal.UndoTo(saved.deformation.frame(),&restored),"all byte owners undo before typed containers");
    check(restored==saved.deformation.frame(),"exact restored frontier");
    check(f.manager.RestoreRollbackFrame(saved.anm,restored),"ANM typed metadata restored with leases retained");
    check(f.pool.RestoreRollbackFrame(saved.deformation,restored),"pool typed metadata restored before checkpoint retirement");
}
ScreenDeformation* create(Fixture& f,u32 width=4,u32 height=5,bool second=false){
    auto* value=f.pool.create(f.resource,width,height,second);check(value!=nullptr,"real pool create");
    value->rectangle(-48,24,96,128);return value;
}
void payload_history(){
    group("multiple checkpoints restore typed vectors, full ANM bytes and original geometry pointers");
    Fixture f;Netplay::RollbackJournal journal;setup(journal);
    auto* first=create(f);auto* second=create(f,3,4,true);f.tick();f.draw();
    const auto initial=image(f);auto a=begin(f,journal,10);
    first->update(f.enemy,0xad);second->update(f.enemy,0x91);f.tick();f.draw();end(f,journal);
    const auto middle=image(f);auto b=begin(f,journal,11);
    first->vertices[3].color=0x76543210;first->positions[7].x+=10;second->rectangle(5,6,7,8);
    first->update(f.enemy,0x91);f.draw();end(f,journal);
    undo(f,journal,*b);same(f,middle,"newer typed vector values exact");b.reset();
    undo(f,journal,*a);same(f,initial,"older typed vector values exact");a.reset();
    f.draw();strips(*first,81,27);strips(*second,82,28);
}
void release_reuse(){
    group("retired callbacks prevent premature reuse; later reuse is reversible");
    Fixture f;Netplay::RollbackJournal journal;setup(journal);auto* first=create(f);f.tick();f.draw();
    const auto initial=image(f);auto a=begin(f,journal,20);const auto old_ids=first->animations;
    check(f.pool.release(first),"release occupied slot");check(!f.pool.release(first),"double release refused");
    for(auto id:old_ids)check(f.manager.find(id)!=nullptr,"strip still alive immediately after semantic release");
    auto* second=create(f,3,6,true);check(second!=first,"live old strips forbid physical slot reuse");
    check(PoolAccess::logical(f.pool)==2&&!PoolAccess::occupied(f.pool,0),"retired payload remains logical owner");
    f.tick();for(auto id:old_ids)check(!f.manager.find(id),"old strips actually retired");
    const auto retired=image(f);end(f,journal);auto b=begin(f,journal,21);
    auto* reused=create(f,6,7,true);check(reused==first,"retired slot reused at same callback address");
    check(reused->vertices.size()==42,"different grid replaces former typed payload");f.tick();f.draw();end(f,journal);
    undo(f,journal,*b);same(f,retired,"inactive old vector contents restored, not only occupied");b.reset();
    undo(f,journal,*a);same(f,initial,"original active deformation and original geometry restored");a.reset();
    check(f.manager.find(old_ids[0])->reserved_418==u32(reinterpret_cast<uintptr_t>(first)),"original callback backreference restored");
}
void future_slots(){
    group("future physical slots remain stable across undo to empty logical frontier");
    Fixture f;Netplay::RollbackJournal journal;setup(journal);const auto empty=image(f);auto a=begin(f,journal,0);
    auto* first=create(f,4,5);auto* second=create(f,3,6,true);
    const auto id1=first->animations[0],id2=second->animations[0];
    auto* vm1=f.manager.find(id1);auto* vm2=f.manager.find(id2);f.tick();f.draw();end(f,journal);
    undo(f,journal,*a);same(f,empty,"empty pool and ANM frontier exact");a.reset();
    check(PoolAccess::logical(f.pool)==0&&PoolAccess::physical(f.pool)==2,"physical reserve survives logical rewind");
    auto b=begin(f,journal,0);auto* replay1=create(f,4,5);auto* replay2=create(f,3,6,true);f.tick();f.draw();end(f,journal);
    check(replay1==first&&replay2==second,"deformation replay uses exact original addresses");
    check(f.manager.find(id1)==vm1&&f.manager.find(id2)==vm2,"ANM replay uses exact original VM slots and IDs");
    strips(*replay1,81,27);strips(*replay2,82,28);undo(f,journal,*b);same(f,empty,"second empty rewind exact");
}
u16 partial_seed(){
    for(u32 seed=0;seed<65536;++seed){Rng rng;rng.seed=u16(seed);if(rng.next32()%2==0&&rng.next32()%2==1)return u16(seed);}
    check(false,"authored RNG fixture has success then failure seed");return 0;
}
void partial_initialization(){
    group("real interpreter fails after first strip; retained partial payload and callbacks restore");
    Fixture f;auto failing=authored_resource(true);Netplay::RollbackJournal journal;setup(journal);
    f.manager.script_rng.seed=partial_seed();const auto empty=image(f);auto a=begin(f,journal,30);
    check(f.pool.create(failing,4,5,false)==nullptr,"second real script bind failure rejects creation");
    check(PoolAccess::logical(f.pool)==1&&!PoolAccess::occupied(f.pool,0),"partially initialized slot retained but released");
    auto* retired=PoolAccess::at(f.pool,0);check(retired->animations[0]&&retired->animations[1]==0,"first strip survives failed second strip");
    check(f.manager.active_count()==1&&f.manager.last_error==3,"actual interpreter failure and active first strip");
    auto* vm=f.manager.find(retired->animations[0]);check(vm&&vm->draw_callback==ScreenDeformation::draw_callback,"partial callback has stable payload");
    retired->rectangle(4,8,20,40);vm->draw_callback(*vm);
    auto* other=create(f,3,4,true);check(other!=retired,"partial live strip prevents reuse");end(f,journal);
    const auto partial=image(f);auto b=begin(f,journal,31);f.tick();auto* reused=create(f,5,6);
    check(reused==retired,"partial retired slot can be reused only after ANM erase");f.tick();f.draw();end(f,journal);
    undo(f,journal,*b);same(f,partial,"partial inactive snapshot retained every vector and geometry");b.reset();
    undo(f,journal,*a);same(f,empty,"partial failure frame undoes RNG, ANM and pool cursor");a.reset();
}
void admission(){
    group("owner, frame, observer, consumed snapshot, expired owner and invalid dimension contracts");
    Fixture f,foreign;Netplay::RollbackJournal journal;setup(journal);DeformationRollbackFrame unopened;
    check(!f.pool.BeginRollbackFrame(journal,unopened)&&!unopened.valid(),"closed journal refuses Begin");
    check(f.pool.release(nullptr),"null release accepted");ScreenDeformation external(f.manager);
    check(!f.pool.release(&external),"foreign pointer rejected");
    check(f.pool.create(f.resource,1,5)==nullptr&&f.pool.create(f.resource,4,2)==nullptr&&f.pool.create(f.resource,129,5)==nullptr,"invalid dimensions retain original rejection");
    auto* value=create(f);f.tick();const auto baseline=image(f);auto a=begin(f,journal,40);DeformationRollbackFrame duplicate;
    check(!f.pool.BeginRollbackFrame(journal,duplicate)&&!duplicate.valid(),"duplicate observer refused");
    check(!f.pool.RestoreRollbackFrame(a->deformation,40),"active observer refuses restore");
    value->rectangle(2,3,4,5);end(f,journal);
    check(!foreign.pool.RestoreRollbackFrame(a->deformation,40),"wrong owner rejected");
    check(!f.pool.RestoreRollbackFrame(a->deformation,41),"wrong frame rejected");
    undo(f,journal,*a);same(f,baseline,"admission errors did not damage legitimate snapshot");
    check(journal.BeginFrame(40),"new journal frame after undo");
    check(!f.pool.BeginRollbackFrame(journal,a->deformation),"consumed typed frame not silently overwritten");check(journal.EndFrame(),"empty healthy frame ends");journal.Clear();
    Netplay::RollbackJournal separate;setup(separate);DeformationRollbackFrame lifetime;
    {ScreenDeformationPool temporary(f.manager);check(separate.BeginFrame(1),"lifetime checkpoint begins");
        check(temporary.BeginRollbackFrame(separate,lifetime),"temporary owner captures empty frame");
        temporary.EndRollbackFrame();check(separate.EndFrame(),"lifetime checkpoint ends");separate.Clear();check(lifetime.valid(),"frame valid before owner exit");}
    check(!lifetime.valid()&&!f.pool.RestoreRollbackFrame(lifetime,1),"expired owner rejects typed restore");
}
void failure_and_leases(){
    group("failed journal blocks transaction; future snapshot leases survive complete undo");
    Fixture f;auto* value=create(f);f.tick();Netplay::RollbackJournal journal;setup(journal);
    const auto initial=image(f);auto* vm=f.manager.find(value->animations[0]);void* original=vm->geometry;auto original_lease=AnmAccess::lease(f.manager,vm);
    auto a=begin(f,journal,50);value->vertices[0].position.x+=1;
    check(f.manager.allocate_geometry(*vm,value->rows*2*sizeof(AnmVertex)),"real geometry replaced in first future frame");
    auto future_lease=AnmAccess::lease(f.manager,vm);check(vm->geometry!=original,"future generation has its own allocation");
    f.draw();end(f,journal);
    auto b=begin(f,journal,51);check(f.pool.release(value),"future release");f.tick();end(f,journal);
    check(!original_lease.expired()&&!future_lease.expired(),"both generations retained after semantic deletion");
    // Do not reset b here: common UndoTo may write to geometry owned only by a
    // newer snapshot. This deliberately tests the safe ordering without UAF.
    undo(f,journal,*a);same(f,initial,"whole history undone while every future lease remains alive");
    check(f.manager.find(value->animations[0])->geometry==original,"original strip allocation restored");
    check(!future_lease.expired(),"newer snapshot stays alive until all byte undo and owner restore complete");
    b.reset();check(future_lease.expired(),"future-only allocation retires after its final lease");
    a.reset();check(!original_lease.expired(),"restored current owner keeps geometry after snapshots retire");
    check(f.pool.release(value),"final release");f.tick();check(original_lease.expired(),"geometry freed after last snapshot and active owner exit");

    Netplay::RollbackJournal failed;auto limits=config();limits.maxBytesPerFrame=1;check(failed.Reset(limits),"small journal configured");
    check(failed.BeginFrame(60),"small journal opens");DeformationRollbackFrame typed;
    check(f.pool.BeginRollbackFrame(failed,typed),"typed-only owner can capture before byte budget failure");
    u32 outside=0;check(!failed.Touch(&outside,sizeof outside)&&failed.Failed(),"separate owner latches shared transaction failure");
    f.pool.EndRollbackFrame();check(!failed.EndFrame(),"failed shared transaction cannot commit");check(!failed.UndoTo(60),"failed shared transaction cannot undo partial bytes");
    DeformationRollbackFrame rejected;check(!f.pool.BeginRollbackFrame(failed,rejected)&&!rejected.valid(),"failed journal refuses new pool observer");failed.Clear();
}
}
#endif
int main(){
    static_assert(sizeof(void*)==4,"component is WASI32, matching game pointer fields");
    ordinary();
#ifdef TH11_MULTIPLAYER
    payload_history();release_reuse();future_slots();partial_initialization();admission();failure_and_leases();
    std::printf("PASS deformation rollback multiplayer: %u groups, %u checks\n",groups,checks);
#else
    std::printf("PASS deformation ordinary: %u groups, %u checks\n",groups,checks);
#endif
    return 0;
}
