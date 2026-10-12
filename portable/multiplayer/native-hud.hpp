#include "../../th15_web/cpp/game/MultiplayerHud.hpp"
struct HudCapture final:HudDrawServices {
 std::vector<HudTextDraw> texts;bool hud_text(const HudTextDraw& v)override{texts.push_back(v);return true;}
 unsigned count(const std::string& v)const{unsigned n=0;for(const auto& t:texts)n+=t.text==v;return n;}
};
struct HudGraphics final:ZunGraphics {
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
  b.score.difficulty=4;b.pilots[1]->owned_life_piece_tier=5;b.score.graze_chapter=345;b.score.graze_total=678;b.score.point_value=9000000;if(count==3)b.pilots[2]->ghost=true;
  const auto game_calls=w.environment.game_rng.calls,visual_calls=w.visual.calls; const auto game_seed=w.environment.game_rng.seed,visual_seed=w.visual.seed;
  MultiplayerHud multiplayer(b,hud);
  HudCapture text;require(multiplayer.queue_text(text),multiplayer.error);
  require(text.count("4.00/4.00")==1&&text.count("1.37/4.00")==1&&text.count("---/---")==1,"Multiplayer HUD personal Power or Extend tier incorrect");
  require(text.count("TEAM")==1&&text.count("90,000")==1&&text.count("345")==1&&text.count("TOTAL 678")==1,"Shared point value or Graze duplicated in HUD");
  require(text.count("2P MARISA *")==1&&(count!=3||text.count("3P REISEN OUT")==1),"HUD local seat or ghost identity incorrect");
  for(const auto& t:text.texts)require(t.position.x>=428&&t.position.x<=620&&t.position.y>=88&&t.position.y<=406,"HUD text escaped its native side panel");
  HudDrawContext ordinary_context;HudCapture ordinary;require(hud.draw(ordinary_context,ordinary),hud.error);const auto ordinary_text_count=ordinary.texts.size();
  ordinary_context.multiplayer_resources=true;HudCapture shared;require(hud.draw(ordinary_context,shared),hud.error);require(shared.texts.size()==2&&ordinary_text_count>shared.texts.size(),"Original HUD resource numbers remained beneath multiplayer groups");
  HudGraphics graphics;AnmRenderer renderer(graphics);multiplayer.prepare(renderer);
  require(renderer.multiplayer_hidden_hud.size()==33,"Original personal HUD widgets not suppressed");
  require(text.count("LIFE")==count&&text.count("BOMB")==count&&text.count("POWER")==count&&text.count("POINT")==1&&text.count("GRAZE")==1,"Resource labels must identify every personal group and shared statistics");
  require(w.environment.game_rng.calls==game_calls&&w.visual.calls==visual_calls&&w.environment.game_rng.seed==game_seed&&w.visual.seed==visual_seed,"HUD presentation consumed battle RNG");
 }
}
