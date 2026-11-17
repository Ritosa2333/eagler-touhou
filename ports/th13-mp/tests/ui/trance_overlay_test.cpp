#include "ui_test_fixtures.hpp"
#include "../harness/test_framework.hpp"
#include "game/StageBattle.hpp"
#include "game/StagePresentation.hpp"
#include "game/AnmOverlay.hpp"
#include <fstream>
using namespace th13;using namespace th13::test;
namespace {
struct Graphics:ZunGraphics {
 touhou::graphics::PipelineState state;
 u32 masks=0,masked_geometry=0,unmasked_geometry=0;
 touhou::graphics::PipelineState& pipeline()override{return state;}
 u32 texture(const AnmResource&,u32)override{return 1;}
 void bind_texture(u32)override{}void set_layout(touhou::graphics::VertexLayout)override{}
 void set_matrix(touhou::graphics::MatrixKind,const Matrix4&)override{}
 void triangles(u32 n,const void*,u32)override{if(state.separateAlphaBlend)masked_geometry+=n;else unmasked_geometry+=n;}
 void primitives(touhou::graphics::Topology,u32 n,const void*,u32)override{
  if(state.separateAlphaBlend&&state.sourceBlend==touhou::graphics::BlendFactor::Zero)++masks;
  else if(state.separateAlphaBlend)masked_geometry+=n;else unmasked_geometry+=n;
 }
 bool select_target(const AnmResource*,u32)override{return true;}bool clear_target(u32,const GraphicsViewport*)override{return true;}
 bool clear_depth(const GraphicsViewport*)override{return true;}void set_viewport(const GraphicsViewport&)override{}
};

std::vector<u8> asset(const char* name){std::ifstream f(std::string("artifacts/reference/raw/")+name,std::ios::binary);return {std::istreambuf_iterator<char>(f),{}};}
TEST(trance_uses_original_embedded_panels_and_exit_scripts) {
 GameRandom rng;AnmEnvironment env;AnmManager animations(rng,env);Graphics graphics;AnmRenderer renderer(graphics);
 for(auto pair:{std::pair<int,const char*>{2,"ascii.anm"},{8,"effect.anm"},{25,"astral.anm"}}){
  auto bytes=asset(pair.second);CHECK(animations.load(pair.first,bytes.data(),u32(bytes.size())));
  if(pair.first==2)CHECK(animations.sprite_fallback(2));
 }
 StageBattle battle;SessionState session;battle.sim.session=&session;battle.player_dead=true;
 const auto empty=synthetic_anm(1,trivial_scripts(1));for(int bank:{4,5,6})CHECK(load_bank(animations,bank,empty));
 StagePresentation presentation(animations,renderer,{4,5,6,8,25},0);
 trance_begin(session);CHECK(presentation.sync(battle,0));
 AnmVm* root=nullptr;
 for(u32 i=0;i<animations.registry.ordered_count(false);++i){auto* vm=animations.registry.find(animations.registry.ordered_handle(false,i));if(vm&&vm->geometry.overlay)root=vm;}
 CHECK(root);if(!root)return;
 auto& overlay=*root->geometry.overlay;CHECK_EQ(overlay.mode,2);CHECK_EQ(root->visual.layer,6);
 for(u32 i=0;i<5;++i){auto& vm=*overlay.panels[i];CHECK_EQ(vm.source_script,i<4?i32(i+3):0);
  CHECK_EQ(vm.resource,animations.resource(i<4?8:25));
  CHECK_EQ(vm.visual.translation.x,i<4?320.f:0.f);CHECK_EQ(vm.visual.translation.y,i<4?240.f:0.f);
 }
 for(int frame=0;frame<80;++frame){CHECK(animations.update());CHECK(presentation.sync(battle,0));}
 renderer.set_viewport({0,0,640,480});
 CHECK(renderer.draw(*root)!=-2);renderer.flush();
 CHECK_EQ(graphics.masks,1u);CHECK(graphics.masked_geometry>0);CHECK(graphics.unmasked_geometry>0);
 CHECK(!graphics.state.separateAlphaBlend);
 trance_end(session,false);CHECK(presentation.sync(battle,0));
 for(u32 i=0;i<4;++i)CHECK_EQ(overlay.panels[i]->source_script,i32(i+7));
 for(int frame=0;frame<180;++frame)CHECK(animations.update());
 bool remains=false;for(u32 i=0;i<animations.registry.ordered_count(false);++i){auto* vm=animations.registry.find(animations.registry.ordered_handle(false,i));remains|=vm&&bool(vm->geometry.overlay);}
 CHECK(!remains);
}
}
int main(){return th13test::run_all("trance overlay");}
