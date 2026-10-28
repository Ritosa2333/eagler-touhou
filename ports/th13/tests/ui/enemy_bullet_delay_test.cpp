#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"
#include "game/StagePresentation.hpp"
using namespace th13;using namespace th13::test;
namespace {
struct Graphics:ZunGraphics {
 touhou::graphics::PipelineState state;
 touhou::graphics::PipelineState& pipeline()override{return state;}
 u32 texture(const AnmResource&,u32)override{return 1;}
 void bind_texture(u32)override{}void set_layout(touhou::graphics::VertexLayout)override{}
 void set_matrix(touhou::graphics::MatrixKind,const Matrix4&)override{}
 void triangles(u32,const void*,u32)override{}void primitives(touhou::graphics::Topology,u32,const void*,u32)override{}
 bool select_target(const AnmResource*,u32)override{return true;}bool clear_target(u32,const GraphicsViewport*)override{return true;}
 bool clear_depth(const GraphicsViewport*)override{return true;}void set_viewport(const GraphicsViewport&)override{}
};
TEST(delayed_bullet_waits_for_its_actual_anm_integer_before_acceleration) {
 GameRandom rng;AnmEnvironment env;AnmManager manager(rng,env);Graphics graphics;AnmRenderer renderer(graphics);
 StagePresentation presentation(manager,renderer,{4,5,6},0);StageSim sim;
 auto scripts=trivial_scripts(36);
 // A real interpreter program, not a second delay counter. It releases the
 // bullet on ANM time4. Before then state2 moves only half speed.
 scripts[35]={sprite(0),{100,0,{10000,0},1},{100,4,{10000,1},1},wait(),stop()};
 auto bytes=synthetic_anm(1,scripts);CHECK(manager.load(5,bytes.data(),u32(bytes.size())));
 sim.interactive=true;sim.bullet_animation=[&](StageBullet&b,bool birth){return presentation.tick_bullet(b,birth);};
 StageEt et;et.aim=1;et.speed1=2;et.ex[0].op=2;et.ex[0].args[4]=1;
 const float pos[]={0,100,0};sim.emit_et(et,0,0,pos,0);
 CHECK_EQ(sim.bullets.size(),size_t(1));if(sim.bullets.empty())return;
 CHECK_EQ(sim.bullets[0].state,2);CHECK_EQ(sim.bullets[0].x,-8.f);
 CHECK(!sim.bullets[0].animation_ready);
 for(int n=0;n<3;++n){sim.advance_bullets();CHECK_EQ(sim.bullets[0].state,2);CHECK_EQ(sim.bullets[0].x,float(-7+n));CHECK(!sim.bullets[0].collision_ready);}
 CHECK(sim.bullets[0].animation_ready);sim.advance_bullets();
 CHECK_EQ(sim.bullets[0].state,1);CHECK_EQ(sim.bullets[0].x,-2.f);CHECK(sim.bullets[0].collision_ready);
}
TEST(completing_wait_runs_acceleration_in_the_same_update) {
 StageSim sim;StageBullet b;b.x=0;b.y=100;b.speed=1;b.vel[0]=1;
 b.ex[0].op=0x80000000;b.ex[0].args[4]=1;
 b.ex[1].op=4;b.ex[1].args[0]=.5f;b.ex[1].args[1]=-999999;b.ex[1].args[4]=2;
 sim.process_bullet_queue(b,true);sim.bullets.push_back(b);sim.advance_bullets();
 CHECK_EQ(sim.bullets[0].x,1.f);sim.advance_bullets();
 CHECK_EQ(sim.bullets[0].x,2.5f);CHECK_EQ(sim.bullets[0].ex4_timer,1);
 CHECK_EQ(sim.bullets[0].vel[0],1.5f);CHECK_EQ(sim.bullets[0].vel[1],0.f);
}
TEST(bullet_animation_delay_consumes_global_fractional_rate) {
 GameRandom rng;AnmEnvironment env;AnmManager manager(rng,env);Graphics graphics;AnmRenderer renderer(graphics);
 StagePresentation presentation(manager,renderer,{4,5,6},0);StageSim sim;
 auto scripts=trivial_scripts(36);
 scripts[35]={sprite(0),{100,0,{10000,0},1},{100,4,{10000,1},1},wait(),stop()};
 const auto bytes=synthetic_anm(1,scripts);CHECK(manager.load(5,bytes.data(),u32(bytes.size())));
 manager.rate=.5f;sim.set_time_rate(.5f);
 sim.bullet_animation=[&](StageBullet&b,bool birth){return presentation.tick_bullet(b,birth);};
 StageBullet bullet;bullet.sprite=0;bullet.subtype=0;bullet.generation=99;bullet.state=2;bullet.animation_ready=false;
 CHECK(presentation.tick_bullet(bullet,true));sim.bullets.push_back(bullet);
 //462450 samples at time0 twice during birth; both advances use .5.
 for(int frame=0;frame<6;++frame){sim.advance_bullets();CHECK(!sim.bullets[0].animation_ready);}
 sim.advance_bullets();CHECK(sim.bullets[0].animation_ready);CHECK_EQ(sim.bullets[0].state,2);
 sim.advance_bullets();CHECK_EQ(sim.bullets[0].state,1);
}
TEST(sprite_culling_uses_original_bounds_and_birth_grace) {
 for(int axis=0;axis<2;++axis){
  StageSim sim;sim.bullet_animation=[](StageBullet&,bool){return true;};
  StageBullet b;b.animation_geometry=true;b.sprite_width=16;b.sprite_height=16;
  b.x=axis==0?200:0;b.y=axis==1?456:100;b.cull_grace=1;
  sim.bullets.push_back(b);sim.advance_bullets();CHECK_EQ(sim.bullets.size(),size_t(1));
  sim.advance_bullets();CHECK(sim.bullets.empty());
 }
 StageSim sim;sim.bullet_animation=[](StageBullet&,bool){return true;};
 StageBullet b;b.animation_geometry=true;b.sprite_width=16;b.sprite_height=16;
 b.x=199.5f;b.y=455.5f;b.cull_grace=0;sim.bullets.push_back(b);
 sim.advance_bullets();CHECK_EQ(sim.bullets.size(),size_t(1));
}
}
int main(){return th13test::run_all("enemy-bullet-delay");}
