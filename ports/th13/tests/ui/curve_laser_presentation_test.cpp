// H-CURVE-DRAW: original ECL711 -> real bullet.anm -> production strip submission.
#include "../ecl/ecl_test_fixtures.hpp"
#include "game/LaserPresentation.hpp"
using namespace th13;
namespace {
struct Graphics final:ZunGraphics {
    touhou::graphics::PipelineState state;
    std::vector<AnmGeometryVertex> strips;
    touhou::graphics::PipelineState& pipeline()override{return state;}
    u32 texture(const AnmResource&,u32 id)override{return id+1;}
    void bind_texture(u32)override{}
    void set_layout(touhou::graphics::VertexLayout)override{}
    void set_matrix(touhou::graphics::MatrixKind,const Matrix4&)override{}
    void triangles(u32,const void*,u32)override{}
    void primitives(touhou::graphics::Topology kind,u32 count,const void* data,u32 stride)override {
        CHECK(kind==touhou::graphics::Topology::Strip);CHECK_EQ(stride,sizeof(AnmGeometryVertex));
        const auto* v=static_cast<const AnmGeometryVertex*>(data);strips.insert(strips.end(),v,v+count+2);
    }
    bool select_target(const AnmResource*,u32)override{return true;}
    bool clear_target(u32,const GraphicsViewport*)override{return true;}
    bool clear_depth(const GraphicsViewport*)override{return true;}
    void set_viewport(const GraphicsViewport&)override{}
};
void real_card(const char* file,const char* routine,i32 samples,i32 color) {
    GameRandom rng{0};AnmEnvironment env;AnmManager manager{rng,env};
    Graphics graphics;AnmRenderer renderer{graphics};StageSim sim{&rng};
    LaserPresentation presentation{manager,renderer,sim,5};
    std::vector<u8> bytes;CHECK(th13test::TestFileReader{}.read("artifacts/reference/raw/bullet.anm",bytes));
    CHECK(manager.load(5,bytes.data(),u32(bytes.size())));
    CHECK(sim.load("artifacts/reference/raw","artifacts/verification/curve-production-20261027/card-draw-events.jsonl",file));
    StageSpawnArguments args;args.integers[1]=16;
    sim.spawn_enemy(routine,0,128,100,nullptr,false,false,&args);
    CHECK(sim.error().empty());CHECK(!sim.lasers.empty());if(sim.lasers.empty())return;
    const auto generation=sim.lasers[0].generation;
    const auto* body=presentation.body(generation);CHECK(body);if(!body)return;
    CHECK_EQ(body->source_script,113);CHECK_EQ(body->visual.sprite,526+color);
    const auto clock=body->timer.current;
    for(int i=0;i<80;++i)sim.advance_lasers();
    CHECK(sim.error().empty());CHECK_EQ(body->timer.current,clock);
    CHECK(presentation.draw());renderer.flush();CHECK(presentation.error.empty());
    CHECK_EQ(graphics.strips.size(),sim.lasers.size()*size_t(samples)*2);
    if(graphics.strips.empty())return;
    CHECK_EQ(graphics.strips[0].uv.x,0.f);
    CHECK_EQ(graphics.strips[0].uv.y,body->visual.uv[0].y);
    CHECK_EQ(graphics.strips[1].uv.y,body->visual.uv[2].y);
    CHECK_EQ(graphics.strips[0].color,0xffffffffu);
    // Curvature must reach submitted geometry; a straight head-tail quad
    // cannot satisfy this across all the adjacent strips.
    bool bent=false;
    for(int i=2;i<samples;++i) {
        const auto& a=graphics.strips[0].position;const auto& b=graphics.strips[2].position;
        const auto& c=graphics.strips[size_t(i)*2].position;
        if(std::abs((b.x-a.x)*(c.y-a.y)-(b.y-a.y)*(c.x-a.x))>.01f)bent=true;
    }
    if(samples==40)CHECK(bent);
}
TEST(stage3_real_card_uses_64_sample_curve_strip){real_card("st03.ecl","BossCard3_at",64,4);}
TEST(stage4_real_card_uses_curved_40_sample_strip){real_card("st04.ecl","BossMiyakoCard2_at",40,8);}
}
int main(){return th13test::run_all("curve-laser-presentation");}
