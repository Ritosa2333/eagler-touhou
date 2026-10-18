#include "../th10_web/cpp/game/StageRenderer.hpp"
#include "../th10_web/cpp/game/GraphicsMath.hpp"
#include "../th10_web/cpp/sdl/NetworkTailPresentation.hpp"
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>
#include <limits>

#ifndef TH_NATIVE_PLATFORM
#error This component test requires the production semantic graphics interface.
#endif
static_assert(sizeof(void*)==4,"Run this component with the WASI32 runner.");

using namespace th10;
using Tail=th10::host::NetworkTailPresentation;

namespace {
const char* active_case="initialization";
u32 checks=0,cases=0,post_only_counterexamples=0;

void require(bool condition,const char* expression,int line) {
    ++checks;
    if(condition)return;
    std::fprintf(stderr,"network-tail-presentation %s:%d: %s\n",
                 active_case,line,expression);
    std::exit(1);
}
#define REQUIRE(condition) require(bool(condition),#condition,__LINE__)

[[noreturn]] void unexpected(const char* operation) {
    std::fprintf(stderr,"network-tail-presentation %s: unexpected %s\n",
                 active_case,operation);
    std::exit(1);
}

enum class CallKind {Pipeline,Texture,Layout,Triangles,Transform,Viewport,Layer,Animation,Clear,Fade};
struct Call {CallKind kind;u32 first=0,second=0;};

struct Device final:AnmRenderEnvironment {
    AnmManager& manager;
    PipelineState state{};
    RenderViewport screen{0,0,640,480};
    std::array<AnmVertex,4> scratch{};
    std::array<AnmVertex,6> submitted_vertices{};
    std::array<Call,256> calls{};
    u32 used=0,triangle_calls=0;
    CameraViewport last_viewport{};
    Matrix4 last_view{},last_projection{};

    explicit Device(AnmManager& value):manager(value) {
        quad=scratch.data();viewport=&screen;global_manager=&manager;
        for(auto& vertex:scratch)vertex.reciprocal_w=1.f;
    }
    void record(CallKind kind,u32 first=0,u32 second=0) {
        REQUIRE(used<calls.size());
        calls[used++]={kind,first,second};
    }
    void reset_trace() {used=0;triangle_calls=0;submitted_vertices={};}
    u32 count(CallKind kind) const {
        u32 result=0;for(u32 i=0;i<used;++i)if(calls[i].kind==kind)++result;
        return result;
    }
    u32 first(CallKind kind,u32 value=0,bool compare_value=false) const {
        for(u32 i=0;i<used;++i)
            if(calls[i].kind==kind&&(!compare_value||calls[i].first==value))return i;
        return used;
    }
    PipelineState& pipeline() override {record(CallKind::Pipeline);return state;}
    void set_texture(void*) override {record(CallKind::Texture);}
    void vertex_format(LayoutParameter layout) override {
        record(CallKind::Layout,static_cast<u32>(layout));
    }
    void draw_triangles(TopologyParameter topology,u32 count,const void* vertices,u32 stride) override {
        record(CallKind::Triangles,static_cast<u32>(topology),count);
        REQUIRE(topology==Primitives::Triangles);
        REQUIRE(count==2&&stride==sizeof(AnmVertex));
        REQUIRE(vertices==manager.vertex_buffer);
        REQUIRE(triangle_calls==0);
        std::memcpy(submitted_vertices.data(),vertices,sizeof(submitted_vertices));
        ++triangle_calls;
    }
    void set_transform(MatrixParameter kind,const Matrix4& matrix) override {
        record(CallKind::Transform,static_cast<u32>(kind));
        for(const auto& row:matrix.elements)for(float value:row)REQUIRE(std::isfinite(value));
        if(kind==Matrices::View)last_view=matrix;
        else if(kind==Matrices::Projection)last_projection=matrix;
        else unexpected("non-camera transform");
    }
    void stream_source(void*,u32) override {unexpected("stream_source");}
    void draw_buffer(TopologyParameter,u32,u32) override {unexpected("draw_buffer");}
    i32 special_draw(AnmManager&,AnmVm&,u32) override {unexpected("special_draw");}
};

// Keep the production Camera and recovered production GraphicsMath. Only the
// external viewport/device calls are recorded; no Stage or Timer is emulated.
struct CameraAdapter final:CameraEnvironment {
    AnmManager* owner;
    Device& device;
    u32 look_at_calls=0,perspective_calls=0,normalize_calls=0;
    CameraAdapter(AnmManager& manager,Device& renderer):owner(&manager),device(renderer) {
        animation_manager=&owner;render_environment=&device;
    }
    void look_at(Matrix4& matrix,const Vec3& eye,const Vec3& target,const Vec3& up) override {
        ++look_at_calls;GraphicsMath::look_at(matrix,eye,target,up);
    }
    void perspective(Matrix4& matrix,float field,float aspect,float near_plane,float far_plane) override {
        ++perspective_calls;GraphicsMath::perspective(matrix,field,aspect,near_plane,far_plane);
    }
    void normalize(Vec3& value) override {++normalize_calls;GraphicsMath::normalize(value,value);}
    void set_viewport(const CameraViewport& view) override {
        device.record(CallKind::Viewport,view.width,view.height);device.last_viewport=view;
    }
    void reset_trace() {look_at_calls=perspective_calls=normalize_calls=0;}
};

struct StageState {
    u32 draw_flags,fade_color;
    i32 previous,current,frame_effect;
    float fractional;
};
StageState observe(const Stage& stage) {
    return {stage.draw_flags,stage.fade_color,stage.fade_timer.previous,
            stage.fade_timer.current,stage.frame_effect,stage.fade_timer.fractional};
}
bool same(const StageState& a,const StageState& b) {
    return a.draw_flags==b.draw_flags&&a.fade_color==b.fade_color&&
        a.previous==b.previous&&a.current==b.current&&a.frame_effect==b.frame_effect&&
        a.fractional==b.fractional;
}

struct StageAdapter final:StageRenderEnvironment {
    Stage& observed_stage;
    AnmManager& manager;
    Device& device;
    std::array<StageState,2> layer_states{};
    u32 layers=0;
    StageAdapter(Stage& stage,AnmManager& owner,Device& renderer,CameraAdapter& view,
                 Camera& world_camera,Camera*& active_camera,u32& screen_flag,
                 u32& fog_flag,float& frame_rate)
        :observed_stage(stage),manager(owner),device(renderer) {
        world=&world_camera;active=&active_camera;screen_space=&screen_flag;
        fog_enabled=&fog_flag;rate=&frame_rate;camera=&view;
    }
    void translation(Matrix4& matrix,const Vec3& offset) override {
        GraphicsMath::translation(matrix,offset);
    }
    void project_points(Vec3* output,const Vec3* input,u32 count,const Camera& view,
                        const Matrix4& transform) override {
        GraphicsMath::project_array(output,sizeof(Vec3),input,sizeof(Vec3),count,
                                   &view.viewport,&view.projection,&view.view,&transform);
    }
    void draw_animation(AnmVm& vm) override {
        device.record(CallKind::Animation);AnmRenderer{manager,device}.draw(vm);
    }
    void draw_layer(u32 layer) override {
        device.record(CallKind::Layer,layer);
        REQUIRE(layers<layer_states.size());
        layer_states[layers++]=observe(observed_stage);
        // The fixture has empty ANM layer lists. Do not mutate game state here.
    }
    void clear(u32 flags,u32 color,const StageClearRect*) override {
        device.record(CallKind::Clear,flags,color);
    }
    void fade(i32 kind,i32 duration) override {
        device.record(CallKind::Fade,static_cast<u32>(kind),static_cast<u32>(duration));
    }
    void reset_trace() {layers=0;layer_states={};}
};

struct Case {
    const char* name;
    u32 flags,color;
    i32 previous,current;
    float fractional,rate;
    i32 expected_previous,expected_current;
    float expected_fractional;
    u32 expected_flags,expected_color;
    bool before_boundary,after_boundary;
};

struct Fixture {
    std::unique_ptr<AnmManager> manager=std::make_unique<AnmManager>();
    Stage stage{};
    StageHeader header{};
    AnmFile animation_file{};
    StageInstance end_instance{-1,0,{0,0,0}};
    Camera world{},ui{};
    Camera* active=nullptr;
    u32 screen_space=1,fog_enabled=0;
    float rate;
    Device device;
    CameraAdapter camera;
    StageAdapter environment;
    AnmVm seed{};
    AnmSprite sprite{};
    u32 texture_token=1;
    std::array<AnmVertex,6> expected_vertices{};

    explicit Fixture(const Case& config)
        :rate(config.rate),device(*manager),camera(*manager,device),
         environment(stage,*manager,device,camera,world,active,screen_space,fog_enabled,rate) {
        stage.file=&header;stage.animation_file=&animation_file;stage.instances=&end_instance;
        stage.draw_flags=config.flags;stage.fade_color=config.color;
        stage.fade_timer={config.previous,config.current,config.fractional,&rate};
        stage.fade_timer_flags=1;stage.draw_distance_squared=1000000.f;
        Camera::initialize(stage.camera,ui);
        stage.camera.target_offset={0,0,-1};
        stage.camera.fog.near_distance=30;stage.camera.fog.far_distance=1200;
        stage.camera.fog.packed_color=0xff102030;
        world=stage.camera;

        sprite.texture=&texture_token;sprite.width=8;sprite.height=12;
        sprite.texture_width=sprite.texture_height=16;
        sprite.u0=sprite.v0=0;sprite.u1=sprite.v1=1;
        animation_file.sprites=&sprite;animation_file.sprite_count=1;
        seed.sprite=&sprite;seed.flags=3;seed.color=0xffffffff;
        seed.scale={1,1};seed.sprite_size={8,12};seed.position={128,128,.5f};
        AnmRenderer renderer{*manager,device};
        renderer.begin_frame();
        REQUIRE(renderer.draw(seed)==0);
        REQUIRE(manager->batch_quads==1);
        REQUIRE(manager->vertex_write==manager->vertex_buffer+6);
        std::memcpy(expected_vertices.data(),manager->vertex_buffer,sizeof(expected_vertices));
        // The real Stage camera flush must consume this real pending ANM batch.
        reset_trace();
    }
    void reset_trace() {device.reset_trace();camera.reset_trace();environment.reset_trace();}
};

// Fixed eligible host inputs isolate the Stage boundary. This component does
// not execute Application/SDL scheduling, RTC, pixel output, or audio.
Tail::Decision decide(std::uint32_t boundaries,double gap_scale=1.) {
    Tail::Sample sample;
    sample.enabled=true;sample.real_fast_sample=true;sample.high_refresh=true;
    sample.completed_forwards=1;sample.complete_frontier=true;
    sample.input_gap_scale=gap_scale;sample.schedule_alpha=.5;sample.gate_ms=12.;
    sample.boundary_flags=boundaries;
    return Tail::Decide(sample);
}

void check_admission_boundaries() {
    active_case="complete-forward-admission-boundaries";
    Tail::Sample eligible;
    eligible.enabled=true;eligible.real_fast_sample=true;eligible.high_refresh=true;
    eligible.completed_forwards=1;eligible.complete_frontier=true;
    eligible.input_gap_scale=1.;eligible.schedule_alpha=.5;eligible.gate_ms=12.;
    eligible.boundary_flags=0;
    REQUIRE(Tail::Decide(eligible)==Tail::Decision::Accept);
    auto sample=eligible;
    sample.enabled=false;REQUIRE(Tail::Decide(sample)==Tail::Decision::Disabled);
    sample=eligible;sample.real_fast_sample=false;
    REQUIRE(Tail::Decide(sample)==Tail::Decision::NotRealFastSample);
    sample=eligible;sample.high_refresh=false;
    REQUIRE(Tail::Decide(sample)==Tail::Decision::NotHighRefresh);
    sample=eligible;sample.completed_forwards=0;
    REQUIRE(Tail::Decide(sample)==Tail::Decision::NotSingleForward);
    sample=eligible;sample.completed_forwards=2;
    REQUIRE(Tail::Decide(sample)==Tail::Decision::NotSingleForward);
    sample=eligible;sample.complete_frontier=false;
    REQUIRE(Tail::Decide(sample)==Tail::Decision::IncompleteFrontier);
    // A finite interior double can round to a float endpoint. Such a Draw
    // would consume RFP1 while the host stayed primed, so reject both edges.
    sample=eligible;sample.schedule_alpha=std::nextafter(1.,0.);
    REQUIRE(Tail::Decide(sample)==Tail::Decision::AlphaNotInterior);
    sample=eligible;sample.schedule_alpha=1.e-300;
    REQUIRE(Tail::Decide(sample)==Tail::Decision::AlphaNotInterior);
    sample=eligible;sample.schedule_alpha=0.;
    REQUIRE(Tail::Decide(sample)==Tail::Decision::AlphaNotInterior);
    sample=eligible;sample.schedule_alpha=1.;
    REQUIRE(Tail::Decide(sample)==Tail::Decision::AlphaNotInterior);
    sample=eligible;sample.gate_ms=10.;
    REQUIRE(Tail::Decide(sample)==Tail::Decision::Accept);
    sample.gate_ms=std::nextafter(14.,0.);
    REQUIRE(Tail::Decide(sample)==Tail::Decision::Accept);
    sample.gate_ms=14.;
    REQUIRE(Tail::Decide(sample)==Tail::Decision::GateOutsideWindow);
    sample.gate_ms=std::nextafter(10.,0.);
    REQUIRE(Tail::Decide(sample)==Tail::Decision::GateOutsideWindow);
    sample=eligible;sample.gate_ms=std::numeric_limits<double>::quiet_NaN();
    REQUIRE(Tail::Decide(sample)==Tail::Decision::InvalidSample);
    sample=eligible;sample.input_gap_scale=std::nextafter(1.,0.);
    REQUIRE(Tail::Decide(sample)==Tail::Decision::InvalidSample);
    sample=eligible;sample.input_gap_scale=std::numeric_limits<double>::infinity();
    REQUIRE(Tail::Decide(sample)==Tail::Decision::InvalidSample);
    for(unsigned bit=0;bit!=13;++bit) {
        sample=eligible;sample.boundary_flags=1u<<bit;
        REQUIRE(Tail::Decide(sample)==Tail::Decision::AuthoredBoundary);
    }
}

void run(const Case& config) {
    active_case=config.name;
    Fixture fixture(config);
    const auto before=observe(fixture.stage);
    const bool before_boundary=Tail::StageHasAuthoredBoundary(&fixture.stage);
    REQUIRE(before_boundary==config.before_boundary);
    std::uint32_t boundaries=before_boundary?Tail::CurrentStageBoundary:0u;

    const i32 result=StageRenderer{fixture.stage,fixture.environment}.draw_foreground();
    const auto after=observe(fixture.stage);
    const bool after_boundary=Tail::StageHasAuthoredBoundary(&fixture.stage);
    if(after_boundary)boundaries|=Tail::CurrentStageBoundary;

    REQUIRE(result==1);
    REQUIRE(after.previous==config.expected_previous);
    REQUIRE(after.current==config.expected_current);
    REQUIRE(after.fractional==config.expected_fractional);
    REQUIRE(after.draw_flags==config.expected_flags);
    REQUIRE(after.fade_color==config.expected_color);
    REQUIRE(after_boundary==config.after_boundary);
    REQUIRE(fixture.stage.fade_timer.rate==&fixture.rate);
    REQUIRE(fixture.stage.fade_timer_flags==1);
    REQUIRE(fixture.stage.frame_effect==0);
    REQUIRE(fixture.environment.layers==2);
    REQUIRE(same(fixture.environment.layer_states[0],before));
    REQUIRE(same(fixture.environment.layer_states[1],before));
    REQUIRE(fixture.device.first(CallKind::Layer,17,true)<
            fixture.device.first(CallKind::Layer,18,true));

    REQUIRE(fixture.device.triangle_calls==1);
    REQUIRE(fixture.manager->batch_quads==0);
    REQUIRE(fixture.manager->flushed_batches==1);
    REQUIRE(std::memcmp(fixture.expected_vertices.data(),fixture.device.submitted_vertices.data(),
                        sizeof(fixture.expected_vertices))==0);
    REQUIRE(fixture.device.first(CallKind::Triangles)<fixture.device.first(CallKind::Transform));
    REQUIRE(fixture.device.first(CallKind::Triangles)<fixture.device.first(CallKind::Layer,17,true));
    REQUIRE(fixture.device.count(CallKind::Transform)==10);
    REQUIRE(fixture.device.count(CallKind::Viewport)==5);
    REQUIRE(fixture.camera.look_at_calls==5);
    REQUIRE(fixture.camera.perspective_calls==5);
    REQUIRE(fixture.camera.normalize_calls==5);
    REQUIRE(fixture.device.count(CallKind::Clear)==0);
    REQUIRE(fixture.device.count(CallKind::Fade)==0);
    REQUIRE(fixture.active==&fixture.world&&fixture.screen_space==0);
    REQUIRE(fixture.fog_enabled==0&&!fixture.device.state.fog);
    REQUIRE(!fixture.device.state.depthWrite&&fixture.device.state.depthCompare==Compare::Always);
    REQUIRE(fixture.manager->tint_enabled==0&&fixture.manager->tint==0x80808080);

    const auto decision=decide(boundaries);
    REQUIRE(decision==(config.before_boundary||config.after_boundary
                       ?Tail::Decision::AuthoredBoundary:Tail::Decision::Accept));
    // The exact same real Stage mutation must remain protected at ordinary,
    // threshold and deep network scales. A gap is not a rendering boundary.
    REQUIRE(decide(boundaries,3.)==decision);
    REQUIRE(decide(boundaries,4.)==decision);
    if(config.before_boundary&&!config.after_boundary) {
        // A post-Draw-only check accepts these real completed-fade states.
        // The latched production pre-Draw predicate must still veto this frame.
        REQUIRE(after.previous==1&&after.current==0&&after.fractional==0);
        REQUIRE(decide(0)==Tail::Decision::Accept);
        REQUIRE(decision==Tail::Decision::AuthoredBoundary);
        ++post_only_counterexamples;
    }

    if(config.expected_flags&8u) {
        fixture.reset_trace();
        const auto hidden=observe(fixture.stage);
        REQUIRE((StageRenderer{fixture.stage,fixture.environment}.draw_foreground()==1));
        REQUIRE(same(observe(fixture.stage),hidden));
        REQUIRE(fixture.device.used==0&&fixture.environment.layers==0);
        REQUIRE(fixture.camera.look_at_calls==0);
        REQUIRE(Tail::StageHasAuthoredBoundary(&fixture.stage));
    }
    ++cases;
    std::printf("{\"case\":\"%s\",\"before_boundary\":%s,\"after_boundary\":%s,"
                "\"before_flags\":%u,\"after_flags\":%u,\"after_current\":%d,"
                "\"after_fractional\":%.3f,\"decision\":%u,\"passed\":true}\n",
                config.name,before_boundary?"true":"false",after_boundary?"true":"false",
                before.draw_flags,after.draw_flags,after.current,double(after.fractional),
                static_cast<unsigned>(decision));
}
}

int main() {
    arithmetic_mode(Precision::Single,Rounding::NearestEven);
    check_admission_boundaries();
    const Case scenarios[]={
        {"stable-fade-zero",1,0x00ffffff,1,0,0,1,1,0,0,1,0x00ffffff,false,false},
        {"last-fade-visible",1,0xa0402010,2,1,1,1,1,0,0,1,0x00ffffff,true,false},
        {"last-fade-timer-only",1,0x00ffffff,2,1,1,1,1,0,0,1,0x00ffffff,true,false},
        {"last-fade-hides-stage",3,0xa0402010,2,1,1,1,1,0,0,9,0x00ffffff,true,true},
        {"half-rate-middle",1,0x00ffffff,3,2,2,.5f,2,1,1.5f,1,0x00ffffff,true,true},
        {"half-rate-truncated-last",1,0xa0402010,2,1,1,.5f,1,0,.5f,1,0x00ffffff,true,true}
    };
    for(const auto& scenario:scenarios)run(scenario);
    REQUIRE(cases==6&&post_only_counterexamples==2);
    std::printf("{\"suite\":\"network-tail-presentation-stage-boundary\","
                "\"cases\":%u,\"checks\":%u,\"post_only_counterexamples\":%u,"
                "\"real_stage_timer_camera_anm\":true,\"real_graphics_math\":true,"
                "\"scope\":\"WASI32 production component; recorded device calls; "
                "no Application lifecycle, browser, pixels, audio or performance claim\","
                "\"passed\":true}\n",cases,checks,post_only_counterexamples);
    return 0;
}
