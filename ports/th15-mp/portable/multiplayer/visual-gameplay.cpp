#define TH15_MP_VISUAL_CHECK
#include "native-gameplay.cpp"
#include "opacity-draw-check.hpp"
struct VisualGraphics final:ZunGraphics {
 touhou::graphics::PipelineState state;
 touhou::graphics::PipelineState& pipeline()override{return state;}
 u32 texture(const AnmResource&,u32 index)override{return index+1;}void bind_texture(u32)override{}
 void set_layout(touhou::graphics::VertexLayout)override{}void set_matrix(touhou::graphics::MatrixKind,const Matrix4&)override{}
 void triangles(u32,const void*,u32)override{}void primitives(touhou::graphics::Topology,u32,const void*,u32)override{}
 bool select_target(const AnmResource*,u32)override{return true;}bool clear_target(u32,const GraphicsViewport*)override{return true;}bool clear_depth(const GraphicsViewport*)override{return true;}void set_viewport(const GraphicsViewport&)override{}
};
int main(int argc,char** argv){
 require(argc==2,"Pass TH15 Japanese archive");RetailAssets files(argv[1]);
 {
  World gifts(files,2,0,{0,1,3});auto& battle=*gifts.battle;
  for(unsigned s=0;s<2;++s){auto& player=*battle.pilots[s]->player;player.motion.position={float(s)*10,400,0};player.motion.position_fixed={i32(s)*1280,51200};}
  auto donate=[&](){for(unsigned tap=0;tap<5;++tap){std::array<MultiplayerInput,3> input{};input[0].held=1;gifts.step(input);gifts.step({});}};
  for(i32 power:{100,150,199}){battle.session.power=power;battle.pilots[1]->session.power=100;donate();require(battle.session.power==power&&battle.pilots[1]->session.power==100,"Sub-2P gift consumed Power");}
  battle.session.power=200;donate();require(battle.session.power==100,"2P gift did not retain 1P floor");
 }
 for(unsigned count:{2u,3u})for(unsigned grazing=0;grazing<count;++grazing){
  World w(files,count,0,{0,1,3});auto& b=*w.battle;
  for(unsigned s=0;s<count;++s){auto& p=*b.pilots[s]->player;p.motion.position={s==grazing?0.f:150.f,400,0};p.motion.position_fixed={i32(p.motion.position.x*128),51200};}
  BulletShooter near;near.position={20,400,0};near.count=near.rows=1;near.speed=0;
  require(b.bullet_scene->manager.emit(near,0),b.bullet_scene->manager.error);w.step({});
  const auto index=b.bullet_scene->manager.first_active();auto* bullet=b.bullet_scene->manager.state(index);
  require(bullet&&(bullet->visual_flags&0x60000u)==0x20000u,"Seat order erased native graze glow");
  require(b.score.graze_total==1,"Graze accounting changed with glow aggregation");
 }
 World w(files,3,1,{0,1,3});auto& b=*w.battle;
 {
  const auto game_rng=w.environment.game_rng,visual_rng=w.visual;
  opacity_draw_check::check(b.pilots[2]->player->visuals.root);
  require(std::memcmp(&game_rng,&w.environment.game_rng,sizeof(Rng))==0&&std::memcmp(&visual_rng,&w.visual,sizeof(Rng))==0,
   "Opacity reference Draw changed native game/visual RNG");
 }
 {
  VisualGraphics graphics;AnmRenderer renderer(graphics);auto& remote=*b.pilots[2]->player;auto& local=*b.pilots[1]->player;
  remote.motion.position=local.motion.position;require(remote.visuals.focus_begin(true,1),"Remote focus creation failed");
   const auto alpha=remote.visuals.root.visual.color,game=w.environment.game_rng.calls;
  b.mp_prepare_presentation(renderer);
  require(renderer.multiplayer_player_opacity.at(&remote.visuals.root)==72,"Near teammate not faded");
  auto* focus=w.animations.registry.find(remote.visuals.focus_handle);require(focus&&renderer.multiplayer_player_opacity.at(focus)==72,"Teammate hitbox not faded");
  require(std::find(renderer.multiplayer_hidden_hitboxes.begin(),renderer.multiplayer_hidden_hitboxes.end(),focus)!=renderer.multiplayer_hidden_hitboxes.end(),"Remote hitbox still visible");
  require(local.visuals.focus_begin(true,1),"Local focus creation failed");b.mp_prepare_presentation(renderer);
  auto* own_focus=w.animations.registry.find(local.visuals.focus_handle);require(own_focus&&std::find(renderer.multiplayer_hidden_hitboxes.begin(),renderer.multiplayer_hidden_hitboxes.end(),own_focus)==renderer.multiplayer_hidden_hitboxes.end(),"Own hitbox hidden");
   const auto visual=w.visual.calls;
   const auto vertices=renderer.pending().size();require(renderer.draw(*focus)==0&&renderer.pending().size()==vertices,"Remote hitbox submitted pixels");
  require(renderer.draw(remote.visuals.root)!=-2,renderer.error);
   require(remote.visuals.root.visual.color==alpha&&game==w.environment.game_rng.calls&&visual==w.visual.calls,"Fade changed native VM or RNG");
   const auto focus_handle=remote.visuals.focus_handle;
   remote.visuals.focus_handle=0;b.mp_prepare_presentation(renderer);
   require(renderer.multiplayer_player_opacity.count(focus)==0,"Detached focus kept a stale opacity override");
   const auto fade_vertices=renderer.pending().size();
   require(renderer.draw(*focus)==0&&renderer.pending().size()==fade_vertices,"Fading teammate hitbox became visible");
   remote.visuals.focus_handle=focus_handle;
   b.mp_options.local_seat=2;b.mp_prepare_presentation(renderer);
   require(renderer.multiplayer_player_opacity.count(&remote.visuals.root)==0,"New local player kept the previous teammate fade");
   require(renderer.multiplayer_player_opacity.at(&local.visuals.root)==72,"Previous local player did not acquire the teammate fade");
   require(std::find(renderer.multiplayer_hidden_hitboxes.begin(),renderer.multiplayer_hidden_hitboxes.end(),focus)==renderer.multiplayer_hidden_hitboxes.end(),"Selected viewpoint hitbox hidden");
   require(std::find(renderer.multiplayer_hidden_hitboxes.begin(),renderer.multiplayer_hidden_hitboxes.end(),own_focus)!=renderer.multiplayer_hidden_hitboxes.end(),"Previous viewpoint hitbox visible");
   b.mp_options.local_seat=1;
  remote.motion.position.x+=100;b.mp_prepare_presentation(renderer);require(renderer.multiplayer_player_opacity.at(&remote.visuals.root)==160,"Far teammate opacity incorrect");
 }
 b.pilots[0]->ghost=true;b.pilots[0]->player->motion.position={0,100,0};
 b.pilots[1]->player->motion.position={100,400,0};b.pilots[2]->player->motion.position={-100,400,0};
 require(b.mp_nearest({0,100,0})!=0,"Ghost still selected");
 unsigned selections=0;const auto select=b.bullet_scene->manager.multiplayer_target;
 b.bullet_scene->manager.multiplayer_target=[&](const Vec3& origin){++selections;const auto target=select(origin);require(target.y==400,"Bullet transform selected ghost position");return target;};
 BulletShooter shot;shot.position={0,100,0};shot.count=shot.rows=1;shot.speed=0;
 require(b.bullet_scene->manager.emit(shot,0),b.bullet_scene->manager.error);selections=0;
 require(b.bullet_scene->update(),b.bullet_scene->error);require(selections>0,"Per-frame bullet target hook missing");
 selections=0;const auto laser_select=b.laser_scene->multiplayer_target;
 b.laser_scene->multiplayer_target=[&](const Vec3& origin){++selections;const auto target=laser_select(origin);require(target.y==400,"Laser transform selected ghost");return target;};
 MovingLaserRequest laser;laser.position={0,100,0};laser.width=8;laser.maximum_length=64;laser.initial_length=32;
 require(b.laser_scene->create_moving(laser)!=0,b.laser_scene->error);require(b.laser_scene->update(1),b.laser_scene->error);require(selections>0,"Moving-laser target hook missing");
 std::puts("PASS: retail 2P/3P graze glow, local-only hitbox including fade-out/viewpoint changes, native VM/RNG retained, ghost excluded from per-frame bullet and moving-laser targets (headless, not GPU/device)");
}
