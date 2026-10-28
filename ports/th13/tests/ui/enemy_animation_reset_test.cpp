#include "ui_test_fixtures.hpp"
#include "../ecl/ecl_test_fixtures.hpp"
#include "game/StageBattle.hpp"
#include "game/StagePresentation.hpp"
using namespace th13; using namespace th13::test;
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

TEST(animation_reset_rebuilds_saved_base_and_disables_turning) {
 GameRandom random;AnmEnvironment env;AnmManager animations(random,env);
 Graphics graphics;AnmRenderer renderer(graphics);StageBattle battle;
 StagePresentation presentation(animations,renderer,{4,5,6},0);
 auto bytes=synthetic_anm(1,trivial_scripts(8));CHECK(load_bank(animations,4,bytes));
 auto enemy=std::make_unique<StageEnemy>();auto* e=enemy.get();e->serial=1;e->anm_file=2;e->mirror_script_base=2;
 battle.sim.enemies.push_back(std::move(enemy));battle.sim.commands.sim=&battle.sim;
 battle.sim.current=e;battle.sim.ecl_slot_anm=true;
 auto command=[&](u16 op,std::initializer_list<u32> args) {
  EclContext c;std::vector<u8> code;th13test::emit(code,0,op,0,255,0,args);
  c.instruction=reinterpret_cast<const EclInstruction*>(code.data());
  CHECK_EQ(battle.sim.commands.execute(c,op),0);
  CHECK(presentation.flush_ecl_animations(battle));
 };
 command(306,{0,1});CHECK_EQ(e->anm_base_script,1);CHECK(e->flags&0x200000u);
 command(303,{0,4});CHECK_EQ(e->anm_script[0],4);CHECK_EQ(e->anm_base_script,1);
 const auto previous=e->anm_slot_handle[0];e->mirror_dir=1;
 command(318,{});CHECK_EQ(e->flags&0x200000u,0u);CHECK_EQ(e->mirror_dir,0);
 CHECK_EQ(e->anm_script[0],1);CHECK(e->anm_slot_handle[0]!=previous);
 auto* vm=animations.registry.find(e->anm_slot_handle[0]);CHECK(vm);
 if(vm){CHECK_EQ(vm->source_script,1);CHECK_EQ(vm->visual.layer,9);}
 command(313,{0});vm=animations.registry.find(e->anm_slot_handle[0]);CHECK(vm);
 if(vm)CHECK_EQ(vm->source_script,6); //saved base1+5, not last303 script4+5
}
TEST(real_st01enm_body_rebinds_keep_one_live_root_and_retire_cleanly) {
 GameRandom random;AnmEnvironment env;AnmManager animations(random,env);
 Graphics graphics;AnmRenderer renderer(graphics);StageBattle battle;
 StagePresentation presentation(animations,renderer,{4,5,6},0);
 std::vector<u8> real,other;
 CHECK(th13test::TestFileReader{}.read("artifacts/reference/raw/st01enm.anm",real));
 CHECK(load_bank(animations,4,real));
 CHECK(load_bank(animations,5,synthetic_anm(1,trivial_scripts(1))));
 CHECK(load_bank(animations,6,synthetic_anm(1,trivial_scripts(1))));
 auto enemy=std::make_unique<StageEnemy>();auto* e=enemy.get();e->serial=1;e->anm_file=2;
 battle.sim.enemies.push_back(std::move(enemy));battle.sim.commands.sim=&battle.sim;
 battle.sim.current=e;battle.sim.ecl_slot_anm=true;
 presentation.enemies_from_ecl=true;
 const auto live_bodies=[&] {
  size_t count=0;
  for(u32 layer=0;layer<33;++layer)
   for(auto* vm: animations.registry.layer(layer))
    if(vm&&vm->resource==animations.resource(4)&&vm->source_script>=0&&
       vm->source_script<=5&&!(vm->visual.render_flags&0x20u))++count;
  return count;
 };
 const auto command=[&](u16 op,std::initializer_list<u32> args) {
  EclContext c;std::vector<u8> code;th13test::emit(code,0,op,0,255,0,args);
  c.instruction=reinterpret_cast<const EclInstruction*>(code.data());
  CHECK_EQ(battle.sim.commands.execute(c,op),0);
  CHECK(presentation.flush_ecl_animations(battle));
  CHECK(animations.update());
 };
 // Real st01enm local scripts: 0 = 306 body, 5 = 313 flipped body,
 // 1..4 = mirror-walk variants. Every rebind must retire the previous root.
 command(306,{0,0});CHECK_EQ(live_bodies(),1u);
 command(303,{0,0});CHECK_EQ(live_bodies(),1u);
 command(306,{0,0});CHECK_EQ(live_bodies(),1u);
 command(313,{0});CHECK_EQ(live_bodies(),1u);
 auto* slot=animations.registry.find(e->anm_slot_handle[0]);
 CHECK(slot);if(slot)CHECK_EQ(slot->source_script,5); // 0x419450 saved base0 + 5
 // Mirror-walk rebind (0x419450: old dir 0, new dir -1 -> script base+1).
 e->flags|=0x200000u;e->mirror_dir=0;e->mirror_prev_x=e->position[0]+1.0f;
 CHECK(presentation.sync(battle,0));
 CHECK(animations.update());
 CHECK_EQ(live_bodies(),1u);
 slot=animations.registry.find(e->anm_slot_handle[0]);
 CHECK(slot);if(slot)CHECK_EQ(slot->source_script,1);
 // Enemy death path (RunScene retire_sink): all slot roots retire, none leak.
 for(u32& handle: e->anm_slot_handle)
  if(handle){CHECK(animations.retire(handle));handle=0;}
 CHECK(animations.update());
 CHECK_EQ(live_bodies(),0u);
}
}
int main(){return th13test::run_all("enemy animation reset");}
