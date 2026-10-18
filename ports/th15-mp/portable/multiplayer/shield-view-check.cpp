#include "native-world.hpp"
#include "../../th15_web/cpp/game/ScreenViews.hpp"
#include <algorithm>
struct ShieldGraphics final:ZunGraphics {
 touhou::graphics::PipelineState state;u32 submitted=0;
 touhou::graphics::PipelineState& pipeline()override{return state;}
 u32 texture(const AnmResource&,u32 index)override{return index+1;}void bind_texture(u32)override{}
 void set_layout(touhou::graphics::VertexLayout)override{}void set_matrix(touhou::graphics::MatrixKind,const Matrix4&)override{}
 void triangles(u32 n,const void*,u32)override{submitted+=n;}void primitives(touhou::graphics::Topology,u32 n,const void*,u32)override{submitted+=n;}
 bool select_target(const AnmResource*,u32)override{return true;}bool clear_target(u32,const GraphicsViewport*)override{return true;}bool clear_depth(const GraphicsViewport*)override{return true;}void set_viewport(const GraphicsViewport&)override{}
};
int main(int argc,char** argv){
 require(argc==2,"Retail archive required");RetailAssets files(argv[1]);
 for(unsigned count:{2u,3u}){
  World w(files,count,0,{3,3,3});auto& b=*w.battle;
  for(unsigned s=0;s<count;++s){auto& p=*b.pilots[s];p.player->motion.position={0,400,0};p.player->motion.position_fixed={0,51200};require(p.bomb->begin(),p.bomb->error);}
  for(unsigned n=0;n<40;++n){for(unsigned s=0;s<count;++s){require(b.pilots[s]->tick_bomb(),b.error);require(b.pilots[s]->tick(),b.error);}require(w.animations.update(false),w.animations.error);}
  ShieldGraphics graphics;AnmRenderer renderer(graphics);ScreenViews views(renderer,w.environment);require(views.camera(DrawCamera::Fullscreen,false),"Camera unavailable");
  for(unsigned local=0;local<count;++local){
   b.mp_options.local_seat=local;const auto game=w.environment.game_rng.calls,visual=w.visual.calls;b.mp_prepare_presentation(renderer);
   for(unsigned s=0;s<count;++s){auto& bomb=static_cast<BombReisen&>(*b.pilots[s]->bomb);auto* root=w.animations.registry.find(bomb.barrier);require(root,"Shield fixture missing");unsigned markers=0,rings=0,marker_pixels=0,ring_pixels=0;
    for(auto* child:w.animations.registry.children(*root)){
     const bool marker=child->source_script>=14&&child->source_script<=16;
     const bool ring=child->source_script>=11&&child->source_script<=13;
     if(!marker&&!ring)continue;
     const bool hidden=std::find(renderer.multiplayer_hidden_hitboxes.begin(),renderer.multiplayer_hidden_hitboxes.end(),child)!=renderer.multiplayer_hidden_hitboxes.end();
     require(hidden==(marker&&s!=local),"Shield marker viewpoint or ring visibility incorrect");
     const auto flags=child->visual.flags,color=child->visual.color;renderer.flush();graphics.submitted=0;const int drawn=renderer.draw(*child);require(drawn!=-2,renderer.error);
     renderer.flush();if(hidden)require(drawn==0&&!graphics.submitted,"Remote shield marker submitted pixels");
     if(marker)marker_pixels+=graphics.submitted;else ring_pixels+=graphics.submitted;
     require(child->visual.flags==flags&&child->visual.color==color,"Viewpoint filter mutated shield VM");markers+=marker;rings+=ring;
    }
    require(markers==3&&rings==3,"Retail shield child scripts changed");
    require((marker_pixels>0)==(s==local)&&ring_pixels>0,"Own marker or teammate rings failed to draw");
   }
   require(game==w.environment.game_rng.calls&&visual==w.visual.calls,"Shield draw filter changed RNG");
  }
  // Keep ownership during native shield shutdown, after its owner loses the handle.
  b.mp_options.local_seat=0;auto& remote=static_cast<BombReisen&>(*b.pilots[1]->bomb);auto* root=w.animations.registry.find(remote.barrier);std::vector<AnmVm*> markers;
  for(auto* child:w.animations.registry.children(*root))if(child->source_script>=14&&child->source_script<=16)markers.push_back(child);
  require(w.animations.interrupt(remote.barrier,1),w.animations.error);remote.barrier=0;b.mp_prepare_presentation(renderer);
  for(auto* marker:markers)require(std::find(renderer.multiplayer_hidden_hitboxes.begin(),renderer.multiplayer_hidden_hitboxes.end(),marker)!=renderer.multiplayer_hidden_hitboxes.end(),"Remote marker reappeared during shield fade-out");
 }
 std::puts("PASS: retail Reisen shield markers local-only for every 2P/3P viewpoint, rings remain visible, fade-out ownership retained, VM and RNG unchanged (headless rendering)");
}
