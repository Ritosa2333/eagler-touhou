// H-ENEMY-FOG: ECL629 ->17x17 geometry ->text82/layer29 production strips.
#include "../ecl/ecl_test_fixtures.hpp"
#include "game/StagePresentation.hpp"
#include <cmath>
using namespace th13;
namespace {
struct Graphics final:ZunGraphics {
    touhou::graphics::PipelineState state;u32 strips=0,vertices=0;bool target=false;
    touhou::graphics::PipelineState& pipeline()override{return state;}
    u32 texture(const AnmResource& file,u32 id)override{target=file.textures[id].kind==AnmTexture::Kind::RenderTarget;return id+1;}
    void bind_texture(u32)override{}
    void set_layout(touhou::graphics::VertexLayout)override{}
    void set_matrix(touhou::graphics::MatrixKind,const Matrix4&)override{}
    void triangles(u32,const void*,u32)override{}
    void primitives(touhou::graphics::Topology kind,u32 n,const void*,u32 stride)override{
        CHECK(kind==touhou::graphics::Topology::Strip);CHECK_EQ(n,32u);
        CHECK_EQ(stride,sizeof(AnmGeometryVertex));++strips;vertices+=n+2;
    }
    bool select_target(const AnmResource*,u32)override{return true;}
    bool clear_target(u32,const GraphicsViewport*)override{return true;}
    bool clear_depth(const GraphicsViewport*)override{return true;}
    void set_viewport(const GraphicsViewport&)override{}
};
struct Rig {
    GameRandom rng;AnmEnvironment env;AnmManager manager{rng,env};Graphics graphics;
    AnmRenderer renderer{graphics};StagePresentation presentation{manager,renderer,{4,5,6,8,9},31};
    StageSim sim;StageEnemy enemy;EclContext context;
    bool init(){std::vector<u8>b;if(!th13test::TestFileReader{}.read("artifacts/reference/raw/text.anm",b)||!manager.load(0,b.data(),u32(b.size())))return false;
        sim.commands.sim=&sim;sim.current=&enemy;sim.enemy_fog=[&](StageEnemy& e,bool reset){return presentation.enemy_fog(e,reset);};return true;}
    int configure(float r,u32 color){std::vector<u8>b;th13test::emit(b,0,629,0,255,0,{float_to_bits(r),color});context.instruction=reinterpret_cast<const EclInstruction*>(b.data());return sim.commands.execute(context,629);}
};
TEST(enemy_fog_ecl_reset_disable_and_retire_do_not_leak_strips) {
    Rig r;CHECK(r.init());CHECK_EQ(r.configure(128,5181359),0);
    CHECK(r.enemy.fog);CHECK_EQ(r.enemy.fog->radius,16.f);
    CHECK(r.manager.update());CHECK_EQ(r.manager.registry.layer(29).size(),16u);
    CHECK_EQ(r.configure(160,3190703),0);CHECK_EQ(r.enemy.fog->radius,16.f);
    CHECK(r.manager.update());CHECK_EQ(r.manager.registry.layer(29).size(),16u);
    CHECK_EQ(r.configure(0,0),0);CHECK(!r.enemy.fog);CHECK(r.manager.update());
    CHECK(r.manager.registry.layer(29).empty());
    CHECK_EQ(r.configure(128,5181359),0);r.presentation.retire_enemy_fog(r.enemy.serial);
    CHECK(r.manager.update());CHECK(r.manager.registry.layer(29).empty());
}
TEST(enemy_fog_production_submits_same_grid_after_anm_tick) {
    Rig r;CHECK(r.init());CHECK_EQ(r.configure(128,5181359),0);
    r.enemy.fog->update({0,128,0});CHECK(r.presentation.enemy_fog(r.enemy,false));
    CHECK_EQ(r.enemy.fog->radius,18.f);
    CHECK(r.manager.update());const auto& layer=r.manager.registry.layer(29);CHECK_EQ(layer.size(),16u);
    for(size_t col=0;col<layer.size();++col){auto* vm=layer[col];CHECK_EQ(vm->source_script,82);CHECK_EQ(vm->visual.draw_mode(),12u);
        // 0x43cac0 mask 0xd9ffffff|0x18000000 only replaces the draw mode and
        // preserves script82's op303 blend field (3 = One/Zero opaque strip).
        CHECK_EQ(vm->visual.blend_mode(),3u);
        CHECK_EQ(vm->visual.layer,29);
        CHECK_EQ(vm->geometry.screen_vertices.size(),34u);
        for(size_t row=0;row<17;++row){
            CHECK(std::memcmp(&vm->geometry.screen_vertices[row*2],&r.enemy.fog->vertices[col*17+row],28)==0);
            CHECK(std::memcmp(&vm->geometry.screen_vertices[row*2+1],&r.enemy.fog->vertices[(col+1)*17+row],28)==0);
        }
    }
    CHECK(r.renderer.draw_layer(layer));r.renderer.flush();CHECK(r.graphics.target);
    CHECK_EQ(r.graphics.strips,16u);CHECK_EQ(r.graphics.vertices,544u);
}
TEST(enemy_fog_geometry_matches_original_grid_uv_and_playfield_clamps) {
    // 0x415ae0 builds both the source and destination grid in the 640x480 @R
    // domain: local_10 = (enemy.x - r - 20) + 320, fVar3 = (enemy.y - r - 20)
    // + 16; span = 2r + 40 with 17 samples. 0x41a3d0 UVs are source/640 and
    // source/480, clamps x<=128→129 / x>512→511 and y<=16→17 / y>=464→463.
    EnemyFog fog;
    fog.target=fog.radius=32.0f;
    fog.color=0x00102030u;
    fog.update({140.0f,0.0f,0.0f});
    const auto& top_left=fog.vertices[0];
    CHECK_EQ(top_left.position.x,408.0f);   // (140-32-20)+320
    CHECK_EQ(top_left.position.y,17.0f);    // (-32-20)+16 = -36 -> clamp y<=16
    CHECK(std::fabs(top_left.uv.x-408.0f/640.0f)<1e-6f);
    CHECK(std::fabs(top_left.uv.y-17.0f/480.0f)<1e-6f);
    CHECK_EQ(top_left.color&0xff000000u,0u);  // outside radius: alpha 0
    // Column 16 starts at exactly x=512. 0x41a3d0 clamps only when x > 512.
    const auto& east=fog.vertices[16*EnemyFog::side];
    CHECK_EQ(east.position.x,512.0f);
    CHECK(std::fabs(east.uv.x-0.8f)<1e-6f); // 512/640, not 511/640
    CHECK_EQ(east.color&0xff000000u,0u);
    // An interior sample keeps alpha 255.
    const auto& inside=fog.vertices[8*EnemyFog::side+4];
    CHECK_EQ(inside.color&0xff000000u,0xff000000u);
    CHECK(std::fabs(inside.uv.x-460.0f/640.0f)<1e-6f); // 408 + 8*(104/16)
    // Bottom clamp: y=464 collapses to 463 (0x41a3d0 `464.0 <= y`).
    EnemyFog bottom;
    bottom.target=bottom.radius=32.0f;
    bottom.update({0.0f,396.0f,0.0f});
    const auto& south=bottom.vertices[16];
    CHECK_EQ(south.position.y,463.0f);
    CHECK(std::fabs(south.uv.y-463.0f/480.0f)<1e-6f);
}
TEST(real_stage1_mboss_ecl_updates_fog_in_constructor_and_later_ticks) {
    Rig r;CHECK(r.init());CHECK(r.sim.load("artifacts/reference/raw",
        "artifacts/verification/enemy-fog-20261027/mboss-events.jsonl","st01.ecl"));
    auto* enemy=r.sim.spawn_enemy("MBoss",144,-16,40,nullptr,false);
    CHECK(enemy&&enemy->fog);if(!enemy||!enemy->fog)return;
    CHECK_EQ(enemy->fog->target,128.f);CHECK_EQ(enemy->fog->color,5181359u);
    CHECK_EQ(enemy->fog->radius,18.f); // constructor41a8a0 includes41a3d0
    CHECK(r.manager.update());CHECK_EQ(r.manager.registry.layer(29).size(),16u);
    for(r.sim.frame=1;r.sim.frame<=80;++r.sim.frame)CHECK(r.sim.step_frame());
    CHECK_EQ(enemy->fog->radius,128.f);CHECK(r.sim.error().empty());
    r.presentation.retire_enemy_fog(enemy->serial);CHECK(r.manager.update());
    CHECK(r.manager.registry.layer(29).empty());
}
}
int main(){return th13test::run_all("enemy-fog");}
