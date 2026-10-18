// Production ring interpreter and distortion initializer enter the real
// AnimationEngine geometry allocators on a legally constructed engine.
// Only RollbackState::Touch is a test delegate. No World capture, production
// Touch dispatcher or Application MarkSimulated gate is claimed by this unit.
#include "../th10_web/cpp/platform/AnimationEngine.hpp"
#include "../th10_web/cpp/multiplayer/RollbackState.hpp"
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

namespace geometry_rollback_oracle {
using namespace th10;
using namespace th10::browser;
using Pool=decltype(std::declval<AnimationEngine&>().rollback_geometry);
constexpr std::size_t Capacity=128,Payload=65536,Stride=sizeof(Pool::Storage);
static_assert(sizeof(void*)==4&&Stride==Payload&&std::is_trivially_copyable_v<Pool>);
struct Totals {
    std::uint64_t checks=0,failures=0,full_pool_comparisons=0,full_pool_mismatches=0;
    std::uint64_t full_owner_comparisons=0,full_owner_mismatches=0;
    std::uint64_t preclear_observations=0,preclear_misses=0;
    std::uint64_t budget_cases=0,budget_rejections=0,budget_unexpected_successes=0;
    std::uint64_t budget_payload_changed=0,positive_control_failures=0;
    std::uint64_t engine_constructors=0,engine_destructors=0;
    std::uint64_t ring_script_checks=0,distortion_checks=0,full_clear_checks=0,tail_zero_checks=0;
    std::uint64_t asset_io_calls=0,gpu_draw_calls=0;
} totals;
struct CaseResult {std::string name;bool packed=false;std::uint64_t checks=0,failures=0;};
std::vector<CaseResult> cases;
const char* current_case="initialization";
[[noreturn]] void broken_fixture(const char* expression,int line){
    std::fprintf(stderr,"geometry rollback fixture: %s line %d: %s\n",current_case,line,expression);
    std::abort();
}
#define REQUIRE(expression) do { if(!(expression))broken_fixture(#expression,__LINE__); } while(false)
void expect(bool value,const char* expression,int line){
    ++totals.checks;if(value)return;++totals.failures;
    if(totals.failures<=12)
        std::fprintf(stderr,"geometry rollback oracle: %s line %d: %s\n",current_case,line,expression);
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

// Opcode 84 requests four actual AnmVertex records. The interpreter stops at
// the future instruction, then animate() executes update_ring_geometry().
struct RingProgram {
    AnmInstruction allocate{84,12,0,0};i32 pairs=2;
    AnmInstruction future{0,8,100,0};
};
static_assert(sizeof(RingProgram)==20&&offsetof(RingProgram,future)==12);
constexpr std::size_t RingBytes=4*sizeof(AnmVertex);
static_assert(RingBytes==112&&sizeof(AnmDistortion)==1200);

struct Fixture;
Fixture* active=nullptr;
bool touch(multiplayer::RollbackState*,void*,std::size_t);

struct Fixture {
    NoFiles host;FileSystem files{host};
    ConstructorGraphics graphics;GraphicsDevice device{graphics};
    Rng script_random{},visual_random{};float speed=1;
    std::unique_ptr<AnimationEngine> engine;
    multiplayer::RollbackState bridge{};
    multiplayer::RollbackPoolCapture<Capacity> capture;
    Netplay::RollbackJournal journal;
    AnmVm vm{};AnmSprite sprite{};RingProgram program{};
    bool packed=false,frame_open=false;
    std::uint64_t first_write_attempts=0,first_write_saved=0,first_write_failed=0;
    const void* expected_first=nullptr;
    std::array<unsigned char,Stride> expected_bytes{};
    bool first_seen=false,first_untouched=false;

    explicit Fixture(unsigned char poison,bool scan=false):packed(scan){
        REQUIRE(!active);
        engine=std::make_unique<AnimationEngine>(files,device,script_random,visual_random,speed);
        graphics.constructed();++totals.engine_constructors;active=this;
        REQUIRE(engine->rollback_state==nullptr);
        // Never byte-clear a polymorphic engine or replace its allocator.
        std::memset(&pool(),poison,sizeof(Pool));
        std::memset(pool().occupied,0,sizeof(pool().occupied));pool().cursor=0;
        vm.initialize();
        reset_journal(sizeof(Pool)+sizeof(vm)+1024,Capacity+16);
    }
    ~Fixture(){
        REQUIRE(active==this);
        // Failed frames are torn down, not silently admitted or undone.
        engine->rollback_state=nullptr;active=nullptr;
        if(vm.geometry){REQUIRE(pool().owns(vm.geometry));engine->release_memory(vm.geometry);vm.geometry=nullptr;}
        engine.reset();EXPECT(graphics.destroyed());++totals.engine_destructors;
    }
    Pool& pool(){return engine->rollback_geometry;}
    const Pool& pool()const{return engine->rollback_geometry;}
    void reset_journal(std::size_t bytes,std::size_t blocks){
        REQUIRE(!frame_open&&journal.Reset({2,bytes,blocks,true,true}));capture.Clear();
    }
    void begin(u32 frame){
        REQUIRE(!frame_open&&engine->rollback_state==nullptr&&journal.BeginFrame(frame));
        REQUIRE(capture.Capture(pool(),[&](void* p,std::size_t n){return journal.Touch(p,n);},packed));
        REQUIRE(journal.Touch(&vm,sizeof(vm)));
        REQUIRE(journal.Touch(&script_random,sizeof(script_random)));
        REQUIRE(journal.Touch(&visual_random,sizeof(visual_random)));
        REQUIRE(journal.Touch(&speed,sizeof(speed)));
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
    void prepare_ring(){
        REQUIRE(!vm.geometry);
        vm.initialize();vm.scale={4,20};vm.color=0xff112233u;
        vm.position={5,6,7};vm.script_position={1,2,3};
        sprite.u0=.25f;sprite.u1=.75f;vm.sprite=&sprite;
        vm.integer_variables[0]=2;vm.integer_variables[1]=1;
        vm.script_begin=vm.instruction=&program.allocate;
        vm.script_timer.rate=&speed;vm.script_timer_flags=1;vm.script_timer.initialize(0);
    }
    void prepare_distortion(){
        REQUIRE(!vm.geometry);
        vm.initialize();vm.position={10,20,0};vm.script_position={1,2,0};vm.color=0xff123456u;
    }
    void check_tail(const void* pointer,std::size_t used){
        REQUIRE(pool().owns(pointer)&&used<=Payload);
        ++totals.tail_zero_checks;const auto* bytes=static_cast<const unsigned char*>(pointer);
        bool clear=true;for(std::size_t i=used;i<Payload;++i)clear=clear&&bytes[i]==0;
        EXPECT(clear);
    }
    void run_ring(){
        REQUIRE(vm.instruction==&program.allocate);
        const i32 result=vm.update(*engine);++totals.ring_script_checks;
        REQUIRE(vm.geometry&&pool().owns(vm.geometry));
        const auto* vertices=static_cast<const AnmVertex*>(vm.geometry);
        EXPECT(result==0&&vm.instruction==&program.future&&vm.script_timer.current==1);
        EXPECT((vm.flags&0x3c00000u)==0x2400000u);
        EXPECT(vertices[0].position.x==28&&vertices[0].position.y==8&&vertices[0].position.z==10);
        EXPECT(vertices[1].position.x==24&&vertices[1].position.y==8&&vertices[1].position.z==10);
        EXPECT(vertices[0].color==vm.color&&vertices[0].reciprocal_w==1&&vertices[0].uv.x==.25f);
        EXPECT(vertices[2].position.x==vertices[0].position.x&&vertices[2].uv.y==1);
        EXPECT(speed==1&&script_random.calls==0&&visual_random.calls==0);
        check_tail(vm.geometry,RingBytes);
    }
    void run_distortion(){
        const auto before_calls=script_random.calls;
        const i32 result=AnmDistortion::initialize(vm,*engine);++totals.distortion_checks;
        REQUIRE(vm.geometry&&pool().owns(vm.geometry));
        const auto& effect=*static_cast<const AnmDistortion*>(vm.geometry);
        EXPECT(result==0&&vm.update_callback==callback_id::DistortionUpdate&&vm.draw_callback==callback_id::DistortionDraw);
        EXPECT(effect.vertices[0].position.x==11&&effect.vertices[0].position.y==22&&effect.vertices[0].position.z==0);
        EXPECT(effect.vertices[0].reciprocal_w==1&&effect.vertices[0].uv.x==.5f&&effect.vertices[0].uv.y==.5f);
        EXPECT(script_random.calls>before_calls&&visual_random.calls==0&&speed==1);
        check_tail(vm.geometry,sizeof(AnmDistortion));
    }
    void* entry(bool distortion,std::size_t bytes){
        return distortion?engine->allocate(static_cast<u32>(bytes)):engine->allocate_geometry(static_cast<u32>(bytes));
    }
    void check_clear(const void* pointer){
        REQUIRE(pool().owns(pointer));++totals.full_clear_checks;
        const auto* bytes=static_cast<const unsigned char*>(pointer);
        bool clear=true;for(std::size_t i=0;i<Payload;++i)clear=clear&&bytes[i]==0;
        EXPECT(clear);
    }
    void* raw(bool distortion,std::size_t bytes){
        auto* pointer=entry(distortion,bytes);REQUIRE(pointer&&pool().owns(pointer));
        check_clear(pointer);return pointer;
    }
    void release(bool distortion,void* pointer){
        if(distortion)engine->release(pointer);else engine->release_memory(pointer);
    }
    void fill(std::size_t count){
        for(std::size_t i=0;i<count;++i){
            auto* pointer=entry((i&1)!=0,1);REQUIRE(pointer&&pool().owns(pointer));
        }
    }
};

bool touch(multiplayer::RollbackState* owner,void* address,std::size_t bytes){
    REQUIRE(active&&owner==&active->bridge&&active->frame_open);
    auto& f=*active;
    // A requested-size Touch (112 / 1200 bytes here) would miss the original
    // allocator's much larger clear. Demand the full production 64 KiB slot.
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
    std::vector<unsigned char> pool;
    std::array<unsigned char,sizeof(AnmVm)> vm{};
    std::array<unsigned char,sizeof(Rng)> script{},visual{};
    std::array<unsigned char,sizeof(float)> speed{};
    std::array<unsigned char,sizeof(RingProgram)> program{};
    std::array<unsigned char,sizeof(AnmSprite)> sprite{};
    explicit Snapshot(const Fixture& f):pool(sizeof(Pool)){
        std::memcpy(pool.data(),&f.pool(),pool.size());
        std::memcpy(vm.data(),&f.vm,vm.size());
        std::memcpy(script.data(),&f.script_random,script.size());
        std::memcpy(visual.data(),&f.visual_random,visual.size());
        std::memcpy(speed.data(),&f.speed,speed.size());
        std::memcpy(program.data(),&f.program,program.size());
        std::memcpy(sprite.data(),&f.sprite,sprite.size());
    }
};
void compare(const Fixture& f,const Snapshot& before){
    REQUIRE(std::memcmp(f.pool().occupied,before.pool.data()+offsetof(Pool,occupied),sizeof(f.pool().occupied))==0);
    u32 cursor=0;std::memcpy(&cursor,before.pool.data()+offsetof(Pool,cursor),sizeof(cursor));
    REQUIRE(f.pool().cursor==cursor);
    ++totals.full_pool_comparisons;
    const bool same_pool=std::memcmp(&f.pool(),before.pool.data(),sizeof(Pool))==0;
    if(!same_pool)++totals.full_pool_mismatches;EXPECT(same_pool);
    ++totals.full_owner_comparisons;
    const bool same_owner=std::memcmp(&f.vm,before.vm.data(),before.vm.size())==0&&
        std::memcmp(&f.script_random,before.script.data(),before.script.size())==0&&
        std::memcmp(&f.visual_random,before.visual.data(),before.visual.size())==0&&
        std::memcmp(&f.speed,before.speed.data(),before.speed.size())==0&&
        std::memcmp(&f.program,before.program.data(),before.program.size())==0&&
        std::memcmp(&f.sprite,before.sprite.data(),before.sprite.size())==0;
    if(!same_owner)++totals.full_owner_mismatches;EXPECT(same_owner);
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
void dormant(bool packed,bool distortion){
    auto holder=std::make_unique<Fixture>(distortion?0x79:0xd3,packed);auto& f=*holder;
    if(distortion)f.prepare_distortion();else f.prepare_ring();
    const Snapshot before(f);
    f.begin(0);f.expect_first(0);
    if(distortion)f.run_distortion();else f.run_ring();
    check_preclear(f);f.end();f.undo(0);compare(f,before);
}
void active_reuse(bool packed){
    auto holder=std::make_unique<Fixture>(0x65,packed);auto& f=*holder;
    auto* original=f.raw(false,RingBytes);std::memset(original,0x35,Payload);
    f.fill(Capacity-1);REQUIRE(f.pool().cursor==0);
    const Snapshot before(f);
    f.begin(0);f.release(false,original);auto* replacement=f.raw(true,sizeof(AnmDistortion));
    std::memset(replacement,0xbc,Payload);
    EXPECT(replacement==original&&f.first_write_saved==0);
    f.end();f.undo(0);compare(f,before);
}
void dormant_reuse(bool packed){
    auto holder=std::make_unique<Fixture>(0x91,packed);auto& f=*holder;
    f.fill(Capacity-1);REQUIRE(f.pool().cursor==Capacity-1);
    const Snapshot before(f);
    f.begin(0);f.expect_first(Capacity-1);auto* original=f.raw(false,RingBytes);
    check_preclear(f);std::memset(original,0x4a,RingBytes);f.release(false,original);
    auto* replacement=f.raw(true,sizeof(AnmDistortion));std::memset(replacement,0x6c,sizeof(AnmDistortion));
    EXPECT(replacement==original&&f.first_write_saved==1);
    f.end();f.undo(0);compare(f,before);
}
void budget(bool packed,bool block_limit){
    auto holder=std::make_unique<Fixture>(0xa7,packed);auto& f=*holder;
    if(block_limit)f.prepare_distortion();else f.prepare_ring();
    const std::size_t initial=sizeof(f.pool().occupied)+sizeof(f.pool().cursor)+sizeof(f.vm)+
        sizeof(f.script_random)+sizeof(f.visual_random)+sizeof(f.speed);
    f.reset_journal(block_limit?sizeof(Pool)+sizeof(f.vm)+1024:initial+Stride-1,block_limit?5:32);
    f.begin(0);REQUIRE(f.journal.BlocksForFrame(0)==5);
    const Snapshot before(f);
    if(block_limit)f.run_distortion();else f.run_ring();
    EXPECT(f.vm.geometry!=nullptr);
    const bool changed=std::memcmp(f.pool().blocks,before.pool.data()+offsetof(Pool,blocks),sizeof(f.pool().blocks))!=0;
    EXPECT(changed);if(changed)++totals.budget_payload_changed;
    EXPECT(f.first_write_failed>0);EXPECT(f.journal.Failed());
    const bool ended=f.journal.EndFrame();++totals.budget_cases;
    if(!ended)++totals.budget_rejections;else ++totals.budget_unexpected_successes;
    EXPECT(!ended);
}
void boundaries(){
    auto holder=std::make_unique<Fixture>(0x5c);auto& f=*holder;
    const Snapshot before(f);
    for(bool distortion:{false,true}){
        EXPECT(f.entry(distortion,0)==nullptr);
        EXPECT(f.entry(distortion,Payload+1)==nullptr);
    }
    compare(f,before);
    constexpr std::size_t sizes[]{1,1023,1024,1025,Payload-1,Payload};
    for(bool distortion:{false,true})for(const auto bytes:sizes){
        auto* pointer=f.raw(distortion,bytes);
        static_cast<unsigned char*>(pointer)[Payload-1]=0x2d;
        f.release(distortion,pointer);
    }
    EXPECT(f.first_write_attempts==0&&!f.journal.IsFrameOpen()&&f.journal.FrameCount()==0);
}
void exhausted_pool(){
    auto holder=std::make_unique<Fixture>(0);auto& f=*holder;
    f.fill(Capacity);REQUIRE(f.pool().cursor==0);
    const Snapshot before(f);
    EXPECT(f.entry(false,1)==nullptr);EXPECT(f.entry(true,1)==nullptr);
    compare(f,before);EXPECT(f.first_write_attempts==0&&f.journal.FrameCount()==0);
}
} // namespace geometry_rollback_oracle

// Test-owned delegate only. Production allocators, AnmVm::update/opcode84,
// update_ring_geometry and AnmDistortion::initialize are linked unchanged.
namespace th10::multiplayer {
bool RollbackState::Touch(void* address,std::size_t bytes){
    return geometry_rollback_oracle::touch(this,address,bytes);
}
}

int main(){
    using namespace geometry_rollback_oracle;
    for(bool packed:{false,true}){
        run_case("ring-script-inactive",packed,[&]{dormant(packed,false);});
        run_case("distortion-inactive",packed,[&]{dormant(packed,true);});
        run_case("active-release-reuse",packed,[&]{active_reuse(packed);},true);
        run_case("inactive-create-release-reuse",packed,[&]{dormant_reuse(packed);});
        run_case("byte-budget-ring-entry",packed,[&]{budget(packed,false);});
        run_case("block-budget-distortion-entry",packed,[&]{budget(packed,true);});
    }
    run_case("invalid-size-and-boundary",false,boundaries,true);
    run_case("exhausted-pool-existing-result",false,exhausted_pool,true);
#ifdef TH10_GEOMETRY_ROLLBACK_LEGACY
    constexpr bool legacy=true;
#else
    constexpr bool legacy=false;
#endif
    std::printf("{\"schema\":1,\"suite\":\"geometry-rollback\",\"complete\":true,"
        "\"legacy\":%s,\"passed\":%s,\"checks\":%llu,\"failures\":%llu,"
        "\"payloadBytes\":%zu,\"storageStride\":%zu,\"capacity\":%zu,"
        "\"fullPoolComparisons\":%llu,\"fullPoolMismatches\":%llu,"
        "\"fullOwnerComparisons\":%llu,\"fullOwnerMismatches\":%llu,"
        "\"preclearObservations\":%llu,\"preclearMisses\":%llu,"
        "\"budgetCases\":%llu,\"budgetRejections\":%llu,\"budgetUnexpectedSuccesses\":%llu,"
        "\"budgetPayloadChanged\":%llu,\"positiveControlFailures\":%llu,"
        "\"engineConstructors\":%llu,\"engineDestructors\":%llu,"
        "\"ringScriptChecks\":%llu,\"distortionChecks\":%llu,\"fullClearChecks\":%llu,\"tailZeroChecks\":%llu,"
        "\"assetIoCalls\":%llu,\"gpuDrawCalls\":%llu,"
        "\"productionEngineLinked\":true,\"productionRingInterpreterLinked\":true,\"productionDistortionLinked\":true,"
        "\"rollbackStateTouchDelegate\":true,\"productionRollbackStateLinked\":false,"
        "\"applicationMarkSimulatedCovered\":false,\"cases\":[",
        legacy?"true":"false",totals.failures?"false":"true",
        static_cast<unsigned long long>(totals.checks),static_cast<unsigned long long>(totals.failures),
        Payload,Stride,Capacity,
        static_cast<unsigned long long>(totals.full_pool_comparisons),static_cast<unsigned long long>(totals.full_pool_mismatches),
        static_cast<unsigned long long>(totals.full_owner_comparisons),static_cast<unsigned long long>(totals.full_owner_mismatches),
        static_cast<unsigned long long>(totals.preclear_observations),static_cast<unsigned long long>(totals.preclear_misses),
        static_cast<unsigned long long>(totals.budget_cases),static_cast<unsigned long long>(totals.budget_rejections),
        static_cast<unsigned long long>(totals.budget_unexpected_successes),static_cast<unsigned long long>(totals.budget_payload_changed),
        static_cast<unsigned long long>(totals.positive_control_failures),
        static_cast<unsigned long long>(totals.engine_constructors),static_cast<unsigned long long>(totals.engine_destructors),
        static_cast<unsigned long long>(totals.ring_script_checks),static_cast<unsigned long long>(totals.distortion_checks),
        static_cast<unsigned long long>(totals.full_clear_checks),static_cast<unsigned long long>(totals.tail_zero_checks),
        static_cast<unsigned long long>(totals.asset_io_calls),static_cast<unsigned long long>(totals.gpu_draw_calls));
    for(std::size_t i=0;i<cases.size();++i){
        const auto& value=cases[i];
        std::printf("%s{\"name\":\"%s\",\"packed\":%s,\"checks\":%llu,\"failures\":%llu}",
            i?",":"",value.name.c_str(),value.packed?"true":"false",
            static_cast<unsigned long long>(value.checks),static_cast<unsigned long long>(value.failures));
    }
    std::puts("]}");return totals.failures?1:0;
}
