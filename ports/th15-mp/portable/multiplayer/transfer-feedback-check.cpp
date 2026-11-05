#include "native-world.hpp"
#include "native-hud.hpp"
#include "../../web/cpp/game/AsciiText.hpp"
struct Glyphs final:AsciiGlyphServices {
 std::vector<i32> sprites;
 bool ascii_glyph(AnmVm& vm)override{
  require(vm.visual.sprite_size.x<=32&&vm.visual.sprite_size.y<=32,"Transfer text selected an oversized banner sprite");
  sprites.push_back(vm.visual.sprite);return true;
 }
};
int main(int argc,char** argv){
 require(argc==2,"Retail archive required");RetailAssets files(argv[1]);
 for(unsigned count:{2u,3u}){
  World w(files,count,0,{0,1,3});auto& b=*w.battle;
  GameHud hud(w.animations,w.progress,b.session,b.score,w.assets.front,w.assets.ascii,w.assets.logo);require(hud.initialize({}),hud.error);
  MultiplayerHud multiplayer(b,hud);require(multiplayer.error.empty(),multiplayer.error);
  for(unsigned seat=0;seat<count;++seat){auto& pilot=*b.pilots[seat];pilot.player->motion.position={0,400,0};pilot.session.extra_lives=2;pilot.life_hold=pilot.power_taps=0;pilot.keys={};}
  auto& giver=*b.pilots[0];giver.life_hold=120;giver.keys.held=8;
  HudGraphics graphics;AnmRenderer renderer(graphics);HudCapture text;
  const auto game_calls=w.environment.game_rng.calls,visual_calls=w.visual.calls;
  require(multiplayer.draw_transfer_feedback(renderer,text),multiplayer.error);
  require(text.texts.size()==1&&text.texts[0].text=="L 3/5","Life gesture did not produce its progress caption");
  AsciiText ascii(w.animations,w.environment);require(ascii.initialize(w.assets.ascii),ascii.error);
  require(ascii.hud_text(text.texts[0]),ascii.error);Glyphs glyphs;require(ascii.draw(2,glyphs),ascii.error);
  require(glyphs.sprites==std::vector<i32>{'L'+66,'3'+66,'/'+66,'5'+66},"Life gesture used the numeric atlas for its letter");
  require(w.environment.game_rng.calls==game_calls&&w.visual.calls==visual_calls,"Drawing transfer feedback advanced authoritative RNG");
 }
 std::puts("PASS real 2P/3P transfer captions select native letter glyphs without banner sprites or RNG changes");
}
