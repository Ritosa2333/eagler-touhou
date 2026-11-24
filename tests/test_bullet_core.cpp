#include "bullet_core.hpp"
#include "enemy_path_runtime.hpp"
#include "stage_scene.hpp"
#include <iostream>
unsigned checks=0;void check(bool b){++checks;if(!b)throw std::runtime_error("Bullet/effect contract failed");}
int wmain(int argc,wchar_t**argv){try{
 if(argc!=2)throw std::runtime_error("Usage: bullet-core-tests <assets>");
 soew::RandomRing rng;for(unsigned i=0;i<256;++i)rng.bytes[i]=static_cast<std::uint8_t>(i);
 {soew::BulletCore pool;soew::EnemyBulletRequest request;request.left=40;request.top=40;request.group=0x20;request.speed=32;check(pool.add(request,0,208,336,rng));check(pool.spawned==1&&pool.bullets[0].velocity==std::array<int,2>{32,0});check(pool.bullets[0].position[0][0]==640);pool.update(0,208,336);check(pool.bullets[0].position[0][0]==672);pool.begin_frame(1);check(pool.bullets[0].position[1][0]==672);}
 {soew::BulletCore pool;soew::EnemyBulletRequest request;request.left=100;request.top=100;request.group=0x25;request.speed=32;pool.add(request,0,208,336,rng);check(pool.spawned==16);check(pool.bullets[0].velocity[0]==32&&pool.bullets[4].velocity[1]==32);}
 {soew::BulletCore pool(0);soew::EnemyBulletRequest request;request.left=40;request.top=40;request.group=0x20;request.speed=16;pool.add(request,0,208,336,rng);check(pool.spawned==0);pool.add(request,0,208,336,rng);check(pool.spawned==1&&pool.bullets[0].velocity[0]==16);}
 {soew::BulletCore pool(2);soew::EnemyBulletRequest request;request.left=40;request.top=40;request.group=0x20;request.speed=64;pool.add(request,0,208,336,rng);check(pool.spawned==2&&pool.bullets[0].velocity[0]==64&&pool.bullets[1].velocity[0]==32);}
 {soew::BulletCore pool;soew::EnemyBulletRequest request;request.left=40;request.top=40;request.group=0;request.speed=32;pool.add(request,0,208,336,rng);check(pool.spawned==150);const auto cursor=rng.cursor;pool.add(request,0,208,336,rng);check(pool.spawned==150&&rng.cursor==cursor);}
 {soew::BulletCore pool;soew::EnemyBulletRequest request;request.kind=request.Bullet16;request.group=0x86;request.speed=32;const auto before=rng.cursor;check(!pool.add(request,0,208,336,rng)&&pool.spawned==0&&rng.cursor==before);}
 // All original normal-group fan counts, and exact invalid-group capacity behavior.
 for(unsigned group=1;group<=0x27;++group){soew::BulletCore pool;soew::EnemyBulletRequest request;request.left=100;request.top=100;request.group=static_cast<std::uint8_t>(group);request.speed=48;request.angle=2;pool.add(request,0,208,336,rng);unsigned expected=150;if(group<=24)expected=2+(group-1)/6;else switch(group){case 0x19:case 0x1a:case 0x20:expected=1;break;case 0x1b:case 0x1c:expected=3;break;case 0x21:expected=4;break;case 0x22:case 0x23:case 0x26:expected=2;break;case 0x24:expected=8;break;case 0x25:expected=16;break;case 0x27:expected=32;break;}check(pool.spawned==expected);}
 // Effect requests preserve serialized parameters, source coordinate wrap and
 // accumulator ordering. Missing/refusing sink must leave instruction untouched.
 for(unsigned op:{0x20u,0x21u,0x22u,0x24u,0x26u,0x27u,0x11u,0xa8u,0xafu}){
  soew::EnemySlots::Slot s;s.raw[14]=1;s.word(0,100);s.word(2,60);s.word(0x16,254);s.raw[0x24]=0x20;s.raw[0x25]=32;
  soew::StageScript::EnemyTemplate type;type.raw[4]=8;type.raw[6]=16;
  std::vector<std::uint8_t> path(soew::enemy_opcode_width(op)+2);path[0]=static_cast<std::uint8_t>(op);for(unsigned n=1;n<soew::enemy_opcode_width(op);++n)path[n]=static_cast<std::uint8_t>(n+1);path[path.size()-2]=0;path[path.size()-1]=1;
  soew::EnemyBulletRequest captured;unsigned sound=999;soew::EnemyTileRequest tile;
  soew::EnemyEffectSink sink;sink.bullet=[&](auto r){captured=r;return true;};sink.sound=[&](auto id){sound=id;return true;};sink.tile=[&](auto r){tile=r;return true;};
  auto result=soew::run_enemy_path(s,path,rng,{0,0,208,336,390,&type,&sink});check(result.status==(op==0x11?soew::EnemyPathResult::Ended:soew::EnemyPathResult::Timed));
  if(op>=0x20&&op<=0x27){check(captured.left==108);check(captured.top==(op==0x26||op==0x27?12:76));check(captured.angle==(op==0x20?0:op==0x22?254:op==0x26||op==0x27?3:2));if(op==0x22)check(s.word(0x16)==256);if(op==0x24||op==0x27)check(captured.kind==captured.Bullet16&&captured.pattern==(op==0x24?3:5));}
  if(op==0x11)check(sound==3&&s.raw[16]==1&&s.word(10)==0&&s.word(8)==0);if(op==0xa8)check(sound==2);if(op==0xaf)check(tile.left==100&&tile.vram_y==2&&tile.image==3);
 }
 // Discriminating fan angle checks (not just bullet counts).
 for(unsigned group=1;group<=24;++group){const unsigned family=(group-1)/6,variant=(group-1)%6;const int widen=int(variant%3);std::vector<int> offsets;
  if(family==0){int k=3+3*widen;offsets={k,-k};}if(family==1){int k=8+8*widen;offsets={0,k,-k};}if(family==2){int k=3+6*widen;offsets={k,-k,k*3,-k*3};}if(family==3){int k=6+6*widen;offsets={0,k,-k,k*2,-k*2};}
  soew::BulletCore pool;soew::EnemyBulletRequest request;request.left=100;request.top=100;request.group=static_cast<std::uint8_t>(group);request.speed=48;request.angle=17;pool.add(request,0,208,336,rng);
  for(unsigned i=0;i<offsets.size();++i){const unsigned direction=(17+offsets[i]+(variant>=3?soew::enemy_atan2(256,120):0))&255;check(pool.bullets[i].velocity==soew::enemy_vector(direction,48));}
 }
 // Rank/playperf tuning over the ordinary gameplay domain, including integer
 // truncation and one extra stack on Hard/Lunatic.
 for(int rank=0;rank<5;++rank)for(int performance=-6;performance<=16;++performance)for(unsigned speed:{16u,32u,63u,96u,255u}){soew::BulletCore pool(rank);pool.playperf=performance;soew::EnemyBulletRequest request;request.left=100;request.top=100;request.group=0x20;request.speed=speed;pool.add(request,0,208,336,rng);int tuned=int(speed)+(int(speed)*performance)/48+(rank==0?-10:rank==3?12:0);if(tuned<16&&rank==0){check(pool.spawned==0);continue;}if(tuned<16)tuned=16;check(pool.bullets[0].velocity==std::array<int,2>{tuned,0});unsigned expected=1;int second=tuned/2+(rank==3?8:0);if((rank==2||rank==3)&&second>=16){expected=2;check(pool.bullets[1].velocity==std::array<int,2>{second,0});}check(pool.spawned==expected);}
 // Inclusive/exclusive collision boundaries, tested directly after zero motion.
 for(bool small:{false,true})for(int dx=-5;dx<=21;++dx)for(int dy=2;dy<=26;++dy){soew::BulletCore pool;auto &b=pool.bullets[0];b.flag=1;b.kind=small?soew::EnemyBulletRequest::Pellet:soew::EnemyBulletRequest::Bullet16;b.position[0]={static_cast<std::uint16_t>((208+dx)*16),static_cast<std::uint16_t>((300+dy)*16)};const bool expected=dx>=(small?7:-3)&&dx<(small?17:19)&&dy>=(small?12:4)&&dy<=(small?22:24);check(pool.update(0,208,300)==expected);check(b.flag==(expected?2u:1u));}
 for(int x:{23,24,416,417})for(int y:{-1,0,383,384}){soew::BulletCore pool;soew::EnemyBulletRequest r;r.left=x;r.top=y;r.group=0x20;r.speed=32;pool.add(r,0,208,336,rng);check(pool.spawned==unsigned(x>=24&&x<=416&&y>=0&&y<=383));}
 {soew::BulletCore pool;auto &b=pool.bullets[0];b.flag=2;b.position[0]={1,2};pool.begin_frame(1);check(b.flag==0);check(soew::BulletCore::pixel(65535)==-1);check(soew::BulletCore::pixel(65520)==-1);check(soew::BulletCore::pixel(65519)==-2);}
 {soew::BulletCore pool;pool.playperf=-32768;soew::EnemyBulletRequest r;r.left=40;r.top=40;r.group=0x20;r.speed=2;pool.add(r,0,208,336,rng);check(pool.spawned==1&&pool.bullets[0].velocity[0]==16);}
 {soew::BulletCore pool;soew::EnemyBulletRequest r;r.left=40;r.top=40;r.group=0x1b;r.angle=4;r.speed=32;soew::RandomRing draw;for(unsigned i=0;i<256;++i)draw.bytes[i]=static_cast<std::uint8_t>(i*7);pool.add(r,0,208,336,draw);check(pool.spawned==5&&draw.cursor==5);for(unsigned i=0;i<5;++i)check(pool.bullets[i].velocity==soew::enemy_vector(4+i*7,32));}
 {soew::BulletCore pool;soew::EnemyBulletRequest r;r.left=40;r.top=40;r.group=0x1c;r.angle=2;r.speed=32;soew::RandomRing draw;for(unsigned i=0;i<256;++i)draw.bytes[i]=static_cast<std::uint8_t>(i*5);pool.add(r,0,208,336,draw);check(pool.spawned==3&&draw.cursor==6);for(unsigned i=0;i<3;++i)check(pool.bullets[i].velocity==soew::enemy_vector(2+(i*2)*5,32+(((i*2+1)*5)&31)));}
 // Long deterministic pool evolution, including retired-slot reuse and shared
 // random cursor. This is repeatability, not an original-runtime oracle.
 {soew::BulletCore a(3),b(3);soew::RandomRing ra,rb;for(unsigned i=0;i<256;++i)ra.bytes[i]=rb.bytes[i]=static_cast<std::uint8_t>(i*13);for(unsigned tick=0;tick<500;++tick){unsigned page=tick&1;a.begin_frame(page);b.begin_frame(page);if(tick%7==0){soew::EnemyBulletRequest r;r.left=80;r.top=40;r.speed=48;r.angle=static_cast<std::uint8_t>(tick);r.group=0x1c;a.add(r,page,208,300,ra);b.add(r,page,208,300,rb);}check(a.update(page,208,300)==b.update(page,208,300));check(a.spawned==b.spawned&&ra.cursor==rb.cursor);for(unsigned i=0;i<150;++i)check(a.bullets[i].position==b.bullets[i].position&&a.bullets[i].velocity==b.bullets[i].velocity&&a.bullets[i].flag==b.bullets[i].flag);}}
 {soew::EnemySlots::Slot s;s.word(0x16,200);s.raw[0xe]=1;const auto before=s.raw;soew::EnemyEffectSink reject;reject.bullet=[](auto){return false;};soew::StageScript::EnemyTemplate t;const std::array<std::uint8_t,3> p={0x22,3,0x10};auto r=soew::run_enemy_path(s,p,rng,{0,0,0,0,0,&t,&reject});check(r.status==r.Blocked&&s.raw==before);}
 for(unsigned stage=0;stage<6;++stage){auto bytes=soew::StageScene::read(std::filesystem::path(argv[1])/("STAGE"+std::to_string(stage)+".DT1"));auto script=soew::StageScript::decode(bytes,true);soew::EnemyPathRuntime runtime(stage,1234);soew::BulletCore bullets;unsigned sound_requests=0,tile_requests=0,rejected_group=0;soew::EnemyEffectSink sink;
  sink.bullet=[&](const auto&r){const bool accepted=bullets.add(r,runtime.page,208,336,runtime.random);if(!accepted)rejected_group=r.group;return accepted;};
  // Explicit RECORDING sinks: not audio playback or VRAM tile application.
  sink.sound=[&](unsigned){++sound_requests;return true;};sink.tile=[&](const auto&){++tile_requests;return true;};
  unsigned guard=0;for(;guard<100000&&!runtime.blocked&&!runtime.scroll.done;++guard){bullets.begin_frame(runtime.page);const auto page=runtime.page;runtime.tick(script,208,336,&sink);if(!runtime.blocked)bullets.update(page,208,336,static_cast<std::uint32_t>(runtime.ticks-1));}
  check(guard<100000);check(runtime.blocked.has_value()||runtime.scroll.done);
  if(stage==1)check(rejected_group==0);
  if(stage==3)check(runtime.blocked&&runtime.blocked->result.reason=="declared formation row unavailable");std::cout<<"stage="<<stage<<" ticks="<<runtime.ticks<<" spawns="<<runtime.spawns<<" bullets="<<bullets.spawned<<" sounds_recorded="<<sound_requests<<" tiles_recorded="<<tile_requests<<" stopped="<<bool(runtime.blocked)<<" rejected_group="<<rejected_group<<" reason="<<(runtime.blocked?runtime.blocked->result.reason:"map_end")<<'\n';
 }
 std::cout<<"bullet_effect_checks="<<checks<<" failures=0\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
