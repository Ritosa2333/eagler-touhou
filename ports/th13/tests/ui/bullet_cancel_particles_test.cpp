#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"
#include "game/StageBattle.hpp"
#include "game/StagePresentation.hpp"
#include "game/BulletAppearance.hpp"
#include <fstream>
using namespace th13;using namespace th13::test;
namespace {
struct Graphics final : ZunGraphics {
    touhou::graphics::PipelineState state;
    std::vector<touhou::graphics::PipelineState> draws;
    touhou::graphics::PipelineState& pipeline() override { return state; }
    u32 texture(const AnmResource&, u32) override { return 1; }
    void bind_texture(u32) override {}
    void set_layout(touhou::graphics::VertexLayout) override {}
    void set_matrix(touhou::graphics::MatrixKind, const Matrix4&) override {}
    void triangles(u32 n, const void*, u32) override { if (n) draws.push_back(state); }
    void primitives(touhou::graphics::Topology, u32 n, const void*, u32) override { if (n) draws.push_back(state); }
    bool select_target(const AnmResource*, u32) override { return true; }
    bool clear_target(u32, const GraphicsViewport*) override { return true; }
    bool clear_depth(const GraphicsViewport*) override { return true; }
    void set_viewport(const GraphicsViewport&) override {}
};

TEST(clear_bullets_spawns_independent_particle_with_original_motion_channel) {
 GameRandom rng;AnmEnvironment env;AnmManager animations(rng,env);Graphics graphics;AnmRenderer renderer(graphics);
 std::ifstream f("artifacts/reference/raw/bullet.anm",std::ios::binary);std::vector<u8> bytes{std::istreambuf_iterator<char>(f),{}};
 CHECK(load_bank(animations,5,bytes));StagePresentation presentation(animations,renderer,{4,5,6},0);
 StageSim sim;SessionState session;sim.session=&session;
 int callbacks=0;sim.bullet_cancel_effect=[&](i32 script,const Vec3&pos,const Vec3&delta){++callbacks;CHECK_EQ(script,139);return presentation.bullet_cancel(script,pos,delta);};
 StageBullet b;b.sprite=0;b.subtype=4;b.x=10;b.y=100;b.z=.1f;b.vel[0]=2;b.vel[1]=-3;b.vel[2]=.25f;sim.bullets.push_back(b);
 sim.clear_bullets();CHECK_EQ(callbacks,1);CHECK(sim.error().empty());CHECK(sim.bullets[0].alive);CHECK_EQ(sim.bullets[0].state,3);
 auto* vm=animations.registry.find(animations.registry.ordered_handle(false,0));CHECK(vm);if(!vm)return;
 CHECK_EQ(vm->source_script,139);CHECK_EQ(vm->visual.translation.x,10.f);CHECK_EQ(vm->visual.translation.y,100.f);CHECK(vm->visual.render_flags&0x40000u);
 CHECK_EQ(vm->interpolators.position.duration,30);CHECK_EQ(vm->interpolators.position.mode,6);
 CHECK_EQ(vm->interpolators.position.start[0],0.f);CHECK_EQ(vm->interpolators.position.end[0],20.f);
 CHECK_EQ(vm->interpolators.position.end[1],-30.f);CHECK_EQ(vm->interpolators.position.end[2],2.5f);
 CHECK_EQ(vm->interpolators.position.timer.current,0);
 sim.clear_bullets();CHECK_EQ(callbacks,1);
 //Script139 is a controller: its child138 owns the visible fragment.
 renderer.set_viewport({0,0,640,480});
 for(int i=0;i<20;++i){CHECK(animations.update());for(u32 layer=0;layer<33;++layer)CHECK(renderer.draw_layer(animations.registry.layer(layer)));renderer.flush();}
 CHECK(!graphics.draws.empty());
}
TEST(cancel_particle_script_uses_original_type_color_mapping) {
 CHECK_EQ(bullet_cancel_script(0,4),139);CHECK_EQ(bullet_cancel_script(17,2),8);
 CHECK_EQ(bullet_cancel_script(24,0),197);CHECK_EQ(bullet_cancel_script(25,0),199);
 CHECK_EQ(bullet_cancel_script(26,0),201);CHECK_EQ(bullet_cancel_script(27,0),207);
}
TEST(etex_cancel_uses_the_same_particle_once) {
 StageSim sim;int particles=0;
 sim.bullet_cancel_effect=[&](i32 script,const Vec3&,const Vec3& delta){++particles;CHECK_EQ(script,139);CHECK_EQ(delta.x,20.f);return true;};
 StageBullet b;b.sprite=0;b.subtype=4;b.vel[0]=2;b.state_age=14;b.ex[0].op=0x400;
 sim.process_bullet_queue(b,false);CHECK(sim.error().empty());CHECK_EQ(particles,1);
 CHECK_EQ(b.state,3);CHECK_EQ(b.state_age,0);CHECK_EQ(b.animation_interrupt,1);
 CHECK(sim.cancel_bullet(b));CHECK_EQ(particles,1);
}
TEST(respawn_cancel_retains_stationary_fade_and_creates_uninterpolated_particle) {
 GameRandom rng;AnmEnvironment env;AnmManager animations(rng,env);Graphics graphics;AnmRenderer renderer(graphics);
 std::ifstream f("artifacts/reference/raw/bullet.anm",std::ios::binary);std::vector<u8> bytes{std::istreambuf_iterator<char>(f),{}};
 CHECK(load_bank(animations,5,bytes));StagePresentation presentation(animations,renderer,{4,5,6},0);
 StageSim sim;sim.set_time_rate(.5f);
 sim.stationary_cancel_effect=[&](i32 script,const Vec3& pos){return presentation.bullet_cancel(script,pos,{},false);};
 StageBullet b;b.sprite=0;b.subtype=4;b.generation=1;b.x=30;b.y=200;b.vel[0]=2;b.vel[1]=-3;
 CHECK(presentation.tick_bullet(b,true));sim.bullets.push_back(b);
 sim.bullet_animation=[&](StageBullet& v,bool birth){return presentation.tick_bullet(v,birth);};
 CHECK(sim.cancel_respawn_bullets({0,200,0},640));
 CHECK_EQ(sim.bullets[0].state,4);CHECK_EQ(sim.bullets[0].x,30.5f);CHECK_EQ(sim.bullets[0].y,199.25f);
 auto* vm=animations.registry.find(animations.registry.ordered_handle(false,0));CHECK(vm);
 if(vm){CHECK_EQ(vm->source_script,139);CHECK_EQ(vm->interpolators.position.duration,0);CHECK_EQ(vm->visual.translation.x,30.f);}
 for(int i=0;i<60 && !sim.bullets.empty();++i){sim.advance_bullets();CHECK(sim.error().empty());if(!sim.bullets.empty())CHECK_EQ(sim.bullets[0].x,30.5f);}
 CHECK(sim.bullets.empty());
}
TEST(respawn_cancel_matches_native_state_protection_and_inclusive_radius_cases) {
 struct Case {i32 state,protection;float x;i32 out;float result_x;};
 const Case cases[]={{1,0,30,4,30.5f},{2,0,30,4,30.5f},{1,1,30,1,30},
                     {3,0,30,3,30},{1,0,642,4,642.5f},{1,0,642.01f,1,642.01f}};
 for(const auto& c:cases){
  StageSim sim;sim.set_time_rate(.5f);StageBullet b;b.state=c.state;b.cancel_protection=c.protection;
  b.x=c.x;b.y=200;b.vel[0]=2;b.vel[1]=-3;b.hit_size[0]=4;
  sim.bullets.push_back(b);int effects=0;
  sim.stationary_cancel_effect=[&](i32,const Vec3& p){++effects;CHECK_EQ(p.x,c.x);CHECK_EQ(p.y,200.f);return true;};
  CHECK(sim.cancel_respawn_bullets({0,200,0},640));
  const auto& v=sim.bullets[0];CHECK_EQ(v.state,c.out);CHECK_EQ(v.x,c.result_x);
  CHECK_EQ(v.y,c.out==4?199.25f:200.f);CHECK_EQ(effects,c.out==4?1:0);
  CHECK_EQ(v.animation_interrupt,c.out==4?1:0);
 }
}
}
int main(){return th13test::run_all("bullet cancellation particles");}
