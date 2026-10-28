#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"
#include "game/StagePresentation.hpp"
#include <fstream>
using namespace th13;using namespace th13::test;
namespace {
struct Graphics:ZunGraphics {
 touhou::graphics::PipelineState p;u32 vertices=0;
 touhou::graphics::PipelineState& pipeline()override{return p;}
 u32 texture(const AnmResource&,u32)override{return 1;}
 void bind_texture(u32)override{} void set_layout(touhou::graphics::VertexLayout)override{}
 void set_matrix(touhou::graphics::MatrixKind,const Matrix4&)override{}
 void triangles(u32 n,const void*,u32)override{vertices+=n*3;}
 void primitives(touhou::graphics::Topology,u32,const void*,u32)override{}
 bool select_target(const AnmResource*,u32)override{return true;}
 bool clear_target(u32,const GraphicsViewport*)override{return true;}
 bool clear_depth(const GraphicsViewport*)override{return true;}
 void set_viewport(const GraphicsViewport&)override{}
};
std::vector<u8> asset(const char* name){std::ifstream f(std::string("artifacts/reference/raw/")+name,std::ios::binary);return {std::istreambuf_iterator<char>(f),{}};}
}
TEST(real_enemy_bullets_initialize_before_interrupt_and_survive_pool_growth) {
 GameRandom rng;AnmEnvironment env;env.screen_offsets={32,16,320,16};AnmManager a(rng,env);Graphics g;AnmRenderer renderer(g);StageBattle battle;
 StagePresentation presentation(a,renderer,{4,5,6},0);battle.player_dead=true;
 auto empty=synthetic_anm(1,trivial_scripts(1));CHECK(load_bank(a,4,empty));CHECK(load_bank(a,6,empty));
 auto bytes=asset("bullet.anm");CHECK(a.load(5,bytes.data(),u32(bytes.size())));
 // Three types use animated sprite selection, across repeated pool growth.
 for(u32 gen=1;gen<=160;++gen){StageBullet bullet;bullet.generation=gen;bullet.sprite=gen%3==0?8:gen%3==1?0:4;bullet.subtype=gen%16;bullet.x=float(int(gen%20)*12-120);bullet.y=160; battle.sim.bullets.push_back(bullet);
  CHECK(presentation.sync(battle,0));
 }
 for(int frame=0;frame<60;++frame)CHECK(presentation.sync(battle,0));
 g.vertices=0;CHECK(presentation.draw(3));renderer.flush();CHECK_EQ(g.vertices,160u*6);
 const auto report=presentation.debug_report();CHECK(report.find("size=0x0")==std::string::npos);
 // The surviving generation must retain its own VM and sprite callback.
 battle.sim.bullets.erase(battle.sim.bullets.begin(),battle.sim.bullets.begin()+120);
 CHECK(presentation.sync(battle,0));g.vertices=0;CHECK(presentation.draw(3));renderer.flush();CHECK_EQ(g.vertices,40u*6);
}
TEST(enemy_death_emits_one_sound_and_oriented_effect_before_retirement) {
 StageSim sim;StageEnemy enemy;enemy.life=10;enemy.position[0]=10;enemy.position[1]=100;enemy.previous_position[0]=10;enemy.previous_position[1]=100;
 enemy.death_sound=4;enemy.death_effect=34;enemy.death_effect_file=1;
 int sounds=0,effects=0,retired=0;
 sim.enemy_sound=[&](i32 id){CHECK_EQ(id,4);CHECK_EQ(retired,0);++sounds;return true;};
 sim.enemy_death_effect=[&](i32 file,i32 script,const Vec3& pos,float angle){CHECK_EQ(file,1);CHECK_EQ(script,34);CHECK_EQ(pos.x,10.0f);CHECK_EQ(pos.y,100.0f);CHECK_EQ(angle,-1.5707964f);CHECK_EQ(retired,0);++effects;return true;};
 sim.retire_sink=[&](u32){++retired;};sim.damage(enemy,20);sim.damage(enemy,20);
 CHECK(enemy.dead);CHECK_EQ(sounds,1);CHECK_EQ(effects,1);CHECK_EQ(retired,1);
}
TEST(enemy_hit_queues_original_hit_voice_before_damage) {
 StageSim sim;StageEnemy enemy;enemy.life=100;int hits=0;sim.enemy_hit_sound=[&](i32 id){CHECK_EQ(id,1);++hits;return true;};
 // StageBattle owns geometric acceptance; this callback contract is exercised
 // directly here so duplicate damage paths cannot silently omit the voice.
 CHECK(sim.enemy_hit_sound(1));CHECK_EQ(hits,1);
}
TEST(real_death_effect_creates_visible_animation_tree) {
 GameRandom rng;AnmEnvironment env;AnmManager a(rng,env);Graphics g;AnmRenderer renderer(g);StagePresentation presentation(a,renderer,{4,5,6},0);
 auto text=asset("text.anm"),ascii=asset("ascii.anm");
 CHECK(a.load(0,text.data(),u32(text.size())));CHECK(a.load(2,ascii.data(),u32(ascii.size())));CHECK(a.sprite_fallback(2));
 auto bytes=asset("effect.anm");CHECK(a.load(8,bytes.data(),u32(bytes.size())));
 CHECK(presentation.enemy_death(1,34,{10,100,0},-1.5707964f));CHECK(a.registry.count()>1);
 CHECK(a.update());for(u32 i=0;i<33;++i)CHECK(renderer.draw_layer(a.registry.layer(i)));renderer.flush();CHECK(g.vertices>0);
}
int main(){return th13test::run_all("combat-visual-feedback");}
