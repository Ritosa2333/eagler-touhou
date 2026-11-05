// Real production AnmManager + AnimationEngine allocation, script and release
// entry points, with a legally constructed/destructed AnimationEngine.
// Only RollbackState::Touch is a test-owned journal delegate: the full World/
// RollbackState capture and Application MarkSimulated gate are NOT linked.
#include "../web/cpp/platform/AnimationEngine.hpp"
#include "../web/cpp/multiplayer/RollbackState.hpp"
#include <eagler/netplay/RollbackJournal.hpp>

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <string>
#include <type_traits>
#include <utility>
#include <vector>

#if !defined(TH_NATIVE_PLATFORM) || !defined(TH_ENABLE_MULTIPLAYER_GAMEPLAY)
#error This suite requires the real native MP animation layout.
#endif

namespace anm_overflow_oracle {
using namespace th10;
using namespace th10::browser;
using Pool=decltype(std::declval<AnimationEngine&>().rollback_animation_overflow);
constexpr std::size_t Capacity=std::extent_v<decltype(std::declval<Pool&>().occupied)>,NativeCapacity=4096;
constexpr std::size_t Payload=sizeof(AnmVm),Stride=sizeof(Pool::Storage);
static_assert(sizeof(void*)==4&&Stride>=Payload);
static_assert(std::is_trivially_copyable_v<Pool>&&std::is_trivially_copyable_v<AnmManager>);

struct Totals {
    std::uint64_t checks=0,failures=0,full_pool_comparisons=0,full_pool_mismatches=0;
    std::uint64_t full_manager_comparisons=0,full_manager_mismatches=0;
    std::uint64_t preclear_observations=0,preclear_misses=0;
    std::uint64_t budget_cases=0,budget_rejections=0,budget_unexpected_successes=0;
    std::uint64_t budget_payload_changed=0,positive_control_failures=0;
    std::uint64_t engine_constructors=0,engine_destructors=0,script_write_checks=0;
    std::uint64_t asset_io_calls=0,gpu_draw_calls=0;
} totals;
struct CaseResult {std::string name;bool packed=false;std::uint64_t checks=0,failures=0;};
std::vector<CaseResult> cases;
const char* current_case="initialization";

[[noreturn]] void broken_fixture(const char* expression,int line){
    std::fprintf(stderr,"ANM overflow fixture: %s line %d: %s\n",current_case,line,expression);
    std::abort();
}
#define REQUIRE(expression) do { if(!(expression))broken_fixture(#expression,__LINE__); } while(false)
void expect(bool value,const char* expression,int line){
    ++totals.checks;if(value)return;++totals.failures;
    if(totals.failures<=12)
        std::fprintf(stderr,"ANM overflow oracle: %s line %d: %s\n",current_case,line,expression);
}
#define EXPECT(expression) expect(bool(expression),#expression,__LINE__)

struct NoFiles final:FileHost {
    [[noreturn]] static void reject(){++totals.asset_io_calls;broken_fixture("unexpected asset/file IO",__LINE__);}
    u32 open(const char*,bool) override{reject();}
    void close(u32) override{reject();}
    u32 size(u32) override{reject();}
    u32 seek(u32,i32,u32) override{reject();}
    u32 read(u32,u8*,u32) override{reject();}
    u32 write(u32,const u8*,u32) override{reject();}
    u32 list(const char*,const char*,u32,char*,u32) override{reject();}
    bool replace(const char*,const u8*,u32) override{reject();}
};

// This mock permits only the real engine constructor's one 80-byte model
// buffer, camera matrices/viewport and matching destructor release. No draw,
// texture, file, GPU readback or runtime geometry operation can silently pass.
struct ConstructorGraphics final:GraphicsHost {
    std::array<AnmModelVertex,4> model{};
    bool constructing=true,live=false;
    u32 creates=0,maps=0,unmaps=0,binds=0,transforms=0,viewports=0,clears=0,releases=0;
    [[noreturn]] static void reject(){broken_fixture("unexpected graphics operation",__LINE__);}
    touhou::graphics::PipelineState& pipeline() override{reject();}
    u32 create_device(const GraphicsPresentation&,u32) override{reject();}
    void bind_texture(u32,u32) override{reject();}
    void vertex_format(u32,LayoutParameter) override{reject();}
    void draw(u32,TopologyParameter,u32,const void*,u32) override{++totals.gpu_draw_calls;reject();}
    void transform(u32 h,MatrixParameter kind,const Matrix4&) override{
        REQUIRE(constructing&&h==0&&transforms<2);
        REQUIRE(kind==(transforms?Matrices::Projection:Matrices::View));++transforms;
    }
    void viewport(u32 h,const CameraViewport& view) override{
        const CameraViewport expected{0,0,640,480,0,1};
        REQUIRE(constructing&&h==0&&transforms==2&&viewports==0);
        REQUIRE(std::memcmp(&view,&expected,sizeof(view))==0);++viewports;
    }
    u32 retain_resource(void*) override{reject();}
    u32 resource_revision(void*) override{reject();}
    u32 release_resource(void* value) override{
        REQUIRE(!constructing&&live&&value==model.data()&&releases==0);
        live=false;++releases;return 0;
    }
    i32 create_texture(u32,u32,u32,void*&) override{reject();}
    void* texture_surface(void*) override{reject();}
    TextureDescription describe_surface(void*) override{reject();}
    TextureLock map_surface(void*) override{reject();}
    void unmap_surface(void*) override{reject();}
    void resource_priority(void*,u32) override{reject();}
    void preload_resource(void*) override{reject();}
    i32 create_vertices(u32 bytes,LayoutParameter format,void*& out) override{
        REQUIRE(constructing&&!live&&bytes==sizeof(model)&&format==Layouts::World&&creates==0);
        out=model.data();live=true;++creates;return 0;
    }
    void* map_vertices(void* value) override{
        REQUIRE(constructing&&live&&value==model.data()&&maps==0);++maps;return model.data();
    }
    void unmap_vertices(void* value) override{
        REQUIRE(constructing&&live&&value==model.data()&&maps==1&&unmaps==0);++unmaps;
    }
    void vertex_buffer(void* value,u32 stride) override{
        REQUIRE(constructing&&live&&value==model.data()&&stride==20&&unmaps==1&&binds==0);++binds;
    }
    void draw_vertices(TopologyParameter,u32,u32) override{++totals.gpu_draw_calls;reject();}
    void clear_target(u32,u32,float,u32,const i32*,u32) override{reject();}
    i32 present_frame() override{reject();}
    void* back_surface() override{reject();}
    i32 reset_presentation(void*) override{reject();}
    void clear_shader() override{REQUIRE(constructing&&clears==0);++clears;}
    i32 create_surface(u32,u32,u32,u32,bool,void*&) override{reject();}
    i32 copy_surface(void*,const TextureRect*,void*,const TextureRect*,u32) override{reject();}
    i32 update_surface(void*,const TextureRect&,void*,i32,i32) override{reject();}
    void constructed(){
        REQUIRE(creates==1&&maps==1&&unmaps==1&&binds==1&&transforms==2&&viewports==1&&clears==1&&live);
        constructing=false;
    }
    bool destroyed()const{return !live&&releases==1;}
};

// Independently authored ANM records. The production interpreter must execute
// position, scale and RGB writes, then stop at the future instruction. None
// requests a texture, child, callback, RNG, or geometry allocation.
struct Program {
    AnmInstruction position{48,20,0,0};float xyz[3]{12.5f,-7.25f,3.0f};
    AnmInstruction scale{50,16,0,0};float xy[2]{1.25f,.75f};
    AnmInstruction color{52,20,0,0};i32 rgb[3]{17,34,51};
    AnmInstruction future{0,8,100,0};
};
static_assert(offsetof(Program,scale)==20&&offsetof(Program,color)==36&&offsetof(Program,future)==56&&sizeof(Program)==64);

struct Fixture;
Fixture* active=nullptr;
bool touch(multiplayer::RollbackState*,void*,std::size_t);

struct Fixture {
    NoFiles host;
    FileSystem files{host};
    ConstructorGraphics graphics;
    GraphicsDevice device{graphics};
    Rng script_random{},visual_random{};
    float speed=1;
    std::unique_ptr<AnimationEngine> engine;
    multiplayer::RollbackState bridge{};
    multiplayer::RollbackPoolCapture<Capacity> capture;
    Netplay::RollbackJournal journal;
    Program program{};
    AnmInstruction* scripts[1]{&program.position};
    AnmFile file{};
    bool packed=false,frame_open=false;
    std::uint64_t first_write_attempts=0,first_write_saved=0,first_write_failed=0;
    const void* expected_first=nullptr;
    std::array<unsigned char,Stride> expected_bytes{};
    bool first_seen=false,first_untouched=false;

    explicit Fixture(unsigned char poison,bool scan=false):packed(scan){
        REQUIRE(!active);
        engine=std::make_unique<AnimationEngine>(files,device,script_random,visual_random,speed);
        graphics.constructed();++totals.engine_constructors;active=this;
        REQUIRE(engine->rollback_state==nullptr&&manager().cursor==0);
        // Reach the original manager's overflow branch using all 4096 real
        // pooled allocations. No test write invents an occupied native slot.
        for(std::size_t i=0;i<NativeCapacity;++i)
            REQUIRE(manager().allocate(*engine)==&manager().pool[i]);
        REQUIRE(manager().cursor==0&&pool().cursor==0);
        for(const auto used:manager().occupied)REQUIRE(used!=0);
        // Only the trivially copyable overflow storage/metadata is poisoned.
        std::memset(&pool(),poison,sizeof(Pool));
        std::memset(pool().occupied,0,sizeof(pool().occupied));pool().cursor=0;
        file.file_index=7;file.script_count=1;file.scripts=scripts;
        reset_journal(sizeof(Pool)+1024,Capacity+16);
    }
    ~Fixture(){
        REQUIRE(active==this);
        // EndFrame failure is sticky, so destruction is teardown, not Undo.
        // Do not let cleanup callbacks write to a failed transaction.
        engine->rollback_state=nullptr;active=nullptr;engine.reset();
        EXPECT(graphics.destroyed());++totals.engine_destructors;
    }
    Pool& pool(){return engine->rollback_animation_overflow;}
    const Pool& pool()const{return engine->rollback_animation_overflow;}
    AnmManager& manager(){return engine->manager;}
    const AnmManager& manager()const{return engine->manager;}
    void reset_journal(std::size_t bytes,std::size_t blocks){
        REQUIRE(!frame_open&&journal.Reset({2,bytes,blocks,true,true}));
        capture.Clear();
    }
    void begin(u32 frame){
        REQUIRE(!frame_open&&engine->rollback_state==nullptr&&journal.BeginFrame(frame));
        REQUIRE(capture.Capture(pool(),[&](void* p,std::size_t n){return journal.Touch(p,n);},packed));
        // The isolated script/allocation path writes only these manager fields.
        // The full manager byte oracle below independently checks that claim.
        REQUIRE(journal.Touch(&manager().cursor,sizeof(manager().cursor)));
        REQUIRE(journal.Touch(&manager().started_scripts,sizeof(manager().started_scripts)));
        REQUIRE(journal.Touch(&manager().registry,sizeof(manager().registry)));
        REQUIRE(journal.Touch(&manager().last_id,sizeof(manager().last_id)));
        frame_open=true;engine->rollback_state=&bridge;
        first_write_attempts=first_write_saved=first_write_failed=0;
    }
    void end(){
        REQUIRE(frame_open&&!journal.Failed()&&journal.EndFrame());
        frame_open=false;engine->rollback_state=nullptr;
    }
    void undo(u32 frame){
        u32 restored=~u32(0);
        REQUIRE(!frame_open&&journal.UndoTo(frame,&restored)&&restored==frame);
        capture.Clear();expected_first=nullptr;
    }
    void expect_first(std::size_t slot){
        REQUIRE(slot<Capacity);expected_first=pool().at(slot);
        std::memcpy(expected_bytes.data(),expected_first,Stride);
        first_seen=first_untouched=false;
    }
    AnmVm* create(bool at=false){
        auto& m=manager();
        const u32 id=at?m.create_at(file,0,{10,20,3},true,AnimationPlacement::WorldBack,*engine,*engine):
            m.create(file,0,7,AnimationPlacement::WorldBack,*engine,*engine);
        auto* vm=m.registry.find(id);REQUIRE(id&&vm);
        ++totals.script_write_checks;
        EXPECT(pool().owns(vm)&&!m.is_pooled(vm));
        EXPECT(vm->script_position.x==12.5f&&vm->script_position.y==-7.25f&&vm->script_position.z==3);
        EXPECT(vm->scale.x==1.25f&&vm->scale.y==.75f&&vm->color==0xff112233u);
        EXPECT(vm->instruction==&program.future&&vm->animation_file==&file&&vm->script_timer.rate==&speed&&vm->script_timer.current==1);
        EXPECT(at?(vm->position.x==234&&vm->position.y==36&&vm->position.z==3&&vm->owner_tag==0):
                  (vm->position.x==0&&vm->position.y==0&&vm->position.z==0&&vm->owner_tag==7));
        EXPECT(speed==1&&script_random.seed==0&&visual_random.seed==0);
        return vm;
    }
    void remove(AnmVm* vm){
        REQUIRE(vm&&pool().owns(vm));
        REQUIRE(manager().remove(*vm,*engine)==0);
    }
    void fill_overflow(std::size_t count){
        for(std::size_t i=0;i<count;++i){
            auto* vm=manager().allocate(*engine);
            REQUIRE(vm&&pool().owns(vm)&&!manager().is_pooled(vm));
        }
    }
};

bool touch(multiplayer::RollbackState* owner,void* address,std::size_t bytes){
    REQUIRE(active&&owner==&active->bridge&&active->frame_open);
    auto& f=*active;
    REQUIRE(f.engine->rollback_state==owner&&f.pool().owns(address)&&bytes==Payload);
    if(address==f.expected_first&&!f.first_seen){
        f.first_seen=true;f.first_untouched=std::memcmp(address,f.expected_bytes.data(),Stride)==0;
    }
    const bool saved=f.capture.TouchSlot(address,bytes,[&](void* p,std::size_t n){
        ++f.first_write_attempts;const bool result=f.journal.Touch(p,n);
        if(result)++f.first_write_saved;return result;
    });
    if(!saved)++f.first_write_failed;return saved;
}

struct Snapshot {
    std::vector<unsigned char> pool,manager;
    explicit Snapshot(const Fixture& f):pool(sizeof(Pool)),manager(sizeof(AnmManager)){
        std::memcpy(pool.data(),&f.pool(),pool.size());
        std::memcpy(manager.data(),&f.manager(),manager.size());
    }
};
void compare(const Fixture& f,const Snapshot& before){
    REQUIRE(std::memcmp(f.pool().occupied,before.pool.data()+offsetof(Pool,occupied),sizeof(f.pool().occupied))==0);
    u32 cursor=0;std::memcpy(&cursor,before.pool.data()+offsetof(Pool,cursor),sizeof(cursor));
    REQUIRE(f.pool().cursor==cursor);
    ++totals.full_pool_comparisons;
    const bool same_pool=std::memcmp(&f.pool(),before.pool.data(),sizeof(Pool))==0;
    if(!same_pool)++totals.full_pool_mismatches;EXPECT(same_pool);
    ++totals.full_manager_comparisons;
    const bool same_manager=std::memcmp(&f.manager(),before.manager.data(),sizeof(AnmManager))==0;
    if(!same_manager)++totals.full_manager_mismatches;EXPECT(same_manager);
}
void check_preclear(const Fixture& f){
    ++totals.preclear_observations;
    if(!f.first_seen||!f.first_untouched)++totals.preclear_misses;
    EXPECT(f.first_seen&&f.first_untouched);
}
template<class Function>
void run_case(const char* name,bool packed,Function function,bool positive=false){
    current_case=name;const auto before_checks=totals.checks,before_failures=totals.failures;
    function();const auto failures=totals.failures-before_failures;
    cases.push_back({name,packed,totals.checks-before_checks,failures});
    if(positive)totals.positive_control_failures+=failures;
}

void dormant(bool packed,bool at){
    auto holder=std::make_unique<Fixture>(at?0xd3:0,packed);auto& f=*holder;
    const Snapshot before(f);
    f.begin(0);f.expect_first(0);f.create(at);check_preclear(f);
    f.end();f.undo(0);compare(f,before);
}
void active_reuse(bool packed){
    auto holder=std::make_unique<Fixture>(0x65,packed);auto& f=*holder;
    auto* original=f.create();f.fill_overflow(Capacity-1);
    REQUIRE(f.pool().cursor==0);
    const Snapshot before(f);
    f.begin(0);f.remove(original);auto* replacement=f.create(true);
    EXPECT(replacement==original&&f.first_write_saved==0);
    f.end();f.undo(0);compare(f,before);
    EXPECT(f.manager().registry.find(original->id)==original);
}
void dormant_reuse(bool packed){
    auto holder=std::make_unique<Fixture>(0x91,packed);auto& f=*holder;
    f.fill_overflow(Capacity-1);REQUIRE(f.pool().cursor==Capacity-1);
    const Snapshot before(f);
    f.begin(0);f.expect_first(Capacity-1);auto* original=f.create();check_preclear(f);
    f.remove(original);auto* replacement=f.create(true);
    EXPECT(replacement==original&&f.first_write_saved==1);
    f.end();f.undo(0);compare(f,before);
}
void history(bool packed){
    auto holder=std::make_unique<Fixture>(0x37,packed);auto& f=*holder;
    const auto simulate=[&](u32 frame){
        f.begin(frame);
        if(auto* node=f.manager().registry.world_head)f.remove(node->value);
        f.create((frame&1)!=0);f.end();
    };
    // Two cycles of two records exercise Undo, replay and retained arena/ring
    // slots without retaining dozens of entire multi-megabyte managers.
    for(u32 cycle=0;cycle<2;++cycle){
        const u32 first=cycle*2;const Snapshot before(f);
        simulate(first);const Snapshot middle(f);
        simulate(first+1);const Snapshot after(f);
        REQUIRE(f.journal.FrameCount()==2);
        f.undo(first);compare(f,before);
        simulate(first);compare(f,middle);
        simulate(first+1);compare(f,after);
        f.journal.DiscardBefore(first+2);EXPECT(f.journal.FrameCount()==0);
    }
}
void budget(bool packed,bool block_limit){
    auto holder=std::make_unique<Fixture>(0xa7,packed);auto& f=*holder;
    const std::size_t initial=sizeof(f.pool().occupied)+sizeof(f.pool().cursor)+
        sizeof(f.manager().cursor)+sizeof(f.manager().started_scripts)+sizeof(f.manager().registry)+sizeof(f.manager().last_id);
    f.reset_journal(block_limit?sizeof(Pool)+1024:initial+Stride-1,block_limit?5:32);
    f.begin(0);REQUIRE(f.journal.BlocksForFrame(0)==5);
    const Snapshot before(f);
    auto* vm=f.create();EXPECT(vm!=nullptr);
    const bool changed=std::memcmp(f.pool().blocks,before.pool.data()+offsetof(Pool,blocks),sizeof(f.pool().blocks))!=0;
    EXPECT(changed);if(changed)++totals.budget_payload_changed;
    EXPECT(f.first_write_failed>0);EXPECT(f.journal.Failed());
    const bool ended=f.journal.EndFrame();++totals.budget_cases;
    if(!ended)++totals.budget_rejections;else ++totals.budget_unexpected_successes;
    EXPECT(!ended);
    // No attempted Undo or admitted-frame claim after sticky failure.
    // Teardown clears the delegate before the real engine destructor runs.
}
void without_capture(){
    auto holder=std::make_unique<Fixture>(0x5c);auto& f=*holder;
    auto* vm=f.engine->allocate_animation();REQUIRE(vm);
    const auto* bytes=reinterpret_cast<const unsigned char*>(vm);
    bool cleared=true,padding_retained=true;
    for(std::size_t i=0;i<Payload;++i)cleared=cleared&&bytes[i]==0;
    for(std::size_t i=Payload;i<Stride;++i)padding_retained=padding_retained&&bytes[i]==0x5c;
    EXPECT(cleared&&padding_retained&&f.first_write_attempts==0);
    EXPECT(!f.journal.IsFrameOpen()&&f.journal.FrameCount()==0);
    f.engine->release_memory(vm);
}
void exhausted_pool(){
    auto holder=std::make_unique<Fixture>(0);auto& f=*holder;
    f.fill_overflow(Capacity);
    // Do not pass this nullptr through AnmManager::allocate: its original
    // unchecked initialize() on exhaustion is a separate, unchanged contract.
    EXPECT(f.engine->allocate_animation()==nullptr);
    EXPECT(f.first_write_attempts==0&&f.journal.FrameCount()==0);
}
} // namespace anm_overflow_oracle

// Explicit test-only owner seam. Neither the allocator nor its preserve
// function is replaced: those execute from the complete AnimationEngine.cpp.
// World setup, the production Touch dispatcher and MarkSimulated are outside
// this unit; a real RollbackState object is used only as the delegate identity.
namespace th10::multiplayer {
bool RollbackState::Touch(void* address,std::size_t bytes){
    return anm_overflow_oracle::touch(this,address,bytes);
}
}

int main(){
    using namespace anm_overflow_oracle;
    for(bool packed:{false,true}){
        run_case("cold-inactive-create",packed,[&]{dormant(packed,false);});
        run_case("poisoned-inactive-create-at",packed,[&]{dormant(packed,true);});
        run_case("active-release-reuse",packed,[&]{active_reuse(packed);},true);
        run_case("inactive-create-release-reuse",packed,[&]{dormant_reuse(packed);});
        run_case("history-ring-replay",packed,[&]{history(packed);});
        run_case("byte-budget-failure",packed,[&]{budget(packed,false);});
        run_case("block-budget-failure",packed,[&]{budget(packed,true);});
    }
    run_case("outside-capture",false,without_capture,true);
    run_case("overflow-exhausted-pool-existing-result",false,exhausted_pool,true);
#ifdef TH10_ANM_OVERFLOW_ROLLBACK_LEGACY
    constexpr bool legacy=true;
#else
    constexpr bool legacy=false;
#endif
    std::printf("{\"schema\":1,\"suite\":\"anm-overflow-rollback\",\"complete\":true,"
        "\"legacy\":%s,\"passed\":%s,\"checks\":%llu,\"failures\":%llu,"
        "\"payloadBytes\":%zu,\"storageStride\":%zu,\"capacity\":%zu,\"nativeCapacity\":%zu,"
        "\"fullPoolComparisons\":%llu,\"fullPoolMismatches\":%llu,"
        "\"fullManagerComparisons\":%llu,\"fullManagerMismatches\":%llu,"
        "\"preclearObservations\":%llu,\"preclearMisses\":%llu,"
        "\"budgetCases\":%llu,\"budgetRejections\":%llu,\"budgetUnexpectedSuccesses\":%llu,"
        "\"budgetPayloadChanged\":%llu,\"positiveControlFailures\":%llu,"
        "\"engineConstructors\":%llu,\"engineDestructors\":%llu,\"scriptWriteChecks\":%llu,"
        "\"assetIoCalls\":%llu,\"gpuDrawCalls\":%llu,"
        "\"productionEngineLinked\":true,\"productionManagerLinked\":true,"
        "\"rollbackStateTouchDelegate\":true,\"productionRollbackStateLinked\":false,"
        "\"applicationMarkSimulatedCovered\":false,\"cases\":[",
        legacy?"true":"false",totals.failures?"false":"true",
        static_cast<unsigned long long>(totals.checks),static_cast<unsigned long long>(totals.failures),
        Payload,Stride,Capacity,NativeCapacity,
        static_cast<unsigned long long>(totals.full_pool_comparisons),static_cast<unsigned long long>(totals.full_pool_mismatches),
        static_cast<unsigned long long>(totals.full_manager_comparisons),static_cast<unsigned long long>(totals.full_manager_mismatches),
        static_cast<unsigned long long>(totals.preclear_observations),static_cast<unsigned long long>(totals.preclear_misses),
        static_cast<unsigned long long>(totals.budget_cases),static_cast<unsigned long long>(totals.budget_rejections),
        static_cast<unsigned long long>(totals.budget_unexpected_successes),static_cast<unsigned long long>(totals.budget_payload_changed),
        static_cast<unsigned long long>(totals.positive_control_failures),
        static_cast<unsigned long long>(totals.engine_constructors),static_cast<unsigned long long>(totals.engine_destructors),
        static_cast<unsigned long long>(totals.script_write_checks),
        static_cast<unsigned long long>(totals.asset_io_calls),static_cast<unsigned long long>(totals.gpu_draw_calls));
    for(std::size_t i=0;i<cases.size();++i){
        const auto& value=cases[i];
        std::printf("%s{\"name\":\"%s\",\"packed\":%s,\"checks\":%llu,\"failures\":%llu}",
            i?",":"",value.name.c_str(),value.packed?"true":"false",
            static_cast<unsigned long long>(value.checks),static_cast<unsigned long long>(value.failures));
    }
    std::puts("]}");return totals.failures?1:0;
}
