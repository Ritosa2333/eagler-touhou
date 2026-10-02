// Resource-free CPU workload using TH10's real 32-bit ANM interpreter,
// arithmetic, renderer, VM layout and rollback journal. No game resources,
// GPU, SDL, network transport, ECL simulation or mobile FPS are measured.
#include "../th10_web/cpp/game/AnmRenderer.hpp"
#include "../th10_web/cpp/multiplayer/RollbackPoolCapture.hpp"
#include <eagler/netplay/RollbackJournal.hpp>
#include <eagler/netplay/SparsePoolCapture.hpp>
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <new>
#include <vector>

using namespace th10;
namespace {
constexpr unsigned NativeCapacity=4096, OverflowCapacity=2048, ReplayFrames=12;
constexpr unsigned WarmupFrames=24, MeasuredFrames=24, Trials=5;
using Clock=std::chrono::steady_clock;
using Overflow=multiplayer::RollbackPool<sizeof(AnmVm),OverflowCapacity>;
[[noreturn]] void fail(const char* expression,int line){
    std::fprintf(stderr,"ANM engine workload: line %d: %s\n",line,expression);std::abort();
}
#define CHECK(value) do{if(!(value))fail(#value,__LINE__);}while(false)
double milliseconds(Clock::time_point start){return std::chrono::duration<double,std::milli>(Clock::now()-start).count();}
u32 bits(float value){u32 result;std::memcpy(&result,&value,sizeof(result));return result;}
u64 hash_bytes(const void* address,std::size_t size,u64 hash=14695981039346656037ull){
    const auto* bytes=static_cast<const u8*>(address);
    for(std::size_t i=0;i<size;++i)hash=(hash^bytes[i])*1099511628211ull;
    return hash;
}
std::vector<u8> bytes(const void* address,std::size_t size){const auto* begin=static_cast<const u8*>(address);return {begin,begin+size};}
struct Instruction {AnmInstruction header;u32 arguments[2];};
static_assert(sizeof(Instruction)==16);
struct AnimationEnvironment final:AnmEnvironment {
    float speed=1;
    Rng rng{};
    Vec3 zero{};
    AnmFile file{};
    std::array<AnmSprite,16> sprites{};
    std::array<u32,4> texture_tokens{};
    u8 loaded_token=0;
    // Add to script-position X/Y, then jump back on the next logical tick.
    // All destinations are VM references: the shared bytecode is immutable.
    std::array<Instruction,3> program{{
        {{9,16,0,1},{bits(10013.0f),bits(.125f)}},
        {{9,16,0,1},{bits(10014.0f),bits(.0625f)}},
        {{4,16,1,0},{0,0}},
    }};
    AnimationEnvironment(){
        rate=&speed;visual_rng=script_rng=&rng;camera_delta=default_tangent=&zero;
        reference_positions[0]=reference_positions[1]=&zero;
        file.loaded=&loaded_token;file.sprites=sprites.data();file.sprite_count=sprites.size();
        for(unsigned i=0;i<sprites.size();++i){
            auto& sprite=sprites[i];sprite.texture=&texture_tokens[i/4];
            sprite.width=8.f+4.f*(i%4);sprite.height=12.f+4.f*(i%3);
            sprite.texture_width=sprite.texture_height=256.f;
            sprite.scale_x=sprite.scale_y=1.f;
            sprite.u0=(i%4)*.25f;sprite.v0=(i/4)*.25f;
            sprite.u1=sprite.u0+.25f;sprite.v1=sprite.v0+.25f;
        }
    }
    void bind_sprite(AnmVm& vm,i32 index) override{CHECK(file.bind_sprite(vm,index)==0);}
    void change_draw_mode(AnmVm&) override{CHECK(false);}
    void* allocate_geometry(u32) override{CHECK(false);return nullptr;}
    AnmVm* spawn_child(AnmVm&,i32,u32) override{CHECK(false);return nullptr;}
    void initialize(AnmVm& vm,unsigned i){
        vm.initialize();vm.id=i+1;bind_sprite(vm,(i/32)%sprites.size());
        vm.position={32.f+float(i%32)*17.f+.125f,24.f+float((i/32)%24)*17.f+.25f,.25f};
        if(i%13==0)vm.position.x=800.f; // Exercise real viewport rejection.
        const unsigned mode=i%4;
        vm.flags=3|(mode<<22)|((i%3)<<18)|(((i/3)%3)<<20)|(((i/256)%2)<<4)|(((i/512)%2)<<31);
        if(i%23==0)vm.flags&=~2u; // Exercise hidden VM rejection.
        vm.scale={i%7==0?-1.f:1.125f,1.f};
        vm.rotation.z=mode%2?.125f:0.f;
        vm.angular_velocity.z=mode%2?.00390625f:0.f;
        vm.uv_velocity={.001953125f,-.0009765625f};
        vm.color=0xc0000000u|((i*0x00110203u)&0x00ffffffu);
        vm.script_begin=vm.instruction=&program[0].header;
        vm.script_timer.rate=rate;vm.script_timer_flags=1;vm.script_timer.initialize(-1);
    }
};
struct Backend final:AnmRenderEnvironment {
    PipelineState state{};
    AnmVertex vertices[4]{};
    RenderViewport view{0,0,640,480};
    unsigned calls=0,quads=0,textures=0;
    Backend(){quad=vertices;viewport=&view;for(auto& vertex:vertices)vertex.reciprocal_w=1;}
    void reset(){calls=quads=textures=0;state={};}
    PipelineState& pipeline() override{return state;}
    void set_texture(void*) override{++textures;}
    void vertex_format(LayoutParameter) override{}
    void draw_triangles(TopologyParameter,u32 count,const void*,u32 stride) override{CHECK(stride==sizeof(AnmVertex));CHECK(count%2==0);++calls;quads+=count/2;}
    void set_transform(MatrixParameter,const Matrix4&) override{CHECK(false);}
    void stream_source(void*,u32) override{CHECK(false);}
    void draw_buffer(TopologyParameter,u32,u32) override{CHECK(false);}
    i32 special_draw(AnmManager&,AnmVm&,u32) override{CHECK(false);return 0;}
};
struct Engine {
    std::unique_ptr<AnmManager> manager=std::make_unique<AnmManager>();
    AnimationEnvironment environment;
    Backend backend;
    AnmRenderer renderer{*manager,backend};
    unsigned count;
    explicit Engine(unsigned n):count(n){
        for(unsigned i=0;i<count;++i){environment.initialize(manager->pool[i],i);manager->occupied[i]=1;}
        manager->cursor=count%NativeCapacity;
    }
    void update(){for(unsigned i=0;i<count;++i)CHECK(manager->pool[i].update(environment)==0);}
    void render(){
        backend.reset();manager->current_texture=nullptr;std::memset(manager->cached_draw_state,0xff,sizeof(manager->cached_draw_state));
        manager->submitted_draws=manager->flushed_batches=0;manager->draw_offset={.25f,-.125f};
        manager->tint_enabled=1;manager->tint=0x80a08080;renderer.begin_frame();
        for(unsigned i=0;i<count;++i)renderer.draw(manager->pool[i]);
        renderer.flush();CHECK(backend.quads==manager->submitted_draws);CHECK(backend.calls==manager->flushed_batches);
        CHECK(std::size_t(manager->vertex_write-manager->vertex_buffer)==backend.quads*6);
    }
    std::size_t vertex_bytes()const{return std::size_t(manager->vertex_write-manager->vertex_buffer)*sizeof(AnmVertex);}
};
struct FrameEvidence {std::vector<u8> vertices;unsigned quads,calls,textures;};
void configure(Netplay::RollbackJournal& journal,std::size_t pool_bytes,unsigned slots){
    CHECK(journal.Reset({14,pool_bytes+8192,slots+8,true,true}));
}
void native_workload(unsigned count){
    Engine engine(count);auto& manager=*engine.manager;
    const auto initial=bytes(manager.pool,sizeof(manager.pool));
    const auto program=bytes(engine.environment.program.data(),sizeof(engine.environment.program));
    Netplay::RollbackJournal journal;configure(journal,sizeof(manager.pool),NativeCapacity);
    Netplay::SparsePoolCapture<NativeCapacity> capture;
    std::array<FrameEvidence,ReplayFrames> evidence;
    const auto save=[&](void* p,std::size_t n){return journal.Touch(p,n);};
    for(unsigned frame=0;frame<ReplayFrames;++frame){
        CHECK(journal.BeginFrame(frame));
        CHECK(journal.Touch(manager.occupied,sizeof(manager.occupied)));CHECK(journal.Touch(&manager.cursor,sizeof(manager.cursor)));
        CHECK(capture.Capture(manager.pool,[&](const AnmVm& vm){return manager.occupied[&vm-manager.pool]!=0;},save));
        CHECK(journal.BlocksForFrame(frame)==3);CHECK(journal.BytesForFrame(frame)==count*sizeof(AnmVm)+sizeof(manager.occupied)+sizeof(manager.cursor));
        engine.update();const auto before_render=bytes(manager.pool,sizeof(manager.pool));engine.render();
        CHECK(std::memcmp(before_render.data(),manager.pool,before_render.size())==0);
        evidence[frame]={bytes(manager.vertex_buffer,engine.vertex_bytes()),engine.backend.quads,engine.backend.calls,engine.backend.textures};
        CHECK(journal.EndFrame());
    }
    const auto final=bytes(manager.pool,sizeof(manager.pool));u32 restored=~0u;
    CHECK(journal.UndoTo(0,&restored));CHECK(restored==0);
    CHECK(std::memcmp(initial.data(),manager.pool,initial.size())==0);
    u64 vertices_hash=14695981039346656037ull;
    for(unsigned frame=0;frame<ReplayFrames;++frame){
        engine.update();engine.render();const auto& expected=evidence[frame];
        CHECK(engine.vertex_bytes()==expected.vertices.size());
        CHECK(std::memcmp(manager.vertex_buffer,expected.vertices.data(),expected.vertices.size())==0);
        CHECK(engine.backend.quads==expected.quads&&engine.backend.calls==expected.calls&&engine.backend.textures==expected.textures);
        vertices_hash=hash_bytes(expected.vertices.data(),expected.vertices.size(),vertices_hash);
    }
    CHECK(std::memcmp(final.data(),manager.pool,final.size())==0);
    CHECK(std::memcmp(program.data(),engine.environment.program.data(),program.size())==0);
    std::printf("{\"suite\":\"native-anm-validation\",\"instances\":%u,\"replay_frames\":%u,\"vm_bytes\":%zu,\"journal_blocks\":3,\"rewind_byte_equal\":true,\"replay_byte_equal\":true,\"render_vm_unchanged\":true,\"vertex_byte_equal\":true,\"vertex_hash\":\"%016llx\"}\n",count,ReplayFrames,sizeof(AnmVm),(unsigned long long)vertices_hash);
    for(unsigned i=0;i<WarmupFrames;++i){engine.update();engine.render();}
    for(unsigned trial=0;trial<Trials;++trial){
        std::memcpy(manager.pool,initial.data(),initial.size());double update_ms=0,render_ms=0;u64 quads=0,calls=0;
        for(unsigned frame=0;frame<MeasuredFrames;++frame){
            auto begin=Clock::now();engine.update();update_ms+=milliseconds(begin);
            begin=Clock::now();engine.render();render_ms+=milliseconds(begin);quads+=engine.backend.quads;calls+=engine.backend.calls;
        }
        const auto hash=hash_bytes(manager.vertex_buffer,engine.vertex_bytes());
        std::printf("{\"suite\":\"native-anm-timing\",\"instances\":%u,\"trial\":%u,\"frames\":%u,\"update_ms\":%.6f,\"render_ms\":%.6f,\"quads\":%llu,\"batches\":%llu,\"vertex_hash\":\"%016llx\"}\n",count,trial,MeasuredFrames,update_ms,render_ms,(unsigned long long)quads,(unsigned long long)calls,(unsigned long long)hash);
    }
}

// This matches the production overflow capacity and payload. The reference is
// the pre-batching touch_pool policy: metadata + one payload Touch per live VM.
// Padding is initialized to zero; compare all bytes after restore regardless.
struct OverflowCapture {
    multiplayer::RollbackPoolCapture<OverflowCapacity> batched;
    bool runs;
    explicit OverflowCapture(bool value):runs(value){}
    bool capture(Overflow& pool,Netplay::RollbackJournal& journal){
        const auto save=[&](void* p,std::size_t n){return journal.Touch(p,n);};
        if(runs)return batched.Capture(pool,save);
        if(!save(pool.occupied,sizeof(pool.occupied))||!save(&pool.cursor,sizeof(pool.cursor)))return false;
        for(unsigned i=0;i<OverflowCapacity;++i)if(pool.active(i)&&!save(pool.at(i),sizeof(AnmVm)))return false;
        return true;
    }
};
void overflow_workload(unsigned count,bool fragmented,bool reverse){
    auto pool=std::make_unique<Overflow>();AnimationEnvironment environment;
    std::vector<unsigned> slots;
    for(unsigned i=0;i<count;++i){
        // Multiplication by an odd number permutes the power-of-two capacity.
        const auto slot=fragmented?(i*17u)%OverflowCapacity:i;slots.push_back(slot);pool->cursor=slot;
        auto* vm=static_cast<AnmVm*>(pool->allocate(sizeof(AnmVm),true));CHECK(vm==pool->at(slot));
        new(vm)AnmVm{};environment.initialize(*vm,i);
    }
    const auto initial=bytes(pool.get(),sizeof(*pool));
    const auto update=[&](){for(auto slot:slots)CHECK(static_cast<AnmVm*>(pool->at(slot))->update(environment)==0);};
    for(unsigned frame=0;frame<ReplayFrames;++frame)update();
    const auto expected_final=bytes(pool.get(),sizeof(*pool));
    for(bool runs:std::array<bool,2>{reverse,!reverse}){
        std::memcpy(pool.get(),initial.data(),initial.size());
        Netplay::RollbackJournal journal;configure(journal,sizeof(*pool),OverflowCapacity);
        OverflowCapture capture(runs);std::size_t saved_bytes=0,blocks=0;
        for(unsigned frame=0;frame<ReplayFrames;++frame){
            CHECK(journal.BeginFrame(frame));CHECK(capture.capture(*pool,journal));
            saved_bytes=journal.BytesForFrame(frame);blocks=journal.BlocksForFrame(frame);
            update();CHECK(journal.EndFrame());
        }
        const auto final=bytes(pool.get(),sizeof(*pool));
        CHECK(final==expected_final);
        CHECK(journal.UndoTo(0));CHECK(std::memcmp(pool.get(),initial.data(),initial.size())==0);
        for(unsigned frame=0;frame<ReplayFrames;++frame)update();
        CHECK(std::memcmp(pool.get(),expected_final.data(),expected_final.size())==0);
        std::printf("{\"suite\":\"overflow-anm-validation\",\"instances\":%u,\"capacity\":%u,\"layout\":\"%s\",\"capture\":\"%s\",\"payload_bytes\":%zu,\"slot_stride\":%zu,\"journal_bytes\":%zu,\"journal_blocks\":%zu,\"rewind_byte_equal\":true,\"replay_byte_equal\":true,\"reference_byte_equal\":true}\n",count,OverflowCapacity,fragmented?"fragmented":"packed",runs?"runs":"per-slot",sizeof(AnmVm),sizeof(Overflow::Storage),saved_bytes,blocks);
        // Prime all journal ring slots and code paths before measuring. Every
        // trial records 12 checkpoints then rewinds, reusing those same arenas.
        for(unsigned pass=0;pass<2+Trials;++pass){
            std::memcpy(pool.get(),initial.data(),initial.size());double capture_ms=0,update_ms=0;
            const auto growths=journal.ArenaGrowths();
            for(unsigned frame=0;frame<ReplayFrames;++frame){
                auto begin=Clock::now();CHECK(journal.BeginFrame(frame));CHECK(capture.capture(*pool,journal));capture_ms+=milliseconds(begin);
                begin=Clock::now();update();update_ms+=milliseconds(begin);CHECK(journal.EndFrame());
            }
            const auto begin=Clock::now();CHECK(journal.UndoTo(0));const double undo_ms=milliseconds(begin);
            CHECK(std::memcmp(pool.get(),initial.data(),initial.size())==0);
            if(pass>=2){
                CHECK(journal.ArenaGrowths()==growths);
                std::printf("{\"suite\":\"overflow-anm-timing\",\"instances\":%u,\"layout\":\"%s\",\"capture\":\"%s\",\"trial\":%u,\"frames\":%u,\"capture_ms\":%.6f,\"update_ms\":%.6f,\"undo_ms\":%.6f,\"journal_bytes\":%zu,\"journal_blocks\":%zu,\"arena_growths\":0}\n",count,fragmented?"fragmented":"packed",runs?"runs":"per-slot",pass-2,ReplayFrames,capture_ms,update_ms,undo_ms,saved_bytes,blocks);
            }
        }
    }
}
} // namespace
int main(int argc,char** argv){
    const bool reverse=argc>1&&std::strcmp(argv[1],"--reverse")==0;
    arithmetic_mode(Precision::Single,Rounding::NearestEven);
    std::puts("{\"suite\":\"scope\",\"runtime\":\"wasm32-wasip1\",\"workload\":\"constructed resource-free actual TH10 ANM engine CPU workload\",\"game_fps\":false,\"gpu\":false,\"mobile_measurement\":false,\"native_warmup_frames\":24,\"trials\":5}");
    native_workload(1200);native_workload(4096);
    overflow_workload(1200,false,reverse);overflow_workload(1200,true,reverse);overflow_workload(2048,false,reverse);
    std::puts("{\"suite\":\"result\",\"passed\":true}");
}
