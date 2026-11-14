#include "anm_test_fixtures.hpp"
#include "game/AnmEnvironment.hpp"
#include "game/AnmRenderer.hpp"
using namespace th13;
namespace {
u32 bits(float f){u32 x;std::memcpy(&x,&f,4);return x;}
struct Graphics final:ZunGraphics {
    touhou::graphics::PipelineState state;
    u32 count=0,stride=0;bool fog=false;float view_x=0,draw_view_x=0;
    touhou::graphics::PipelineState& pipeline()override{return state;}
    u32 texture(const AnmResource&,u32)override{return 1;}
    void bind_texture(u32)override{}
    void set_layout(touhou::graphics::VertexLayout)override{}
    void set_matrix(touhou::graphics::MatrixKind kind,const Matrix4& m)override {
        if(kind==touhou::graphics::MatrixKind::View)view_x=m.m[12];
    }
    void triangles(u32,const void*,u32)override{}
    void primitives(touhou::graphics::Topology,u32 n,const void*,u32 s)override {
        count+=n;stride=s;fog=state.fog;draw_view_x=view_x;
    }
    bool select_target(const AnmResource*,u32)override{return true;}
    bool clear_target(u32,const GraphicsViewport*)override{return true;}
    bool clear_depth(const GraphicsViewport*)override{return true;}
    void set_viewport(const GraphicsViewport&)override{}
};
TEST(wind_native_initialization_rng_and_history_samples) {
    AnmEnvironment env;env.screen_translation={10,20,30};env.reference_position={0,0,1};
    env.stage_direction={0,0,1};env.background_delta={.125,.25,-.5};env.stage_difficulty=1;
    GameRandom random{0};AnmWindTrail wind;wind.initialize(random,env);
    CHECK_EQ(random.seed(),32508);CHECK_EQ(random.index(),21u);
    CHECK_EQ(wind.color,0x8091bb66u);
    CHECK(wind.update(env,128,1));
    CHECK_EQ(bits(wind.points[0].x),3263177892u);
    CHECK_EQ(bits(wind.points[0].y),3282352718u);
    CHECK_EQ(bits(wind.points[0].z),1137078829u);
    CHECK_EQ(bits(wind.vertices[0].position.x),3263832924u);
    for(int f=1;f<80;++f)CHECK(wind.update(env,128,1));
    CHECK_EQ(bits(wind.points[0].x),3266271069u);
    CHECK_EQ(bits(wind.points[0].y),1145162470u);
    CHECK_EQ(bits(wind.points[0].z),1145815878u);
    CHECK_EQ(random.index(),21u); // updates do not consume RNG
}
TEST(wind_real_effect_script_fades_and_retires_through_manager) {
    GameRandom random{0};AnmEnvironment env;env.reference_position={0,0,1};env.stage_direction={0,0,1};
    AnmManager manager{random,env};th13test::TestFileReader reader;std::vector<u8> bytes;
    CHECK(reader.read("artifacts/reference/raw/ascii.anm",bytes));CHECK(manager.load(2,bytes.data(),u32(bytes.size())));CHECK(manager.sprite_fallback(2));
    CHECK(reader.read("artifacts/reference/raw/effect.anm",bytes));CHECK(manager.load(8,bytes.data(),u32(bytes.size())));
    const u32 handle=manager.create(8,136,-1,0);auto* vm=manager.registry.find(handle);
    CHECK(vm);if(!vm)return;
    CHECK(manager.spawn_effect(*vm,6));CHECK(vm->geometry.wind);
    CHECK_EQ(vm->visual.layer,2);CHECK_EQ(vm->visual.sprite,43);
    CHECK_EQ(vm->interpolators.alpha.duration,160);
    const u32 draws=random.index();
    for(int f=0;f<120;++f)CHECK(manager.update());
    CHECK(manager.registry.find(handle));CHECK_EQ(random.index(),draws);
    Graphics graphics;AnmRenderer renderer{graphics};
    renderer.camera.view.m[12]=99;
    renderer.scenery_camera.view.m[12]=12;renderer.scenery_camera_ready=true;
    CHECK_EQ(renderer.draw(*vm),0);
    CHECK_EQ(graphics.count,62u);CHECK_EQ(graphics.stride,24u);CHECK(graphics.fog);
    CHECK_EQ(graphics.draw_view_x,12.f);CHECK_EQ(renderer.camera.view.m[12],99.f);
    CHECK_EQ(graphics.view_x,99.f);CHECK(!graphics.state.fog);
    for(int f=120;f<220;++f)CHECK(manager.update());
    CHECK(!manager.registry.find(handle));CHECK(manager.error.empty());
}
}
int main(){return th13test::run_all("anm-wind-trail");}
