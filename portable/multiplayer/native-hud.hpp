#include "../../th15_web/cpp/game/MultiplayerHud.hpp"
struct HudCapture final:HudDrawServices {
 std::vector<HudTextDraw> texts;bool hud_text(const HudTextDraw& v)override{texts.push_back(v);return true;}
 unsigned count(const std::string& v)const{unsigned n=0;for(const auto& t:texts)n+=t.text==v;return n;}
};
struct HudGraphics:ZunGraphics {
 touhou::graphics::PipelineState state;
 touhou::graphics::PipelineState& pipeline()override{return state;}
 u32 texture(const AnmResource&,u32 index)override{return index+1;}void bind_texture(u32)override{}
 void set_layout(touhou::graphics::VertexLayout)override{}void set_matrix(touhou::graphics::MatrixKind,const Matrix4&)override{}
 void triangles(u32,const void*,u32)override{}
 void primitives(touhou::graphics::Topology,u32,const void*,u32)override{}
 bool select_target(const AnmResource*,u32)override{return true;}bool clear_target(u32,const GraphicsViewport*)override{return true;}bool clear_depth(const GraphicsViewport*)override{return true;}void set_viewport(const GraphicsViewport&)override{}
};
void hud_rules(RetailAssets& files){
 for(unsigned count:{2u,3u}){
  World w(files,count,1,{0,1,3});auto& b=*w.battle;
  GameHud hud(w.animations,w.progress,b.session,b.score,w.assets.front,w.assets.ascii,w.assets.logo);require(hud.initialize({}),hud.error);
  const i32 powers[]={400,137,0},stock[]={8,3,0};for(unsigned s=0;s<count;++s){b.pilots[s]->session.power=powers[s];b.pilots[s]->session.extra_lives=stock[s];b.pilots[s]->session.life_pieces=s+1;b.pilots[s]->session.bombs=s+1;b.pilots[s]->session.bomb_pieces=4-s;}
  w.progress.difficulty=b.score.difficulty=4;b.pilots[1]->owned_life_piece_tier=5;b.score.graze_chapter=345;b.score.graze_total=678;b.score.point_value=9000000;if(count==3)b.pilots[2]->ghost=true;
  const auto game_calls=w.environment.game_rng.calls,visual_calls=w.visual.calls; const auto game_seed=w.environment.game_rng.seed,visual_seed=w.visual.seed;
  MultiplayerHud multiplayer(b,hud);
  HudCapture text;require(multiplayer.queue_text(text),multiplayer.error);
  require(text.count("4.")==1&&text.count("37")==1&&text.count("---/---")==1,"Multiplayer HUD native Power typography or personal Extend tier incorrect");
  require(text.count("90,000")==1&&text.count("345")==1&&text.count("678")==1,"Shared point value or Graze duplicated in HUD");
  require(text.count("2P")==1&&(count!=3||text.count("3P")==1),"HUD seat identity incorrect");
  for(const auto& t:text.texts)require(t.position.x>=428&&t.position.x<=620&&t.position.y>=84&&t.position.y<=465,"HUD text escaped its native side panel");
  require(!text.count("LIFE")&&!text.count("BOMB")&&!text.count("POWER")&&!text.count("POINT")&&!text.count("GRAZE")&&!text.count("TEAM")&&!text.count("*")&&!text.count("-"),"Text substitutes remained instead of original HUD assets");
  HudDrawContext ordinary_context;HudCapture ordinary;require(hud.draw(ordinary_context,ordinary),hud.error);const auto ordinary_text_count=ordinary.texts.size();
  ordinary_context.multiplayer_resources=true;HudCapture shared;require(hud.draw(ordinary_context,shared),hud.error);require(shared.texts.size()==2&&ordinary_text_count>shared.texts.size(),"Original HUD resource numbers remained beneath multiplayer groups");
  HudGraphics graphics;AnmRenderer renderer(graphics);require(multiplayer.prepare(renderer),multiplayer.error);
  require(renderer.multiplayer_hidden_hud.size()==33,"Original personal HUD widgets not suppressed");
  auto* root=w.animations.registry.find(hud.root);auto* panel=w.animations.registry.find_child_script(*root,3,0);require(panel&&(panel->visual.color&0xffffff)==0xffffff,"Native HUD background was tinted");
  // Compare rendered sprite selection with the actual single-player widgets,
  // including personal fragments and the Extra Extend limit.
  for(unsigned seat=0;seat<count;++seat){
   b.score.life_piece_tier=seat?b.pilots[seat]->owned_life_piece_tier:0;
   require(hud.life(stock[seat],seat+1)&&hud.bombs(seat+1,4-seat),hud.error);
   for(unsigned i=0;i<8;++i){
    auto* native_life=w.animations.registry.find(hud.life_icons[i]);auto* native_bomb=w.animations.registry.find(hud.bomb_icons[i]);
    require(w.animations.interrupt(hud.life_icons[i],native_life->pending_interrupt,true)&&w.animations.interrupt(hud.bomb_icons[i],native_bomb->pending_interrupt,true),w.animations.error);
    for(auto expected:{native_life,native_bomb}){
     unsigned occurrence=0;AnmVm* instance=nullptr;
     for(unsigned n=0;auto handle=w.animations.registry.ordered_handle(true,n);++n){auto* vm=w.animations.registry.find(handle);if(vm->source_script==expected->source_script&&vm!=expected){if(occurrence++==seat){instance=vm;break;}}}
     require(instance&&instance->visual.sprite==expected->visual.sprite&&instance->sprite_resource==expected->sprite_resource&&instance->visual.sprite_size.x==expected->visual.sprite_size.x&&instance->visual.scale.x==expected->visual.scale.x,"Multiplayer icon or fragment fill differs from original asset");
     Vec3 actual,original;require(anm_position(*instance,actual)&&anm_position(*expected,original),"HUD positions unavailable");
     require(actual.x==original.x&&actual.y-original.y==float(seat)*(count==3?106.f:132.f),"Native icons were resized or relocated outside their seat row");
    }
   }
  }
  require(w.environment.game_rng.calls==game_calls&&w.visual.calls==visual_calls&&w.environment.game_rng.seed==game_seed&&w.visual.seed==visual_seed,"HUD presentation consumed battle RNG");
 }
}
