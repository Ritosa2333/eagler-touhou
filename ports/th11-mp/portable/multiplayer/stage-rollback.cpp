// Real TH11 Stage/STD/ANM/deformation/semantic Draw component oracle.
// No retail assets, browser, native world, GPU pixel or performance claim.
#include "../../th11_web/cpp/game/Stage.hpp"
#include "../../th11_web/cpp/game/AnmRenderer.hpp"
#include "../../th11_web/cpp/game/ScreenFade.hpp"
#ifdef TH11_MULTIPLAYER
#include <eagler/netplay/RollbackJournal.hpp>
#endif
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <type_traits>
#include <vector>

namespace {
using namespace th11;
unsigned checks=0,groups=0;
void check(bool ok,const char* message){
    ++checks;if(!ok){std::fprintf(stderr,"FAIL Stage group %u check %u: %s\n",groups,checks,message);std::exit(1);}
}
void group(const char* name){++groups;std::printf("GROUP %u %s\n",groups,name);}
void bytes(std::vector<u8>& out,const void* value,std::size_t size){
    if(!size)return;const auto* p=static_cast<const u8*>(value);out.insert(out.end(),p,p+size);
}
template<class T>void append(std::vector<u8>& out,const T& value){
    static_assert(std::is_trivially_copyable<T>::value,"oracle value");bytes(out,&value,sizeof value);
}
u32 word(float v){u32 result;std::memcpy(&result,&v,4);return result;}
void op(AnmScript& s,i16 code,std::initializer_list<u32> args={},i16 time=0){
    const AnmInstruction head{code,u16(8+args.size()*4),time,0};append(s.bytes,head);
    for(auto arg:args)append(s.bytes,arg);
}
AnmResource animation_file(){
    AnmResource file;file.textures.resize(1);file.sprites.push_back({0,0,0,0,32,32,0,0,1,1});file.scripts.resize(83);
    for(auto& script:file.scripts){op(script,3,{0});op(script,53,{word(0),word(0),word(.03125f)});op(script,63);op(script,-1);}
    file.scripts[1].bytes.clear();op(file.scripts[1],3,{0});op(file.scripts[1],2,{},1);op(file.scripts[1],-1);
    return file;
}
StageResource stage_file(){
    StageResource file;file.animation_count=2;
    for(u32 i=0;i<2;++i){
        StageObject object;object.id=i16(i);object.layer=u8(i?8:0);object.position={0,300,0};object.size={128,128,128};
        object.primitives.push_back({0,i16(i),i,{0,300,0},{48,48}});file.objects.push_back(object);
        file.instances.push_back({i16(i),0,{0,0,0}});
    }
    file.instances.push_back({0,1,{1000000,0,0}});
    file.instructions.push_back({0,13,12,0,{0xff123456}});
    file.instructions.push_back({0,12,12,12,{5}});
    file.instructions.push_back({1000000,0,8,24,{}});
    return file;
}
struct Device final:ZunGraphics{
    touhou::graphics::PipelineState state;
    std::vector<u8> output;
    unsigned clears=0,submissions=0;
    touhou::graphics::PipelineState& pipeline()override{return state;}
    u32 texture(const AnmResource&,u32 index)override{return index+1;}
    void bind_texture(u32 handle)override{append(output,handle);}
    void set_layout(touhou::graphics::VertexLayout layout)override{append(output,layout);}
    void set_matrix(touhou::graphics::MatrixKind,const Matrix4&)override{}
    void triangles(u32 count,const void* vertices,u32 stride)override{
        ++submissions;append(output,count);append(output,stride);bytes(output,vertices,std::size_t(count)*3*stride);
    }
    void primitives(touhou::graphics::Topology kind,u32 count,const void* vertices,u32 stride)override{
        ++submissions;append(output,kind);append(output,count);append(output,stride);
        bytes(output,vertices,std::size_t(touhou::graphics::vertex_count(kind,count))*stride);
    }
    bool select_target(const AnmResource*,u32)override{return true;}
    bool clear_target(u32 color,const GraphicsViewport*)override{++clears;append(output,color);return true;}
};
struct Fixture{
    AnmResource file=animation_file();
    AnmManager manager;
    u32 color=0xffabcdef;
    SceneCamera camera;
    Stage stage;
    ScreenFades fades;
    Device device;
    AnmRenderer renderer;
    explicit Fixture(bool start=true):stage(manager,file,file,color,3),renderer(device){
        stage.active_camera=&camera;
        check(stage.initialize(stage_file(),1,camera,start),"real Stage initialize");
        for(auto& vm:stage.object_animations)vm.flags=(vm.flags&~0x3c00000u)|(5u<<22);
        camera=stage.state.camera;device.initialize_game_pipeline();
    }
    void tick(){
        check(stage.update(),"real STD/VM Stage update");
        check(manager.update(true)&&manager.update(false),"real ANM registry update");
    }
    void draw(){
        check(stage.draw_background(renderer,camera,fades),"real background Draw");
        check(stage.draw_foreground(renderer,camera),"real foreground Draw");
        renderer.flush();
    }
};
std::vector<u8> image(const Fixture& f,bool normalize_new_geometry=false){
    const auto& s=f.stage;std::vector<u8> out;
    append(out,s.state);append(out,s.object_animations.size());
    for(const auto& source:s.object_animations){auto vm=source;if(normalize_new_geometry)vm.geometry=nullptr;append(out,vm);}
    append(out,s.resource.objects.size());for(const auto& object:s.resource.objects)append(out,object.flags);
    append(out,s.resource.instances.size());for(const auto& instance:s.resource.instances)append(out,instance.flags);
    append(out,s.error.size());bytes(out,s.error.data(),s.error.size());
#ifdef TH11_MULTIPLAYER
    append(out,s.mesh);append(out,s.multiplayer_frozen_draw);append(out,s.multiplayer_tint_owner);append(out,s.multiplayer_tint_source);
#else
    append(out,s.mesh.get());
#endif
    if(s.mesh){
        append(out,s.mesh->columns);append(out,s.mesh->rows);
        append(out,s.mesh->vertices.size());for(const auto& v:s.mesh->vertices)append(out,v);
        append(out,s.mesh->positions.size());for(const auto& p:s.mesh->positions)append(out,p);
        append(out,s.mesh->animations.size());for(auto id:s.mesh->animations)append(out,id);
    }
    append(out,f.color);append(out,f.camera);append(out,f.manager.rate);
    append(out,f.manager.script_rng);append(out,f.manager.visual_rng);
    append(out,f.fades.effects.size());for(const auto& fade:f.fades.effects)append(out,fade);
    return out;
}
void same(const Fixture& f,const std::vector<u8>& before,const char* label){check(image(f)==before,label);}
void ordinary(){
    group("ordinary STD, visible/culled objects, authored fade and deformation");
    Fixture f;check(f.stage.object_animations.size()==2,"fixed object VM count");
    f.tick();check(f.color==0xff123456,"STD color writes external owner");
    check(!(f.stage.resource.objects[1].flags&1),"ended object script clears mutable resource active flag");
    f.draw();check(f.stage.state.drawn_objects>0&&f.stage.state.culled_objects>0,"real visibility uses near and far instances");
    check(!(f.stage.resource.instances[2].flags&1),"culled instance flag changed by Draw");
    check(f.device.clears>0&&f.device.submissions>0,"real renderer reached graphics boundary");
    f.stage.state.draw_flags=3;f.stage.state.fade_timer.set(1,&f.manager.rate);f.draw();
    check((f.stage.state.draw_flags&8)&&f.stage.state.fade_timer.current==0,"last authored foreground fade hides outgoing stage");
    check(f.stage.stage_deformation(1),"real deformation initialize");
    check(f.stage.mesh&&f.stage.mesh->rows==25,"mode one native dimensions");
    check(f.stage.stage_deformation(2),"real deformation replacement");
    check(f.manager.update(false),"logical strip erase consumed");
    check(f.stage.mesh&&f.stage.mesh->rows==17,"mode two native dimensions");
}
}
#ifdef TH11_MULTIPLAYER
namespace {
Netplay::RollbackJournalConfig config(){
    Netplay::RollbackJournalConfig c;c.maxFrames=8;c.maxBytesPerFrame=4*1024*1024;c.maxBlocksPerFrame=4096;return c;
}
struct Frame{
    AnmRollbackFrame anm;
    StageRollbackFrame stage;
    ScreenFades fades;
};
std::unique_ptr<Frame> begin(Fixture& f,Netplay::RollbackJournal& j,u32 number){
    auto frame=std::make_unique<Frame>();frame->fades=f.fades;
    check(j.BeginFrame(number),"open fresh shared checkpoint");
    check(f.manager.BeginRollbackFrame(j,frame->anm),"ANM owner captures geometry and independent VMs");
    check(f.stage.BeginRollbackFrame(j,frame->stage),"Stage typed/byte owner begins");
    check(j.Touch(&f.color,sizeof f.color)&&j.Touch(&f.camera,sizeof f.camera),"external scene owners captured separately");
    check(!j.Failed(),"Stage embedded VMs do not overlap ANM body ownership");
    return frame;
}
void end(Fixture& f,Netplay::RollbackJournal& j){
    f.stage.EndRollbackFrame();f.manager.EndRollbackFrame();
    check(!f.stage.RollbackFrameActive()&&!f.stage.deformation_pool.RollbackFrameActive(),"both Stage observers detached");
    check(j.EndFrame(),"complete full Update/Draw journal transaction");
}
void undo(Fixture& f,Netplay::RollbackJournal& j,const Frame& frame){
    check(f.stage.ValidateRollbackFrame(frame.stage,frame.stage.frame()),"Stage structure preflight BEFORE byte undo");
    u32 restored=~0u;check(j.UndoTo(frame.stage.frame(),&restored)&&restored==frame.stage.frame(),"whole shared journal undo");
    check(f.manager.RestoreRollbackFrame(frame.anm,restored),"ANM typed metadata restored after all bytes");
    check(f.stage.RestoreRollbackFrame(frame.stage,restored),"Stage flags/error/deformation typed values restored");
    f.fades=frame.fades;f.renderer.invalidate();
}
void semantic_roundtrip(){
    group("full state, embedded geometry and authored Draw exact rewind/replay");
    Fixture f;Netplay::RollbackJournal j;check(j.Reset(config()),"journal config");
    auto& vm=f.stage.object_animations[0];
    check(f.manager.allocate_geometry(vm,64),"external Stage VM geometry allocated");std::memset(vm.geometry,0x4b,64);
    void* original_geometry=vm.geometry;
    f.stage.state.effects_enabled=1;f.stage.state.draw_flags=3;f.stage.state.fade_timer.set(1,&f.manager.rate);
    const auto before=image(f);
    auto action=[&](){
        f.tick();check(f.stage.stage_animation(f.stage.state.script_animations[0],0),"real embedded Stage VM rebinding");
        check(f.manager.allocate_geometry(vm,128),"replace Stage embedded geometry");std::memset(vm.geometry,0x72,128);
        f.stage.error="typed speculative error value";f.stage.multiplayer_tint_owner=2;f.stage.multiplayer_tint_source=0xff909090;
        f.renderer.invalidate();f.device.output.clear();f.draw();
    };
    auto frame=begin(f,j,10);action();end(f,j);
    const auto after=image(f),normalized_after=image(f,true),output=f.device.output;
    check(after!=before,"authored transaction changes state");
    check((f.stage.state.draw_flags&8)&&f.stage.state.effects_enabled==2,"semantic Draw writes fade and effects count");
    undo(f,j,*frame);same(f,before,"all Stage owner bytes, flags, external scene values and strings restored");
    check(vm.geometry==original_geometry,"original geometry address restored");
    const auto* p=static_cast<const u8*>(vm.geometry);bool intact=true;for(u32 i=0;i<64;++i)intact&=p[i]==0x4b;
    check(intact,"original external geometry bytes retained");frame.reset();
    auto replay=begin(f,j,10);action();end(f,j);
    // Replaced allocations need not have identical heap addresses. Everything
    // except the new geometry pointer is compared at the same semantic endpoint.
    check(image(f,true)==normalized_after,"resimulation reproduces all Stage endpoint values apart from fresh heap identity");
    const auto* replacement=static_cast<const u8*>(vm.geometry);bool rewritten=true;
    for(u32 i=0;i<128;++i)rewritten&=replacement[i]==0x72;
    check(rewritten,"replayed fresh geometry contents reproduce");
    undo(f,j,*replay);same(f,before,"second exact rewind");
    check(f.device.output==output,"real authored draw submissions reproduce");
}
void deformation_history(){
    group("direct two-frame rewind preserves retired callback owners and same-slot replay");
    Fixture f;Netplay::RollbackJournal j;check(j.Reset(config()),"journal config");
    check(f.stage.stage_deformation(2),"initial Stage distortion");
    f.stage.state.deformation_mode=2;f.stage.state.deformation_radius=192;f.stage.state.deformation_target=112;
    f.tick();auto* original=f.stage.mesh;const auto first_ids=original->animations;const auto before=image(f);
    auto frame20=begin(f,j,20);
    check(f.stage.stage_deformation(1),"replace Stage distortion before old strip deletion");
    auto* second=f.stage.mesh;check(second!=original,"callback-live old payload not reused");
    auto* old_vm=f.manager.find(first_ids[0]);check(old_vm&&old_vm->draw_callback,"retired strip callback still exists this tick");
    old_vm->draw_callback(*old_vm); // Real callback must still read the old owner.
    f.stage.state.deformation_mode=1;f.tick();f.draw();end(f,j);
    const auto middle=image(f);
    auto frame21=begin(f,j,21);
    check(f.stage.stage_deformation(2),"third Stage distortion");
    f.stage.state.deformation_mode=2;f.tick();f.draw();end(f,j);
    // Keep BOTH frames and their geometry leases until the entire Undo completes.
    undo(f,j,*frame20);same(f,before,"one UndoTo crosses both frames exactly");
    check(f.stage.mesh==original,"original deformation raw pointer recovered");
    check(f.manager.find(first_ids[0])->reserved_418==u32(reinterpret_cast<uintptr_t>(original)),"restored callback points at original physical owner");
    frame21.reset();frame20.reset();
    auto replay=begin(f,j,20);check(f.stage.stage_deformation(1),"replay deformation creation");
    check(f.stage.mesh==second,"new logical Stage deformation reuses same physical owner");
    f.stage.state.deformation_mode=1;f.tick();f.draw();end(f,j);same(f,middle,"deformation replay exact owner payload");
    undo(f,j,*replay);same(f,before,"replayed deformation can rewind again");
}
void structural_barrier(){
    group("closed retained checkpoints forbid deferred vector growth until retired");
    Fixture f(false);Netplay::RollbackJournal j;check(j.Reset(config()),"journal config");
    const auto before=image(f);auto frame=begin(f,j,30);
    check(!f.stage.start_objects(),"open capture rejects start_objects before resize");
    check(!f.stage.initialize(stage_file(),2,f.camera),"open capture rejects resource replacement");
    end(f,j);
    check(!f.stage.RollbackFrameActive()&&f.stage.RollbackHistoryRetained(),"closed observer still retains structural history");
    check(!f.stage.start_objects(),"closed historical capture rejects start_objects");
    check(!f.stage.initialize(stage_file(),2,f.camera),"closed historical capture rejects initialize");
    same(f,before,"all structural rejections are no-write");
    j.DiscardBefore(31);frame.reset();check(!f.stage.RollbackHistoryRetained(),"last retained frame releases structural barrier");
    check(f.stage.start_objects()&&f.stage.object_animations.size()==2,"confirmed empty frontier permits original deferred activation");
}
void preflight(){
    group("preflight rejects foreign/stale shape before Undo and expired owner");
    Fixture f,other;Netplay::RollbackJournal j;check(j.Reset(config()),"journal config");
    auto frame=begin(f,j,40);f.tick();f.draw();end(f,j);const auto changed=image(f);
    check(!other.stage.ValidateRollbackFrame(frame->stage,40),"foreign Stage rejected");
    check(!f.stage.ValidateRollbackFrame(frame->stage,41),"wrong frontier rejected");
    same(f,changed,"foreign and wrong-frame preflight do not mutate live Stage");
    // Deliberate test-only misuse retains the original allocation while moving
    // the public vector. Validate must reject BEFORE anyone calls journal Undo.
    auto held=std::move(f.stage.object_animations);f.stage.object_animations=held;
    check(!f.stage.ValidateRollbackFrame(frame->stage,40),"relocated vector rejected before byte undo");
    f.stage.object_animations=std::move(held);
    f.stage.resource.instances.push_back({});
    check(!f.stage.ValidateRollbackFrame(frame->stage,40),"resource shape change rejected before byte undo");
    f.stage.resource.instances.pop_back();
    // The push may relocate instances even after pop. Do not call Undo on this
    // deliberately broken identity; retire the history before the next capture.
    j.Clear();frame.reset();
    check(!f.stage.RollbackHistoryRetained(),"retired invalid history releases guard");
    // Clear retires the ring AND unconfigures the common journal. A fresh
    // subsequent history must explicitly restore its validated configuration.
    check(j.Reset(config()),"reconfigure journal after retiring deliberately invalid history");
    const auto next=image(f);auto exact=begin(f,j,41);f.tick();f.draw();end(f,j);undo(f,j,*exact);
    same(f,next,"valid subsequent history still restores exact Stage");exact.reset();
    std::unique_ptr<Frame> orphan;
    {auto owned=std::make_unique<Fixture>();orphan=begin(*owned,j,42);end(*owned,j);j.Clear();}
    check(!orphan->stage.valid(),"destroyed Stage expires frame identity");
    check(!f.stage.ValidateRollbackFrame(orphan->stage,42),"expired foreign frame rejected");
}
void failure(){
    group("failed capture cannot commit and retained frame still guards layout");
    Fixture f(false);Netplay::RollbackJournal j;auto c=config();c.maxBytesPerFrame=64;
    check(j.Reset(c)&&j.BeginFrame(0),"tiny journal opens");auto frame=std::make_unique<StageRollbackFrame>();
    check(!f.stage.BeginRollbackFrame(j,*frame)&&!frame->valid()&&j.Failed(),"StageState capacity failure latched");
    f.stage.EndRollbackFrame();check(!f.stage.RollbackFrameActive()&&!f.stage.deformation_pool.RollbackFrameActive(),"failed capture observers detached");
    check(!j.EndFrame(),"failed Stage transaction cannot commit");u32 restored=~0u;
    check(!j.UndoTo(0,&restored)&&!f.stage.ValidateRollbackFrame(*frame,0),"failed checkpoint cannot restore");
    check(!f.stage.start_objects(),"failed retained capture still protects addressed storage");
    j.Clear();frame.reset();check(f.stage.start_objects(),"cleared failed history permits initialization");
}
}
#endif
int main(){
    static_assert(sizeof(void*)==4,"execute exact WASI32 layout");
    static_assert(sizeof(th11::StageState)==0x3158,"authored Stage ABI");
    ordinary();
#ifdef TH11_MULTIPLAYER
    semantic_roundtrip();deformation_history();structural_barrier();preflight();failure();
    std::printf("PASS Stage rollback multiplayer: %u groups, %u checks\n",groups,checks);
#else
    std::printf("PASS Stage ordinary: %u groups, %u checks\n",groups,checks);
#endif
}
