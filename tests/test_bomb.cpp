#include "bomb_core.hpp"
#include "bullet_core.hpp"
#include "player_life.hpp"
#include <iostream>
unsigned checks=0;void check(bool v){++checks;if(!v)throw std::runtime_error("Bomb contract failed check="+std::to_string(checks));}
int main(){try{
 for(unsigned type=0;type<3;++type){soew::BombCore b;soew::RandomRing r;std::uint8_t stock=2;check(b.start(stock,type)&&stock==1&&b.active&&b.clear_requested);check(!b.start(stock,type)&&stock==1);b.clear_requested=false;
  for(int i=1;i<=32;++i){b.update(r);check(b.frame==i&&b.circle_frame==i&&!b.circle_done&&b.visual==soew::BombCore::Circle&&r.cursor==0);}
  b.update(r);check(b.frame==33&&!b.circle_done&&b.circle_frame==32);b.update(r);check(b.frame==0&&b.circle_done&&b.circle_frame==0);unsigned elapsed=34,clear_count=0;while(b.active&&elapsed<300){b.update(r);++elapsed;if(b.clear_requested){++clear_count;b.clear_requested=false;}if(b.active)check(b.damage(0)==1&&b.damage(1)==0);}
  check(b.finished&&!b.active&&clear_count==1&&b.palette_tone==100);check(elapsed==(type==0?190u:type==1?190u:162u));check(b.frame==(type==0?156:type==1?184:128));if(type==1)check(b.overlay_frame==18);if(type==0)check(b.spark_requests.size()==71);// A:53 particle steps, B:72 (body33..104), C:67; four draws each.
  check(r.cursor==(type==0?212:type==1?32:12));
 }
 {soew::BombCore b;std::uint8_t stock=0;check(!b.start(stock,0)&&!b.active);}
 {soew::BulletCore b;auto &a=b.bullets[0];a.flag=1;a.position[1]={1600,3200};b.bullets[1].flag=2;b.clear(0);check(a.flag==2&&b.bullets[1].flag==0&&a.position[0]==a.position[1]);b.begin_frame(1);check(a.flag==0);}
 {soew::BulletCore bullets;soew::EnemyBulletRequest req;req.left=200;req.top=100;req.speed=32;req.group=0x1c;soew::RandomRing r;check(bullets.add(req,0,200,300,r,true)&&r.cursor==0&&bullets.spawned==0);}
 {soew::PlayerLife l;l.invincibility=0;l.hit=true;check(!l.input_allowed());soew::BombCore b;std::uint8_t stock=1;if(l.input_allowed())b.start(stock,0);check(stock==1&&!b.active);l.hit=false;check(l.input_allowed());check(b.start(stock,0));l.bomb_invincible=true;l.register_hit();l.player_phase_begin();check(!l.hit);l.bomb_invincible=false;l.invincibility=70;l.player_phase_begin();check(l.invincibility==69);}
 std::cout<<"bomb_core_checks="<<checks<<" failures=0\n";return 0;
}catch(const std::exception&e){std::cerr<<e.what()<<'\n';return 1;}}
