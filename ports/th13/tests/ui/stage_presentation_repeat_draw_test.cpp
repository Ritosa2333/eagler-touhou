#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"
#include "game/StagePresentation.hpp"
#include <fstream>
#include <vector>
using namespace th13;
using namespace th13::test;
namespace {
struct Graphics final : ZunGraphics {
    touhou::graphics::PipelineState state;
    const AnmVm* player=nullptr;
    std::vector<AnmGeometryVertex> vertices;
    touhou::graphics::PipelineState& pipeline() override { return state; }
    u32 texture(const AnmResource&,u32) override { return 1; }
    void bind_texture(u32) override {}
    void set_layout(touhou::graphics::VertexLayout) override {}
    void set_matrix(touhou::graphics::MatrixKind,const Matrix4&) override {}
    void presentation_range(const AnmVm& vm,u32,u32,u32) override { player=&vm; }
    void triangles(u32 n,const void* data,u32 stride) override {
        if(stride!=sizeof(AnmGeometryVertex)) return;
        const auto* v=static_cast<const AnmGeometryVertex*>(data);
        vertices.insert(vertices.end(),v,v+3*n);
    }
    void primitives(touhou::graphics::Topology,u32,const void*,u32) override {}
    bool select_target(const AnmResource*,u32) override { return true; }
    bool clear_target(u32,const GraphicsViewport*) override { return true; }
    bool clear_depth(const GraphicsViewport*) override { return true; }
    void set_viewport(const GraphicsViewport&) override {}
};
bool quad_equal(const AnmVisualState& a,const AnmVisualState& b) {
    for(int i=0;i<4;++i)
        if(a.quad[i].x!=b.quad[i].x || a.quad[i].y!=b.quad[i].y || a.quad[i].z!=b.quad[i].z) return false;
    return a.inherited_color==b.inherited_color;
}
}
TEST(player_presentation_draw_restores_renderer_scratch_and_repeats_geometry) {
    GameRandom random; AnmEnvironment environment; AnmManager animations(random,environment);
    Graphics graphics; AnmRenderer renderer(graphics); StageBattle battle;
    StagePresentation presentation(animations,renderer,{4,5,6},0);
    const auto empty=synthetic_anm(1,trivial_scripts(1));
    std::ifstream file("/work/artifacts/reference/raw/pl00.anm",std::ios::binary);
    const std::vector<u8> player{std::istreambuf_iterator<char>(file),{}};
    CHECK(!player.empty());
    CHECK(load_bank(animations,4,empty));CHECK(load_bank(animations,5,empty));CHECK(load_bank(animations,6,player));
    battle.player_x=0; battle.player_y=300; battle.defer_item_side=true;
    CHECK(presentation.sync(battle,0));
    CHECK(presentation.draw(1)); renderer.flush();
    CHECK(graphics.player!=nullptr); CHECK_EQ(graphics.vertices.size(),size_t(6));
    if(!graphics.player || graphics.vertices.size()!=6)return;
    auto* vm=const_cast<AnmVm*>(graphics.player);
    // Distinct stored values make the assertion independent of whether the
    // first draw happened to leave identical renderer scratch behind.
    vm->visual.quad[0].x=12345.f;
    vm->visual.inherited_color=0x12345678u;
    const auto baseline=vm->visual; const auto rotation=vm->variables.rotation;
    const auto first=graphics.vertices;
    // Negative control: the underlying renderer changes its scratch quad/color
    // when called directly on this same real, visible presentation VM.
    CHECK_EQ(renderer.draw(*vm),0); renderer.flush();
    CHECK(!quad_equal(baseline,vm->visual));
    vm->visual=baseline; vm->variables.rotation=rotation;
    for(int pass=0;pass<2;++pass) {
        graphics.vertices.clear();
        CHECK(presentation.draw(1)); renderer.flush();
        CHECK(quad_equal(baseline,vm->visual));
        CHECK_EQ(vm->variables.rotation.x,rotation.x);CHECK_EQ(vm->variables.rotation.y,rotation.y);CHECK_EQ(vm->variables.rotation.z,rotation.z);
        CHECK_EQ(graphics.vertices.size(),first.size());
        if(graphics.vertices.size()==first.size()) for(size_t i=0;i<first.size();++i) {
            CHECK_EQ(graphics.vertices[i].position.x,first[i].position.x);
            CHECK_EQ(graphics.vertices[i].position.y,first[i].position.y);
            CHECK_EQ(graphics.vertices[i].position.z,first[i].position.z);
            CHECK_EQ(graphics.vertices[i].color,first[i].color);
        }
    }
}
int main(){return th13test::run_all("stage-presentation-repeat-draw");}
