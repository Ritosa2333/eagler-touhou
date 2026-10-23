#include "../../web/cpp/game/ScreenViews.hpp"
struct GhostGraphics final:HudGraphics {
 std::vector<AnmGeometryVertex> submitted;
 void triangles(u32 count,const void* data,u32 stride)override{require(stride==sizeof(AnmGeometryVertex),"Unexpected ghost vertex layout");const auto* p=static_cast<const AnmGeometryVertex*>(data);submitted.insert(submitted.end(),p,p+count*3);}
};
void ghost_rules(RetailAssets& files){
 for(unsigned count:{2u,3u})for(unsigned seat=0;seat<count;++seat)for(i32 state:{4,2}){
  World w(files,count,0,{0,1,3});auto& b=*w.battle;auto& dead=*b.pilots[seat];
  for(unsigned s=0;s<count;++s){auto& p=*b.pilots[s]->player;p.motion.position={140,100,0};p.motion.position_fixed={17920,12800};}
  dead.player->motion.position={0,400,0};dead.player->motion.position_fixed={0,51200};dead.player->life.state=state;
  BulletShooter bullet;bullet.position={20,400,0};bullet.count=bullet.rows=1;bullet.speed=0;
  require(b.bullet_scene->manager.emit(bullet,0),b.bullet_scene->manager.error);
  BattleFrame frame;frame.player_enabled=frame.enemies_enabled=frame.items_enabled=false;require(b.step(frame),b.error);
  require(b.score.graze_total==0&&b.score.graze_chapter==0&&b.score.point_value==1000000,"Dead player grazed a real bullet");
  require(dead.game_over(),b.error);require(b.step(frame),b.error);
  b.mp_contact_seat=seat;require(b.record_graze({20,400,0}),b.error);b.mp_contact_seat=0;
  require(b.score.graze_total==0&&b.score.point_value==1000000,"Ghost still awarded Graze or point value");
  auto& live=*b.pilots[(seat+1)%count];live.player->motion.position={0,400,0};live.player->motion.position_fixed={0,51200};
  require(b.step(frame),b.error);require(b.score.graze_total==1,"Ghost exclusion also disabled a living teammate's Graze");
 }
 for(unsigned kind=0;kind<3;++kind){
  World w(files,2,0,{0,1,0});auto& b=*w.battle;auto& dead=*b.pilots[0];
  b.pilots[1]->player->motion.position={140,100,0};require(dead.game_over(),b.error);
  dead.player->motion.position={0,400,0};dead.player->motion.position_fixed={0,51200};
  if(kind==0){MovingLaserRequest r;r.position={0,380,0};r.initial_length=r.maximum_length=80;r.width=16;require(b.laser_scene->create_moving(r)>0,b.laser_scene->error);}
  if(kind==1){StationaryLaserRequest r;r.position={0,380,0};r.initial_length=r.maximum_length=80;r.width=16;r.active=1000;auto id=b.laser_scene->create_stationary(r);require(id>0,b.laser_scene->error);auto* m=b.laser_scene->stationary_motion(id);m->state=2;m->width=16;}
  if(kind==2){CurveLaserRequest r;r.position={0,380,0};r.angle=0;r.width=16;r.speed=2;r.count=32;require(b.laser_scene->create_curved(r)>0,b.laser_scene->error);}
  for(unsigned n=0;n<12;++n)require(b.laser_scene->update(1,0),b.laser_scene->error);
  require(b.score.graze_total==0&&b.score.point_value==1000000,"Ghost grazed a moving, stationary or curved laser");
  b.pilots[1]->player->motion.position={0,400,0};b.pilots[1]->player->motion.position_fixed={0,51200};require(b.pilots[1]->tick(),b.error);for(unsigned n=0;n<6;++n)require(b.laser_scene->update(1,0),b.laser_scene->error);
  require(b.score.graze_total>0,"Laser fixture never reached a living player's Graze box");
 }
 for(unsigned local:{0u,2u}){
  World w(files,3,local,{0,1,3});auto& b=*w.battle;auto& ghost=*b.pilots[local];
  ghost.session.extra_lives=0;ghost.player->life.state=4;ghost.player->life.age.set(8);require(ghost.tick(),b.error);
  require(ghost.session.extra_lives==-1&&!ghost.ghost,"Ghost bypassed the original death animation");
  for(unsigned n=0;n<31&&!ghost.ghost;++n)require(ghost.tick(),b.error);
  require(ghost.ghost&&ghost.player->motion.option_count==0&&!ghost.player->visuals.focus_handle&&!ghost.player->visuals.barrier_handle,"Final death did not clear active player effects");
  require(!ghost.alive()&&!ghost.allow_bomb()&&b.mp_nearest(ghost.player->motion.position)!=i32(local),"Ghost remains an operable player or ECL target");
  const auto start=ghost.player->motion.position_fixed;const auto seed=w.environment.game_rng.seed;const auto calls=w.environment.game_rng.calls;
  const auto power=ghost.session.power,bombs=ghost.session.bombs,score=b.score.score;
  for(unsigned n=0;n<900;++n){ghost.input.held=3|8|0x40;ghost.input.touch_mode=2;ghost.input.touch_x=184;ghost.input.touch_y=32;require(ghost.tick(),b.error);const auto p=ghost.player->motion.position_fixed;require(p.x>=-23552&&p.x<=23552&&p.y>=4096&&p.y<=55296,"Ghost drift left TH11's playfield bounds");}
  require(ghost.player->motion.position_fixed.x!=start.x||ghost.player->motion.position_fixed.y!=start.y,"Ghost did not float");
  require(w.environment.game_rng.seed==seed&&w.environment.game_rng.calls==calls&&ghost.session.power==power&&ghost.session.bombs==bombs&&b.score.score==score,"Ghost input or drift altered gameplay resources or RNG");
  auto& root=ghost.player->visuals.root;root.visual.flags&=~3u;root.visual.color&=0xffffff;
  w.environment.screen_offsets={320,16,320,16};
  GhostGraphics graphics;AnmRenderer renderer(graphics);ScreenViews views(renderer,w.environment);require(views.camera(DrawCamera::Fullscreen,false),"Ghost camera unavailable");require(b.mp_draw(renderer),b.error);renderer.flush();
  bool visible=false;for(const auto& v:graphics.submitted)visible|=(v.color>>24)==112;
  require(visible&&!(root.visual.flags&3)&&!(root.visual.color>>24),"Hidden native death sprite did not render as a translucent ghost, or drawing mutated it");
  b.mp_options.local_seat=(local+1)%3;graphics.submitted.clear();require(b.mp_draw(renderer),b.error);renderer.flush();visible=false;for(const auto& v:graphics.submitted)visible|=(v.color>>24)==88;require(visible,"Remote ghost sprite did not use TH11's translucent alpha");b.mp_options.local_seat=local;
  World other(files,3,local==0?2:0,{0,1,3});auto& twin=*other.battle->pilots[local];require(twin.game_over(),other.battle->error);twin.player->motion.position_fixed=start;twin.player->motion.position={float(start.x)/128,float(start.y)/128,0};
  for(unsigned n=0;n<900;++n)require(twin.tick(),other.battle->error);
  require(twin.ghost_random==ghost.ghost_random&&twin.player->motion.position_fixed.x==ghost.player->motion.position_fixed.x&&twin.player->motion.position_fixed.y==ghost.player->motion.position_fixed.y,"Ghost drift depends on local seat");
 }
}

// A retained live pilot and a fresh Replay pilot must begin each stage with
// the same private drift stream, including another death after revival.
void ghost_stage_rules(RetailAssets& files){
 for(unsigned count:{2u,3u})for(unsigned seat=0;seat<count;++seat){
  World retained(files,count,0,{0,1,3}),fresh(files,count,count-1,{0,1,3});
  auto& old=*retained.battle->pilots[seat];
  require(old.game_over(),retained.battle->error);
  for(unsigned n=0;n<331;++n)require(old.tick(),retained.battle->error);
  StageAssets left(files,retained.animations,&retained.assets),right(files,fresh.animations,&fresh.assets);
  require(left.load(2,0),left.error);require(right.load(2,0),right.error);
  retained.progress.stage=fresh.progress.stage=2;
  require(retained.battle->replace_stage(left.scripts,left.enemy_banks,left.spell_resources(0)),retained.battle->error);
  require(fresh.battle->replace_stage(right.scripts,right.enemy_banks,right.spell_resources(0)),fresh.battle->error);
  auto& twin=*fresh.battle->pilots[seat];
  for(auto* pilot:{&old,&twin}){
   pilot->player->motion.position={0,400,0};pilot->player->motion.position_fixed={0,51200};
   require(pilot->game_over(),pilot->world.error);
  }
  const auto seed=retained.environment.game_rng.seed;const auto calls=retained.environment.game_rng.calls;
  for(unsigned n=0;n<720;++n){
   require(old.tick(),retained.battle->error);require(twin.tick(),fresh.battle->error);
   require(old.player->motion.position_fixed.x==twin.player->motion.position_fixed.x&&old.player->motion.position_fixed.y==twin.player->motion.position_fixed.y,
     "Retained ghost diverged from a fresh Replay pilot after stage transition at "+std::to_string(n));
  }
  require(retained.environment.game_rng.seed==seed&&retained.environment.game_rng.calls==calls,"Ghost stage reset consumed gameplay RNG");
 }
 std::puts("PASS: 2P/3P retained and fresh Replay ghost drift agrees for 720 frames after stage transition");
}
